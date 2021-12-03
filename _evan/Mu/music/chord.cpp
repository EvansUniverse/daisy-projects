/* 
 * Copyright (C) 2021 Evan Pernu - All Rights Reserved
 * You may use, distribute and modify this code under the
 * terms of the GNU AGPLv3 license.
 * 
 * You should have received a copy of the GNU AGPLv3 license with
 * this file (LICENSE.md). If not, please write to: evanpernu@gmail.com, 
 * or visit: https://www.gnu.org/licenses/agpl-3.0.en.html
 */

#pragma once

#include <algorithm>
#include <map>
#include <vector>

#include "theory.h"
#include "chord.h"

using namespace mu;

DiatonicChord::DiatonicChord(){};

DiatonicChord::DiatonicChord(
        int theRoot = 24, // C2
        int theModeRoot = 24, // C2
        std::string theMode = "Major", 
        std::string theVoicing = "Triad"
    ){
    root = theRoot;
    mode = theMode;
    modeRoot = theModeRoot;
    voicing = theVoicing;

    this->SetRoot(theRoot);
    this->SetMode(theMode);
    this->SetVoicing(theVoicing);

    string ="";
    this->UpdateString();
}

void DiatonicChord::UpdateChord(){
    int degree;
    int oct;
    int chordLen = static_cast<int>(voicingToScaleDegrees.at(voicing).size());
    int scaleLen = static_cast<int>(modeToSemitones.at(mode).size());
    semis = std::vector<int>(chordLen);

    // For each degree in the chord
    for (int i = 0; i < chordLen; i++){
        // Get the degree
        degree = voicingToScaleDegrees.at(voicing)[i];

        // // Figure out how many octaves above 0 it is
        // oct = degree / (scaleLen + 1);
        // if (oct > 0){
        //     degree = degree % scaleLen;
        // }

        // Offset by 1 since the values of the maps are 1-inedexed
        degree--;

        // Calculate the semitone value
        semis[i] = modeToSemitones.at(mode)[degree]; //+ 12 * (oct + mOct->index) + mTonic->index;

        // If the value exceeds our note range, bring it up/down an octave until it fits
        while (semis[i] > MAX_NOTE){
            semis[i] -= 12;
        }
        while (semis[i] < MIN_NOTE){
            semis[i] += 12;
        }
    }

    this->UpdateString();
}

void DiatonicChord::SetRoot(int theRoot){
    root = theRoot;
    UpdateChord();
}

// TODO this is untested
void DiatonicChord::SetRootByNote(int theRoot){
    // Quantize theRoot
    if(!isDiatonic(theRoot, mode)){
        theRoot == MAX_NOTE ? theRoot-- : theRoot++;
    }

    // Set root to the scale degree of theRoot
    std::vector<int> s = modeToSemitones.at(mode);
    theRoot = distance(s.begin(), std::find(s.begin(), s.end(), theRoot % 12)) + 1;
    UpdateChord();
}

void DiatonicChord::SetModeRoot(int theModeRoot){
    modeRoot = theModeRoot;
    UpdateChord();
}

void DiatonicChord::SetMode(std::string theMode){
    mode = theMode;
    UpdateChord();
}

void DiatonicChord::SetVoicing(std::string theVoicing){
    voicing = theVoicing;
    UpdateChord();
}

void DiatonicChord::Transpose(int i) {
    if (modeRoot + i < MIN_NOTE || modeRoot + i > MAX_NOTE){
        return;
    }
    modeRoot += i;
    UpdateChord();
}


// TODO make this more accurate/robust (i.e. "A minor" instead of "A triad")
void DiatonicChord::UpdateString(){
    string = allNotes5Oct[root] + " " + voicing;

    if(addOctDown){
        string += "-";
    }
    if(addOctUp){
        string += "+";
    }
}