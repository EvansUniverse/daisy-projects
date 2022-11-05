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

#include <functional>
#include <string>

using namespace daisy;
using namespace daisysp;

namespace ev_theory {
    //                                     ##### Tempo #####
    // Tempo is essentially a metronome; it manages musical timing. Can run its own clock or detect
    // BPM from an external source.
    //
    //                                   ##### Terminology #####
    // Ticks: There are RESOLUTION ticks per whole note (bar). 
    // By default 256, making them effectively 256th notes.
    //
    // Pulses: Tempo expects the user to call pulse() every x notes, depending on operating mode.
    // Think of it as "tap tempo".
    //
    // Callback: Tempo keeps a user-supplied callback function, which is executed every tick.
    //
    //                                     ##### Usage #####
    // Tempo has 2 modes of operation:
    // 
    // Internal mode: Clock time is set using setBPM() or by calling pulse() for every PPN notes (by 
    // default every 16th note) aka "tap tempo". Call update() reapeatedly in the main event loop. 
    // It will invoke callback() if the clock has advanced a tick. 
    //
    // External mode: Doesn't use a clock. Simply calls callback() every time pulse() is called. This 
    // mode allows Tempo to smoothly  handle tempo changes, e.g. if a user is noodling with a clock knob
    // in real time.
    //
    // For perfect synchronization, pulse() should be called before update() in the main event loop.
    //
    //                                     ##### NOTE #####
    // - Tempo uses daisy seed's TIM_2 peripheral.
    // - Some operations appear to mess with the time deltas in a way that impacts tempo. For example, the 
    //   following line, which fills an empty rectangle, causes tempo to speed up by an audible ~15bpm:
    //   hw->display.DrawRect(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, false, true)
    // - See Jellybeans for an intended usage example.
    class Tempo {
    private:
        TimerHandle* timer;

        // Not yet functional, just here to show the default value
        // Pulses per (whole) note
        const uint16_t PPN = 16;

        // Internal BPM, >MIN_BPM & <MAX_BPM
        float bpm;

        // Index of the current tick within the current bar
        uint16_t tick;


        // true: Internal mode
        // false: External mode
        bool internal;

        // true: clock is started
        // false: clock is stopped
        bool isRunning;

        // If true, tempo will tick the next update()
        bool isForceTick;

        // Callback function, called each tick
        std::function<void ()> callback;

        std::string string;

        // Used to ascertain bpm and keep clock
        uint32_t prevTickTime;
        uint32_t prevPulseTime;
        uint32_t freq;
        float    msPerTick;

        // Used to calculate as a moving average of the last 16 steps
        std::vector<float> deltas;
        uint8_t avgIdx;

        // Lag since the last tick, used to compensate for tempo drift from external clock
        float lag;
        
    public:
        // Index of the current PPNth note pulse
        uint8_t pulseCount;

        // Size of the moving average window used to detect tempo from external sources. 
        // Higher values mean more precision but higher cpu load. Default 16.
        uint8_t windowSize;

        // Weird behavior occurs around 5-10bpm, possibly due to the 
        // timerhandle's 32-bit  time wrapping around. 20 seems like a 
        // reasonable and musical min value.
        constexpr static float MIN_BPM = 20.f;

        // Values above ~500 start to become wildly inaccurate.
        // To support >500 BPM later down the road, we'd probably
        // have to measure the deltas in ns instead of ms. 500 is
        // plenty high anyways, so this probably won't be changed.
        constexpr static float MAX_BPM = 500.f;

        // The number of "ticks" per bar
        // TODO: consider higher (or variable) resolutions if swing doesn't sound natural enough.
        const static uint16_t RESOLUTION = 256;

        // Don't use this
        Tempo();

        // @param internal
        // @param callback function, called each tick
        Tempo(bool, std::function<void ()>);

        // Re-enable the timer. Only needs to be called if stop() has been called
        void start();

        // Disable the timer. It will still internally tick but the callback function will not fire
        void stop();

        // Internal mode: Invokes callback() if the tick has advanced
        // External mode: does nothing
        void update();

        // Essentially a "tap tempo" tap
        //
        // Internal mode: Tells the clock a new pulse has been detected,
        // set BPM according to last pulse.
        // External mode: Call callback()
        void pulse();

        // Guarantees that temp will tick next time update() is called. Useful for keeping tempo in sync with an internal clock.
        void forceTick();

        // Does nothing in external mode
        void setBPM(float);

        // @param true=internal mode, false = external mode
        // resets state when called to avoid issues.
        void setClock(bool);

        float getBPM();

        uint16_t getTick();

        // @return false if stopped
        bool getIsRunning();

        // @return a long and informative string, good for debug purposes.
        std::string toString();

        // @return a string representation of BPM padded with spaces so that 
        // it's always 5 chars long e.g. "128.2" or "90.0 ".
        std::string bpmToString();
   };
} // namespace ev_theory