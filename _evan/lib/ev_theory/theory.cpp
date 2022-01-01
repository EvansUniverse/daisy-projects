/* 
 * Copyright (C) 2021 Evan Pernu. Author: Evan Pernu
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

#include <algorithm>
#include <string> 
#include <vector>

#include "theory.h"

namespace ev_theory {
    uint8_t quantizeNoteToRange(int8_t i) {
        while (true) {
            if (i > MAX_NOTE){
                i -= 12;
            } else if (i < MIN_NOTE){
                i += 12;
            } else {
                break;
            }
        }
        return i;
    }

    uint8_t quantize(uint8_t note, std::string mode, uint8_t modeRoot){
        int base = cScaleEquivalent(note, modeRoot);

        // See if that note is in the given mode
        const std::vector<int> v = modeToSemitones.at(mode);
        //if (std::find(v.begin(), v.end(), base) == v.end()){

        for (uint8_t i = 0; i < v.size(); i++){
            if(v[i] == base) {
                // If the note is diatonic, return it immediately
                return note;
            }
        }

        // If the note isn't diatonic, quantize it. Move 1 semitone down if possible,
        // otherwise 1 up.
        //
        // NOTE: This relies on the property that every non-diatonic note is 1 semitone
        // away from a diatonic note. I'll need to rework this if I implement exotic 
        // scales with 3+ semitone distances.
        if (note == MIN_NOTE){
            note++;
        } else {
            note--;
        }
        
        return note;
    };

    uint8_t noteToScaleDegree(uint8_t note, std::string mode, uint8_t modeRoot){
        note = quantize(note, mode, modeRoot) % 12;
        int base = cScaleEquivalent(note, modeRoot);
        
        const std::vector<int> v = modeToSemitones.at(mode);
        for(uint8_t i = 0; i < v.size(); i++){
            if (v[i] == base){
                return i;
            } 
        }

        // If you've reached this line, there's a bug
        return 1; 
    }

    uint8_t scaleDegreeToNote(uint8_t degree, std::string mode, uint8_t modeRoot){
        const std::vector<int>* v = &(modeToSemitones.at(mode));
        int base = v->at(degree);
        return (base + modeRoot) % 12; 
    }

    float semitoneToDac(int semi) {
        // In Daisy Seed's DAC, 0=0v and 4095=5v. 4095/5=819, meaning 819 (dac units?)
        // per volt or octave.
        const float DAC_UNITS_PER_OCT = 819.2f;

        // !!! HACK !!!
        // Idk why but values of 0 are about a half a semitone out of tune. Adding an offset of 25 to nonzero
        // values seems to fix this. I'll leave this hack here until I figure out what he issue is.
        // relevant thread: https://forum.electro-smith.com/t/bug-found-in-daisy-examples-patch-sequencer/2159/3
        //
        // TODO: 25 is still a wee bit off, run more precise tests to figure out a more accurate offset.
        if (semi == 0) {
            return 0.f;
        }
        const float OFFSET = 25.f;
    
        return round((semi / 12.f) * DAC_UNITS_PER_OCT) + OFFSET; 
    }

    float centsToDac(int cents) {
        // In Daisy Seed's DAC, 0=0v and 4095=5v. 4095/5=819, meaning 819 (dac units?)
        // per volt or octave.
        const float DAC_UNITS_PER_SEMI = 819.2f/12.f;
    
        return round(DAC_UNITS_PER_SEMI * cents * .01f); 
    }

    // FIXME doesn't render 0
    std::string floatToString(float f, uint8_t dec){
        if (f == 0.) {
            return "0";
        }

        for (uint8_t i = 0; i < dec; i++) {
            f = f * 10.;
        }

        std::string ret = std::to_string(static_cast<int>(round(f)));
        ret.insert(ret.end()-dec, 1, '.');
        return ret;
    }
} // namespace ev_theory

/* Helpers */

uint8_t cScaleEquivalent(uint8_t note, uint8_t modeRoot){
    return (note + 12 - modeRoot) % 12;
}