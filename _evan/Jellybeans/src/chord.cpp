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

DiatonicChord::DiatonicChord() : DiatonicChord(0, "Major", "Triad", 0){}

DiatonicChord::DiatonicChord(int theRoot, std::string theMode, std::string theVoicing, int theOctave){
    root      = theRoot;
    octave    = theOctave;
    mode      = theMode;
    voicing   = theVoicing;
    inversion = 0;
    updateChord();
}

/* Updaters */

void DiatonicChord::updateChord(){
    int degree;
    int chordLen = static_cast<int>(voicingToScaleDegrees.at(voicing).size());
    int scaleLen = 7; // static_cast<int>(modeToSemitones.at(mode).size()); // TODO in the future, may need this for exotic scales
    semis = std::vector<int>(chordLen);
    length = chordLen;

    // Populate semis
    for (int i = 0; i < chordLen; i++){
        // Get the degree
        degree = voicingToScaleDegrees.at(voicing)[i];

        // Offset if note is in higher octave registers
        int offset = 0;
        while (degree > scaleLen) {
            degree -= scaleLen;
            offset++;
        }

        // Calculate the note's semitone value
        semis[i] = modeToSemitones.at(mode)[degree-1] + (12 * offset) + (12 * octave) + root;
        semis[i] = quantizeNoteToRange(semis[i]);
    }

    // Calculate inversion
    for (int i = 0; i < inversion; i++){
        semis.push_back(semis.front() + 12); 
        semis.erase(semis.begin());
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

/*  Getters */

int DiatonicChord::getNoteAt(int n){
    // If out of bounds, return the first note.
    // This behavior could be modified in the future.
    if (n >= length) {
        return semis[0];
    }
    return semis[n];
}

int DiatonicChord::getRoot(){
    return root;
}

int DiatonicChord::getLength(){
    return length;
}

std::string DiatonicChord::toString(){
    return string;
}

/* Setters */

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

void DiatonicChord::setOctave(int i){
    octave = i;
    updateChord();
}

void DiatonicChord::setInversion(int i){
    inversion = i;
    updateChord();
}