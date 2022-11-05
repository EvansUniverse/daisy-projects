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

    class StereoReverb: public AudioEffect {
    public:
        float hpf;

        ReverbSc* sc;

        Svf* filterL;
        Svf* filterR;

        // Don't use
        StereoReverb(){};

        StereoReverb(float sampleRate, ReverbSc* r){
            sc = r;
            level = AFX_MAX_LEVEL;

            sc->Init(sampleRate);
            sc->SetFeedback(0.85f);
            sc->SetLpFreq(18000.0f);

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

            sc->Process(inl, inr, &outl, &outr);

            filterR->Process(outr);
            filterL->Process(outl);

            outr = filterR->High();
            outl = filterL->High();
        };

        void setHpf(float f){
            hpf = f;
            filterL->SetFreq(f);
            filterR->SetFreq(f);
        };
    };
} // namespace ev_dsp














