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

#include "../lib/ev_theory/theory.h"
#include "../lib/ev_theory/chord.h"
#include "../lib/ev_theory/arp.h"
#include "../lib/ev_theory/tempo.h"
#include "../lib/ev_gui/gui.h"

/* Menu values specific to Jellybeans */
namespace jellybeans {

    // Order must match order of OpMode
    const std::vector<std::string> opModes {
        "Arp",
        "Quant",
        "Oct",
    };

    const std::vector<std::string> clockModes {
        "BPM",
        "PerTrig",
    };

    // Given the 1V/oct and 0-5V range of the CV out port,
    // we are limited to a 5 octave register. Voicings span
    // up to 2 octaves and coarse tuning (mRoot) spans another,
    // leaving us 2 octaves of room for upwards transposition.
    //
    // Note that the indices of the elements are also their octave distances from 0
    const std::vector<std::string> allOctaves {
        "-3",
        "-2",
        "-1",
        "0",
        "+1",
        "+2",
        "+3",
    };
}