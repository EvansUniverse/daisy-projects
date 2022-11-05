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

namespace ev_dsp {
    const static int16_t COMP_MIN_THRESH = -40;
    const static int16_t COMP_MAX_THRESH = 0;

    // AudioEffect wrapper for 2x DaisySP::Compressor
    class StereoCompressor: public AudioEffect {
    public:
        Compressor* compL;
        Compressor* compR;

        // Don't use
        StereoCompressor(){};

        StereoCompressor(float samplerate){
            compL = new Compressor();
            compL->Init(samplerate);
            compL->AutoMakeup(true);

            compR = new Compressor();
            compR->Init(samplerate);
            compR->AutoMakeup(true);
        };

        // @param audio in
        // @return audio out
        void processAudio(float &outl, float &outr, float inl, float inr){
            outl = compL->Process(inl);
            outr = compR->Process(inr);
        };

        // @param -80 <= i <= 0
        void setThresh(float i){
            compL->SetThreshold(i);
            compR->SetThreshold(i);
        };

        // @param 1.0 <= i <= 40
        void setRatio(float i){
            compL->SetRatio(i);
            compR->SetRatio(i);
        };

        // @param 0.001 <= i <= 10
        void setAttack(float i){
            compL->SetAttack(i);
            compR->SetAttack(i);
        };

        // @param 0.001 <= i <= 10
        void setRelease(float i){
            compL->SetRelease(i);
            compR->SetRelease(i);
        };
    };


    // AudioEffect wrapper for 2x DaisySP::Limiter
    class StereoLimiter: public AudioEffect {
    public:
        Limiter* limL;
        Limiter* limR;
        float preGain;

        StereoLimiter(){
            limL = new Limiter();
            limL->Init();

            limR = new Limiter();
            limR->Init();

            preGain = 1.f;
        };

        // @param audio in
        // @return audio out
        void processAudio(float &outl, float &outr, float inl, float inr){
            limL->ProcessBlock(&inl, static_cast<size_t>(1), preGain);
            limR->ProcessBlock(&inr, static_cast<size_t>(1), preGain);

            outr = inr;
            outl = inl;
        };

        // @param 0 <= i <= 1000
        void setGain(int16_t i){
            preGain = .001 * i * 10;
        };
    };
} // namespace ev_dsp