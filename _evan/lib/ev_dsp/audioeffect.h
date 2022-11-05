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
#include "utils.h"

using namespace daisy;
using namespace daisysp;

namespace ev_dsp {
    static const int16_t AFX_MIN_LEVEL = 0;
    static const int16_t AFX_MAX_LEVEL = 1000;

    // 1/AFX_MAX_LEVEL 
    // Multiply a level value by this to get a percentage float
    static const float AFX_RATIO = .001; 
    
    // Parent class for audio effects, will allow some interchangeability between them  
    class AudioEffect {
    public:
        // Percentage volume level
        // 0 <= level <= 1.0
        float level;

        // Percentage wet
        // 0 <= wet <= 1.0
        float wet;

        static constexpr float ratio = .001;  // TODO rm
        float samplerate;

        AudioEffect(){
            level = AFX_MIN_LEVEL;
        };

        // @param patch->AudioSampleRate()
        AudioEffect(float samplerate){
            level = AFX_MIN_LEVEL;
        };

        // Processes audio in mono
        //
        // @param audio in
        // @return audio out
        virtual float processAudio(float f){
            return f * level;
        };

        // Processes audio in stereo
        //
        // @param audio out L
        // @param audio out R
        // @param audio in L
        // @param audio in R
        virtual void processAudio(float &outl, float &outr, float inl, float inr){};

        // Processes audio from mono to stereo
        //
        // @param audio out L
        // @param audio out R
        // @param audio in L
        // @param audio in R
        virtual void processAudio(float &outl, float &outr, float in){};

        // @param a value between AFX_MIN_LEVEL and AFX_MAX_LEVEL
        void setLevel(int16_t i){
            level = quantize(AFX_MIN_LEVEL, AFX_MAX_LEVEL, i) * AFX_RATIO;
        };

        // 0 <= i <= 1000
        void setWet(uint16_t i){
            wet = quantize(AFX_MIN_LEVEL, AFX_MAX_LEVEL, i) * AFX_RATIO;
        }

        float getWetAudio(float f){
            return AFX_RATIO * level * f;
        };

        float getDryAudio(float f){
            return AFX_RATIO * (AFX_MAX_LEVEL - level) * f;
        };
    };

} // namespace ev_dsp