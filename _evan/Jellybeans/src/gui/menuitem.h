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
    //
    // Currently does not support lengths > 255
    class MenuItem {
    private:
        std::string title;

        // String of spaces used by menu to make sure things line up neatly
        // TODO compute programatically
        std::string pad;

        // List of selectable values for this menuItem
        std::vector<std::string> values;

        // Position within values
        uint8_t index;

        // Callback function to be executed every time this MenuItem's
        // value is changed
        std::function<void ()> callback;

        bool hidden;
    public:
        // Don't use
        MenuItem();

        // @param name of the menu that will be displayed on screen
        // @param pad
        // @param list of possible values
        // @param index of default value, must be a valid index
        // @param callback function that runs every time the MenuItem's value is changed
        MenuItem(std::string, std::string, std::vector<std::string>, uint8_t , std::function<void ()>);

        // Increment/decrement index, call callback function
        void increment();
        void decrement();

        // If given an illegal value, will convert it to the nearest legal value
        // @param desired index value
        void setIndex(uint8_t);

        // @param if true, don't render
        void setHidden(bool);

        std::string getDisplayString();
        std::string getTitle();
        std::string getValue();
        uint8_t size();
        float floatSize();
        uint8_t getIndex();
        bool getHidden();
    };
}