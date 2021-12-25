/* 
 * Copyright (C) 2021 Evan Pernu - All Rights Reserved
 * You may use, distribute and modify this code under the
 * terms of the GNU AGPLv3 license.
 * 
 * You should have received a copy of the GNU AGPLv3 license with
 * this file (LICENSE.md). If not, please write to: evanpernu@gmail.com, 
 * or visit: https://www.gnu.org/licenses/agpl-3.0.en.html
 */

#pragma once

#include "menuitem.h"

#include <map>
#include <string>
#include <vector>

namespace patch_gui {
    class Menu {
    private:
        std::vector<MenuItem*> items;

        // Allows O(log(n)) retrieval by name
        std::map<std::string, int> itemNameToIndex;

        // Currently selected menu item (will appear at the top of the list)
        int index;
    public:
        Menu();

        // Creates and appends a new MenuItem
        // @param name of the menu that will be displayed on screen
        // @param pad
        // @param list of possible values
        // @param index of default value, must be a valid index
        // @param callback function that runs every time the MenuItem's value is changed
        void append(std::string, std::string, std::vector<std::string>, int , std::function<void ()>);

        int size();

        // @param name
        MenuItem* getItem(std::string);

        // @param index
        MenuItem* getItem(int);

        // @return the currently selected item
        MenuItem* getItem();

        int getIndex();

        // If given an out of bounds value, sets it to the nearest in-bounds value.
        // @param index
        void setIndex(int);
    };
}