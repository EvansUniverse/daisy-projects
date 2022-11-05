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

#include "../lib/ev_theory/theory.h"
#include "../lib/ev_gui/gui.h"
#include "../lib/ev_dsp/dsp.h"

namespace omakase {
    const std::vector<std::string> compRatios {
        "1:1.5",
        "1:2",
        "1:4",
        "1:10",
    };

    // Corresponded to the elements with same index in compRatios
    const std::vector<float> compRatiosToFloat {
        1.5f,
        2.f,
        4.f,
        10.f,
    };

    const std::vector<std::string> compAtks {
        ".1",
        ".3",
        "1",
        "3",
        "10",
        "30",
    };

    // Corresponded to the elements with same index in compAtks
    const std::vector<float> compAtksToFloat {
        .1f,
        .3f,
        1.f,
        3.f,
        10.f,
        30.f,
    };

    const std::vector<std::string> compRels {
        ".1",
        ".3",
        ".6",
        "1.2",
    };

    // Corresponded to the elements with same index in compRels
    const std::vector<float> compRelsToFloat {
        .1f,
        .3f,
        .6f,
        1.2f,
    };

    const std::vector<std::string> sendCfgs {
        "1 Stereo",
        "2 Mono",
    };

    const std::vector<std::string> yesOrNo {
        "Yes",
        "No",
    };

    const std::vector<std::string> blank {
        "",
    };  
}
