/* 
 * Copyright (C) 2021 Evan Pernu - All Rights Reserved
 * You may use, distribute and modify this code under the
 * terms of the GNU AGPLv3 license.
 * 
 * You should have received a copy of the GNU AGPLv3 license with
 * this file (LICENSE.md). If not, please write to: evanpernu@gmail.com, 
 * or visit: https://www.gnu.org/licenses/agpl-3.0.en.html
 */

#include <algorithm>
#include <map>
#include <vector>
#include <string>

#include "theory.h"
#include "chord.h"

using namespace jellybeans;

DiatonicChord::DiatonicChord(){
    root = 0;
    mode = "Major";
    voicing = "Triad";

    this->setRoot(1);
    this->setMode("Major");
    this->setVoicing("Triad");
}

DiatonicChord::DiatonicChord(int theRoot, std::string theMode, std::string theVoicing){
    root = theRoot;
    mode = theMode;
    voicing = theVoicing;

    this->setRoot(theRoot);
    this->setMode(theMode);
    this->setVoicing(theVoicing);
}

/*
 * Updaters
 */

void DiatonicChord::updateChord(){
    int degree;
    int chordLen = static_cast<int>(voicingToScaleDegrees.at(voicing).size()); // death occurs in one of these 3 lines
    int scaleLen = static_cast<int>(modeToSemitones.at(mode).size());
    semis = std::vector<int>(chordLen);
    length = chordLen;

    // For each note in the chord
    for (int i = 0; i < chordLen; i++){
        // Get the degree
        degree = voicingToScaleDegrees.at(voicing)[i];

        // Offset by 1 since the values of the maps are 1-inedexed
        degree--;

        // Offset notes in higher octave registers
        int offset = 0;
        while (degree + 1 > scaleLen) {
            degree -= scaleLen;
            offset++;
        }

        // Calculate the semitone value
        semis[i] = modeToSemitones.at(mode)[degree] + (12 * offset) + root;

        // If the value exceeds our note range, bring it up/down an octave until it fits
        while (semis[i] > MAX_NOTE){
            semis[i] -= 12;
        }
        while (semis[i] < MIN_NOTE){
            semis[i] += 12;
        }
    }

    this->updateString();
}

// Displayed as a list of notes e.g. "C E G#"
void DiatonicChord::updateString(){
    string = "";
     for(int i : semis) 
        string += allNotes[i % 12] + " ";


    // Other option I'm entertaining: list of semis e.g. "0 4 7"
    // for(int i : semis) 
    //     string += std::to_string(i) + " ";

    // Other option I'm entertaining e.g "C# Triad"
    // TODO make this more accurate/robust (i.e. "A minor triad" instead of "A triad")
    // string =  allNotes5Oct[root] + " " + voicing;
}

/*
 * Setters
 */

void DiatonicChord::setRoot(int i){
    root = i;
    updateChord();
}

void DiatonicChord::setMode(std::string s){
    mode = s;
    updateChord();
}

void DiatonicChord::setVoicing(std::string s){
    voicing = s;
    updateChord();
}

/*
 * Getters
 */

int DiatonicChord::getNoteAt(int n){
    // If out of bounds, return the first note.
    // This behavior could be modified in the future.
    if (n >= length) {
        return semis[0];
    }
    return semis[n];
}

int DiatonicChord::getLength(){
    return length;
}

std::string DiatonicChord::toString(){
    return string;
}
