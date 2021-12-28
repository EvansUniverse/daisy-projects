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

DiatonicChord::DiatonicChord() : DiatonicChord(0, "Major", 0, "Triad", 0){}

DiatonicChord::DiatonicChord(
        uint8_t     theModeRoot,
        std::string theMode,
        uint8_t     theDegree,
        std::string theVoicing,
        uint8_t     theOctave
){
    degree    = theDegree;
    octave    = theOctave;
    mode      = theMode;
    modeRoot  = theModeRoot;
    voicing   = theVoicing;
    inversion = 0;
    updateChord();
}

/* Updaters */

void DiatonicChord::updateChord(){
    length = static_cast<uint8_t>(voicingToScaleDegrees.at(voicing).size());
    semis = std::vector<uint8_t>(length);

    //uint8_t root = scaleDegreeToNote(degree, mode, modeRoot);

    uint8_t noteDegree;

    // Populate semis
    for (uint8_t i = 0; i < length; i++){
        noteDegree = (degree + voicingToScaleDegrees.at(voicing)[i]-1) % 7;
        // Get the note
        semis[i] = scaleDegreeToNote(noteDegree, mode, modeRoot);

        // TODO octave modifier

        // Make sure it's higher than the previous note
        while (i > 0 && semis[i] < semis[i-1]) {
            semis[i] += 12;
        }

       semis[i] = quantizeNoteToRange(semis[i]);
    }

    // Calculate inversion
    for (uint8_t i = 0; i < inversion; i++){
        semis.push_back(semis.front() + 12); 
        semis.erase(semis.begin());
       // semis.back() = quantizeNoteToRange(semis.back());
    }

    this->updateString();
}

// Displayed as a list of notes e.g. "C E G#"
void DiatonicChord::updateString(){
    string = "";
    //  for(uint8_t i : semis) 
    //     string += allNotes[i % 12] + " ";

    // Other option I'm entertaining: e.g. "A minor iii"
   string += allNotes[modeRoot] + " " + std::to_string(degree) + " - ";

    // Other option I'm entertaining: list of semis e.g. "0 4 7"
    for(uint8_t i : semis) 
        string += std::to_string(i) + " ";

    // Other option I'm entertaining: e.g "C# Triad"
    // TODO make this more accurate/robust (i.e. "A minor triad" instead of "A triad")
    // string =  allNotes5Oct[degree] + " " + voicing;
}

/*  Getters */

uint8_t DiatonicChord::getNoteAt(uint8_t n){
    // If out of bounds, return the first note.
    // This behavior could be modified in the future.
    if (n >= length) {
        return semis[0];
    }
    return semis[n];
}

uint8_t DiatonicChord::getDegree(){
    return degree;
}

uint8_t DiatonicChord::getLength(){
    return length;
}

std::string DiatonicChord::toString(){
    return string;
}

/* Setters */

void DiatonicChord::setDegreeByNote(uint8_t i){
    degree = noteToScaleDegree(i, mode, modeRoot);
    updateChord();
}

void DiatonicChord::setDegree(uint8_t i){
    if (i >= scaleLen ){
        i = scaleLen - 1;
    }
    degree = i;
    updateChord();
}

void DiatonicChord::setModeRoot(uint8_t i){
    modeRoot = i % 12;
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

void DiatonicChord::setOctave(uint8_t i){
    octave = i;
    updateChord();
}

void DiatonicChord::setInversion(uint8_t i){
    inversion = i;
    updateChord();
}