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
#include "audioeffect.h"

using namespace daisy;
using namespace daisysp;

#define INT_MAX_DELAY ((size_t)(10.0f * 48000.0f))

namespace ev_dsp {

    class StereoDelay: public AudioEffect {
    public:
        static const size_t MAX_DELAY = INT_MAX_DELAY; // TODO const

        float feedback;
        float time;
        float smoothedTime;

        float hpf;

        DelayLine<float, INT_MAX_DELAY>* delayR;
        DelayLine<float, INT_MAX_DELAY>* delayL;

        Svf* filterL;
        Svf* filterR;

        // Don't use
        StereoDelay(){};

        StereoDelay(float sampleRate, DelayLine<float, INT_MAX_DELAY>* l, DelayLine<float, INT_MAX_DELAY>* r){
            delayL = l;
            delayR = r;

            samplerate = sampleRate;

            delayL->SetDelay(samplerate * 0.8f); // half second delay
            delayR->SetDelay(samplerate * 0.8f); // half second delay
            feedback = 0.3;
            level = AFX_MAX_LEVEL;

            filterL = new Svf();
            filterL->Init(sampleRate);
            filterL->SetRes(0.0f); // TODO play with this number
            filterL->SetDrive(0.0f);

            filterR = new Svf();
            filterR->Init(sampleRate);
            filterR->SetRes(0.0f); // TODO play with this number
            filterR->SetDrive(0.0f);

            setHpf(0.f);
        };

        // @param audio in
        // @return audio out
        void processAudio(float &outl, float &outr, float inl, float inr){
            fonepole(smoothedTime, time, 0.0005f);
            delayL->SetDelay(smoothedTime);
            delayR->SetDelay(smoothedTime);

            float delsigL = delayL->Read();
            float delsigR = delayR->Read();

            delayL->Write(inl + (delsigL * feedback));
            delayR->Write(inr + (delsigR * feedback));

            filterR->Process(delsigR);
            filterL->Process(delsigL);

            outr = filterR->High();
            outl = filterL->High();
        };

        void setHpf(float f){
            hpf = f;
            filterL->SetFreq(f);
            filterR->SetFreq(f);
        };

        // @param 0 <= i <= 1000
        void setTime(int16_t i){
            float k = i * .001f;
            time = (0.001f + (k * k) * 5.0f) * samplerate;
        };

        // @param 0 <= i <= 1000
        void setFeedback(int16_t i){
            feedback = i * .001f;
        };
    };
} // namespace ev_dsp














