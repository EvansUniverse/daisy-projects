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

#define SHORT_HOLD_DURATION_MS 300
#define LONG_HOLD_DURATION_MS 2500
#define HOLD_BAR_WIDTH 24
#define HOLD_BAR_HEIGHT 4

#include "daisy_patch.h"
#include "patchGui.h"

using namespace daisy;
using namespace ev_gui;

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
    didHold    = false;
    timeHeld   = 0.f;
    cvCatch    = false;
    zoneB      = theMenu->getNumPages()-1;
    isZoneB    = false;
    drawHoldBar = false;

    pageMode = menu->getNumPages() > 1;

    headers = std::vector<std::string>(numHeaders);
    for (uint8_t i = 0; i < headers.size(); i++){
        headers[i] = "";
    }

    // for some reason, after I updatetd libDaisy on 11/5/2022, `popupLines[0] = "_END_";` caused a hard fault..?
    popupLines.push_back("_END_"); 
    
    popupRemainingCycles = 0;

    cvParams      = std::array<Parameter*, 4>{NULL, NULL, NULL, NULL};
    cvVals        = std::array<float,      4>{0.f, 0.f, 0.f, 0.f};
    cvMenuItems   = std::array<MenuItem*,  4>{NULL, NULL, NULL, NULL};
    cvIsCaught    = std::array<bool,       4>{false, false, false, false};
}

void PatchGui::assignToCV(std::string title, uint8_t cvNum){
    assignToCV(menu->getItem(title), cvNum);
}

void PatchGui::assignToCV(MenuItem* m, uint8_t cvNum){
    cvNum--;
    Parameter* p = new Parameter();

    cvIsCaught[cvNum] = false;

    cvParams[cvNum] = p;
    p->Init(patch->controls[cvNum], 0.f, static_cast<float>(m->floatSize()), Parameter::LINEAR);
    cvMenuItems[cvNum] = m;
    cvVals[cvNum] = p->Process();
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
            float  curCvVal = cvParams[i]->Process();

            // !!! HACK !!!
            // The cv values received by patches pots aren't a perfect 0.0 - 1.0; it differs a bit each device
            // (I've tested on 3) but is somewhere roughly around 0.005 - 0.98. In order to compensate for this
            // and allow users to get perfectly high/low values, we add/subtract to the value based on the
            // estimated knob position. This value was calculated based on trial and error to get the most 
            // comfortable result for the user.
            curCvVal += 0.028f * (curCvVal - menu->getItem(i)->floatSize() / 2) + menu->getItem(i)->floatSize() * 0.011f;
            if (curCvVal < 0){
                curCvVal = 0;
            } else if (curCvVal > menu->getItem(i)->floatSize()){
                curCvVal = menu->getItem(i)->floatSize();
            }

            // If the current CV value is within 3% of the current menuItem value, "catch" the knob
            float catchTolerance = 0.03f * menu->getItem(i)->floatSize();
            if(cvCatch && !cvIsCaught[i]
                    && curCvVal + menu->getItem(i)->getMin() <= menu->getItem(i)->getFloatIndex() + catchTolerance
                    && curCvVal + menu->getItem(i)->getMin() >= menu->getItem(i)->getFloatIndex() - catchTolerance)
            {
                cvIsCaught[i] = true;
            }

            if(((cvCatch && cvIsCaught[i]) || !cvCatch) && std::abs(curCvVal - static_cast<float>(cvVals[i])) > (HYSTERESIS_THRESH * menu->getItem(i)->floatSize())){
                cvMenuItems[i]->setIndex(static_cast<uint16_t>(curCvVal) + menu->getItem(i)->getMin());
                cvVals[i] = curCvVal;
            }
        }
    }

    // Process encoder input
    if(pageMode){
        // In page mode, encoder changes page
        // Update selected menu item's value
        int32_t inc = patch->encoder.Increment();
        if (inc > 0){
            pageFwd();
        } else if (inc < 0){
            pageBack();
        }

        if(patch->encoder.Pressed() && !didHold){
            timeHeld = patch->encoder.TimeHeldMs();
        }
        
        if(didHold && patch->encoder.FallingEdge()){
            didHold = false;
        } else if(!didHold && patch->encoder.FallingEdge() && timeHeld < SHORT_HOLD_DURATION_MS){
            setPopup(msgPress, popupTimePress, popupTypePress);
            cbPress();
            didHold = false;
            timeHeld = 0;
        } else if(!didHold && patch->encoder.FallingEdge() && timeHeld < LONG_HOLD_DURATION_MS){
            setPopup(msgShortHold, popupTimeShortHold, popupTypeShortHold);
            cbShortHold();
            didHold = false;
            timeHeld = 0;
        } else if(!didHold && patch->encoder.Pressed() && timeHeld >= LONG_HOLD_DURATION_MS){
            setPopup(msgLongHold, popupTimeLongHold, popupTypeLongHold);
            cbLongHold();
            didHold = true;
            timeHeld = 0;
        }

    } else {
        // In pageless mode, encoder scrolls through menu and selects items
        if(!isEditing){
            // Update menu position
            menu->setIndex(menu->getIndex() + patch->encoder.Increment());
            isEditing = patch->encoder.RisingEdge();
        } else {
            // Update selected menu item's value
            int32_t inc = patch->encoder.Increment();
            if (inc > 0){
                menu->getItem()->increment();
            } else if (inc < 0){
                menu->getItem()->decrement();
            }

            isEditing = !patch->encoder.RisingEdge();
        }
    }
}

