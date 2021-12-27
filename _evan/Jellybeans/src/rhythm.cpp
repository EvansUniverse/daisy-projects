/* 
 * Copyright (C) 2021 Evan Pernu - All Rights Reserved
 * You may use, distribute and modify this code under the
 * terms of the GNU AGPLv3 license.
 * 
 * You should have received a copy of the GNU AGPLv3 license with
 * this file (LICENSE.md). If not, please write to: evanpernu@gmail.com, 
 * or visit: https://www.gnu.org/licenses/agpl-3.0.en.html
 */

#include "daisysp.h"
#include "daisy_patch.h"
#include "rhythm.h"
#include "theory.h"

using namespace daisy;
using namespace daisysp;
using namespace jellybeans;

Rhythm::Rhythm(){}

Rhythm::Rhythm(bool theInternal, std::function<void ()> theCallback){
    internal = theInternal;
    callback = theCallback;

    bpm       = 0;
    prevTime  = 0;
    string    = "";
    msPerTick = 0;
    isNewTick = true;
    tick = 0;
    
    timer = new TimerHandle();
    timer->Init(TimerHandle::Config {
        TimerHandle::Config::Peripheral::TIM_2,
        TimerHandle::Config::CounterDir::UP});
}

void Rhythm::update(){
    if (getDelta() >= msPerTick){
        tick++;
        tick = tick % RESOLUTION;

        if(tick == 0){
            callback();
        }
    }
}

float Rhythm::getDelta(){
    // TODO millis may not be accurate enough
    uint32_t newTick = timer->GetTick();
    uint32_t freq = timer->GetFreq();
    return 1000. * ((float)(newTick - prevTime) / (float)freq);    
}

void Rhythm::pulse(){
    if (internal){
        return;
    }

    uint32_t newTick = timer->GetTick();
    uint32_t freq = timer->GetFreq();
    float delta = 1000. * ((float)(newTick - prevTime) / (float)freq);    
    prevTime = newTick;

    // Calculate current BPM
    bpm = static_cast<uint16_t>(std::round(60000./delta)); // 60,000 ms/min

    // TODO fixme
    //
    // !!! HACK !!!
    // BPM values come up a little short, possibly due to processing 
    // time that occurs between each tick. Compensate for this by adding
    // the observed difference.
    //
    // Values are still a little off, however. 
    // * Values <= 160 are fully accurate
    // * Values <= 220 are accurate +/- 1
    // * Values <= 240 are accurate +/- 2
    // * As values grow past 270, they become less and less accurate
    if (bpm >= 78 && bpm < 137) {
        bpm += 1;
    } else if (bpm >= 137 && bpm < 196) {
        bpm += 2;
    } else if (bpm >= 196 && bpm < 272) {
        bpm += 4;
    } else if (bpm >= 272) {
        bpm += 7;
    } // If bpm > 300, you're on your own lol

    // Values above ~500 start to become wildly inaccurate.
    // If I want to support >500 BPM later down the road,
    // I'll have to measure the deltas in ns instead of ms.
    if (bpm > 500) {
        bpm = 500;
    }

    // Weird behavior occurs around 5bpm; 20 seems like a reasonable
    // and musical min value
    if (bpm < 20) {
        bpm = 20;
    }

    // 1 pulse = ((RESOLUTION ticks per bar)/(4 pulses per bar)) ticks
    // (60,000 ms per min)/(BPM pulses per minute) = ms per pulse
    // Therefore, (4 * 60,000) / (RESOLUTION * BPM) = ms per tick
    msPerTick = 240000. / (RESOLUTION * bpm);

    //string = std::to_string(bpm);
    string = "b " + std::to_string(bpm) + " m " + floatToString(msPerTick, 2) + " t " + std::to_string(tick);
}

void Rhythm::start(){
    timer->Start();
    prevTime = timer->GetTick();
}

void Rhythm::stop(){
    timer->Stop();
}

/* Setters */

void Rhythm::setBPM(uint16_t i){
    if (internal){
        bpm = i;
    }
}

/* Getters */

uint16_t Rhythm::getTick(){
    return tick;
}

float Rhythm::getBPM(){
    return bpm;
}

std::string Rhythm::toString(){
    return string;
   //return std::to_string(tick);
}