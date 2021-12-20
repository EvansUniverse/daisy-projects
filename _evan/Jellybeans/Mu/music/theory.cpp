/* 
 * Copyright (C) 2021 Evan Pernu - All Rights Reserved
 * You may use, distribute and modify this code under the
 * terms of the GNU AGPLv3 license.
 * 
 * You should have received a copy of the GNU AGPLv3 license with
 * this file (LICENSE.md). If not, please write to: evanpernu@gmail.com, 
 * or visit: https://www.gnu.org/licenses/agpl-3.0.en.html
 */

#include <algorithm>
#include <string> 
#include <vector>

#include "theory.h"

using namespace mu;

bool isDiatonic(int note, std::string mode){
    std::vector<int> v = modeToSemitones.at(mode);
    return std::find(v.begin(), v.end(), note % 12) != v.end();
};