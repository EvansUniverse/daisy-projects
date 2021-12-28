/* 
 * Copyright (C) 2021 Evan Pernu - All Rights Reserved
 * You may use, distribute and modify this code under the
 * terms of the GNU AGPLv3 license.
 * 
 * You should have received a copy of the GNU AGPLv3 license with
 * this file (LICENSE.md). If not, please write to: evanpernu@gmail.com, 
 * or visit: https://www.gnu.org/licenses/agpl-3.0.en.html
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