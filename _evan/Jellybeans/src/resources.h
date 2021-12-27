/* 
 * Copyright (C) 2021 Evan Pernu - All Rights Reserved
 * You may use, distribute and modify this code under the
 * terms of the GNU AGPLv3 license.
 * 
 * You should have received a copy of the GNU AGPLv3 license with
 * this file (LICENSE.md). If not, please write to: evanpernu@gmail.com, 
 * or visit: https://www.gnu.org/licenses/agpl-3.0.en.html
 */

#include "theory.h"
#include "chord.h"
#include "arp.h"
//#include "rhythm.h"
#include "gui/gui.h"

/* Menu values specific to Jellybeans */
namespace jellybeans {
    // Given the 1V/oct and 0-5V range of the CV out port,
    // we are limited to a 5 octave register. Voicings span
    // up to 2 octaves and coarse tuning (mRoot) spans another,
    // leaving us 2 octaves of room for upwards transposition.
    //
    // Note that the indices of the elements are also their octave distances from 0
    const std::vector<std::string> allOctaves {
        "0",
        "+1",
        "+2",
    };

    const std::vector<std::string> allBassOctaves {
        "0",
        "+1",
        "+2",
        "+3",
    };

    // DEBUG
    const std::vector<std::string> emptyVect {"N/A"};
}