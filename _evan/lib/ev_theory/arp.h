/* 
 * Copyright (C) 2021, 2022 Evan Pernu. Author: Evan Pernu
 * 
 * You may use, distribute and modify this code under the
 * terms of the GNU AGPLv3 license.
 * 
 * This program is part of "Evan's Daisy Projects".
 * 
 * "Evan's Daisy Projects" is free software: you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as published
 * by the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#include <string>
#include <vector>

#include "chord.h"

namespace ev_theory {
    class Arp {
    private:
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
    public:
        Arp();

        // @param pattern - ∈ arpPatterns
        Arp(std::string);

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

        void resetState();

        void resetPosition();
    };

    const std::vector<std::string> arpPatterns {
        "Up",
        "Down",
        "U+D In",
        "U+D Ex",
        "Random"
    };
} // namespace ev_theory