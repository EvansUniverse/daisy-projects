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

#include "daisysp.h"
#include "daisy_patch.h"
#include "utils.h"

using namespace daisy;
using namespace daisysp;

namespace ev_dsp {
    static const int8_t MAX_PAN = 50;
    static const int8_t MIN_PAN = -50;
    static const int8_t DEFAULT_PAN = 0;

    // Stereo panning
    //
    // Note that when panned center, the output signal will be attenuated by -3db
    class Pan: public AudioEffect  {
    public:
        // Cache panning coefficients to save on comnputation
        float panCoefficientL;
        float panCoefficientR;

        Pan(){
            setPan(DEFAULT_PAN);
        };

        // Processes audio from mono to stereo
        //
        // @param audio out L
        // @param audio out R
        // @param audio in L
        // @param audio in R
        virtual void processAudio(float &outL, float &outR, float in){
            outL = in * panCoefficientL;
            outR = in * panCoefficientR;
        };

        // @param a value between -50 (full left) and 50 (full right)
        void setPan(int8_t i){
    
            // These coefficients are determined using the "constant power panning" algorithm:
            // http://www.cs.cmu.edu/~music/icm-online/readings/panlaws/index.html
            //
            // 't' is the panning angle in radians, from 0 (far left), to pi/4 (middle), to pi/2 (far right)
            // 0 < t < pi/2
            // Left coefficient = cos(t)
            // Right coefficient = sin(t)
            //
            // This results in panning where the net output is always 0. When panned dead center, the signal
            // is attenuated by about -3db.
            float angle = (i + 50) * 0.01 * HALFPI_F;
            panCoefficientR = cos(angle);
            panCoefficientL = sin(angle);
        }
    };
} // namespace ev_dsp