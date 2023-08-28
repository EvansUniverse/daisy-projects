/* 
 * Copyright (C) 2022 Evan Pernu. Author: Evan Pernu
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
#include "filter.h"
#include "utils.h"

using namespace daisy;
using namespace daisysp;

namespace ev_dsp {
    const float DJF_MIN_FREQ = 70;
    const uint8_t LEN_DJF = 10;
    uint16_t BREAKS_DJF[LEN_DJF] = {100, 200, 300, 400, 500, 600, 700, 800, 900, 1000};
    float TARGS_DJF[LEN_DJF] = {50, 75, 125, 200, 300, 500, 1000, 2000, 4500, 9000};
    
    // Emulates the LPF/HPF combo filter on a DJ mixer (mono)
    class DjFilter {
    public:
        // min/max levels user can input
        static const int16_t DJF_MIN_LEVEL = -1000;
        static const int16_t DJF_MAX_LEVEL = 1000;

        Svf* filter;

        // Emulates the filter knob on a DJ mixer
        // -1000 <= level <= 1000
        //
        // 0: no filter
        // 0 to -1000: low pass filter
        // 0 to 1000: high pass filter
        int16_t level;

        float maxFreq;

        float freq;

        // If true, the filter won't dip into frequency ranges that mute the incoming signal
        // (true by default)
        bool dontMute;

        // If true, fully mute the signal
        bool isMuting; 

        // Don't use
        DjFilter(){};

        // @param patch->AudioSampleRate()
        DjFilter(float samplerate){
            filter = new Svf();
            filter->Init(samplerate);
            filter->SetRes(0.3f);
            filter->SetDrive(0.0f);

            setLevel(0);
            dontMute = true;
            isMuting = false;
        };

        // @param audio in
        // @return audio out
        float processAudio(float f){
            if(isMuting){
                return 0.f;
            }

            if(level > -5 && level < 5){
                // If level is close enough to 0, don't touch the incoming audio
                return f;
            }

            filter->Process(f);
            if(level < 0){
                // LPF
                return filter->Low();
            } else {
                // HPF
                return filter->High();
            }
        }

        // @param audio out L
        // @param audio out R
        // @param audio in L
        // @param audio in R
        virtual void processAudio(float &outL, float &outR, float inL, float inR){
            outL = processAudio(inL);
            outR = processAudio(inR);
        };

        // Sets filter frequency based on "level" aka knob position
        //
        // @param a value between -1000 and 1000
        void setLevel(int16_t i){
            i = constrain(DJF_MIN_LEVEL, DJF_MAX_LEVEL, i);
            level = i;

            // Set the filter frequency relative to the "knob" position
            if (i < 0){
                // For LPF ( which goes CCW not CW), Invert the equation so that the 
                // filter goes lower as knob goes CCW
                i = DJF_MAX_LEVEL + i;
            }

            freq = piecewiseLinear(i, BREAKS_DJF, TARGS_DJF, LEN_DJF);
            
            if(level < 0){
              // Ensure that the LPF doesn't dip into inaudibly low hz ranges
              freq += DJF_MIN_FREQ;
            }

            if (!dontMute) {    
                // Ensure that values sufficiently close to muting the signal fully mute the signal
                isMuting = level > DJF_MAX_LEVEL * 0.95 || level < DJF_MIN_LEVEL * 0.95;
            }
            
            // Frequencies <1 cause SVF to crash
            filter->SetFreq(max(freq, MIN_SVF_FREQ));
        };

        // @param a value between 0 and 1000
        int16_t getLevel(){
            return level;
        };
    };
} // namespace ev_dsp