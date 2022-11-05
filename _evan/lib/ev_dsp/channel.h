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
#include "djfilter.h"
#include "distortion.h"
#include "pan.h"

using namespace daisy;
using namespace daisysp;

namespace ev_dsp {
    static const uint16_t DEFAULT_SEND_LEVEL = 0;

    // Number of outputs, each with their own configurable level. This allows for an
    // adjustable number of sends.
    static const int8_t NUM_OUTS = 5;

    // Emulates a single channel strip. Mono in, stereo out.
    //
    // This was designed specifically for Omakase but maybe someone else
    // will find it useful.
    //
    // Signal path:
    // Gain -> Distortion -> DJ Filter -> pan -> levels & outputs 1...NUM_OUTS
    class Channel {
    public:
        // Output level, multiplies the input signal at the end of the signal chain.
        // Separate value for each output
        // 0.0 <= levels[x] <= 1.0
        float levels[NUM_OUTS];

        // Pre-fx gain level
        // 0.0 <= gain <= 2.0
        float gain;

        // Stereo outputs
        float outsL[NUM_OUTS];
        float outsR[NUM_OUTS];

        DjFilter* djf;
        Distortion* dist;
        Pan* pan;

        // Don't use
        Channel(){};

        Channel(float samplerate){
            for(uint8_t i = 0; i < NUM_OUTS; i++){
                levels[i] = 0.f;
            }

            setGain(AFX_MAX_LEVEL);
            pan = new Pan();
            djf = new DjFilter(samplerate);
            dist = new Distortion(samplerate);
        };

        // Processes audio. Intended to be called in the parent's AudioCallback()
        void processAudio(float in){
            in = in * gain;
            in = dist->processAudio(in);
            in = djf->processAudio(in);
            float outL, outR;
            pan->processAudio(outL, outR, in);

            for(uint8_t i = 0; i < NUM_OUTS; i++){
                outsL[i] = outL * levels[i];
                outsR[i] = outR * levels[i];
            }
        }

        // @param 0 <= i <= 1000
        void setGain(int16_t i){
            gain = i * .001 + 1.f;
        }

        // @param 0 <= i <= 1000
        void setLevel(uint8_t ch, uint16_t i){
            levels[ch] = quantize(AFX_MIN_LEVEL, AFX_MAX_LEVEL, i) * AFX_RATIO;
        }
    };
} // namespace ev_dsp