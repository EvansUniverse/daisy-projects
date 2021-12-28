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

using namespace patch_gui;

MenuItem::MenuItem(){};

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
    hidden   = false;
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

void MenuItem::setIndex(uint8_t i){
    if (i > size() - 1) {
        i = size() - 1;
    }
    index = i;
    callback();
};

void MenuItem::setHidden(bool b){
    hidden = b;
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

uint8_t MenuItem::getIndex(){
    return index;
}

uint8_t MenuItem::size(){
    return static_cast<uint8_t>(values.size());
}

float MenuItem::floatSize(){
    return static_cast<float>(values.size());
}

bool MenuItem::getHidden(){
    return hidden;
};