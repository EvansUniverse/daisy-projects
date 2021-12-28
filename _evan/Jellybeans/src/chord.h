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

    // Stores a mode information and uses it to generate chords of 
    // the specified degree and voicing.
    class DiatonicChord {
    private:
        // String representation of the chord
        std::string string;

        // Semitone value of each note in the chord
        std::vector<uint8_t> semis;

        // Semitone value of the current mode's root note
        uint8_t modeRoot;

        // string ∈ theory.h::modes
        std::string mode;

        // Scale degree the current chord's root note
        uint8_t degree;

        // Number of notes in the chord
        uint8_t length;

        // TODO: Use with caution, there is currently weak protection from 
        // the octave bringing notes out of register. Notes will crudely be
        // quantized into register, which may not produce musical results.
        uint8_t octave;

        // ∈ theory.h::voicings
        std::string voicing;

        // 0 = uninverted
        uint8_t inversion;

        // TODO in the future, may need to set it dynamically for exotic scales
        // e.g. static_cast<uint8_t>(modeToSemitones.at(mode).size());
        const uint8_t scaleLen = 7;

    public:
        // Default is C major triad
        DiatonicChord();

        // @param modeRoot - semitone value, 0-11
        // @param mode     - ∈ theory.h::allModes
        // @param degree   - scale degree of mode, 0-6
        // @param voicing  - ∈ theory.h::voicings
        // @param octave   - valid index of theory::allOctaves
        DiatonicChord(uint8_t, std::string, uint8_t, std::string, uint8_t);

        // Updates the value of string, so that it only needs to be 
        // computed when necessary
        void updateString();

        // Given the chord's properties, formulates the chord.
        // Call this every time a property of the chord is updated.
        void updateChord();

        // @return semis[n], or semis[0] if n is out of bounds
        uint8_t getNoteAt(uint8_t);

        uint8_t getLength();

        uint8_t getDegree();

        uint8_t getModeRoot();

        std::string toString();

        // @param desired scale degree of mode, 0-6
        void setDegree(uint8_t);

        // @param semitone value of desired scale degree
        void setDegreeByNote(uint8_t);
    
        // @param modeRoot - semitone value, 0-11
        void setModeRoot(uint8_t);

        // @param string ∈ theory.h::allModes
        void setMode(std::string);

        // @param valid index of theory::allOctaves
        void setOctave(uint8_t);

        // @param string ∈ theory.h::voicings
        void setVoicing(std::string);

        // @param valid index of theory.h::allInversions
        void setInversion(uint8_t);
    };
}