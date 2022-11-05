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

#pragma once

namespace ev_dsp {
    float quantize(float min, float max, float input){
        if (input < min){
            input = min;
        } else if (input > max){
            input = max;
        }
        return input;
    }

    // uint16_t quantize(uint16_t min, uint16_t max, uint16_t input){
    //     if (input < min){
    //         input = min;
    //     } else if (input > max){
    //         input = max;
    //     }
    //     return input;
    // }

    int16_t quantize(int16_t min, int16_t max, int16_t input){
        if (input < min){
            input = min;
        } else if (input > max){
            input = max;
        }
        return input;
    }
   
} // namespace ev_dsp