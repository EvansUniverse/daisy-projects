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

#include <functional>
#include <string>

using namespace daisy;
using namespace daisysp;

namespace jellybeans {

    // Manages musical timing. Can manage its own clock or detect BPM 
    // from an external source.
    //
    //                    ##### Usage #####
    // 
    // Call update() every frame. It will return true if the clock has
    // advanced a frame, at which point you know the beat has advanced
    // by 1/RESOLUTIONth note.
    //
    // External mode: Receives a clock pulse from an external source.
    // Call pulse() upon every clock pulse.
    //
    // Internal mode: Clock time is set using setBPM()
    class Rhythm {
    private:
        TimerHandle* timer;

        // For now we'll just take 1PPQ, in the future use this to make it variable
        // uint8_t ppq;

        // The number of "ticks" per bar
        // TODO: consider higher (or variable) resolutions
        // if swing doesn't sound natural enough.
        const uint16_t RESOLUTION = 256;

        // Capped at 500
        uint16_t bpm;

        // Index of the current tick within the current bar
        uint16_t tick;

        // If true, use internal clock
        bool internal;

        // Callback function, called each tick
        std::function<void ()> callback;

        std::string string;

        // Used to ascertain bpm
        uint32_t prevTime;
        uint32_t freq;
        float msPerTick;

        bool isNewTick;

        // Delta since last tick (ms)
        float getDelta();

    public:
        // Don't use this
        Rhythm();

        // @param if true, internal clock mode
        // @param callback function, called each tick
        Rhythm(bool, std::function<void ()>);

        void init();
        void start();
        void stop();

        // Tells the clock a new pulse has been detected
        // Does nothing in internal mode
        void pulse();

        // Does nothing in external mode
        void setBPM(uint16_t);

        // Sets internal/external clock mode
        void setInternal(bool);
        
        float getBPM();

        uint16_t getTick();

        // Intended to be called every frame
        // Invokes callback() if the tick has advanced
        void update();

        // E.g. "BPM: 128"
        std::string toString();
   };
}