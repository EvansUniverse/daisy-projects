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
    //                                     ##### Rhythm #####
    // Rhythm is essentially a metronome; it manages musical timing. Can run its own clock or detect
    // BPM from an external source.
    //
    //                                   ##### Terminology #####
    // Ticks: There are RESOLUTION ticks per whole note (bar). 
    // By default 256, making them effectively 256th notes.
    //
    // Pulses: Rhythm expects the user to call pulse() every x notes, depending on operating mode.
    // Think of it as "tap tempo".
    //
    // Callback: Rhythm keeps a user-supplied callback function, which is executed every tick.
    //
    //                                     ##### Usage #####
    // Rhythm has 2 modes of operation:
    // 
    // Internal mode: Clock time is set using setBPM() or by calling pulse() for every PPN notes (by 
    // default every quarter note) aka "tap tempo". Call update() reapeatedly in the main event loop. 
    // It will invoke callback() if the clock has advanced a tick. Rhythm does not handle tempo changes 
    // smoothly in this mode; so it's adviseable to keep a constant tempo.
    //
    // External mode: Doesn't use a clock. Simply calls callback() every time pulse() is called. This 
    // mode allows Rhythm to smoothly  handle tempo changes, e.g. if a user is noodling with a clock knob
    // in real time.
    class Rhythm {
    private:
        TimerHandle* timer;

        // The number of "ticks" per bar
        // TODO: consider higher (or variable) resolutions
        // if swing doesn't sound natural enough.
        const uint16_t RESOLUTION = 256;

        // Not yet functional, just here to show the default value
        // Pulses per (whole) note
        const uint16_t PPN = 4;

        // Capped at 500
        uint16_t bpm;

        // Index of the current tick within the current bar
        uint16_t tick;

        // true: Internal mode
        // false: External mode
        bool internal;

        // Callback function, called each tick
        std::function<void ()> callback;

        std::string string;

        // Used to ascertain bpm and keep clock
        uint32_t prevTickTime;
        uint32_t prevPulseTime;
        uint32_t freq;
        float    msPerTick;

    public:
        // Weird behavior occurs around 5-10bpm, possibly due to the 
        // timerhandle's 32-bit  time wrapping around. 20 seems like a 
        // reasonable and musical min value.
        const static uint8_t MIN_BPM = 20;

        // Values above ~500 start to become wildly inaccurate.
        // To support >500 BPM later down the road, we'd probably
        // have to measure the deltas in ns instead of ms.
        const static uint16_t MAX_BPM = 500;
        // Don't use this
        Rhythm();

        // @param internal
        // @param callback function, called each tick
        Rhythm(bool, std::function<void ()>);

        // start/stop timer
        void start();
        void stop();

        // Internal mode: Tells the clock a new pulse has been detected,
        // set BPM according to last pulse.
        // External mode: Call callback()
        void pulse();

        // Does nothing in external mode
        void setBPM(uint16_t);

        // @param true=internal mode, false = external mode
        void setClock(bool);

        uint16_t getBPM();

        uint16_t getTick();

        // Intended to be called in the main event loop.
        // Internal mode: Invokes callback() if the tick has advanced
        // External mode: does nothing
        void update();

        // @return a long and informative string, good for debug purposes.
        std::string toString();

        // @return a string representation of BPM padded with spaces so that 
        // it's 3 chars long e.g. "128" or "90 ".
        std::string bpmToString();
   };
} // namespace ev_theory