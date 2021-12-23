/* 
 * Copyright (C) 2021 Evan Pernu - All Rights Reserved
 * You may use, distribute and modify this code under the
 * terms of the GNU AGPLv3 license.
 * 
 * You should have received a copy of the GNU AGPLv3 license with
 * this file (LICENSE.md). If not, please write to: evanpernu@gmail.com, 
 * or visit: https://www.gnu.org/licenses/agpl-3.0.en.html
 */

#include <string>
#include <vector>

#include "chord.h"

namespace jellybeans {
    class Arp {
    private:
        // Maximum arp steps
        int maxSteps;

        // Tracks the current position in arpTraversal
        int traversalIndex;

        // Current step index, 0 based
        int step;

        // If true, a new note is being played this frame
        bool newNote; 

        // The CV value currently being sent to Patches' DAC's output 1
        // This is stored so that it's only calculated upon a change
        float dacValue;

        // Number of clock pulses that have been received since the last reset
        //int clockCount;

        // The arp's clock division (step to the next note every X clock pulses)
        // TODO convert to int, out of say 256 notes per bar, so that we can use rapid pulses for fractional timing and swing
        //int clockDiv;

        // Stores which arp note should be played at each next step.
        // Contains a list of indices of arpNotes
        std::vector<int> traversal;

        // Must be an element of mu::arpPatterns
        std::string pattern;

        // String representation of the arp
        std::string string;

        // Current root note
        float root; //TODO delete?

        // Underlying chord
        DiatonicChord* chord;
    public:
        Arp();// : Arp(8, DiatonicChord(1, 0, "Major", "Triad"), "Up", 1) {};

        // @param maxSteps
        // @param chord
        // @param pattern - must be an element of arpPatterns
        // @param clockDiv
        Arp(int, DiatonicChord*, std::string, int);

        // Intended to be called every time a clock pulse is received
        void OnClockPulse();
    
        // Updates the arp traversal values based on the current pattern
        void UpdateTraversal();

        // Called every time the arp steps to the next note
        void UpdateStep();

        void UpdateString();

        std::string toString();

        // Returns true if there's a new note to consume.
        // Resets the "new note" bool, effectively telling the 
        // arp "I've consumed the note"
        bool getNewNote();

        float getDacValue();

        //void setMaxSteps(int);
        //void setChord(DiatonicChord*);
        void setPattern(std::string);
    };

    const std::vector<std::string> arpPatterns {
        "Up",
        "Down",
        "U+D In",
        "U+D Ex",
        "Random"
    };

    const std::vector<std::string> arpRhythms {
        "None",
        "Sw 25%",
        "Sw 50%",
        "Sw 75%",
        "Sw 100%"
    };
}