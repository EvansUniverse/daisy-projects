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

        // If true, a new note is being played this frame and a trigger
        // should be sent out.
        bool trig; 

        // The CV value currently being sent to Patches' DAC's output 1
        // This is stored so that it's only calculated when necessary
        float dacValue;

        // Stores which arp note should be played at each next step.
        // Contains a list of indices of arpNotes
        std::vector<int> traversal;

        // Must be an element of mu::arpPatterns
        std::string pattern;

        // String representation of the arp
        std::string string;

        // Underlying chord
        DiatonicChord* chord;

        void resetState();
    public:
        Arp();// : Arp(8, DiatonicChord(1, 0, "Major", "Triad"), "Up", 1) {};

        // @param maxSteps
        // @param chord
        // @param pattern - ∈ arpPatterns
        Arp(int, DiatonicChord*, std::string);

        // Intended to be called every time a clock pulse is received
        void onClockPulse();
    
        // Updates the arp traversal values based on the current pattern
        void updateTraversal();

        // Called every time the arp steps to the next note
        void updateStep();

        void updateString();

        std::string toString();

        // Returns true if there's a new note trigger. Also resets the 
        // trigger bool, effectively telling the arp "I've consumed the trigger"
        bool getTrig();

        float getDacValue();

        DiatonicChord* getChord();

        // @param ∈ arpPatterns
        void setPattern(std::string);
    };

    const std::vector<std::string> arpPatterns {
        "Up",
        "Down",
        "U+D In",
        "U+D Ex",
        "Random"
    };
}