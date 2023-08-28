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

#pragma once

#include <map>
#include <vector>
#include <string>

namespace ev_theory {
    // Max possible notes 1v/oct for a 0-5v range is 60
    const int MAX_NOTE = 59;
    const int MIN_NOTE = 0;

    // In Daisy Seed's DAC, 0=0v and 4095=5v. 4095/5=819, meaning 819 (dac units?)
    // per volt or octave.
    const float DAC_UNITS_PER_OCT = 819.2f;
    const float DAC_UNITS_PER_SEMI = DAC_UNITS_PER_OCT/12.f;

    // In Daisy Seed's DAC, 0=0v and 4095=5v.
    const int16_t MIN_DAC_VALUE = 0;
    const int16_t MAX_DAC_VALUE = 4095;

    // This one's pretty obvious but making it a constant makes the code a bit more
    // readable.
    const uint8_t SEMIS_PER_OCT = 12;

    // @param the semitone value of a note
    // @return if the note exceeds our range, bring it up/down an octave until it fits
    int8_t quantizeNoteToRange(int16_t);

    // NOTE: passing this function an int8_t will cause an overflow issue. Use int16_t.
    //
    // @param semi: semitone value corresponding to theory::allNotes5Oct
    // @return value that can be supplied to Daisy's DAC for CV out 
    //         using the function patch.seed.dac.WriteValue()
    int16_t semitoneToDac(int16_t);

    // Used for adding offsets to pre-existing DAC values.
    //
    // @param cent value
    // @return value that can be supplied to Daisy's DAC for CV out 
    //         using the function patch.seed.dac.WriteValue()
    int16_t centsToDac(int16_t);

    // **IMPORTANT** 
    // Before sending any outputs from semitoneToDac and/or centsToDac to the output, you'll need to run
    // them through this function.
    //
    // Brings out-of-range values within range. Does so by octave increments so that the resulting value
    // still sounds musical.
    //
    // Adds an offset (described below) to the value.
    //
    // !!! HACK !!!
    // Idk why but values of 0 are about a half a semitone out of tune on Daisy Patch. Adding an offset of 25 to
    // nonzero values seems to fix this. I'll leave this hack here until I figure out what he issue is.
    // relevant thread: https://forum.electro-smith.com/t/bug-found-in-daisy-examples-patch-sequencer/2159/3
    //
    // Update: This appears to be a hardware issue, seems like telling Daisy Patch or Field (could be other devices too,
    // these are just the ones I've personally tested) to output a voltage of 0, e.g. `seed.dac.WriteValue(DacHandle::Channel::ONE, 0));`
    // results in it outputting a voltage of about .04 - This results in audio output for the lowest note to be out of tune. 
    // Options to fix this are either disable the lowest note or add an offset of about 25 to all other notes to compensate.
    //
    // TODO: 25 is still a wee bit off, run more precise tests to figure out a more accurate offset.
    //
    // @param a DAC value. Usable range is 0-4095, which outputs 0v-5v
    uint16_t prepareDacValForOutput(int16_t);
    const uint8_t DAC_OFFSET_FOR_NONZERO_VALUES = 25;

    // Brings out-of-range values within range. Does so by octave increments so that the resulting value
    // still sounds musical.
    //
    // Same thing as prepareDacValForOutput() but without adding the offset
    uint16_t quantizeDacValToRange(int16_t);

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
    uint8_t constrain(uint8_t, std::string, uint8_t);

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

    const std::vector<std::string> clockDivs {
        "1/128",
        "1/64",
        "1/32",
        "1/16",
        "1/8",
        "1/4",
        "1/2", 
        "1",
        "2",
        "4",
        "8",
        "16",
        "32",
        "64",
        "128",
    };

    const std::map<std::string, int> clockDivTo256ths {
        {"1/128", 2},
        {"1/64",  4},
        {"1/32",  8},
        {"1/16",  16},
        {"1/8",   32},
        {"1/4",   64},
        {"1/2",   128},
        {"1",     256},
        {"2",     512},
        {"4",     1024},
        {"8",     2048},
        {"16",    4096},
        {"32",    8192},
        {"64",    16384},
        {"128",   32768},
    };

    const std::map<int, std::string> conv256thToClockDiv {
        {2,     "1/128"},
        {4,     "1/64"},
        {8,     "1/32"},
        {16,    "1/16"},
        {32,    "1/8"},
        {64,    "1/4"},
        {128,   "1/2"},
        {256,   "1"},
        {512,   "2"},
        {1024,  "4"},
        {2048,  "8"},
        {4096,  "16"},
        {8192,  "32"},
        {16384, "64"},
        {32768, "128"},
    };
} // namespace ev_theory

/* Helpers */

// @param note
// @param modeRoot
// @return C scale equivalent of that note in that mode
uint8_t cScaleEquivalent(uint8_t, uint8_t);