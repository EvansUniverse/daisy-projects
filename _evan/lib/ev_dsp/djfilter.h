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

using namespace daisy;
using namespace daisysp;

namespace ev_dsp {
    
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

        // Freq values < 1 cause svf to crash
        // Realistically, though, anything under 40 doesn't make an audible change
        static constexpr float DJF_MIN_FREQ = 40;

        float maxFreq;

        // Used for exponential knob behavior
        float coefficient;

        // TODO used for debugging, might remove later as it's redundant
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
            level = 0;

            // svf's max frequency is samplerate/3 (which is 48,000/3=16,000 on patch)
            // everything from about 7000 and up doesn't make much of a difference so
            // to keep things musical, we'll reduce the max freq 
            maxFreq = (samplerate / 3) * .6;

            // freq = (coefficient * level)^2
            // --> maxFreq = (coefficient * DJF_MAX_LEVEL)^2
            // --> coefficient = sqrt(maxFreq)/DJF_MAX_LEVEL
            coefficient = std::sqrt(maxFreq) / DJF_MAX_LEVEL;

            filter = new Svf();
            filter->Init(samplerate);
            filter->SetRes(0.3f); // TODO play with this number
            filter->SetDrive(0.0f);
            filter->SetFreq(maxFreq);

            dontMute = true;
            isMuting = false;
        };

        // @param audio in
        // @return audio out
        float processAudio(float f){
            if(isMuting){
                return 0.f;
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

        // Sets filter frequency based on "level" aka knob position
        //
        // @param a value between -1000 and 1000
        void setLevel(int16_t i){
            if(i > DJF_MAX_LEVEL){
                level = DJF_MAX_LEVEL;
            } else if (i < DJF_MIN_LEVEL) {
                level = DJF_MIN_LEVEL;
            } else {
                level = i;
            }

            // Set the filter frequency relative to the "knob" position
            if (i < 0){
                // For LPF ( which goes CCW not CW), Invert the equation so that the 
                // filter goes lower as knob goes CCW
                i = DJF_MAX_LEVEL + i;
            }

            // freq = (lpfCoefficient * level)^2
            freq = (coefficient * i) * (coefficient * i);
            
            if(dontMute){
                // Add DJF_MIN_FREQ ensures that the filter doesn't dip into unusably low hz ranges
                freq += DJF_MIN_FREQ;
            } else {
                // Ensure that values sufficiently close to muting the signal fully mute the signal
                isMuting = level > DJF_MAX_LEVEL * 0.95 || level < DJF_MIN_LEVEL * 0.95;
            }
            
            filter->SetFreq(freq);
        };

        // @param a value between 0 and 1000
        int16_t getLevel(){
            return level;
        };
    };

    // Basically just a wrapper that groups 2 dj filters
    class StereoDjFilter {
    public: 
        DjFilter* djfL;
        DjFilter* djfR;

        // Don't use
        StereoDjFilter(){};

        // @param patch->AudioSampleRate()
        StereoDjFilter(float samplerate){
            djfL = new DjFilter(samplerate);
            djfR = new DjFilter(samplerate);
        };

        // @param audio out L
        // @param audio out R
        // @param audio in L
        // @param audio in R
        virtual void processAudio(float &outL, float &outR, float inL, float inR){
            outL = djfL->processAudio(inL);
            outR = djfR->processAudio(inR);
        };

        // Sets filter frequency based on "level" aka knob position
        //
        // @param a value between -1000 and 1000
        void setLevel(int16_t i){
            djfL->setLevel(i);
            djfR->setLevel(i);
        };
    };
} // namespace ev_dsp