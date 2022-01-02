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

#include "menuItem.h"
#include "menuItemInt.h"

#include <map>
#include <string>
#include <vector>

namespace patch_gui {
    class Menu {
    private:
        std::vector<MenuItem*> items;

        // Allows more performant getItem() calls
        std::map<std::string, uint8_t> itemNameToIndex;

        // Currently selected menu item (will appear at the top of the list)
        uint8_t index;
    public:
        Menu();

        // Creates and appends a new MenuItem
        //
        // @param name of the menu that will be displayed on screen
        // @param pad
        // @param list of possible values
        // @param index of default value, must be a valid index
        // @param callback function that runs every time the MenuItem's value is changed
        void append(std::string, std::string, std::vector<std::string>, uint8_t , std::function<void ()>);

        // Creates and appends a new MenuItemInt
        //
        // @param name of the menu that will be displayed on screen
        // @param pad
        // @param min
        // @param max
        // @param defualt
        // @param callback function that runs every time the MenuItemInt's value is changed
        void append(std::string, std::string, int16_t, int16_t, int16_t , std::function<void ()>);

        uint8_t size();

        // @param title
        MenuItem* getItem(std::string);

        // @param index
        MenuItem* getItem(uint8_t);

        // @return the currently selected item
        MenuItem* getItem();


        // @return index
        uint8_t getIndex();

        // If given an out of bounds value, sets it to the nearest in-bounds value.
        // @param index
        void setIndex(int);
    };
}