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

    // Menu item whos value is an element of a list of predefined strings.
    class MenuItem {
    protected:
        // Name of the parameter that displays on-screen, also used to 
        // retrieve this item from a Menu.
        std::string title;

        // String of spaces used by menu to make sure things line up neatly
        // TODO compute programatically
        std::string pad;

        // List of selectable values for this menuItem
        std::vector<std::string> values;

        // Index within values
        int16_t index;

        // Callback function to be executed every time this MenuItem's
        // value is changed
        std::function<void ()> callback;

    public:
        // Don't use this
        MenuItem(){};

        // @param name of the menu that will be displayed on screen
        // @param pad
        // @param list of possible values
        // @param index of default value, must be a valid index
        // @param callback function that runs every time the MenuItem's value is changed
        MenuItem(std::string, std::string, std::vector<std::string>, uint8_t , std::function<void ()>);

        // Increment/decrement index, call callback function
        virtual void increment();
        virtual void decrement();

        // If given an illegal value, will convert it to the nearest legal value
        // @param desired index value
        virtual void setIndex(int16_t);

        // Sets the index based on a float from 0-1 (representing 0-100%)
        // Used when reading in CV/knob values
        // @param a float between 0-1
        virtual void setIndexByFloat(float);

        virtual void setNotch(int16_t n, uint16_t l){};

        virtual std::string getDisplayString();
        std::string getTitle();
        virtual std::string getValue();
        virtual uint32_t size();
        float floatSize();
        virtual int16_t getIndex();
        float getFloatIndex();

        // The minimum possible value of index
        virtual int16_t getMin(){return 0;};
    };
}