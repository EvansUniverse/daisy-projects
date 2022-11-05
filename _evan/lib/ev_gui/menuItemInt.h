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

#pragma once

#include <functional>
#include <vector>
#include <string>

namespace ev_gui {

    // MenuItem that holds an int value
    class MenuItemInt: public MenuItem {
    private:
        int16_t min;
        int16_t max;

        int16_t notch;
        uint16_t notchLen;

    public:
        // Don't use this
        MenuItemInt(){};

        // @param name of the menu that will be displayed on screen
        // @param pad
        // @param min
        // @param max
        // @param defualt
        // @param callback function that runs every time the MenuItem's value is changed
        MenuItemInt(std::string, std::string, int16_t, int16_t, int16_t , std::function<void ()>);

        void increment();
        void decrement();

        /* Setters */

        // If given an illegal value, will convert it to the nearest legal value
        // Sets index and calls callback
        // @param desired index value
        void setIndex(int16_t);

        // Sets the index based on a float from 0-1 (representing 0-100%)
        // Used when reading in CV/knob values
        // Sets index and calls callback
        // @param a float between 0-1
        void setIndexByFloat(float);

        // If max > current index, sets index to max
        // @param max
        void setMax(int16_t);

        // Sets a digital "Notch", making it easiers for users to set certain exact values
        //
        // @param Notch
        // @param values this far on either side of the notch will be set to the notch value
        void setNotch(int16_t n, uint16_t l){ 
            notch = n;
            notchLen = l;
        };

        /* Getters */

        std::string getDisplayString();

        // @return a string representation of index, suitable for display
        std::string getValue();

        // @return maximum range of values for index
        uint32_t size();

        int16_t getMin();
    };
}