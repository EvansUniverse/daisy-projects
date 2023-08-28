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
#include "envelope.h"
#include "filter.h"

using namespace daisy;
using namespace daisysp;

namespace ev_dsp {
    
    // Stereo envelope -> LPF -> VCA
    class StereoVcf {
    public:
        float lvl; // todo might not need

        LpFilter* lpf; // LpFilter is stateless so we can use the same one for both channels
        AhrEnvelope* env;

        // If true, apply envelope to VCA
        bool vca;

        // The amount of envelope applied to the filter (0 - 1000)
        uint16_t lpfEnv;

        // Don't use
        StereoVcf(){};

        StereoVcf(float samplerate){
            lpf = new LpFilter(samplerate);
            env = new AhrEnvelope();
            env->setTaperOn(500);
            env->setTaperOff(2000);

            lvl = 0.f;
        }

        void processAudio(float &outl, float &outr, float inl, float inr){
            if(vca && !env->isCycling()){
                // If envelope is not cycling in vca mode, mute audio output and don't perform unneccessary calculations
                outl = 0.f;
                outr = 0.f;
                return;
            }
            outl = inl;
            outr = inr;

            // Prevents clicking when the sound abruptly starts or ends.
            outl = outl * env->getTaperLevel();
            outr = outr * env->getTaperLevel();

            // Apply env to lpf
            int16_t offset = (int16_t) ((lpfEnv - 500) * 2) * env->getLevel();
            lpf->setOffset(offset);

            // Apply lpf
            outl = lpf->processAudio(outl);
            outr = lpf->processAudio(outr);

            // Apply VCA
            if(vca){
                outl = outl * env->getLevel();
                outr = outr * env->getLevel();
            }
        };

        // Sets filter frequency envelope amount based on "level" aka knob position
        //
        // @param i: 0-1000 
        // 500 -> 1000 is positive, 
        // 500 -> 0 is negative, 
        // 500 is no env.
        void setLpfEnv(uint16_t i){
            lpfEnv = constrain((uint16_t) 0, (uint16_t) 1000, i);
        }

        // If true, envelope will be applied to VCA
        void setVca(bool b){
            vca = b;
        }

        // @return the level the filter would be at during the envelope's peak (0-1000)
        uint16_t getEnvLvl(){
            int16_t l = lpf->getFreq() + ((lpfEnv - 500) * 2);
            l = constrain((int16_t) 0, (int16_t) 1000, l);
            return (uint16_t) l;
        }
    };
} // namespace ev_dsp