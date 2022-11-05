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

#include "tempo.h"
#include "theory.h"

using namespace daisy;
using namespace daisysp;
using namespace ev_theory;

Tempo::Tempo(){}

Tempo::Tempo(bool theInternal, std::function<void ()> theCallback){
    internal = theInternal;
    callback = theCallback;

    bpm           = 120.f;
    prevTickTime  = 0;
    prevPulseTime = 0;
    msPerTick     = 0;
    tick          = 0;
    pulseCount    = 0;
    isRunning     = true;
    avgIdx        = 0;
    lag           = 0.f;
    windowSize    = 16;
    
    timer = new TimerHandle();
    timer->Init(TimerHandle::Config{});
    timer->Start();
}

// Ticks once every 1/RESOLUTION note (1/256 by default)
void Tempo::update(){
    // Does nothing in external mode or if stopped
    if (!internal || !isRunning){
        return;
    }

    // Get delta since last tick
    uint32_t newTick = timer->GetTick();
    uint32_t freq = timer->GetFreq();
    float delta = 1000. * ((float)(newTick - prevTickTime) / (float)freq);

    // If conditions are met, advance ticker
    if (delta + lag >= msPerTick || isForceTick){
        // Since ticks must be  >=, they will often trigger a tad bit late due to processing time.
        // The cumulative late triggers result in tempo lagging behind the external source. To prevent
        // this, subtract how long the tick actually took from how long it should have take.
        if(isForceTick){
            lag = 0;
        } else {
            lag = delta - msPerTick;
        }
        
        prevTickTime = newTick;
        tick++;
        tick = tick % RESOLUTION;
        callback();
        isForceTick = false;
    }
}

// Occurs once every 1/PPN note
void Tempo::pulse(){
    // In external mode, simply invoke callback() 
    if (!internal){
        callback();
        return;
    }

    // Get delta since last pulse
    uint32_t newTick = timer->GetTick();
    uint32_t freq = timer->GetFreq();
    float delta = 1000. * ((float)(newTick - prevPulseTime) / (float)freq);

    float pLag = prevPulseTime - newTick;

    prevPulseTime = newTick;

    avgIdx++;
    avgIdx = avgIdx % windowSize;
    deltas[avgIdx] = delta;
    float avg = 0.f;
    for(uint8_t i = 0; i < windowSize; i++){
        // TODO keep a running total instead of summing the window each pulse. This caused issues for some reason, I'll have to revisit later.
        avg += deltas[i];
    }
    avg /= static_cast<float>(windowSize);

    // Calculate current BPM based upon delta (60,000 ms/min, 16 ticks per bar)
    setBPM(60000.f/avg/4.f);

    pulseCount++;
    pulseCount = pulseCount % PPN;

    uint32_t correctTick = pulseCount * 16;
    if(correctTick == 0){
        correctTick = 255;
    } else {
        correctTick--;
    }

    if (tick > correctTick){
        // If tick is ahead, send it back
        tick = correctTick;
    } else {
        // If tick is behind, make up for it
        while(correctTick > tick){
            isForceTick = true;
            update();
        }
    }

    lag += pLag;
}



void Tempo::start(){
    isRunning = true;
    prevTickTime = timer->GetTick();
}

void Tempo::stop(){
    isRunning = false;
}

void Tempo::forceTick(){
    isForceTick = true;
}

/* Setters */

void Tempo::setBPM(float i){
    if (internal){ // TODO std::max/min would be cleaner?
        if (i > MAX_BPM) {
            i = MAX_BPM;
        } else if (i < MIN_BPM) {
            i = MIN_BPM;
        }
        bpm = i;

        // Calculate ms per tick with the current BPM
        //
        // 1 pulse = ((RESOLUTION ticks per bar)/(16 pulses per bar)) ticks
        // (60,000 ms per min)/(4*BPM pulses per minute) = ms per pulse
        // Therefore, (PPN * 60,000) / (RESOLUTION * BPM) = ms per tick
        msPerTick = (PPN * 60000.f) / (RESOLUTION * bpm *4);
    }
}

void Tempo::setClock(bool b){
    // reset state
    bpm           = 120.f; // TODO may cause issues?
    prevTickTime  = 0;
    prevPulseTime = 0;
    msPerTick     = 0;
    tick          = 0;

    internal = b;
}

/* Getters */

uint16_t Tempo::getTick(){
    return tick;
}

float Tempo::getBPM(){
    return bpm;
}

bool Tempo::getIsRunning(){
    return isRunning;
}

std::string Tempo::toString(){
    return "b" + floatToString(bpm, 2) + "m" + floatToString(msPerTick, 2) + "t" + std::to_string(tick) + "p" + std::to_string(pulseCount);
}

std::string Tempo::bpmToString(){
    std::string ret = floatToString(bpm, 1);
    if (ret.size() == 4){
        ret += " ";
    } else if (ret.size() == 3){
        ret += "  ";
    }
    return ret;
};