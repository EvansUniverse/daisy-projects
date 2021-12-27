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
    const int MAX_NOTE = 59; // Temporarily moved down from 60 due to dac conversion bug
    const int MIN_NOTE = 0;

    // Returns true if the given note is diatonic to the given mode
    // @param a note's index in semitones from C or C0
    // @param an element of mu::allModes
    // bool isDiatonic(int, std::string);

    // If the note exceeds our note range, bring it up/down an octave until it fits
    // @param the semitone value of a note
    int quantizeNoteToRange(int);

    // Converts a semitone value to data that can be supplied to Daisy Seed's DAC
    // for CV, using the function patch.seed.dac.WriteValue()
    //
    // In Daisy Seed's DAC, 0=0v and 4095=5v. 4095/5=819, meaning 819 (dac units?)
    // per volt or octave.
    //
    // @param semi: an integer between 0-60 representing the number of semitones from low C
    float semitoneToDac(int);

    // Converts float to string
    // @param float
    // @param decimalPlaces
    std::string floatToString(float, uint8_t);

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
    const std::array<std::string, 60> allNotes5Oct {
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
        // Temporarily removed due to dac conversion bug
        // "B4"
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

    // Maps chord voicings to the scale degrees they contain (in relation to the chord's root note)
    const std::map<std::string, std::vector<int>> voicingToScaleDegrees {
        {"Triad",    std::vector<int>{1, 3, 5}},
        {"Triad+",   std::vector<int>{1, 3, 5, 8}},
        {"7th",      std::vector<int>{1, 3, 5, 7}},
        {"7th+",     std::vector<int>{1, 3, 5, 7, 8}},
        {"9th",      std::vector<int>{1, 3, 5, 7, 9}},
        {"11th",     std::vector<int>{1, 3, 5, 7, 9, 11}},
        {"13th",     std::vector<int>{1, 3, 5, 7, 9, 11, 13}},
        {"6th",      std::vector<int>{1, 3, 5, 6}},
        {"Sus2",     std::vector<int>{1, 2, 5}},
        {"Sus4",     std::vector<int>{1, 4, 5}},
        // disabled til its bug is fixed
        // bug is likely caused by some values in the KB chord being non-diatonic.
        // might need to implement a # system or something to make it work
        //{"Kenny B.", std::vector<int>{1, 5, 9, 10, 14, 18}}, 
        {"Power",    std::vector<int>{1, 5}},
        {"Power+",   std::vector<int>{1, 5, 8}},
        {"Shell 1",  std::vector<int>{1, 7, 10}},
        {"Shell 2",  std::vector<int>{1, 10, 14}},
    };

    // All possible chord voicings
    const std::vector<std::string> voicings {
        "Triad",
        "Triad+",
        "7th",
        "7th+",
        "9th",
        "11th",
        "13th",
        "6th",
        "Sus2",
        "Sus4",
        // Some values in this chord don't render correctly, 
        // Disabled til its bug is fixed
        //"Kenny B.",  // Kenny Barron chord 
        "Power",
        "Shell 1",
        "Shell 2"
    };


    const std::vector<std::string> allInversions {
        "None",
        "1st",
        "2nd",
        "3rd"
    };

    // const std::vector<std::string> allDrops {
    //     "None",
    //     "Drop 2",
    //     "Drop 3",
    //     "Drop 4"
    // };

    const std::vector<std::string> allPPQs {
        "4",
    };

    const std::vector<std::string> allClockDivs {
        "1/16",
        "1/8",
        "1/4",
        "1/2", 
        "1",
        "2",
        "4",
        "8",
        "16"
    };
}