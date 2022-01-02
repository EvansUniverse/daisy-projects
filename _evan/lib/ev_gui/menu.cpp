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

using namespace patch_gui;

Menu::Menu(){
    index = 0;
}

void Menu::append(
        std::string theTitle,
        std::string thePad,
        std::vector<std::string> theValues,
        uint8_t theDefault,
        std::function<void ()> theCallback
){
    MenuItem* m = new MenuItem(theTitle, thePad, theValues, theDefault, theCallback);
    items.push_back(m);
    itemNameToIndex[m->getTitle()] = static_cast<uint8_t>(items.size()-1);
}

void Menu::append(
        std::string theTitle,
        std::string thePad,
        int16_t theMin,
        int16_t theMax,
        int16_t theDefault,
        std::function<void ()> theCallback
){
    MenuItem* m = new MenuItemInt(theTitle, thePad, theMin, theMax, theDefault, theCallback);
    items.push_back(m);
    itemNameToIndex[m->getTitle()] = static_cast<uint8_t>(items.size()-1);
}

/* Setters */

// TODO consider a "wraparound" behavior where scrolling past the end takes you back to the beginning
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
    return static_cast<uint8_t>(items.size());
}

MenuItem* Menu::getItem(std::string name){
    return items[itemNameToIndex.at(name)];
}

MenuItem* Menu::getItem(uint8_t i){
    return items[i];
}

MenuItem* Menu::getItem(){
    return items[index];
}

uint8_t Menu::getIndex(){
    return index;
}