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

#include "menuItem.h"
#include "menuItemInt.h"

#include <string>
#include <vector>
#include <functional>

using namespace ev_gui;

MenuItemInt::MenuItemInt(
        std::string theTitle,
        std::string thePad,
        int16_t theMin,
        int16_t theMax,
        int16_t theDefault,
        std::function<void ()> theCallback
){
    title    = theTitle;
    pad      = thePad;
    min      = theMin;
    max      = theMax;
    index    = theDefault;
    callback = theCallback;
    notch    = 0;
    notchLen = 0;
};

void MenuItemInt::increment(){
    if (index < max){
        index++;
    }  
    callback();
};

void MenuItemInt::decrement(){
    if (index > min){
        index--;
    }
    callback();
};

/* Setters */

void MenuItemInt::setIndex(int16_t i){
    if(i >= notch - notchLen && i <= notch + notchLen){
        index = notch;
    } else {
        index = i;
    }
    
    callback();
};

void MenuItemInt::setIndexByFloat(float i){
    // C++ float->int casting always rounds down, add 0.5 to round both ways
    index = min + static_cast<uint16_t>((i * (floatSize() + 0.5f)));
    callback();
};

void MenuItemInt::setMax(int16_t i){
    max = i;
    if(index > max){
        index = max;
    }
};

/* Getters */

std::string MenuItemInt::getDisplayString() {
    return title + pad + getValue();
};

std::string MenuItemInt::getValue() {
    return std::to_string(index);
}

uint32_t MenuItemInt::size(){
    return std::abs(max - min);
}

int16_t MenuItemInt::getMin(){
    return min;
}