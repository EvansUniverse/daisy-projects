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
    template <typename T> T constrain(T min, T max, T i)
    {
        if (i < min){
            return min;
        } else if (i > max){
            return max;
        }
        return i;
    }

    template <typename T> T max(T x, T y)
    {
        return (x > y) ? x : y;
    }

    template <typename T> T min(T x, T y)
    {
        return (x < y) ? x : y;
    }

    // Constrains between 0 and 1000 (prevents invalid knob value)
    uint16_t constrain_afx(uint16_t i){ 
        return constrain((uint16_t) 0, (uint16_t)  1000, i);
    }

    // TODO: piecewiseLinear is probably better, maybe just nix this
    //
    // Scales the input values at a rate determined by the given breakpoints and multipliers.
    // This allows the entire range of a settings knob to be fine-tuned to musically pleasing numbers.
    //   For example, let b = {5, 10} and m = {10, 20}
    //   psuedoExponential(4,  b, m, 2) = 4
    //   psuedoExponential(9,  b, m, 2) = 5 + 4*10 = 45
    //   psuedoExponential(20, b, m, 2) = 5 + 5*10 + 10*20 = 255 
    //
    // @param input: the value to scale
    // @param breakpoints: list of values at which input scales at a different level.
    // @param multipliers: list of multipliers for each breakpoint. Must have same length as breakpoints.
    // @param the length of breakpoints and multipliers
    float psuedoExponential(float input, float breakpoints[], float multipliers[], uint8_t len){
        float add = 0;
        for(uint8_t i = 0; i < len; i++){
            if (input < breakpoints[i]){
                return input + add;
            }
            add += multipliers[i] * (input - breakpoints[i]);
        } 
        return input + add;
    }

    // Piecewise f(x)=m*x function
    // This allows the entire range of a settings knob to be fine-tuned to musically pleasing numbers.
    //   For example, let b = {10, 20, 30}, t = {20, 200, 2000}
    //   piecewiseLinear(5, b, t, 2)  = 10
    //   piecewiseLinear(15, b, t, 2) = 120 
    //   piecewiseLinear(25, b, t, 2) = 1200
    //
    // @param input
    // @param breakpoints
    // @param targets
    // @param len: the length of breakpoints & targets (must be the same)
    float piecewiseLinear(uint16_t input, uint16_t breakpoints[], float targets[], uint8_t len){
        if (input >= breakpoints[len-1]) {
            return targets[len-1];
        }

        uint8_t i;
        for(i = 0; i < len; i++){
            if (input <= breakpoints[i]){
                break;
            }
        }

        float prev_break = breakpoints[i];
        float prev_sum = 0;
        if (i > 0) {
            prev_sum = targets[i-1];
        }
        float ret = (((float) (input - prev_break))/((float) breakpoints[i])) * targets[i] + prev_sum;
        //return min(targets[len], ret);
        return ret;
    }
} // namespace ev_dsp