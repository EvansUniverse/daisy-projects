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

// Number of readings we average over (quasi-sliding window)
#define WINDOW_SIZE 100

// Number of samples we refresh after
#define REFRESH_RATE 48

using namespace daisy;
using namespace daisysp;

namespace ev_dsp {
    // Crudely approximates the volume of an incoming audio signal.
    // Good enough for displaying level for monitoring, not for DSP.
    class VolumeMeter  {
    public:
        float level;
        float max;
        uint16_t i;

        VolumeMeter(){    
            i = 0;
            max = 0;
            level = 0;
        };

        // Call this every sample
        void processAudio(float in){
            if (in > max){ 
                max = in;
            }

            i++;
            if(i > REFRESH_RATE){
                refresh();
                max = 0;
                i = 0;
            }
        };

        float read(){
            return level;
        }

    private:
        void refresh(){
            float l;
            if(max == 0){
                l = 0;
            } else {                
                // approximate the percieved volume loosely based off on decibel value.
                // decibels = 20*log10(linear)
                l = 1.f - fabsf(log10(max)); // TODO abs may be redundant`
            }

            level = (l + level * (WINDOW_SIZE-1)) / WINDOW_SIZE;
        }
    };
} // namespace ev_dsp