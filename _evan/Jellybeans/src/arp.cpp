/* 
 * Copyright (C) 2021 Evan Pernu - All Rights Reserved
 * You may use, distribute and modify this code under the
 * terms of the GNU AGPLv3 license.
 * 
 * You should have received a copy of the GNU AGPLv3 license with
 * this file (LICENSE.md). If not, please write to: evanpernu@gmail.com, 
 * or visit: https://www.gnu.org/licenses/agpl-3.0.en.html
 */

#include "arp.h"
#include "chord.h"

using namespace jellybeans;

Arp::Arp(){
    maxSteps = 8;
    step     = 0;
    traversalIndex = 0;
    noteDacOutput1 = 0.f;
    clockCount  = 0;
    pattern = "Up";
    clockDiv = 1;
    chord = DiatonicChord();
    string = "EMPTY ARP";

    this->UpdateTraversal();
}

Arp::Arp(int theMaxSteps, DiatonicChord theChord, std::string thePattern, int theClockDiv){
    maxSteps = theMaxSteps;
    step     = 0;
    traversalIndex = 0;
    noteDacOutput1 = 0.f;
    clockCount  = 0;
    pattern = thePattern;
    clockDiv = theClockDiv;
    chord = theChord;
    string = "EMPTY ARP";

    this->UpdateTraversal();
};

// Updates the arp traversal values based on the current pattern
void Arp::UpdateTraversal(){
    // traversal = std::vector<int>{};

    // if (pattern == "Down") {
    //     for (int i = chord.length-1; i >= 0; i--) {
    //         traversal.push_back(i);
    //     }

    // } else if (pattern == "U+D In") {
    //     for (int i = 0; i < chord.length; i++) {
    //         traversal.push_back(i);
    //     }
    //             for (int i = chord.length-1; i > 0; i--) {
    //         traversal.push_back(i);
    //     }

    // } else if (pattern == "U+D Ex") {
    //     for (int i = 0; i < chord.length-1; i++) {
    //         traversal.push_back(i);
    //     }
    //     for (int i = chord.length-1; i > 0; i--) {
    //         traversal.push_back(i);
    //     }

    // } else if (pattern == "Random") {// TODO implement
    //     // -1 will represent "random value"
    //     traversal.push_back(-1);

    // } else { // pattern == "Up"
    //     for (int i = 0; i < chord.length; i++) {
    //         traversal.push_back(i);
    //     }
    // }    

    // traversalIndex = 0;
}

// Intended to be called every time a clock pulse is received
void Arp::OnStep(){
    // clockCount++;
    // if (clockCount >= clockDiv){
    //     clockCount = 0;
    //     this->UpdateStep();
    // }
}

void Arp::UpdateStep(){
    // int semi;
    // step = traversal[traversalIndex];

    // if (step < 0) {
    //     // Random note
    //     // TODO this random method is biased, created a stronger random function
    //     // Maybe keep this one as a separate option (e.g. "BadRandom") if it produces interesting
    //     // musical results
    //     semi = chord.GetNoteAt(rand() % chord.length);
    // } else {
    //     semi = chord.GetNoteAt(step);
    // }

    //debugString = std::to_string(semi);

   // trigOut = true;
    //arpNoteDacOutput1 = SemitoneToDac(semi);
    
    // traversalIndex++;
    // traversalIndex = traversalIndex % static_cast<int>(traversal.size());

    // this->UpdateString();
}

void Arp::UpdateString(){
    // string = "";

    // for(int i = 0; i < maxSteps; i++){
    //     if (i == step){
    //         string += std::to_string(chord.GetNoteAt(i));
    //     } else {
    //         string += "_";
    //     }
    // }
}