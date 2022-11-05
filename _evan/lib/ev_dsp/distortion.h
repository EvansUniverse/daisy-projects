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
    const uint8_t DIST_DEFAULT_TYPE = 1;

    const std::vector<std::string> distAlgos {
        "Bypass",
        "Tanh",
        "Atan",
        "HardClip",
        "SoftClip",
        "Saturate",
        "Bitcrush",
        "7",
    };

    
    // Audio distortion/saturation with multiple algorithms
    class Distortion: public AudioEffect {
    public:
        Fold* fold;

        uint8_t algorithm;
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
            setType(DIST_DEFAULT_TYPE);

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
            if(drive == 0){
                return f;
            }

            float r = f; // Keep f in case we need its original value later
            float d;

            switch(algorithm){
                case 0: //bypass
                {
                    break;
                }
                case 1: // tanh
                    // Signal is multiplied by drive before being fed into distortion algorithm
                    // 1.0 <= drive <= 51.0
                    d = 1.0f + (drive * .05f);
                    r = tanh(r * drive);
                    break;
                case 2: // atan
                    // Signal is multiplied by drive before being fed into distortion algorithm
                    // 1.0 <= drive <= 51.0
                    d = 1.0f + (drive * .05f);
                    r = HALFPI_F * atan(r * d);
                    break;
                case 3: // hard clip
                    d = 1.0f + (drive * .2f);
                    r = r * d;
                    if(r > 1.f){
                        r = 1.f;
                    }
                    break;
                case 4: // soft clip
                    d = 1.0f + (drive * .2f);
                    r = r * d;
                    r = SoftClip(r);
                    break;
                case 5: // soft saturate
                    d = 1.0f + (drive * .2f);
                    r = r * d;
                    r = soft_saturate(r, 1.0);
                    break;
                case 6: // bitcrush/srr
                    // Most of this code was stolen from DaisySp but calling pow()
                    // added like 5% to the binary size so I've modified it
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
        void setType(uint8_t i){
            algorithm = i;
        };

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
        

        // @param 0 <= i <= 1000
        void setDrive(uint16_t i){
            drive = i;
            crush_rate_ = (1.f - drive * .001 + .001) * samplerate / 10;
        };
    };
} // namespace ev_dsp