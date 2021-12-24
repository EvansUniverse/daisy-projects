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

//#include <cmath>
#include <string>

namespace jellybeans {
    // Converts a semitone value to data that can be supplied to Daisy Seed's DAC
    // for CV, using the function patch.seed.dac.WriteValue()
    //
    // In Daisy Seed's DAC, 0=0v and 4095=5v. 4095/5=819, meaning 819 (dac units?)
    // per volt or octave.
    //
    // @param semi: an integer between 0-60 representing the number of semitones from low C
    float semitoneToDac(int semi) {
        if (semi == 0) { // DEBUG
            return 0.f;
        }

        // TODO: Idk why, but values of 0 are about a half a semitone out of tune. Adding 25 here seems to fix this.
        // I'll leave this hack here until someone on the forums can help me figure out a better equation.
        //
        // TODO: 25 is still a wee bit off, use the tuner to figure out a more precise offset.
        return round((semi / 12.f) * 819.2f) + 25.f; 
    }

    // Converts a float from a libdaisy.Param's .Process() method to an int value, rounded down
    int paramValueToInt(float f) {
        return static_cast<int>(f);
    }
}