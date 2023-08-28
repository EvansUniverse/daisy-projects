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

namespace musubi {

    const std::vector<std::string> onOrOff {
        "Off",
        "On",
    };

    const std::vector<std::string> cascadeOpts {
        "Rst",
        "Cscd",
    };

    const std::vector<std::string> lpfOpts {
        "Normal", // DaisySP::SVF
        "Moog",
        "Off",
    };

    const std::vector<std::string> yesOrNo {
        "No",
        "Yes",
    };

    const std::vector<std::string> gateInOpts {
        "Trig",
        "Gate",
    };

    const std::vector<std::string> blankOpts {
        "",
    };  

    const std::vector<std::string> outputOpts {
        "1-2s 3-4m", // 1 stereo out (1+2), 2 mono outs (3&4)
        "2 stereo",  // 2 stereo outs (1+2, 3+4)
        "4 mono",    // 4 mono outs (1, 2, 3, 4)
        "mx 1" ,     // channel 1 bypasses EG. 2 stereo outs: 1+2 is EG output, 3+4 is mixer output
        "mx 1-2",
        "mx 1-3",
        "mx 1-4",
    };  
}
