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

#include "daisysp.h"
#include "daisy_patch.h"

#include "rhythm.h"
#include "theory.h"

using namespace daisy;
using namespace daisysp;
using namespace ev_theory;

Rhythm::Rhythm(){}

Rhythm::Rhythm(bool theInternal, std::function<void ()> theCallback){
    internal = theInternal;
    callback = theCallback;

    bpm           = 0;
    prevTickTime  = 0;
    prevPulseTime = 0;
    msPerTick     = 0;
    tick          = 0;
    
    timer = new TimerHandle();
    timer->Init(TimerHandle::Config {
        TimerHandle::Config::Peripheral::TIM_2,
        TimerHandle::Config::CounterDir::UP});
}

void Rhythm::update(){
    // Does nothing in external mode
    if (!internal){
        return;
    }

    // Get delta since last tick
    uint32_t newTick = timer->GetTick();
    uint32_t freq = timer->GetFreq();
    float delta = 1000. * ((float)(newTick - prevTickTime) / (float)freq);

    if (delta >= msPerTick){
        tick++;
        tick = tick % RESOLUTION;
        prevTickTime = newTick;
        callback();
    }
}

void Rhythm::pulse(){
    // In external mode, simply invoke callback() 
    if (!internal){
        callback();
        return;
    }

    // Get delta since last pulse
    uint32_t newTick = timer->GetTick();
    uint32_t freq = timer->GetFreq();
    float delta = 1000. * ((float)(newTick - prevPulseTime) / (float)freq);    
    prevPulseTime = newTick;

    // Calculate current BPM based upon delta (60,000 ms/min)
    setBPM(static_cast<uint16_t>(std::round(60000./delta)));
}

void Rhythm::start(){
    timer->Start();
}

void Rhythm::stop(){
    timer->Stop();
}

/* Setters */

void Rhythm::setBPM(uint16_t i){
    if (internal){
        if (i > MAX_BPM) {
            i = 500;
        } else if (i < MIN_BPM) {
            i = 20;
        }
        bpm = i;

        // Calculate ms per tick with the current BPM
        //
        // 1 pulse = ((RESOLUTION ticks per bar)/(4 pulses per bar)) ticks
        // (60,000 ms per min)/(BPM pulses per minute) = ms per pulse
        // Therefore, (4 * 60,000) / (RESOLUTION * BPM) = ms per tick
        msPerTick = 240000. / (RESOLUTION * bpm);
    }
}

void Rhythm::setClock(bool b){
    // reset state
    bpm           = 0;
    prevTickTime  = 0;
    prevPulseTime = 0;
    msPerTick     = 0;
    tick          = 0;

    internal = b;
}

/* Getters */

uint16_t Rhythm::getTick(){
    return tick;
}

uint16_t Rhythm::getBPM(){
    return bpm;
}

std::string Rhythm::toString(){
    return "b " + std::to_string(bpm) + " m " + floatToString(msPerTick, 2) + " t " + std::to_string(tick);
}

std::string Rhythm::bpmToString(){
    std::string ret = std::to_string(bpm);
    if (ret.size() == 2){
        ret += " ";
    } else if (ret.size() == 1){
        ret += "  ";
    }
    return ret;
};