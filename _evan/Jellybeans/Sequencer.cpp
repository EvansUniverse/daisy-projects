/* 
 * Copyright (C) 2020 Evan Pernu - All Rights Reserved
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
 * Jellybeans is an arpeggiator eurorack module designed for the 
 * Electrosmith Daisy Patch platform.
 */


#include "daisysp.h"
#include "daisy_patch.h"
#include "daisysp.h"
#include <string>
#include <array>
#include <map>

using namespace daisy;
using namespace daisysp;

DaisyPatch patch;

// If true, the bottom row will display debug data instead of a menu item
const bool debugMode = true;

// Maximum possible arp steps
// Font size allows max 18 chars across, limiting the step display to 18.
// Subtract to account for 2 char step values.
const int maxArpSteps = 17;

// The semitone values for each step
std::array<int, maxArpSteps> arpValues;

// Represents whether each step should be played or not
std::array<bool, maxArpSteps>  arpTrigs;

// Current step index, 0 based
int arpStep; 

// Length of the current arp pattern
int arpLength;

// Number of clock pulses that have been received since the last reset
int clockCount;

// Current root note
float root;

bool trigOut;

const FontDef font = Font_7x10;
const int     fontWidth = 7;
const int     fontHeight = 10;

int  menuPos;
bool isEditing;

std::string arpString;
std::string debugString;

// Note that the indices of these elements also correspond to
// their semitone distances from C.
const std::vector<std::string> allNotes {
    "C",
    "C#",
    "D",
    "D#",
    "E",
    "F",
    "F#",
    "G",
    "G#",
    "A",
    "A#",
    "B"
};

const std::vector<std::string> allScales {
    "Major",
    "Minor",
    "Dorian",
    "Phyrgi", // Phyrgian
    "Lydian",
    "Mixo",   // Mixolydian
    "Locri",  // Locrian
};

// Maps scale names to their first octave of semitone values
std::map<std::string, std::vector<int>> scalesToSemitones {
    {"Major",  std::vector<int>{0, 2, 4, 5, 7, 9, 11}},
    {"Minor",  std::vector<int>{0, 2, 3, 5, 7, 8, 10}},
    {"Dorian", std::vector<int>{0, 2, 3, 5, 7, 9, 10}},
    {"Phyrgi", std::vector<int>{0, 1, 3, 5, 7, 9, 10}},
    {"Lydian", std::vector<int>{0, 2, 4, 6, 7, 9, 11}},
    {"Mixo",   std::vector<int>{0, 2, 4, 5, 7, 9, 10}},
    {"Locri",  std::vector<int>{0, 1, 3, 5, 6, 8, 10}},
};


const std::vector<std::string> allVoicings {
    "Triad",
    "7th",
    "9th",
    "11th",
    "13th",
    "6th",
    "Sus2",
    "Sus4",
    // "KennyB",  // Kenny Barron chord
    "Power"
};

// Maps voicings to the scale degrees they contain
std::map<std::string, std::vector<int>> voicingToScaleDegrees {
    {"Triad",  std::vector<int>{1, 3, 5}},
    {"7th",    std::vector<int>{1, 3, 5, 7}},
    {"9th",    std::vector<int>{1, 3, 5, 7, 9}},
    {"11th",   std::vector<int>{1, 3, 5, 7, 9, 11}},
    {"13th",   std::vector<int>{1, 3, 5, 7, 9, 11, 13}},
    {"6th",    std::vector<int>{1, 3, 5, 6}},
    {"Sus2",   std::vector<int>{1, 2, 5}},
    {"Sus4",   std::vector<int>{1, 4, 5}},
    // TODO add this back in later; it will require special treatment
    // because it spans so many octaves.
    // {"KennyB", std::vector<int>{1, 7, 14, 15, 22, 29}}, 
    {"Power",  std::vector<int>{1, 5}}
};


const std::vector<std::string> allOrders {
    "Up",
    "Down",
    "U+D In",
    "U+D Ex",
    "Random"
};

const std::vector<std::string> allRhythms {
    "None",
    "Sw 25%",
    "Sw 50%",
    "Sw 75%",
    "Sw 100%"
};

const std::vector<std::string> allInversions {
    "None",
    "Drop 2",
    "Drop 3",
    "Drop 4"
};

// Given the 1V/oct and 0-5V range of the CV out port,
// we are limited to a 5 octave register. Voicings span
// up to 2 octaves and coarse tuning (mTonic) spans another,
// leaving us 2 octaves of room for upwards transposition.
//
// Note that the indices of the elements are also their octave distances from 0
const std::vector<std::string> allOctaves {
    "0",
    "+1",
    "+2",
};

