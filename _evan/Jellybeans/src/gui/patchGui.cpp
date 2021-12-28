/* 
 * Copyright (C) 2021 Evan Pernu - All Rights Reserved
 * You may use, distribute and modify this code under the
 * terms of the GNU AGPLv3 license.
 * 
 * You should have received a copy of the GNU AGPLv3 license with
 * this file (LICENSE.md). If not, please write to: evanpernu@gmail.com, 
 * or visit: https://www.gnu.org/licenses/agpl-3.0.en.html
 */

#include "daisy_patch.h"
#include "patchGui.h"

using namespace daisy;
using namespace patch_gui;

PatchGui::PatchGui(){}

// @param patch reference
// @param FontDef
// @param Font width
// @param Font height
PatchGui::PatchGui(
        DaisyPatch* thePatch,
        Menu* theMenu,
        FontDef* theFont,
        uint8_t theWidth,
        uint8_t theHeight
){
    patch      = thePatch;
    menu       = theMenu;
    font       = theFont;
    fontWidth  = theWidth;
    fontHeight = theHeight;

    isEditing = false;

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
    // Clear display
    patch->display.Fill(false);  

    // Draw the top bar
    drawString(headerStr, 0, 0);
    patch->display.DrawLine(0, fontHeight + 1, SCREEN_WIDTH, fontHeight + 1, true);

    // Draw the cursor indicator
    drawString(">", 0, fontHeight + 1);

    uint8_t headerCount = 1; //TODO set for header vect

    // Number of menu items that will fit on the screen
    uint8_t listSize = (SCREEN_HEIGHT - (headerCount * fontHeight)) / fontHeight; 

    if (debug){
        // If in debug mode, reserve the bottom menu item's space for debug data
        listSize--;
        patch->display.DrawLine(0, 53, 128, 53, true);
        drawString(debugStr, 2, 54);
    }

    // Draw each menu item
    for(uint8_t i = menu->getIndex(); i < menu->getIndex() + listSize; i++){
        if (i < menu->size()){
            drawString(
                    menu->getItem(i)->getDisplayString(), 
                    fontWidth, 
                    (i - menu->getIndex()) * fontHeight + fontHeight + 2
            );
        }    
    }

    // Write display buffer to OLED
    patch->display.Update();
}

void PatchGui::renderDebug(){
    patch->display.Fill(false);  
    patch->display.DrawLine(0, 53, 128, 53, true);
    drawString(debugStr, 2, 54);
    patch->display.Update();
}

void PatchGui::setHeaderStr(std::string s){
    headerStr = s;
}

void PatchGui::setDebugStr(std::string s){
    debugStr = s;
}

void PatchGui::setDebug(bool b){
    debug = b;
}