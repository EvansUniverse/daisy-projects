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

    // @param the semitone value of a note
    // @return if the note exceeds our range, bring it up/down an octave until it fits
    int quantizeNoteToRange(int);

    // Converts 
    //
    // @param semi: semitone value corresponding to theory::allNotes5Oct
    // @return value that can be supplied to Daisy Seed's DAC for CV out 
    //         using the function patch.seed.dac.WriteValue()
    float semitoneToDac(int);

    // @param float
    // @param decimalPlaces
    // @return string representation of the float e.g. (1.041, 2) -> "1.04"
    std::string floatToString(float, uint8_t);

    // Quantizes a note to a mode. Always quantizes down, unless the note is 0, 
    // in which case it quantizes up.
    //
    // @param semitone value
    // @param mode
    // @param mode root
    // @return semitone value of the quantized note
    uint8_t quantize(uint8_t, std::string, uint8_t);

    // @param semitone value
    // @param mode
    // @param mode root
    // @return 0-indexed scale degree of the given note (quantized)
    uint8_t noteToScaleDegree(uint8_t, std::string, uint8_t);

    // @param 0-indexed scale degree (<6)
    // @param mode
    // @param mode root
    // @return semitone value of that degree
    uint8_t scaleDegreeToNote(uint8_t, std::string, uint8_t);

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
        "B4",
        //"C5",
    };

    // Note: Mode names are currently abbreviated due to 
    // Daisy Patch screen size limitations.
    //
    // TODO support exotic modes
    const std::vector<std::string> modes {
        "Major",
        "Dorian",
        "Phyrgi", // Phyrgian
        "Lydian",
        "Mixo",   // Mixolydian
        "Minor",
        "Locri",  // Locrian
    };

    // Maps scale names to their first octave of semitone values
    const std::map<std::string, std::vector<int>> modeToSemitones {
        {"Major",  std::vector<int>{0, 2, 4, 5, 7, 9, 11}},
        {"Dorian", std::vector<int>{0, 2, 3, 5, 7, 9, 10}},
        {"Phyrgi", std::vector<int>{0, 1, 3, 5, 7, 9, 10}},
        {"Lydian", std::vector<int>{0, 2, 4, 6, 7, 9, 11}},
        {"Mixo",   std::vector<int>{0, 2, 4, 5, 7, 9, 10}},
        {"Minor",  std::vector<int>{0, 2, 3, 5, 7, 8, 10}},
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

    const std::vector<std::string> allPPNs {
        // Fractional values will be added when rhythm is fixed
        // "1/16",
        // "1/8",
        // "1/4",
        // "1/2", 
        "1",
        "2",
        // I figured the non-power-of-two values might be cool for polymetric timing.
        // I'll remove/adjust them based on how well they work.
        "3", 
        "4",
        "5",
        "6",
        "7",
        "8",
        "16",
        "32",
        "64",
    };
} // namespace jellybeans

/* Helpers */

// @param note
// @param modeRoot
// @return C scale equivalent of that note in that mode
uint8_t cScaleEquivalent(uint8_t, uint8_t);