const std::vector<std::string> allClockInDivs {
    // "1/2", // TODO figure out how to interpolate for fractional clock values
    // "1/4",
    // "1/8",
    // "1/16",
    // "1/32",
    // "1/64",
    "1",
    "2",
    "4",
    "8",
    "16"
};

std::map<std::string, int> clockInDivToInt {
    {"1",  1},
    {"2",  2},
    {"4",  4},            
    {"8",  8},
    {"16", 16}
};

void UpdateControls();
void UpdateOled();
void UpdateOutputs();
void UpdateArpNotes();
void UpdateArpStep();
void UpdateArpString();
void OnClockPulseIn();

void DrawString(std::string, int, int);
float SemitoneToDac(int);

// Menu item whos value is an element of a list of predefined strings
// They are also used to store settings
class MenuItem {
  public:
    int index;
    std::string name;
    std::string displayName; // name with spaces appended
    std::vector<std::string> values;

    MenuItem(){};

    // aDefault must be a valid index in aValues
    MenuItem(std::string aName, std::vector<std::string> aValues, int aDefault){
        name = aName;
        values = aValues;
        index = aDefault;

        // For everything to line up neatly, each menu item's displayName 
        // needs to be padded with enough spaces to be in line with the 
        // end of the longest name, plus 1 additional space. The longest 
        // menu item name is currently "Inversion" with 10 chars.
        displayName = aName;
        for (unsigned int i = 0; i < 10 - name.length(); i++) {
            displayName += " ";
        }
    };

    std::string DisplayValue() {
        return  displayName + values[index];
    };

    std::string Value() {
        return values[index];
    }

    void Increment(){
        index++;
        index = index % values.size();
        OnChange();
    };

    void Decrement(){
        index--;
        if (index < 0){
            index = values.size() - 1;
        }
        OnChange();
    };

    void OnChange(){
        UpdateArpNotes();
        UpdateArpString();
    };
};

std::array<MenuItem, 10> menuItems;
// Macros to make the code more readable
// TODO these may cause some fuckery
MenuItem *mTonic     = &menuItems[0];
MenuItem *mScale    = &menuItems[1];
MenuItem *mDivision  = &menuItems[2];
MenuItem *mVoicing   = &menuItems[3];
MenuItem *mOrder     = &menuItems[4];
MenuItem *mRhythm    = &menuItems[5];
MenuItem *mInversion = &menuItems[6];
MenuItem *mOctRng    = &menuItems[7];
MenuItem *mOct       = &menuItems[8];
MenuItem *mClockDiv  = &menuItems[9];


int main(void) {
    // Initialize hardware
    patch.Init();

    // if (!debugMode){
    //     DrawStartupScreen();
    //     Sleep(2)
    // }

    // Initialize menu items
    menuItems[0] = MenuItem("Tonic",     allNotes,       0); // Tonic
    menuItems[1] = MenuItem("Scale",     allScales,      0);
    menuItems[2] = MenuItem("N/A",       allClockInDivs, 0); // Division
    menuItems[3] = MenuItem("Voicing",   allVoicings,    0);
    menuItems[4] = MenuItem("N/A",       allOrders,      0); // Order
    menuItems[5] = MenuItem("N/A",       allRhythms,     0); // Rhythm
    menuItems[6] = MenuItem("N/A",       allInversions,  0); // Inversion
    menuItems[7] = MenuItem("N/A",       allOctaves,     0); // Oct Rng
    menuItems[8] = MenuItem("Octave",    allOctaves,     0);
    menuItems[9] = MenuItem("Clock In",  allClockInDivs, 0);

    // Initialize variables
    arpStep     = 0;
    arpLength   = 0;
    clockCount  = 0;
    trigOut     = false;
    menuPos     = 0;
    isEditing   = false;
    debugString = "Startup";

    // Initialize arp
    UpdateArpNotes();

    // // God only knows what this fucking thing does
    patch.StartAdc();

    // Main event loop
    while(1){
        UpdateControls();
        UpdateOled();
        UpdateOutputs();
    }
}

void UpdateControls() {
    patch.ProcessAnalogControls();
    patch.ProcessDigitalControls();

    if(!isEditing)
    {
        // Update menu position
        menuPos += patch.encoder.Increment();
    
        if (menuPos > (int) menuItems.size() - 1) {
            menuPos = (int) menuItems.size() - 1;
        } else if (menuPos <= 0) {
            menuPos = 0;
        }

        isEditing = patch.encoder.RisingEdge() ? true : false;
    }
    else
    {
        int inc = patch.encoder.Increment();
        if (inc > 0){
            menuItems[menuPos].Increment();
        } else if (inc < 0){
            menuItems[menuPos].Decrement();
        }

        isEditing = patch.encoder.RisingEdge() ? false : true;
    }

    // Update step with respect to clock
    //
    // Currently, we'll just do 1 step per clock pulse
    if(patch.gate_input[0].Trig() || patch.gate_input[1].Trig())
    {
        OnClockPulseIn();
    }
}

