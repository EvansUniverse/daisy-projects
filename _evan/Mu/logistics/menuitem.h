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

namespace mu {

    // Menu item whos value is an element of a list of predefined strings
    // They are also used to store settings
    class MenuItem {
    public:
        int index;
        std::string name;
        std::string displayName; // name with spaces appended
        std::vector<std::string> values;

        // typedef void (*OnChangeCallback)();
        // OnChangeCallback myOnChangeCallback;
        std::function<void ()> myOnChangeCallback;

        MenuItem();

        // aDefault must be a valid index in aValues
        MenuItem(std::string, std::vector<std::string>, int , std::function<void ()>);

        std::string DisplayValue();

        std::string Value();

        void Increment();

        void Decrement();

        void SetIndex(int);

        // Executed every time this item's value is changed
        void OnChange();
    };
}