void PatchGui::assignCVsToPage(){
    for(uint8_t i = 0; i < 4; i++){
        if (menu->size() > i){
            assignToCV(menu->getItem(i), i+1);
        }
    }
}

void PatchGui::pageFwd(){
    if(isZoneB && menu->getPage() + 1 >= menu->getNumPages()){
        setPage(zoneB, true);
    } else if(!isZoneB && menu->getPage() + 1 >= zoneB){
        setPage(0, true);
    } else {
        setPage(menu->getPage() + 1, true);
    }
};

void PatchGui::pageBack(){ // TODO factor this into menu
    if(isZoneB && menu->getPage() == zoneB){
        setPage(menu->getNumPages() -1, false);
    } else if (!isZoneB && menu->getPage() == 0){
        setPage(zoneB-1, false);
    } else {
        setPage(menu->getPage() - 1, false);
    }
};

void PatchGui::setPage(uint8_t i, bool b){
    menu->setPage(i);

    while(menu->getIsHidden(menu->getPage())){
        if(b){
            menu->setPage(i + 1);
        } else {
            menu->setPage(i - 1);
        }
    }

    assignCVsToPage();
};

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

    // Draw hold graphic
    if(drawHoldBar && patch->encoder.Pressed()){
        patch->display.DrawRect(SCREEN_WIDTH - 2 - HOLD_BAR_WIDTH, 2, SCREEN_WIDTH - 2, HOLD_BAR_HEIGHT + 2, false, true);
        patch->display.DrawRect(SCREEN_WIDTH - 2 - HOLD_BAR_WIDTH, 2, SCREEN_WIDTH - 2, HOLD_BAR_HEIGHT + 2, true, false);

        float pxPerMsHeld = static_cast<float>(HOLD_BAR_WIDTH)/ static_cast<float>(LONG_HOLD_DURATION_MS);
        uint8_t holdPx;
        if(didHold){
            holdPx = HOLD_BAR_WIDTH;
        } else {
            holdPx = static_cast<uint8_t>(pxPerMsHeld * timeHeld);
        }
        patch->display.DrawRect(SCREEN_WIDTH - 2 - HOLD_BAR_WIDTH, 2, SCREEN_WIDTH - 2 - HOLD_BAR_WIDTH + holdPx, HOLD_BAR_HEIGHT + 2, true, true);
    }

    // Draw line under headers
    patch->display.DrawLine(0, y, SCREEN_WIDTH, y, true);

    // Draw cursor indicator,
    y++;
    if (!pageMode){
        if (isEditing){
            drawString("*", 0, y);
        } else {
            drawString(">", 0, y);
        }
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

    // Draw popup
    if(popupRemainingCycles > 0)
    {
        popupRemainingCycles--;

        uint8_t x;
        if(popupType){
            // Banner
            patch->display.DrawRect(4, 4, 124, fontHeight + 6, false, true);
            patch->display.DrawRect(4, 4, 124, fontHeight + 6, true, false);
            y = 6;
            x = 6;
        } else {
            // Center
            patch->display.DrawRect(10, 10, 118, 54, false, true);
            patch->display.DrawRect(10, 10, 118, 54, true, false);
            y = 12;
            x = 12;
        }

        for(uint8_t i = 0; popupLines[i] != "_END_"; i++){
            drawString(popupLines[i], x, y);
            y += fontHeight;
        }
    }

    // Write display buffer to OLED
    patch->display.Update();
}


void PatchGui::setHeader(std::string s, uint8_t i){
    headers[i] = s;
}

void PatchGui::setCbPress(std::function<void ()> cb, std::vector<std::string> msg, uint32_t i, bool b){
    cbPress =  cb;
    msgPress = msg;
    popupTimePress = i;
    popupTypePress = b;
};

void PatchGui::setCbPress(std::function<void ()> cb){
    setCbPress(cb, std::vector<std::string>{}, 0, false);
}

void PatchGui::setCbShortHold(std::function<void ()> cb, std::vector<std::string> msg, uint32_t i, bool b){
    cbShortHold =  cb;
    msgShortHold = msg;
    popupTimeShortHold = i;
    popupTypeShortHold = b;
};

void PatchGui::setCbShortHold(std::function<void ()> cb){
    setCbShortHold(cb, std::vector<std::string>{}, 0, false);
}

void PatchGui::setCbLongHold(std::function<void ()> cb, std::vector<std::string> msg, uint32_t i, bool b){
    cbLongHold =  cb;
    msgLongHold = msg;
    popupTimeLongHold = i;
    popupTypeLongHold = b;
};

void PatchGui::setCbLongHold(std::function<void ()> cb){
    setCbLongHold(cb, std::vector<std::string>{}, 0, false);
}

void PatchGui::setPopup(std::vector<std::string> lines, uint32_t cycles, bool type){
    popupLines = lines;
    popupLines.push_back("_END_");
    popupRemainingCycles = cycles;
    popupType = type;
}

void PatchGui::keepPopup(){
    popupRemainingCycles = 1;
}

void PatchGui::changeZones(){
    if(zoneB > 0){
        isZoneB = !isZoneB;
        if(isZoneB){
            setPage(zoneB);
        } else {
            setPage(0);
        }
    }
}

void PatchGui::setIsHidden(uint8_t i, bool b){
    menu->setIsHidden(i, b);

    if(b && menu->getPage() == i){
        pageFwd();
    }
}
