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

namespace patch_gui {

    // MenuItem that holds an int value
    class MenuItemInt: public MenuItem {
    private:
        int16_t min;
        int16_t max;

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

        // If given an illegal value, will convert it to the nearest legal value
        // @param desired index value
        void setIndex(int16_t);

        
        std::string getDisplayString();

        // @return a string representation of index, suitable for display
        std::string getValue();

        // @return maximum range of values for index
        uint32_t size();

        // @return index
        int16_t getIntValue();
    };
}