/* 
 * Copyright (C) 2021 Evan Pernu - All Rights Reserved
 * You may use, distribute and modify this code under the
 * terms of the GNU AGPLv3 license.
 * 
 * You should have received a copy of the GNU AGPLv3 license with
 * this file (LICENSE.md). If not, please write to: evanpernu@gmail.com, 
 * or visit: https://www.gnu.org/licenses/agpl-3.0.en.html
 */

#pragma once

#include <functional>
#include <vector>
#include <string>

namespace jellybeans {

    // Menu item whos value is an element of a list of predefined strings
    // They are also used to store settings
    class MenuItem {
    private:
        int index;
        std::string name;
        std::string displayName; // name with spaces appended
        std::vector<std::string> values;

        std::function<void ()> onChangeCallback;
    public:
        MenuItem();

        // @param name of the menu that will be displayed on screen
        // @param list of possible values
        // @param index of default value, must be a valid index // TODO maybe remove this?
        // @param callback function that runs every time the MenuItem's value is changed
        MenuItem(std::string, std::vector<std::string>, int , std::function<void ()>);

        std::string displayValue();

        std::string value();

        void increment();

        void decrement();

        void setIndex(int);

        // Executed every time this item's value is changed
        void onChange();
    };
}