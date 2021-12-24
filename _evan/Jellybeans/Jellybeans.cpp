/* 
 * Copyright (C) 2021 Evan Pernu - All Rights Reserved
 * You may use, distribute and modify this code under the
 * terms of the GNU AGPLv3 license.
 * 
 * You should have received a copy of the GNU AGPLv3 license with
 * this file (LICENSE.md). If not, please write to: evanpernu@gmail.com, 
 * or visit: https://www.gnu.org/licenses/agpl-3.0.en.html
 *
 * 
 * ====================================================
 * =   __       _        _                            =
 * =   \ \  ___| | |_   _| |__   ___  __ _ _ __  ___  =
 * =    \ \/ _ \ | | | | | '_ \ / _ \/ _` | '_ \/ __| =
 * = /\_/ /  __/ | | |_| | |_) |  __/ (_| | | | \__ \ =
 * = \___/ \___|_|_|\__, |_.__/ \___|\__,_|_| |_|___/ =
 * =                |___/                             =
 * ====================================================
 * 
 * 
 * Jellybeans is an arpeggiator eurorack module designed for the 
 * Electrosmith Daisy Patch platform.
 */


#include "daisysp.h"
#include "daisy_patch.h"
#include "daisysp.h"
#include "resources.h"


#include <string>
#include <array>

using namespace daisy;
using namespace daisysp;
using namespace jellybeans;

DaisyPatch patch;

// If true, the bottom row will display debug data instead of a menu item
const bool debugMode = true;
std::string debugString;

// The note vallue currently being sent to Patches' DAC's output 1
// This is stored so that it's only calculated upon a change
float arpNoteDacOutput1;

const FontDef font = Font_7x10;
const int     fontWidth = 7;
const int     fontHeight = 10;

/// Menu navigating vars
int  menuPos;
bool isEditing;


// Given the 1V/oct and 0-5V range of the CV out port,
// we are limited to a 5 octave register. Voicings span
// up to 2 octaves and coarse tuning (mTonic) spans another,
// leaving us 2 octaves of room for upwards transposition.
//
// Note that the indices of the elements are also their octave distances from 0
const std::vector<std::string> allOctaves {
    "0",
    "+1", // TODO re-enable these once out-of-bounds notes have been handled
    "+2"
};

void updateControls();
void updateOled();
void updateOutputs();
//void onClockPulseIn();
void drawString(std::string, int, int);

Arp* arp;

std::array<MenuItem, 10> menuItems;

// Reference vars to make the code more readable
MenuItem *mPattern   = &menuItems[0];
MenuItem *mDivision  = &menuItems[1];
MenuItem *mVoicing   = &menuItems[2];
MenuItem *mInversion = &menuItems[3];
MenuItem *mTonic     = &menuItems[4];
MenuItem *mScale     = &menuItems[5];
MenuItem *mRhythm    = &menuItems[6];
MenuItem *mOctRng    = &menuItems[7];
MenuItem *mOct       = &menuItems[8];
MenuItem *mClockDiv  = &menuItems[9];

Parameter patternParam, divisionParam, voicingParam, inversionParam;
int patternCurCvVal, divisionCurCvVal, voicingCurCvVal, inversionCurCvVal;

// Callback functions invoked whenever menu parameters are changed
void cb(){
    arp->updateTraversal();
};

void cbPattern(){
    arp->setPattern(mPattern->value());
    arp->updateTraversal();
};

void cbVoicing(){
    arp->getChord()->setVoicing(mVoicing->value());
    updateOled();
    arp->updateTraversal();
    updateOled();
};

int main(void) {
    // Initialize hardware
    patch.Init();

    // Initialize arp
    arp = new Arp();

    // Initialize menu items
    // Note that the positions of items 0-3 need to remain fixed
    menuItems[0] = MenuItem("Pattern  ", arpPatterns,    0, cbPattern);
    menuItems[1] = MenuItem("N/A      ", allClockInDivs, 0, cb); // Division
    menuItems[2] = MenuItem("Voicing  ", voicings,       0, cbVoicing);
    menuItems[3] = MenuItem("N/A      ", allInversions,  0, cb); // Inversion
    menuItems[4] = MenuItem("Tonic    ", allNotes,       0, cb);
    menuItems[5] = MenuItem("Scale    ", modes,          0, cb);
    menuItems[6] = MenuItem("N/A      ", arpRhythms,     0, cb); // Rhythm
    menuItems[7] = MenuItem("N/A      ", allOctaves,     0, cb); // Oct Rng
    menuItems[8] = MenuItem("Octave   ", allOctaves,     0, cb);
    menuItems[9] = MenuItem("Clock In ", allClockInDivs, 0, cb);

    // Initialize CV params
    patternParam.Init(patch.controls[0],   0.f, static_cast<float>(arpPatterns.size()),    Parameter::LINEAR);
    divisionParam.Init(patch.controls[1],  0.f, static_cast<float>(allClockInDivs.size()), Parameter::LINEAR);
    voicingParam.Init(patch.controls[2],   0.f, static_cast<float>(voicings.size()),       Parameter::LINEAR);
    inversionParam.Init(patch.controls[3], 0.f, static_cast<float>(allInversions.size()),  Parameter::LINEAR);
    patternCurCvVal   = static_cast<int>(patternParam.Process());
    divisionCurCvVal  = static_cast<int>(divisionParam.Process());
    voicingCurCvVal   = static_cast<int>(voicingParam.Process());
    inversionCurCvVal = static_cast<int>(inversionParam.Process());

    // Initialize variables
    arpNoteDacOutput1 = 0.f;
    menuPos     = 0;
    isEditing   = false;
    debugString = "I'm a debug string";

    patch.StartAdc();

    // Main event loop
    while(1){
        updateControls();
        updateOled();
        updateOutputs();
    }
}

