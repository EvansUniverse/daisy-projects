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

        // Semitone value of each note in the chord
        std::vector<int> semis;

        // string ∈ theory.h::modes
        std::string mode;

        // Semitone value of the current root note
        int root;

        // Number of notes in the chord
        int length;

        // TODO: Use with caution, there is currently weak protection from 
        // the octave bringing notes out of register. Notes will crudely be
        // quantized into register.
        int octave;

        // ∈ theory.h::voicings
        std::string voicing;
    public:

        // Default is C major triad
        DiatonicChord(); // : DiatonicChord(0, "Major", "Triad") {};

        // @param root    - semitone value, 0-11
        // @param mode    - ∈ theory.h::allModes
        // @param voicing - ∈ theory.h::voicings
        // @param octave  - valid index of theory::allOctaves
        DiatonicChord(int, std::string, std::string, int);

        // Updates the value of string, so that it only needs to be 
        // computed when necessary
        void updateString();

        // Given the chord's properties, formulates the chord.
        // Call this every time a property of the chord is updated.
        void updateChord();

        // @return semis[n], or semis[0] if n is out of bounds
        int getNoteAt(int);

        // @return the number of notes in the chord
        int getLength();

        // @return semitone value of the root
        int getRoot();

        std::string toString();

        // @param semitone value the desired root note
        void setRoot(int);

        // @param string ∈ theory.h::allModes
        void setMode(std::string);

        // @param valid index of theory::allOctaves
        void setOctave(int);

        // @param string ∈ theory.h::voicings
        void setVoicing(std::string);
    };
}