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

#include "menu.h"

#include <map>
#include <string>
#include <vector>

using namespace ev_gui;

Menu::Menu(uint8_t numPages){
    index = 0;
    page = 0;

    for(uint8_t i = 0; i < numPages; i++){
        std::map<std::string, uint8_t> m;
        itemNameToIndex.push_back(m);

        std::vector<MenuItem*> v;
        pages.push_back(v);

        pageNames.push_back("");
        hiddenPages.push_back(false);
    }
}

void Menu::append(
        uint8_t thePage,
        std::string theTitle,
        std::string thePad,
        std::vector<std::string> theValues,
        uint8_t theDefault,
        std::function<void ()> theCallback
){
    MenuItem* m = new MenuItem(theTitle, thePad, theValues, theDefault, theCallback);
    pages[thePage].push_back(m);
    itemNameToIndex[thePage][m->getTitle()] = static_cast<uint8_t>(pages[thePage].size()-1);
}

void Menu::append(
        uint8_t thePage,
        std::string theTitle,
        std::string thePad,
        int16_t theMin,
        int16_t theMax,
        int16_t theDefault,
        std::function<void ()> theCallback
){
    MenuItem* m = new MenuItemInt(theTitle, thePad, theMin, theMax, theDefault, theCallback);
    pages[thePage].push_back(m);
    itemNameToIndex[thePage][m->getTitle()] = static_cast<uint8_t>(pages[thePage].size()-1);
}

/* Setters */

// TODO consider a "wraparound" behavior where scrolling past the end takes you back
// to the beginning (maybe make it a configurable option)
void Menu::setIndex(int i){
    if (i >= size()){
        i = size() - 1;
    } else if (i < 0) {
        i = 0;
    }
    index = i;
}

/* Getters */

uint8_t Menu::size(){
    return static_cast<uint8_t>(pages[page].size());
}

uint8_t Menu::size(uint8_t p){
    return static_cast<uint8_t>(pages[p].size());
}

MenuItem* Menu::getItem(std::string name){
    return pages[page][itemNameToIndex[page].at(name)];
}

MenuItem* Menu::getItem(uint8_t p, std::string name){
    return pages[p][itemNameToIndex[p].at(name)];
}

MenuItem* Menu::getItem(uint8_t i){
    return pages[page][i];
}

MenuItem* Menu::getItem(uint8_t i, uint8_t p){
    return pages[p][i];
}

MenuItem* Menu::getItem(){
    return pages[page][index];
}

uint8_t Menu::getIndex(){
    return index;
}