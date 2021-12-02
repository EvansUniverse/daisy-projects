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

namespace mu {

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

    const std::vector<std::string> allScales {
        "Major",
        "Minor",
        "Dorian",
        "Phyrgi", // Phyrgian
        "Lydian",
        "Mixo",   // Mixolydian
        "Locri",  // Locrian
    };

    // Maps scale names to their first octave of semitone values
    const std::map<std::string, std::vector<int>> scalesToSemitones {
        {"Major",  std::vector<int>{0, 2, 4, 5, 7, 9, 11}},
        {"Minor",  std::vector<int>{0, 2, 3, 5, 7, 8, 10}},
        {"Dorian", std::vector<int>{0, 2, 3, 5, 7, 9, 10}},
        {"Phyrgi", std::vector<int>{0, 1, 3, 5, 7, 9, 10}},
        {"Lydian", std::vector<int>{0, 2, 4, 6, 7, 9, 11}},
        {"Mixo",   std::vector<int>{0, 2, 4, 5, 7, 9, 10}},
        {"Locri",  std::vector<int>{0, 1, 3, 5, 6, 8, 10}},
    };

    const std::vector<std::string> allVoicings {
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
        // disabled til its bug is fixed
        //"Kenny B.",  // Kenny Barron chord 
        "Power",
        "Power+",
        "Shell 1",
        "Shell 2"
    };

    // Maps voicings to the scale degrees they contain
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
        //{"Kenny B.", std::vector<int>{1, 5, 9, 10, 14, 18}}, 
        {"Power",    std::vector<int>{1, 5}},
        {"Power+",   std::vector<int>{1, 5, 8}},
        {"Shell 1",  std::vector<int>{1, 7, 10}},
        {"Shell 2",  std::vector<int>{1, 10, 14}},
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