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

#include "utils.h"
#include "daisysp.h"
#include "daisy_patch.h"
#include "audioeffect.h"
#include "envelope.h"
#include "filter.h"

using namespace daisy;
using namespace daisysp;

namespace ev_dsp {

    // Stereo reverb with a high-pass filter
    class StereoReverb: public AudioEffect {
    public:
        // Don't use
        StereoReverb(){};

        StereoReverb(float sampleRate, ReverbSc* r){
            wet = 0.3;
            dry = 1.0;

            env = new AhrEnvelope(25000, 1, 1);
            env->setAttackContour(350);
            setPredelay(0);

            sc = r;
            sc->Init(sampleRate);
            sc->SetFeedback(0.85f);
            sc->SetLpFreq(18000.0f);

            hpf = new Svf();
            hpf->Init(sampleRate);
            hpf->SetRes(0.0f);
            hpf->SetDrive(0.0f);

            lpf = new Svf();
            lpf->Init(sampleRate);
            lpf->SetRes(0.0f);
            lpf->SetDrive(0.0f);

            setHpf(0);
            setLpf(0);
        };

        // @param audio in
        // @return audio out
        void processAudio(float &outl, float &outr, float inl, float inr){
            if (level < 5){
                // Bypass
                outl = inl;
                outr = inr;
                return;
            }

            // The "predelay" is just an attack envelope on the input signal
            env->tick();
            if(env->state == rising){
                inl = inl * env->getLevel();
                inr = inr * env->getLevel();
            }
  
            sc->Process(inl, inr, &outl, &outr);

            hpf->Process(outr);
            outr = hpf->High();
            hpf->Process(outl);
            outl = hpf->High();

            lpf->Process(outr);
            outr = lpf->Low();
            lpf->Process(outl);
            outl = lpf->Low();

            outr = outr * wet + inr * dry;
            outl = outl * wet + inl * dry;
        };

        // Values of 1000 will create infinite feedback
        //
        // @param feedback amount (0 - 1000)
        void setFeedback(uint16_t i){
            if (i > 990){
                i = 1000;
            } else if (i < 10){
                i = 0;
            }

            // Values under 500 seem to do nothing
            sc->SetFeedback(.5f + i * 0.0005f);
        };

        // Musically useful one-knob control of the mix and feedback
        //
        // @param level (0 - 1000)
        void setLevel(uint16_t i){
            level = min(AFX_MAX_LEVEL, i);

            if( level < 5){
                // Bypass, don't bother calculating
                return;
            }

            float l; // = level / 1000.f;

            if (level < 200){ // 0-200 small ambience
                l = level / 200.f;
                // dry 100%
                dry = 1.f;
                // wet 30% - 40%
                wet = .3f + l * .1f;
                // feedback 30% - 40%
                setFeedback(300 + l * 100);

            } else if ( level < 500){ // 200-500 medium ambience
                l = (level - 200) / 300.f;
                // dry 100%
                dry = 1.f;
                // wet 40% - 50%
                wet = .4f + l * .1f;
                // feedback 40% - 60%
                setFeedback(400 + l * 200); 

            } else if ( level < 750){ // 500-750 large ambience
                l = (level - 500) / 250.f;
                // dry 100% - 90%
                dry = 1.f - l * .1f;
                // wet 50% - 60%
                wet = .5f + l * .1f;
                // feedback 60% - 75%
                setFeedback(600 + l * 150);

            } else { // 750-1000 wash out
                l = (level - 750) / 250.f;
                // dry 90% - 0%
                dry = 1.f - l * .9f;
                // wet 60% - 90%
                wet = .6f + l * .3f;
                // feedback 75% - 95%
                setFeedback(750 + l * 200);
            }
        };

        // @param cutoff (0 - 1000)
        void setHpf(uint16_t i){
            hpf->SetFreq(getSvfFreq(i)); 
        };

        // @param cutoff (0 - 1000), cutoff freq descends as this value ascends
        void setLpf(uint16_t i){
            lpf->SetFreq(getSvfFreq(1000 - i));
        };

        // @param predelay (0 - 1000)
        void setPredelay(uint16_t i){
            env->setAttack(i);
        };

        // Indicates that a new note came in, we should reset predelay index
        void trigger(){
            env->trigger();
        }

    private:
        ReverbSc* sc;
        Svf* hpf;
        Svf* lpf;
        AhrEnvelope* env; // For predelay
    };
} // namespace ev_dsp














