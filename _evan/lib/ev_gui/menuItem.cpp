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

#include <string>
#include <vector>
#include <functional>

using namespace ev_gui;

MenuItem::MenuItem(
        std::string theTitle,
        std::string thePad,
        std::vector<std::string> theValues,
        uint8_t theDefault,
        std::function<void ()> theCallback
){
    title    = theTitle;
    pad      = thePad;
    values   = theValues;
    index    = theDefault;
    callback = theCallback;
};

void MenuItem::increment(){
    index++;
    index = index % values.size();
    callback();
};

void MenuItem::decrement(){
    if (index == 0){
        index = values.size() - 1;
    } else {
        index--;
    }
    callback();
};

/* Setters */

void MenuItem::setIndex(int16_t i){
    if (i > static_cast<int16_t>(values.size()) - 1) {
        i = static_cast<int16_t>(values.size()) - 1;
    } else if (i <= 0) {
        i = 0;
    }
    index = i;
    callback();
};

void MenuItem::setIndexByFloat(float i){
    index = static_cast<uint16_t>(i * floatSize());
    callback();
};

/* Getters */

std::string MenuItem::getDisplayString() {
    return  title + pad + getValue();
};

std::string MenuItem::getTitle() {
    return  title;
};

std::string MenuItem::getValue() {
    return values[index];
}

int16_t MenuItem::getIndex(){
    return index;
}

uint32_t MenuItem::size(){
    return static_cast<uint32_t>(values.size());
}

float MenuItem::floatSize(){
    return static_cast<float>(size());
}

float MenuItem::getFloatIndex(){
    return static_cast<float>(getIndex());
}