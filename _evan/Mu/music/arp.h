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
#include <array>
#include <map>
#include <vector>

namespace mu {
    class Arp {
    public:
        // Maximum possible arp step
        int maxArpSteps;

        // Tracks the current position in arpTraversal
        int arpTraversalIndex;

        // Current step index, 0 based
        int arpStep; 

        // The note vallue currently being sent to Patches' DAC's output 1
        // This is stored so that it's only calculated upon a change
        float arpNoteDacOutput1;

        // Number of clock pulses that have been received since the last reset
        int clockCount;

        // The semitone values for each step
        std::vector<int> arpNotes; //TODO FYI this was formerly an array w/ length of maxArpSteps

        // Stores which arp note should be played at each next step.
        // Contains a list of indices of arpNotes
        std::vector<int> arpTraversal;

        // If true, the arp is currently traveling up
        // if false, it's currently traveling down
        bool goingUp;

        // String representation of the arp
        std::string arpString;

        // Current root note
        float root;

        Arp();
        Arp(int maxLength);

        // Intended to be called every time a clock pulse is received
        void OnStep();

        // Updates note and length data for the arp
        void UpdateArpNotes();

        // Updates the arp traversal values based on the current pattern
        void UpdateArpTraversal();

        // Called every time the arp steps to the next note
        //
        // TODO: modify for other patterns besides up
        void UpdateArpStep();

        // Updates the string used to display the arp
        void UpdateArpString();
    };
}