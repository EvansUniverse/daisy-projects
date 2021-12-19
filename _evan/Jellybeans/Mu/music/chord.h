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

#include "theory.h"

namespace mu {

    // Notes are represented as ints, as they correspond to mu::allNotes5Oct
    // For performance reasons, all functions assume valid input
    class DiatonicChord {
    public:
        // String representation of the chord
        // e.g. "A minor"
        std::string string;

        // The diatonic distance, in relation to the root, of 
        // each note in the chord (ascending order)
        std::vector<int> notes;

        // Semitone value of each note in the chord
        std::vector<int> semis;

        // Must be an element of mu::modes (theory.h)
        std::string mode;

        // Root note of current mode
        int modeRoot;

        // Scale degree of the current root note
        int root;

        // the number of notes in the chord
        int length;

        // Must be an element of in mu::voicings (chord.h)
        std::string voicing;

        DiatonicChord() : DiatonicChord(1, 0, "Major", "Triad") {};

        // @param chord root - scale degree  int from 0-7
        // @param mode root - the note value, int from 0-12
        // @param mode - an element of mu::allModes
        // @param voicing - an element of mu::voicings
        DiatonicChord(int, int, std::string, std::string);

        // Updates the value of string, so that it only needs to be 
        // computed when necessary
        void UpdateString();

        // @param the index of a note
        // Sets the root of the chord to the given note. If it's a non-diatonic note, 
        // transpose it up a semitone to become diatonic.
        void SetRootByNote(int);

        // @param the scale degree of the desired root note, int from 0-7
        void SetRoot(int);

        // @param the note value of the desired mode root, int from 0-12
        void SetModeRoot(int);

        // @param an element of mu::allModes
        void SetMode(std::string);

        // @param an element of mu::voicings
        void SetVoicing(std::string);

        // Transposes all notes in the chord by the given number of semitones.
        // If a note would fall out of register, do nothing.
        void Transpose(int);

        // Given the chord's properties, formulates the chord.
        // Call this every time a property of the chord is updated.
        void UpdateChord();

        // @return semis[n], or semis[0] n is out of bounds
        int GetNoteAt(int);
    };

    // All possible chord voicings
    const std::vector<std::string> voicings {
        "Triad",
        "Triad+",
        "7th",
        "7th+"
        "9th",
        "11th",
        "13th",
        "6th",
        "Sus2",
        "Sus4",
        // disabled til its bug is fixed
        //"Kenny B.",  // Kenny Barron chord 
        "Power",
        "Shell 1",
        "Shell 2"
    };

    // Maps voicings to the scale degrees they contain (in relation to the chord's root note)
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
        //{"Kenny B.", std::vector<int>{1, 5, 9, 10, 14, 18}}, 
        {"Power",    std::vector<int>{1, 5}},
        {"Power+",   std::vector<int>{1, 5, 8}},
        {"Shell 1",  std::vector<int>{1, 7, 10}},
        {"Shell 2",  std::vector<int>{1, 10, 14}},
    };
}