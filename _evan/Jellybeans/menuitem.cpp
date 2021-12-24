/* 
 * Copyright (C) 2021 Evan Pernu - All Rights Reserved
 * You may use, distribute and modify this code under the
 * terms of the GNU AGPLv3 license.
 * 
 * You should have received a copy of the GNU AGPLv3 license with
 * this file (LICENSE.md). If not, please write to: evanpernu@gmail.com, 
 * or visit: https://www.gnu.org/licenses/agpl-3.0.en.html
 */

#include "menuitem.h"

#include <string>
#include <vector>
#include <functional>

using namespace jellybeans;

MenuItem::MenuItem(){};

// aDefault must be a valid index in aValues
MenuItem::MenuItem(std::string aName, std::vector<std::string> aValues, int aDefault, std::function<void ()> cb){
    name = aName;
    values = aValues;
    index = aDefault;
    onChangeCallback = cb;

    // TODO: implement a menu class that contains a list of menuItems, it can track this
    // For everything to line up neatly, each menu item's displayName 
    // needs to be padded with enough spaces to be in line with the 
    // end of the longest name, plus 1 additional space. The longest 
    // menu item name is currently "Inversion" with 10 chars.
    displayName = aName;
    for (unsigned int i = 0; i < 10 - name.length(); i++) {
        displayName += " ";
    }
};

std::string MenuItem::displayValue() {
    return  displayName + values[index];
};

std::string MenuItem::value() {
    return values[index];
}

void MenuItem::increment(){
    index++;
    index = index % values.size();
    onChange();
};

void MenuItem::decrement(){
    index--;
    if (index < 0){
        index = values.size() - 1;
    }
    onChange();
};

void MenuItem::setIndex(int i){
    index = i;
    onChange();
};

// Executed every time this item's value is changed
void MenuItem::onChange(){
    onChangeCallback();
};