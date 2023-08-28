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

#define BREAKS_TAN_LEN 5
#define BREAKS_CLIP_LEN 3
float BREAKS_TAN[5] = {0.2, 0.4, 0.6, 0.8, 0.9};
float MULTS_TAN[5] = {1.1, 1.3, 1.6, 2, 3};
float BREAKS_CLIP[3] = {0.5, 0.7, 0.9};
float MULTS_CLIP[3] = {1.1, 1.3, 2};

namespace ev_dsp {

    const std::vector<std::string> distAlgos {
        "Bypass",
        "Tanh",
        "Atan",
        "HardClip",
        "SoftClip",
        "Saturate",
        "Bitcrush",
    };

    enum DistType{
        bypass,
        tan_h,
        a_tan,
        hard_clip,
        soft_clip,
        saturate,
        bitcrush
    };

    // Audio distortion/saturation with multiple algorithms
    class Distortion: public AudioEffect {
    public:
        Fold* fold;

        DistType type;
        uint16_t drive;
        uint16_t tone;
        float vol;
        float samplerate;

        float crush_rate_;
        int   bit_depth_;

        // Don't use
        Distortion(){};

        Distortion(float sr){
            samplerate = sr;

            setLevel(AFX_MAX_LEVEL);
            setDrive(AFX_MIN_LEVEL);
            setType(tan_h);

            bit_depth_   = 8;
            crush_rate_  = 10000;
            fold = new Fold();
            fold->Init();
        }

        // Note that all distortion algorithms are stateless, so to process a stereo signal
        // just call this function on both channels.
        //
        // @param audio in
        // @return audio out
        float processAudio(float f){
            if(drive < 5){
                // Sufficiently low knob values should bypass
                return f;
            }

            // In most places, drive is calculated then signal is multiplied by drive before being fed into distortion function
            // At this point, 0 <= d <= 1
            float d = drive * .01f;

            // Keep f in case we need its original value later
            float r = f;

            switch(type){
                case bypass: 
                {
                    break;
                }
                case tan_h:
                    d = 1.f + psuedoExponential(d, BREAKS_TAN, MULTS_TAN, BREAKS_TAN_LEN);
                    r = tanh(r * d);
                    break;
                case a_tan:
                    d = 1.f + psuedoExponential(d, BREAKS_TAN, MULTS_TAN, BREAKS_TAN_LEN);
                    r = HALFPI_F * atan(r * d);
                    break;
                case hard_clip:
                    d = 1.f + psuedoExponential(d, BREAKS_CLIP, MULTS_CLIP, BREAKS_CLIP_LEN);
                    r = r * d;
                    if(r > 1.f){
                        r = 1.f;
                    }

                    // Hard clip gets particularly loud, tame the signal a bit as the drive increases
                    r = r * (1.f - 0.4 * (drive/1000.f));
                    break;
                case soft_clip:
                    d = 1.0f + psuedoExponential(d, BREAKS_CLIP, MULTS_CLIP, BREAKS_CLIP_LEN);
                    r = SoftClip(r * d);
                    break;
                case saturate:
                    d = 1.0f + psuedoExponential(d, BREAKS_CLIP, MULTS_CLIP, BREAKS_CLIP_LEN);
                    r = soft_saturate(r * d, 1.0);
                    break;
                case bitcrush:
                    // Most of this code was stolen from DaisySp but calling pow()
                    // added 5% to the binary size so I've modified it
                    float foldamt = samplerate / crush_rate_;
                    float out;
                    out = r * 65536.0f;
                    out += 32768;
                    out *= (bit_depth_ / 65536.0f);
                    out = floor(out);
                    out *= (65536.0f / bit_depth_) - 32768;
                    fold->SetIncrement(foldamt);
                    out = fold->Process(out);
                    out /= 65536.0;
                    r = out;
                    break;
            }

            return r;
        };

        // @param algorithm number, by index in ev_dsp::distAlgos
        void setType(DistType d){
            type = d;
        };

        // Tone will affect the sound of the distortion for the following algorithms:
        // - bitcrush/srr: adjusts bit depth
        //
        // @param 0 <= i <= 1000
        void setTone(uint16_t i){
            tone = i;

            // This value is used for bitcrush, cache it here to save computations
            int pow = static_cast<int>(tone * .001 * 20) + 2;
            bit_depth_ = 1;
            for(uint8_t i = 0; i < pow; i++){
                bit_depth_ *= 2;
            }
        };
        
        // Sets the drive amount. Values < 5 will bypass the distortion.
        //
        // @param 0 <= i <= 1000
        void setDrive(uint16_t i){
            drive = i;
            crush_rate_ = (1.f - drive * .001 + .001) * samplerate / 10;
        };
    };
} // namespace ev_dsp