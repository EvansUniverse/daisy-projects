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

#include "../lib/ev_gui/gui.h"
#include "../lib/ev_theory/tempo.h"
#include "../lib/ev_theory/theory.h"
#include "../lib/ev_theory/midi.h"
#include "sequencer/sequencer.h"



/* Menu values specific to Gumdrops */
namespace gumdrops {

    // Index of each button corresponds to its step number
    // Contrary to the bass ackwards info on Field's from panel, the upper row
    // of buttons is B and the bottom row is A
    const size_t stepLeds[] = {
        DaisyField::LED_KEY_B1,
        DaisyField::LED_KEY_B2,
        DaisyField::LED_KEY_B3,
        DaisyField::LED_KEY_B4,
        DaisyField::LED_KEY_B5,
        DaisyField::LED_KEY_B6,
        DaisyField::LED_KEY_B7,
        DaisyField::LED_KEY_B8,
        DaisyField::LED_KEY_A1,
        DaisyField::LED_KEY_A2,
        DaisyField::LED_KEY_A3,
        DaisyField::LED_KEY_A4,
        DaisyField::LED_KEY_A5,
        DaisyField::LED_KEY_A6,
        DaisyField::LED_KEY_A7,
        DaisyField::LED_KEY_A8,
    };

    // Index of each element corresponds to its knob number
    const size_t knobLeds[] = {
        DaisyField::LED_KNOB_1,
        DaisyField::LED_KNOB_2,
        DaisyField::LED_KNOB_3,
        DaisyField::LED_KNOB_4,
        DaisyField::LED_KNOB_5,
        DaisyField::LED_KNOB_6,
        DaisyField::LED_KNOB_7,
        DaisyField::LED_KNOB_8,
    };

    // Step length options given to the user
    const std::vector<std::string> stepLengths {
        "1/64",
        "1/32",
        "1/16",
        "1/8",
        "1/4",
        "1/2",
        "1 Bar",
        "2 Bars",
        "4 Bars",
        "8 Bars",
    };

    // Translates the "length" values that the user sees (which are in steps) to ticks.
    const std::map<std::string, int> stepLengthsToTicks {
        {"1/64",   4},
        {"1/32",   8},
        {"1/16",   16},
        {"1/8",    32},
        {"1/4",    64},
        {"1/2",    128},
        {"1 Bar",  256},
        {"2 Bars", 512},
        {"4 Bars", 1024},
        {"8 Bars", 2048},
    };
}