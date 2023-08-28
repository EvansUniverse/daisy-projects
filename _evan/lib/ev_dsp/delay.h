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
#include "filter.h"

using namespace daisy;
using namespace daisysp;

#define INT_MAX_DELAY ((size_t)(.65f * 48000.0f)) // Samplerate (1 second) = 48000
#define FEEDBACK_POLLING_COEFF 0.0002f

namespace ev_dsp {

    class StereoDelay: public AudioEffect {
    public:
        static const size_t MAX_DELAY = INT_MAX_DELAY; // TODO const

        float feedback;
        float currentDelay;

        bool pingPong;

        DelayLine<float, INT_MAX_DELAY>* delayR;
        DelayLine<float, INT_MAX_DELAY>* delayL;

        Svf* hpf;
        Svf* lpf;

        // Don't use
        StereoDelay(){};

        StereoDelay(float sampleRate, DelayLine<float, INT_MAX_DELAY>* l, DelayLine<float, INT_MAX_DELAY>* r){
            delayL = l;
            delayR = r;
            samplerate = sampleRate;

            pingPong = false;

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
            setLevel(0);
            setFeedback(200);

            clear();
        };

        // @param audio in
        // @return audio out
        void processAudio(float &outl, float &outr, float inl, float inr){
            delayL->SetDelay(currentDelay);
            delayR->SetDelay(currentDelay);

            float readL = delayL->Read();
            float readR = delayR->Read();

            if(pingPong){
                // Ping-pong delay writes incoming audio to only one delay line, then they take
                // turns writing to each other:
                //
                //             out L      out R
                //               ^          ^
                //               |          |
                // L+R in --> L line --> R line --
                //               ^                |
                //               |                |
                //               ------------------
                delayL->Write((feedback * readR) + inl);
                delayR->Write(feedback * readL);
            } else {
                // Conversely, normal stereo delay maintains 2 separate delay lines that
                // read and write from themselves:
                //
                //             out L
                //               ^ 
                //               |
                // L in --> L line ----
                //               ^    |
                //               |    |
                //               ------
                //
                //             out R
                //               ^ 
                //               |
                // R in --> R line ----  
                //               ^    |
                //               |    |
                //               ------
                delayL->Write((feedback * readL) + inl);
                delayR->Write((feedback * readR) + inr);
            }

            outl = readL * wet;
            outr = readR * wet;

            // Apply filters
            hpf->Process(outr);
            outr = hpf->High();
            hpf->Process(outl);
            outl = hpf->High();

            lpf->Process(outr);
            outr = lpf->Low();
            lpf->Process(outl);
            outl = lpf->Low();
        };

        // Musically useful one-knob control of wet and feedback
        //
        // @param 0 - 1000
        void setLevel(uint16_t i){
            level = i;
            if(i < 20){
                // If the knob is low enough, just mute
                wet = 0;
                return;
            }
            
            float l = min(AFX_MAX_LEVEL, i) * AFX_RATIO;

            // Wet should always be a bit quieter than dry,
            // but not so quiet that it's barely audible
            wet = constrain(0.15f, 0.9f, l);

            // Feedback should never be high enough to go infinite,
            // and never low enough to not be distinguishable
            feedback = constrain(0.1f, 0.95f, l);
        };

        // @param cutoff (0 - 1000)
        void setHpf(uint16_t l){
            hpf->SetFreq(getSvfFreq(l));
        };

        // @param cutoff (0 - 1000)
        void setLpf(uint16_t l){
            lpf->SetFreq(getSvfFreq(1000 - l));
        };

        // Sets time as a fraction of the max delay length
        //
        // @param 0 <= i <= 1000
        void setTime(int16_t i){
            currentDelay = max(0.05f, i * .001f) * INT_MAX_DELAY;
            clear(); // Prevents artifacts & crashes
        };

        // @param 0 <= i <= 1000
        void setFeedback(int16_t i){
            feedback = i * .001f;
        };

        // @param if true, enable ping-pong
        void setPingPong(bool b){
            pingPong = b;
            clear();
        }

        // Clear contents of delay lines, which will mute the delay
        void clear(){
            delayL->Reset();
            delayR->Reset();
        }
    };
} // namespace ev_dsp














