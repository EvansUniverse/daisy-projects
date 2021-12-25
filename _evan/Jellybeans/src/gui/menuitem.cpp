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

MenuItem::MenuItem(std::string theTitle, std::string thePad, std::vector<std::string> theValues, int theDefault, std::function<void ()> theCallback){
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
    index--;
    if (index < 0){
        index = values.size() - 1;
    }
    callback();
};

/* Setters */

void MenuItem::setIndex(int i){
    index = i;
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

int MenuItem::getIndex(){
    return index;
}

int MenuItem::size(){
    return static_cast<int>(values.size());
}

float MenuItem::floatSize(){
    return static_cast<float>(values.size());
}