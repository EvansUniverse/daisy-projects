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

namespace patch_gui {

    // Menu item whos value is an element of a list of predefined strings
    // They are also used to store settings
    class MenuItem {
    private:
        std::string title;

        // String of spaces used by menu to make sure things line up neatly
        // TODO compute programatically
        std::string pad;

        // List of selectable values for this menuItem
        std::vector<std::string> values;

        // Position within values
        int index;

        // Callback function to be executed every time this MenuItem's
        // value is changed
        std::function<void ()> callback;
    public:
        // Don't use
        MenuItem();

        // @param name of the menu that will be displayed on screen
        // @param pad
        // @param list of possible values
        // @param index of default value, must be a valid index
        // @param callback function that runs every time the MenuItem's value is changed
        MenuItem(std::string, std::string, std::vector<std::string>, int , std::function<void ()>);

        // Increment/decrement index, call callback function
        void increment();
        void decrement();

        // @param desired index value, within bounds of len(values)
        void setIndex(int);

        std::string getDisplayString();
        std::string getTitle();
        std::string getValue();
        int size();
        float floatSize();
        int getIndex();
    };
}