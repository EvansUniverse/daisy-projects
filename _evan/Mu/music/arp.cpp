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

using namespace mu;



Arp::Arp(){};
Arp::Arp(int maxLength){
    // Initialize variables
    arpStep     = 0;
    arpTraversalIndex = 0;
    arpNoteDacOutput1 = 0.f;
    clockCount  = 0;

    // Initialize arp
    this->UpdateArpTraversal();
    this->UpdateArpNotes();
};

// Intended to be called every time a clock pulse is received
void Arp::OnStep(){
    clockCount++;
    if (clockCount >= clockInDivToInt[mClockDiv->Value()]){
        clockCount = 0;
        this->UpdateArpStep();
    }
}

// Updates note and length data for the arp
void Arp::UpdateArpNotes(){
    int degree;
    int oct;
    int chordLen = static_cast<int>(voicingToScaleDegrees.at(mVoicing->Value()).size());
    int scaleLen = static_cast<int>(scalesToSemitones.at(mScale->Value()).size());

    // For each degree in the chord
    //
    // bug: 5th note in kenny barron chords (both major and minor) resolve to octaves
    for (int i = 0; i < chordLen; i++){
        // Get the degree
        degree = voicingToScaleDegrees.at(mVoicing->Value())[i];

        // Figure out how many octaves above 0 it is
        oct = degree / (scaleLen + 1);
        if (oct > 0){
            degree = degree % scaleLen;
        }

        // Offset by 1 since the values of the maps are 1-inedexed
        degree--;

        // Calculate the semitone value
        arpNotes[i] = scalesToSemitones.at(mScale->Value())[degree] + 12 * (oct + mOct->index) + mTonic->index;

        // If the value exceeds our note range, bring it up/down an octave until it fits
        while (arpNotes[i] > 60){
            arpNotes[i] -= 12;
        }
        while (arpNotes[i] < 0){
            arpNotes[i] += 12;
        }
    }

    this->UpdateArpString();
}

// Updates the arp traversal values based on the current pattern
void Arp::UpdateArpTraversal(){
    arpTraversal = std::vector<int>{};
    int chordLen = static_cast<int>(voicingToScaleDegrees.at(mVoicing->Value()).size());

    if (mPattern->Value() == "Down") {
        for (int i = chordLen-1; i >= 0; i--) {
            arpTraversal.push_back(i);
        }

    } else if (mPattern->Value() == "U+D In") {
        for (int i = 0; i < chordLen; i++) {
            arpTraversal.push_back(i);
        }
                for (int i = chordLen-1; i > 0; i--) {
            arpTraversal.push_back(i);
        }

    } else if (mPattern->Value() == "U+D Ex") {
        for (int i = 0; i < chordLen-1; i++) {
            arpTraversal.push_back(i);
        }
        for (int i = chordLen-1; i > 0; i--) {
            arpTraversal.push_back(i);
        }

    } else if (mPattern->Value() == "Random") {// TODO implement
        // -1 will represent "random value"
        arpTraversal.push_back(-1);

    } else { // mPattern->Value() == "Up"
        for (int i = 0; i < chordLen; i++) {
            arpTraversal.push_back(i);
        }
    }    

    arpTraversalIndex = 0;
}

// Called every time the arp steps to the next note
//
// TODO: modify for other patterns besides up
void Arp::UpdateArpStep()
{
    int semi;

    arpStep = arpTraversal[arpTraversalIndex];

    if (arpStep < 0) {
        // Random note
        // TODO this random method is biased, created a stronger random function
        // Maybe keep this one as a separate option (e.g. "BadRandom") if it produces interesting
        // musical results
        semi = arpNotes[rand() % arpNotes.size()];
    } else {
        semi = arpNotes[arpStep];
    }

    debugString = std::to_string(semi);

    trigOut = true;
    arpNoteDacOutput1 = SemitoneToDac(semi);
    
    arpTraversalIndex++;
    arpTraversalIndex = arpTraversalIndex % static_cast<int>(arpTraversal.size());

    this->UpdateArpString();
}

// Updates the string used to display the arp
void Arp::UpdateArpString(){
    arpString = "";
    int chordSize = static_cast<int>(voicingToScaleDegrees.at(mVoicing->Value()).size());

    for(int i = 0; i < chordSize; i++){
        if (i == arpStep){
            arpString += std::to_string(arpNotes[i]);
        } else {
            arpString += "_";
        }
    }
}