// Handle any input to Patches' controls
void updateControls() {
    patch.ProcessAnalogControls();
    patch.ProcessDigitalControls();

    // Parse CV values
    int curCvVal;
    // Pattern
    curCvVal = static_cast<int>(patternParam.Process());
    if(curCvVal != patternCurCvVal){
        menuItems[0].setIndex(curCvVal);
        patternCurCvVal = curCvVal;
    }
    // Division
    curCvVal = static_cast<int>(divisionParam.Process());
    if(curCvVal != divisionCurCvVal){
        menuItems[1].setIndex(curCvVal);
        divisionCurCvVal = curCvVal;
    }
    // Voicing
    curCvVal = static_cast<int>(voicingParam.Process());
    if(curCvVal != voicingCurCvVal){
        menuItems[2].setIndex(curCvVal);
        voicingCurCvVal = curCvVal;
    }
    // Inversion
    curCvVal = static_cast<int>(inversionParam.Process());
    if(curCvVal != inversionCurCvVal){
        menuItems[3].setIndex(curCvVal);
        inversionCurCvVal = curCvVal;
    }

    if(!isEditing)
    {
        // Update menu position
        menuPos += patch.encoder.Increment();
    
        if (menuPos > (int) menuItems.size() - 1) {
            menuPos = (int) menuItems.size() - 1;
        } else if (menuPos <= 0) {
            menuPos = 0;
        }

        isEditing = patch.encoder.RisingEdge();
    }
    else
    {
        // Update selected menu item's value
        int inc = patch.encoder.Increment();
        if (inc > 0){
            menuItems[menuPos].increment();
        } else if (inc < 0){
            menuItems[menuPos].decrement();
        }

        isEditing = !patch.encoder.RisingEdge();
    }

    // Update step with respect to clock
    // Accept input from either GATE IN
    //
    // Currently, we'll just do 1 step per clock pulse
    if(patch.gate_input[0].Trig() || patch.gate_input[1].Trig())
    {
        arp->onClockPulse();
    }
}

// Update Patches' screen
//
// Display on Daisy Patch is 128x64p
// With 7x10 font, this means it's limited to:
//  - 18 chars horizontally (w/2p to spare)
//  - 6 chars vertically (w/4p to spare)
void updateOled() {
    // Clear display
    patch.display.Fill(false);  

    // Draw the top bar
    drawString(arp->toString(), 0, 0);
    patch.display.DrawLine(0, 11, 128, 11, true);

    // Draw the cursor indicator
    drawString(">", 0, 11);

    int listSize = 5;

    if (debugMode){
        // If in debug mode, reserve the bottom menu item's space for debug data
        debugString = arp->getChord()->toString();
        listSize--;
        patch.display.DrawLine(0, 53, 128, 53, true);
        drawString(debugString, 2, 54);
    }

    // Draw each menu item
    for(int i = menuPos; i < menuPos + listSize; i++){
        if (i < (int) menuItems.size()){
            drawString(menuItems[i].displayValue(), fontWidth, (i - menuPos) * fontHeight + 12);
        }    
    }

    // Write display buffer to OLED
    patch.display.Update();
}

// Updates Patches' output values
void updateOutputs()
{
    patch.seed.dac.WriteValue(DacHandle::Channel::ONE, arp->getDacValue());

    dsy_gpio_write(&patch.gate_output, arp->getNewNote());
}

/*
 * Utility functions
 */

// Utility to perform a silly little dance where we set the cursor, 
// convert a std::string to char*, and pass it to WriteString()
void drawString(std::string str, int x, int y){
    patch.display.SetCursor(x, y);
    char* cstr = &str[0];
    patch.display.WriteString(cstr, font, true);
}