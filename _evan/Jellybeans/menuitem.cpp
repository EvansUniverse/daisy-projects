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

MenuItem::MenuItem(std::string theTitle, std::vector<std::string> theValues, int theDefault, std::function<void ()> theCallback){
    values   = theValues;
    index    = theDefault;
    title    = theTitle;
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

void MenuItem::setIndex(int i){
    index = i;
    callback();
};

std::string MenuItem::getDisplayString() {
    return  title + getValue();
};

std::string MenuItem::getValue() {
    return values[index];
}