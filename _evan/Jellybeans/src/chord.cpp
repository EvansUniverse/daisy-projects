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
//#include <iostream>

#include "theory.h"
#include "chord.h"

using namespace jellybeans;

DiatonicChord::DiatonicChord(){
    root = 1;
    mode = "Major";
    modeRoot = 0;
    voicing = "Triad";

    this->SetRoot(1);
    this->SetMode("Major");
    this->SetVoicing("Triad");

    string ="";
    this->UpdateString();
}

DiatonicChord::DiatonicChord(int theRoot, int theModeRoot, std::string theMode, std::string theVoicing){
    root = theRoot;
    mode = theMode;
    modeRoot = theModeRoot;
    voicing = theVoicing;

    this->SetRoot(theRoot);
    this->SetMode(theMode);
    this->SetVoicing(theVoicing);

    string ="kord";
    //this->UpdateString();
}

void DiatonicChord::UpdateChord(){
    // int degree;
    // int oct;
    // int chordLen = static_cast<int>(voicingToScaleDegrees.at("Triad").size());
    // //int scaleLen = static_cast<int>(modeToSemitones.at(mode).size());
    // semis = std::vector<int>(chordLen);

    // length = chordLen;

    // // For each note in the chord
    // for (int i = 0; i < chordLen; i++){
    //     // Get the degree
    //     degree = voicingToScaleDegrees.at(voicing)[i];

    //     // Offset by 1 since the values of the maps are 1-inedexed
    //     degree--;

    //     // Calculate the semitone value
    //     semis[i] = modeToSemitones.at(mode)[degree]; //+ 12 * (oct + mOct->index) + mTonic->index;

    //     // If the value exceeds our note range, bring it up/down an octave until it fits
    //     while (semis[i] > MAX_NOTE){
    //         semis[i] -= 12;
    //     }
    //     while (semis[i] < MIN_NOTE){
    //         semis[i] += 12;
    //     }
    // }

    // this->UpdateString();
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

int DiatonicChord::GetNoteAt(int n){
    if (n >= length) {
        return semis[0];
    }
    return semis[n];
}

// TODO make this more accurate/robust (i.e. "A minor" instead of "A triad")
void DiatonicChord::UpdateString(){
    //string = allNotes5Oct[root] + " " + voicing;
}