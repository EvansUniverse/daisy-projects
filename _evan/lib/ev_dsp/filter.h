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

using namespace daisy;
using namespace daisysp;

namespace ev_dsp {
    const uint8_t LEN_LPF = 10;
    const float MIN_SVF_FREQ = 2;
    uint16_t BREAKS_LPF[LEN_LPF] = {100, 200, 300, 400, 500, 600, 700, 800, 900, 1000};
    float TARGS_SVF[LEN_LPF] = {50, 100, 200, 350, 500, 700, 1000, 3000, 8000, 15900};
    float TARGS_MG[LEN_LPF] = {80, 160, 300, 500, 1000, 1800, 2800, 5000, 9000, 15890};

    // svf: DaisySp::SVF, a boring normal filter
    // mg:  moog ladder
    // off: bypass
    enum LpfType {svf, mg, lpf_off};
    
    // Lowpass filter with different algorithms
    class LpFilter {
    public:
        // min/max levels user can input
        static const uint16_t LPF_MIN_LEVEL = 0;
        static const uint16_t LPF_MAX_LEVEL = 1000;

        LpfType type;

        Svf* filter_svf;
        MoogLadder* filter_mg;

        // 0 is fully closed
        // 1000 is fully open
        uint16_t level;

        uint16_t levelOffset;

        float freq;

        // Freq values < 1 cause svf to crash
        // Realistically, though, anything under 40 doesn't make an audible change
        static constexpr float LPF_MIN_FREQ = 40;

        // If true, fully mute the signal
        bool isMuting; 

        // Don't use
        LpFilter(){};

        // @param patch->AudioSampleRate()
        LpFilter(float samplerate){
            level = LPF_MAX_LEVEL;

            filter_svf = new Svf();
            filter_svf->Init(samplerate);
            filter_svf->SetRes(0.1f);
            filter_svf->SetFreq(MIN_SVF_FREQ);

            filter_mg = new MoogLadder();
            filter_mg->Init(samplerate);
            filter_svf->SetRes(0.1f); 
            filter_svf->SetFreq(0.f);

            isMuting = false;
            setType(svf);
        };

        // @param audio in
        // @return audio out
        float processAudio(float f){
            if(isMuting){
                return 0.f;
            }

            switch(type){
            case svf:
                filter_svf->Process(f);
                return filter_svf->Low();
                break;
            case mg:
                // Ladder outputs rather quiet, so add gain to compensate
                return filter_mg->Process(f) * 5.f;
                break;
            case lpf_off:
                return f;
                break;
            }
            return 0.f; // Unreachable
        }

        // Sets filter frequency based on "level" aka knob position
        //
        // @param a value between 0 and 1000
        void setFreq(uint16_t i){
            level = constrain((uint16_t) 0, (uint16_t) 1000, i);;
            computeLevel();
        }

        // Offsets filter frequency based on "level" aka knob position.
        // This would be used, for example, to apply an envelope to the filter while retaining
        // its original level.
        //
        // @param a value between 0 and 1000
        void setOffset(int16_t i){
            levelOffset = i;
            computeLevel();
        }

        // @return level offset (-1000 to 1000)
        int16_t getOffset(){
            return levelOffset;
        }

        // Sets filter resonance based on "level" aka knob position
        //
        // @param a value between 0 and 1000
        void setResonance(int16_t i){
            filter_svf->SetRes(i/1000.f);
            filter_mg->SetRes(i/1000.f);
        };

        // @param a value between 0 and 1000
        int16_t getLevel(){
            return level;
        };

        void setType(LpfType t){
            type = t;
            computeLevel();
        }

        // @return the filter's current level, factoring in offset (0-1000)
        uint16_t getTrueLevel(){
            int16_t l = level + levelOffset; // Not worried about int overflow here since max level is 1000
            l = constrain((int16_t) 0, (int16_t) 1000, l);
            return (uint16_t) l;
        }

    private:

        void computeLevel(){
            int16_t l = getTrueLevel();
            
            // Ensure that values sufficiently close to muting the signal fully mute the signal
            if (l < LPF_MIN_LEVEL * 0.98){
                isMuting = true;
                return;
            }

            switch(type){
            case svf:
                filter_svf->SetFreq(piecewiseLinear(l, BREAKS_LPF, TARGS_SVF, LEN_LPF));
                break;
            case mg:
                filter_mg->SetFreq(piecewiseLinear(l, BREAKS_LPF, TARGS_MG, LEN_LPF) + 100.f);
                break;
            default:
                break;
            }
        };
    };

    // Helper used by external libraries
    //
    // @param i 0-1000
    float getSvfFreq(uint16_t i){
        return piecewiseLinear(i, BREAKS_LPF, TARGS_SVF, LEN_LPF);
    }
} // namespace ev_dsp