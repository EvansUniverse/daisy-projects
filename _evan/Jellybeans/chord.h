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

#include "theory.h"

namespace jellybeans {

    // Notes are represented as ints, as they correspond to mu::allNotes5Oct
    // For performance reasons, all functions assume valid input
    class DiatonicChord {
    private:
        // String representation of the chord
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

        int length;

        // Must be an element of voicings
        std::string voicing;
    public:

        // Default is C major triad
        DiatonicChord(); // : DiatonicChord(1, 0, "Major", "Triad") {};

        // @param chord root - scale degree  int from 0-7
        // @param mode root - the note value, int from 0-12
        // @param mode - an element of mu::allModes
        // @param voicing - an element of mu::voicings
        DiatonicChord(int, int, std::string, std::string);

        // Updates the value of string, so that it only needs to be 
        // computed when necessary
        void updateString();

        // @param the index of a note
        // Sets the root of the chord to the given note. If it's a non-diatonic note, 
        // transpose it up a semitone to become diatonic.
        void setRootByNote(int);

        // @param the scale degree of the desired root note, int from 0-7
        void setRoot(int);

        // @param the note value of the desired mode root, int from 0-12
        void setModeRoot(int);

        // @param an element of mu::allModes
        void setMode(std::string);

        // @param an element of mu::voicings
        void setVoicing(std::string);

        // Transposes all notes in the chord by the given number of semitones.
        // If a note would fall out of register, do nothing.
        void transpose(int);

        // Given the chord's properties, formulates the chord.
        // Call this every time a property of the chord is updated.
        void updateChord();

        // @return semis[n], or semis[0] if n is out of bounds
        int getNoteAt(int);

        // @return the number of notes in the chord
        int getLength();

        std::string toString();
    };
}