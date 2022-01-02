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

#include "daisy_patch.h"
#include "patchGui.h"

using namespace daisy;
using namespace patch_gui;

// @param patch reference
// @param FontDef
// @param Font width
// @param Font height
PatchGui::PatchGui(
        DaisyPatch* thePatch,
        Menu* theMenu,
        FontDef* theFont,
        uint8_t theWidth,
        uint8_t theHeight,
        uint8_t numHeaders
){
    patch      = thePatch;
    menu       = theMenu;
    font       = theFont;
    fontWidth  = theWidth;
    fontHeight = theHeight;
    isEditing  = false;

    headers = std::vector<std::string>(numHeaders);
    for (uint8_t i = 0; i < headers.size(); i++){
        headers[i] = "";
    }

    cvParams    = std::array<Parameter*, 4>{NULL, NULL, NULL, NULL};
    cvVals      = std::array<uint16_t,   4>{0, 0, 0, 0};
    cvMenuItems = std::array<MenuItem*,  4>{NULL, NULL, NULL, NULL};
}

void PatchGui::assignToCV(std::string title, uint8_t cvNum){
    cvNum--;
    Parameter* p = new Parameter();
    MenuItem* m = menu->getItem(title);

    cvParams[cvNum] = p;
    p->Init(patch->controls[cvNum], 0.f, static_cast<float>(m->floatSize()), Parameter::LINEAR);
    cvMenuItems[cvNum] = m;
    cvVals[cvNum] = static_cast<uint16_t>(p->Process());
}

void PatchGui::drawString(std::string str, uint8_t x, uint8_t y){
    patch->display.SetCursor(x, y);
    char* cstr = &str[0];
    patch->display.WriteString(cstr, *font, true);
}

void PatchGui::drawStartupScreen(std::string s, std::string v, uint32_t t){
    patch->display.Fill(false);

    // Draw s inside a rectangle
    uint8_t len = s.size() * fontWidth;
    uint8_t x = (SCREEN_WIDTH - len)/2;
    uint8_t y = (SCREEN_HEIGHT - fontHeight)/2;
    drawString(s, x, y+2);
    patch->display.DrawRect(x-5, y-5, SCREEN_WIDTH - x + 5, SCREEN_HEIGHT - y + 5, true);

    // Draw v
    len = v.size() * fontWidth;
    x = (SCREEN_WIDTH - len)/2;
    y = SCREEN_HEIGHT - fontHeight;
    drawString(v, x, y);

    patch->display.Update();
    patch->DelayMs(t);
}

void PatchGui::updateControls(){
    // Process CV input
    for (uint8_t i = 0; i < 4; i++){
        if (cvParams[i] !=  NULL) {
            uint16_t curCvVal = static_cast<uint16_t>(cvParams[i]->Process());
            if(curCvVal != cvVals[i]){
                cvMenuItems[i]->setIndex(curCvVal);
                cvVals[i] = curCvVal;
            }
        }
    }

    // Process encoder input
    if(!isEditing){
        // Update menu position
        menu->setIndex(menu->getIndex() + patch->encoder.Increment());
        isEditing = patch->encoder.RisingEdge();
    } else {
        // Update selected menu item's value
        int inc = patch->encoder.Increment();
        if (inc > 0){
            menu->getItem()->increment();
        } else if (inc < 0){
            menu->getItem()->decrement();
        }

        isEditing = !patch->encoder.RisingEdge();
    }
}

// Display on Daisy Patch is 128x64p
// With 7x10 font, this means it's limited to:
//  - 18 chars horizontally (w/2p to spare)
//  - 6 chars vertically (w/4p to spare)
void PatchGui::render(){
    uint8_t y = 0;

    // Clear display
    patch->display.Fill(false);  

    // Draw headers
    for (uint8_t i = 0; i < headers.size(); i++){
        if (i % 2 == 0){
            drawString(headers[i], 0, y);
        } else {
            drawString(headers[i], SCREEN_WIDTH - (headers[i].size()*fontWidth), y);
            y += fontHeight + 1;
        }
    }

    // Draw line under headers
    patch->display.DrawLine(0, y, SCREEN_WIDTH, y, true);

    // Draw cursor indicator
    y++;
    if (isEditing){
        drawString("*", 0, y);
    } else {
        drawString(">", 0, y);
    }

    // Draw each menu item
    uint8_t listSize = (SCREEN_HEIGHT - y) / fontHeight; 
    y++;
    for(uint8_t i = 0; i < listSize; i++){
        if (menu->getIndex() + i < menu->size()){
            drawString(menu->getItem(menu->getIndex() + i)->getDisplayString(), fontWidth, y);
            y += fontHeight;
        }    
    }
    //drawString(std::to_string(listSize) + " " + std::to_string(y) + "         ", 0, 0);

    // Write display buffer to OLED
    patch->display.Update();
}


void PatchGui::setHeader(std::string s, uint8_t i){
    headers[i] = s;
}
