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
        std::string title;

        // List of selectable values for this menuItem
        std::vector<std::string> values;

        // Position within values
        int index;

        // Callback function to be executed every time this MenuItem's
        // value is changed
        std::function<void ()> callback;
    public:
        MenuItem();

        // @param name of the menu that will be displayed on screen
        // @param list of possible values
        // @param index of default value, must be a valid index // TODO maybe remove this?
        // @param callback function that runs every time the MenuItem's value is changed
        MenuItem(std::string, std::vector<std::string>, int , std::function<void ()>);

        // Increment/decrement index, call callback function
        void increment();
        void decrement();

        // @param desired index value, within bounds of len(values)
        void setIndex(int);

        std::string getDisplayString();
        std::string getValue();
    };
}