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

    // bool isDiatonic(int note, std::string mode){
    //     std::vector<int> v = modeToSemitones.at(mode);
    //     return std::find(v.begin(), v.end(), note % 12) != v.end();
    // };

    float semitoneToDac(int semi) {
        if (semi == 0) {
            return 0.f;
        }

        // !!! HACK !!!
        // Idk why, but values of 0 are about a half a semitone out of tune. Adding 25 here and keeping 0
        // values as 0 seems to fix this. I'll leave this hack here until I figure out what he issue is.
        // relevant thread: https://forum.electro-smith.com/t/bug-found-in-daisy-examples-patch-sequencer/2159/3
        //
        // TODO: 25 is still a wee bit off, run more precise tests to figure out a more accurate offset.
        return round((semi / 12.f) * 819.2f) + 25.f; 
    }

    std::string floatToString(float f, uint8_t decimalPlaces){
        for (uint8_t i = 0; i < decimalPlaces; i++) {
            f = f * 10.;
        }

        // TODO add decimal point
        return std::to_string(static_cast<int>(round(f)));
    }
}