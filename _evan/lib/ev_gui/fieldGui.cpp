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

#include "daisy_field.h"
#include "fieldGui.h"

#define HYSTERESIS_THRESH 0.005

using namespace daisy;
using namespace ev_gui;

FieldGui::FieldGui(
        DaisyField* theHw,
        Menu*    theMenu,
        FontDef* theFont,
        uint8_t theWidth,
        uint8_t theHeight,
        uint8_t numHeaders
){
    hw         = theHw;
    menu       = theMenu;
    font       = theFont;
    fontWidth  = theWidth;
    fontHeight = theHeight;

    headers = std::vector<std::string>(numHeaders);
    for (uint8_t i = 0; i < headers.size(); i++){
        headers[i] = "";
    }

    // for some reason, after I updatetd libDaisy on 11/5/2022, `popupLines[0] = "_END_";` caused a hard fault..?
    popupLines.push_back("_END_"); 
    knobsDisabled = false;
    popupRemainingCycles = 0;

    knobVals      = std::array<float,     8>{0, 0, 0, 0, 0, 0, 0, 0};
    knobIsChanged = std::array<bool,      8>{false, false, false, false, false, false, false, false};
    cvVals        = std::array<float,     4>{0, 0, 0, 0};
    cvMenuItems   = std::array<MenuItem*, 4>{NULL, NULL, NULL, NULL};
}

void FieldGui::drawString(std::string str, uint8_t x, uint8_t y){
    hw->display.SetCursor(x, y);
    char* cstr = &str[0];
    hw->display.WriteString(cstr, *font, true);
}

void FieldGui::drawStartupScreen(std::string s, std::string v, uint32_t t){
    hw->display.Fill(false);
    drawBoxMessage(s);

    // Draw v
    uint8_t len = v.size() * fontWidth;
    uint8_t x = (SCREEN_WIDTH - len)/2;
    uint8_t y = SCREEN_HEIGHT - fontHeight;
    drawString(v, x, y);

    hw->display.Update();
    hw->DelayMs(t);
}

void FieldGui::drawBoxMessage(std::string s){
    uint8_t len = s.size() * fontWidth;
    uint8_t x = (SCREEN_WIDTH - len)/2;
    uint8_t y = (SCREEN_HEIGHT - fontHeight)/2;
    drawString(s, x, y+2);
    hw->display.DrawRect(x-5, y-5, SCREEN_WIDTH - x + 5, SCREEN_HEIGHT - y + 5, true);
}

void FieldGui::updateControls(){
    // TODO disabling this until it's needed
    //
    // // Process CV input
    // for (uint8_t i = 0; i < 4; i++){
    //     float val = hw->GetCvValue(i);
    //     MenuItem* m = cvMenuItems[i];
    //     if (m != NULL && std::abs(cvVals[i] - val) > HYSTERESIS_THRESH){
    //         cvVals[i] = val;
    //         m->setIndex(static_cast<uint16_t>(val * m->floatSize()));
    //     }
    // }

    // Determine which knobs have been turned
    for(uint8_t i = 0; i < 8; i++){
        float knobVal = hw->GetKnobValue(i); 

        if (std::abs(knobVals[i] - knobVal) > HYSTERESIS_THRESH) {
            knobVals[i] = knobVal;
            knobIsChanged[i] = true;
        } else {
            knobIsChanged[i] = false;
        }
    }

    // Process Knob input
    if(!knobsDisabled){
        for (uint8_t i = 0; i < 8; i++){
            if (i < menu->size() && knobIsChanged[i]){
                menu->getItem(i)->setIndexByFloat(hw->GetKnobValue(i));
            }
        }
    }
}

// Display on Daisy Field is 128x64p
// With 7x10 font, this means it's limited to:
//  - 18 chars horizontally (w/2p to spare)
//  - 6 chars vertically (w/4p to spare)
void FieldGui::render(){
    uint8_t y = 0;

    // Draw headers
    for (uint8_t i = 0; i < headers.size(); i++){
        if (i % 2 == 0){
            if (i != 0){
                y += fontHeight+1;
            }
            drawString(headers[i], 0, y);
        } else {
            drawString(headers[i], SCREEN_WIDTH - (headers[i].size()*fontWidth), y);
        }
    }
    y += fontHeight+1;

    // Draw line under headers
    hw->display.DrawLine(0, y, SCREEN_WIDTH, y, true);
    y+=2;

    if(popupRemainingCycles > 0){
        // Draw popup
        popupRemainingCycles--;
        for(uint8_t i = 0; popupLines[i] != "_END_"; i++){
            drawString(popupLines[i], 0, y);
            y += fontHeight;
        }
    } else {
        // Draw each menu item
        uint8_t listSize = (SCREEN_HEIGHT - y) / fontHeight; 
        for(uint8_t i = 0; (i < listSize && i < menu->size()); i++){
            drawString(std::to_string(i + 1) + "  " + menu->getItem(i)->getDisplayString(), 0, y);
            y += fontHeight;  
        }
    }

    // Write display buffer to OLED
    hw->display.Update();
}

void FieldGui::assignCV(uint8_t page, std::string title, uint8_t cvNum){
    cvMenuItems[cvNum-1] = menu->getItem(title);
}

void FieldGui::unassignCV(uint8_t cvNum){
    cvMenuItems[cvNum-1] = NULL;
}

void FieldGui::pageFwd(){
    if(menu->getNumPages() <= menu->getPage() + 1){
        menu->setPage(0);
    } else {
        menu->setPage(menu->getPage() + 1);
    }
};

void FieldGui::pageBack(){
    if(menu->getPage() == 0){
        menu->setPage(menu->getNumPages()-1);
    } else {
        menu->setPage(menu->getPage() - 1);
    }
};

/* Setters */

void FieldGui::setPopup(std::vector<std::string> lines, uint32_t cycles){
    popupLines = lines;
    popupLines.push_back("_END_");
    popupRemainingCycles = cycles;
}

void FieldGui::keepPopup(){
    popupRemainingCycles = 1;
}

void FieldGui::setHeader(std::string s, uint8_t i){
    headers[i] = s;
}
