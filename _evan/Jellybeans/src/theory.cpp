/* 
 * Copyright (C) 2021 Evan Pernu - All Rights Reserved
 * You may use, distribute and modify this code under the
 * terms of the GNU AGPLv3 license.
 * 
 * You should have received a copy of the GNU AGPLv3 license with
 * this file (LICENSE.md). If not, please write to: evanpernu@gmail.com, 
 * or visit: https://www.gnu.org/licenses/agpl-3.0.en.html
 */



#include <algorithm>
#include <string> 
#include <vector>

#include "theory.h"

/* private helpers */
uint8_t cScaleEquivalent(uint8_t note, uint8_t modeRoot){
    if (note < modeRoot){
        note--;
    }
    return (note - modeRoot) % 12;
}

namespace jellybeans {
    int quantizeNoteToRange(int i) {
        while (i > MAX_NOTE){
            i -= 12;
        }
        while (i < MIN_NOTE){
            i += 12;
        }
        return i;
    }

    uint8_t quantize(uint8_t note, std::string mode, uint8_t modeRoot){
        int base = cScaleEquivalent(note, modeRoot);

        // See if that note is in the given mode
        const std::vector<int> v = modeToSemitones.at(mode);
        if (std::find(v.begin(), v.end(), base) == v.end()){
            // NOTE: This relies on the property that every non-diatonic note is 1 semitone
            // away from a diatonic note. I'll need to rework this if I implement exotic 
            // scales with 3+ semitone distances.
            if (note == 0){
                note++;
            } else {
                note--;
            }
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
        return (base + modeRoot % 12); 
       // return 1;
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

    // TODO distinguish between lowercase and uppercase depending on quality
    std::string intToNumeral(uint8_t i){
        switch (i) {
            case 1:
                return "I  ";
            case 2:
                return "II ";
            case 3:
                return "III";
            case 4:
                return "IV ";
            case 5:
                return "V  ";
            case 6:
                return "VI ";
            case 7:
                return "VII";
        }
        return "N/A";
    }
}