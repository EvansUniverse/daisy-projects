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

#include <map>
#include <vector>
#include <string>

namespace jellybeans {

    const int MAX_NOTE = 60;
    const int MIN_NOTE = 0;

    // Returns true if the given note is diatonic to the given mode
    // @param a note's index in semitones from C or C0
    // @param an element of mu::allModes
    bool isDiatonic(int, std::string);

    // This seems redundant but it may be needed for fractional clock divs
    const std::map<std::string, int> clockInDivToInt {
        {"1",  1},
        {"2",  2},
        {"4",  4},            
        {"8",  8},
        {"16", 16}
    };

    // Note that the indices of these elements also correspond to
    // their semitone distances from C.
    const std::vector<std::string> allNotes {
        "C",
        "C#",
        "D",
        "D#",
        "E",
        "F",
        "F#",
        "G",
        "G#",
        "A",
        "A#",
        "B"
    };

    // Note that the indices of these elements also correspond to
    // their semitone distances from C0.
    const std::vector<std::string> allNotes5Oct {
        "C0",
        "C#0",
        "D0",
        "D#0",
        "E0",
        "F0",
        "F#0",
        "G0",
        "G#0",
        "A0",
        "A#0",
        "B0",
        "C1",
        "C#1",
        "D1",
        "D#1",
        "E1",
        "F1",
        "F#1",
        "G1",
        "G#1",
        "A1",
        "A#1",
        "B1",
        "C2",
        "C#2",
        "D2",
        "D#2",
        "E2",
        "F2",
        "F#2",
        "G2",
        "G#2",
        "A2",
        "A#2",
        "B2",
        "C3",
        "C#3",
        "D3",
        "D#3",
        "E3",
        "F3",
        "F#3",
        "G3",
        "G#3",
        "A3",
        "A#3",
        "B3",
        "C4",
        "C#4",
        "D4",
        "D#4",
        "E4",
        "F4",
        "F#4",
        "G4",
        "G#4",
        "A4",
        "A#4",
        "B4"
        // "C5" ??? TODO does this fit
    };

    // Note: Mode names are currently abbreviated due to 
    // Daisy Patch screen size limitations.
    //
    // TODO support exotic modes
    const std::vector<std::string> modes {
        "Major",
        "Minor",
        "Dorian",
        "Phyrgi", // Phyrgian
        "Lydian",
        "Mixo",   // Mixolydian
        "Locri",  // Locrian
    };

    // Maps scale names to their first octave of semitone values
    const std::map<std::string, std::vector<int>> modeToSemitones {
        {"Major",  std::vector<int>{0, 2, 4, 5, 7, 9, 11}},
        {"Minor",  std::vector<int>{0, 2, 3, 5, 7, 8, 10}},
        {"Dorian", std::vector<int>{0, 2, 3, 5, 7, 9, 10}},
        {"Phyrgi", std::vector<int>{0, 1, 3, 5, 7, 9, 10}},
        {"Lydian", std::vector<int>{0, 2, 4, 6, 7, 9, 11}},
        {"Mixo",   std::vector<int>{0, 2, 4, 5, 7, 9, 10}},
        {"Locri",  std::vector<int>{0, 1, 3, 5, 6, 8, 10}},
    };

    const std::vector<std::string> allInversions {
        "None",
        "Drop 2",
        "Drop 3",
        "Drop 4"
    };

    const std::vector<std::string> allClockInDivs {
        // "1/2", // TODO figure out how to interpolate for fractional clock values
        // "1/4",
        // "1/8",
        // "1/16",
        // "1/32",
        // "1/64",
        "1",
        "2",
        "4",
        "8",
        "16"
    };
}