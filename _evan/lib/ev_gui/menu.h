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

namespace ev_gui {
    class Menu {
    private:
        // Maps item names indices, enabling O(1) getItem() calls
        // Indices in the vector correspond to page number
        std::vector<std::map<std::string, uint8_t>> itemNameToIndex;

        // Currently selected menu item (will appear at the top of the list)
        uint8_t index;

        // Contains each page of menu items
        // Indices in the vector correspond to page number
        std::vector<std::vector<MenuItem*>> pages;

        // Title of each page, to be displayed in header
        std::vector<std::string> pageNames;

        // Index of the current page
        uint8_t page;

        std::vector<bool> hiddenPages;
    public:
        // Don't use
        // TODO make it create a 0 page menu
        Menu();

        // @param number of pages
        Menu(uint8_t);

        // Creates and appends a new MenuItem
        //
        // @param page the item is appeded to (0-indexed)
        // @param name of the menu that will be displayed on screen
        // @param pad
        // @param list of possible values
        // @param index of default value, must be a valid index
        // @param callback function that runs every time the MenuItem's value is changed
        void append(uint8_t, std::string, std::string, std::vector<std::string>, uint8_t , std::function<void ()>);

        // Creates and appends a new MenuItemInt
        //
        // @param page the item is appended to (0-indexed)
        // @param name of the menu that will be displayed on screen
        // @param pad
        // @param min
        // @param max
        // @param defualt
        // @param callback function that runs every time the MenuItemInt's value is changed
        void append(uint8_t, std::string, std::string, int16_t, int16_t, int16_t , std::function<void ()>);

        /* Setters */

        // If given an out of bounds value, sets it to the nearest in-bounds value.
        // @param index
        void setIndex(int);

        // @param page number
        void setPage(uint8_t i){ page = i; };

        void setPageName(uint8_t i, std::string s){ pageNames[i] = s; };

        // @param index
        // @param isHidden
        void setIsHidden(uint8_t i, bool b){
            hiddenPages[i] = b;
        };

        /* Getters */

        // Returns the given item in the current page
        // @param title
        MenuItem* getItem(std::string);

        // Returns the given item in the given page
        // @param page number
        // @param title
        MenuItem* getItem(uint8_t, std::string);

        // Returns the given item in the current page
        // @param index within page
        MenuItem* getItem(uint8_t);

        // Returns the given item in the given page
        // @param index within page
        // @param page number
        MenuItem* getItem(uint8_t, uint8_t);

        // @return the currently selected item
        MenuItem* getItem();

        // @return index
        uint8_t getIndex();

        // @return page (0-indexed)
        uint8_t getPage(){ return page; };

        // @param page index
        // @return page title
        std::string getPageName(uint8_t i){ return pageNames[i]; };

        // @return number of pages
        uint8_t getNumPages(){ return static_cast<uint8_t>(pages.size()); };

        bool getIsHidden(uint8_t i){ return hiddenPages[i]; };

        // Number of elements in the current page
        uint8_t size();

        // Number of elements in the given page
        // @param page
        uint8_t size(uint8_t);
    };
}