// Display on Daisy Patch is 128x64p
// With 7x10 font, this means it's limited to:
//  - 18 chars horizontally (w/2p to spare)
//  - 6 chars vertically (w/4p to spare)
void UpdateOled() {
    // Clear display
    patch.display.Fill(false);  

    // Draw the top bar
    DrawString(arpString, 0, 0);
    patch.display.DrawLine(0, 11, 128, 11, true);

    // Draw the cursor indicator
    DrawString(">", 0, 11);

    int listSize = 5;

    if (debugMode){
        // If in debug mode, reserve the bottom menu item's space for debug data
        listSize--;
        patch.display.DrawLine(0, 53, 128, 53, true);
        DrawString(debugString, 2, 54);
    }

    // Draw each menu item
    for(int i = menuPos; i < menuPos + listSize; i++){
        if (i < (int) menuItems.size()){
            DrawString(menuItems[i].DisplayValue(), fontWidth, (i - menuPos) * fontHeight + 12);
        }    
    }

    // Write display buffer to OLED
    patch.display.Update();
}

void UpdateOutputs()
{
    patch.seed.dac.WriteValue(DacHandle::Channel::ONE, SemitoneToDac(arpValues[arpStep]));

    dsy_gpio_write(&patch.gate_output, trigOut);
    trigOut = false;
}

// Updates note and length data for the arp
void UpdateArpNotes(){
    int degree;
    int oct;
    int chordLen = static_cast<int>(voicingToScaleDegrees[mVoicing->Value()].size());
    int scaleLen = static_cast<int>(scalesToSemitones[mScale->Value()].size());

    // For each degree in the chord
    //
    // bug: major 7th results in 8
    for (int i = 0; i < chordLen; i++){
        // Get the degree
        degree = voicingToScaleDegrees[mVoicing->Value()][i];

        // Figure out how many octaves above 0 it is
        oct = degree / (scaleLen + 1);
        if (oct > 0){
            degree = degree % scaleLen;
        }

        // Offset by 1 since the values of the maps are 1-inedexed
        degree--;

        // Calculate the semitone value
        arpValues[i] = scalesToSemitones[mScale->Value()][degree] + 12 * (oct + mOct->index) + mTonic->index;

        debugString = "o: " + std::to_string(oct) + " d: " + std::to_string(degree) + " v: " + std::to_string(arpValues[i]);
    }

    // Set the arp length to match the new chord
    arpLength = voicingToScaleDegrees[mVoicing->Value()].size();

    // If the current arpStep exceeds the new arpLength, reduce it.
    //
    // TODO: this behavior could be a configurable setting,
    // or maybe something else sounds better. Could maybe
    // tinker with this later and figure out what sounds the best.
    // For example, could just reset to 0 instead.
    arpStep = arpStep % arpLength;

    UpdateArpString();
}

// Called every time a clock pulse is received
void OnClockPulseIn(){
    clockCount++;
    if (clockCount >= clockInDivToInt[mClockDiv->Value()]){
        clockCount = 0;

        UpdateArpStep();
    }
}

// Called every time the arp steps to the next note
//
// TODO: modify for other patterns besides up
void UpdateArpStep(){
    arpStep++;
    arpStep = arpStep % arpLength;
    trigOut = arpTrigs[arpStep];

    UpdateArpString();
}

// Updates the string used to display the arp
void UpdateArpString(){
    arpString = "";

    for(int i = 0; i < arpLength; i++){
        if (i == arpStep){
            arpString += std::to_string(arpValues[i]);
        } else {
            arpString += "_";
        }
    }
}


/*
 * Utility functions
 */

// Utility to perform a silly little dance where we set the cursor, 
// convert a std::string to char*, and pass it to WriteString()
void DrawString(std::string str, int x, int y){
    patch.display.SetCursor(x, y);
    char* cstr = &str[0];
    patch.display.WriteString(cstr, font, true);
}

// Converts a semitone value to data that can be supplied to Daisy Seed's DAC
// for CV, using the function patch.seed.dac.WriteValue()
//
// semi: an integer between 0-60 representing the number of semitones from low C
//
// FYI: In Daisy Seed's DAC, 0=0v and 4095=5v. 4095/5=819, meaning 819 dac units
// per volt or octave.
float SemitoneToDac(int semi) {
    return round((semi / 12.f) * 819.2f);
}