/* 
 * Copyright (C) 2021 Evan Pernu - All Rights Reserved
 * You may use, distribute and modify this code under the
 * terms of the GNU AGPLv3 license.
 *
 * You should have received a copy of the GNU AGPLv3 license with
 * this file (LICENSE.md). If not, please write to: evanpernu@gmail.com, 
 * or visit: https://www.gnu.org/licenses/agpl-3.0.en.html
 * 
 * A K K U N                                                        
 * 
 * Akkun is a proof of concept for a teachable melody generation AI, made to
 * run on the Electrosmith Daisy Eurorack Module
 */


#include "daisysp.h"
#include "daisy_patch.h"
#include "daisysp.h"
#include <string>
#include <array>
#include <map>
#include

using namespace daisy;
using namespace daisysp;

DaisyPatch patch;

// If true, the bottom row will display debug data instead of a menu item
const bool debugMode = true;

// Maximum possible seq steps
const int maxSeqSteps = 16;

// The semitone values for each step
std::array<int, maxSeqSteps> seqNotes;

// Tracks the current position in seqNotes
int seqStepIndex;

// Current step index, 0 based
int arpStep; 

// The note vallue currently being sent to Patches' DAC's CV 1 output
// This is stored so that it's only calculated upon a change
float seqNoteDacOutput1;

// Number of clock pulses that have been received since the last reset
int clockCount;

// If true, send gate through Patches' Gate 1 output
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

// // Given the 1V/oct and 0-5V range of the CV out port,
// // we are limited to a 5 octave register. Voicings span
// // up to 2 octaves and coarse tuning (mTonic) spans another,
// // leaving us 2 octaves of room for upwards transposition.
// //
// // Note that the indices of the elements are also their octave distances from 0
// const std::vector<std::string> allOctaves {
//     "0",
//     // "+1", // TODO re-enable these once out-of-bounds notes have been handled
//     // "+2"
// };

// const std::vector<std::string> allClockInDivs {
//     // "1/2", // TODO figure out how to interpolate for fractional clock values
//     // "1/4",
//     // "1/8",
//     // "1/16",
//     // "1/32",
//     // "1/64",
//     "1",
//     "2",
//     "4",
//     "8",
//     "16"
// };

// std::map<std::string, int> clockInDivToInt {
//     {"1",  1},
//     {"2",  2},
//     {"4",  4},            
//     {"8",  8},
//     {"16", 16}
// };

void UpdateControls();
void UpdateOled();
void UpdateOutputs();
void UpdateArpNotes();
void UpdateArpStep();
void UpdateArpTraversal();
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

    void SetIndex(int i){
        index = i;
        OnChange();
    };

    // Executed every time this item's value is changed
    void OnChange(){
        UpdateArpNotes();
        UpdateArpTraversal();
        UpdateArpString();
    };
};

std::array<MenuItem, 10> menuItems;

// Reference bars to make the code more readable
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


int main(void) {
    // Initialize hardware
    patch.Init();

    // if (!debugMode){
    //     DrawStartupScreen();
    //     Sleep(2)
    // }

    // Initialize menu items
    // Note that the positions of items 0-3 need to remain fixed 
    menuItems[0] = MenuItem("Learn",   allPatterns,    0);
    menuItems[1] = MenuItem("N/A",       allClockInDivs, 0); // Division
    menuItems[2] = MenuItem("Voicing",   allVoicings,    0);
    menuItems[3] = MenuItem("N/A",       allInversions,  0); // Inversion
    menuItems[4] = MenuItem("Tonic",     allNotes,       0);
    menuItems[5] = MenuItem("Scale",     allScales,      0);
    menuItems[6] = MenuItem("N/A",       allRhythms,     0); // Rhythm
    menuItems[7] = MenuItem("N/A",       allOctaves,     0); // Oct Rng
    menuItems[8] = MenuItem("Octave",    allOctaves,     0);
    menuItems[9] = MenuItem("Clock In",  allClockInDivs, 0);

    // Initialize CV params
    patternParam.Init(patch.controls[0], 0.f, static_cast<float>(allPatterns.size()), Parameter::LINEAR);
    patternCurCvVal = static_cast<int>(patternParam.Process());
    divisionParam.Init(patch.controls[1], 0.f, static_cast<float>(allClockInDivs.size()), Parameter::LINEAR);
    divisionCurCvVal = static_cast<int>(divisionParam.Process());
    voicingParam.Init(patch.controls[2], 0.f, static_cast<float>(allVoicings.size()), Parameter::LINEAR);
    voicingCurCvVal = static_cast<int>(voicingParam.Process());
    inversionParam.Init(patch.controls[3], 0.f, static_cast<float>(allInversions.size()), Parameter::LINEAR);
    inversionCurCvVal = static_cast<int>(inversionParam.Process());

    // Initialize variables
    arpStep     = 0;
    arpTraversalIndex = 0;
    arpNoteDacOutput1 = 0.f;
    clockCount  = 0;
    trigOut     = false;
    menuPos     = 0;
    isEditing   = false;
    debugString = "init debug str";

    // Initialize arp
    UpdateArpTraversal();
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

// Handle any input to Patches' controls
void UpdateControls() {
    patch.ProcessAnalogControls();
    patch.ProcessDigitalControls();

    // Parse CV values
    int curCvVal;
    // Pattern
    curCvVal = static_cast<int>(patternParam.Process());
    if(curCvVal != patternCurCvVal){
        menuItems[0].SetIndex(curCvVal);
        patternCurCvVal = curCvVal;
    }
    // Division
    curCvVal = static_cast<int>(divisionParam.Process());
    if(curCvVal != divisionCurCvVal){
        menuItems[1].SetIndex(curCvVal);
        divisionCurCvVal = curCvVal;
    }
    // // Voicing
    curCvVal = static_cast<int>(voicingParam.Process());
    if(curCvVal != voicingCurCvVal){
        menuItems[2].SetIndex(curCvVal);
        voicingCurCvVal = curCvVal;
    }
    // // Inversion
    curCvVal = static_cast<int>(inversionParam.Process());
    if(curCvVal != inversionCurCvVal){
        menuItems[3].SetIndex(curCvVal);
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

        isEditing = patch.encoder.RisingEdge() ? true : false;
    }
    else
    {
        // Update selected menu item's value
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

// Update Patches' screen
//
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

// // Updates note and length data for the arp
// void UpdateArpNotes(){
//     int degree;
//     int oct;
//     int chordLen = static_cast<int>(voicingToScaleDegrees[mVoicing->Value()].size());
//     int scaleLen = static_cast<int>(scalesToSemitones[mScale->Value()].size());

//     // For each degree in the chord
//     //
//     // bug: 5th note in kenny barron chords (both major and minor) resolve to octaves
//     for (int i = 0; i < chordLen; i++){
//         // Get the degree
//         degree = voicingToScaleDegrees[mVoicing->Value()][i];

//         // Figure out how many octaves above 0 it is
//         oct = degree / (scaleLen + 1);
//         if (oct > 0){
//             degree = degree % scaleLen;
//         }

//         // Offset by 1 since the values of the maps are 1-inedexed
//         degree--;

//         // Calculate the semitone value
//         arpNotes[i] = scalesToSemitones[mScale->Value()][degree] + 12 * (oct + mOct->index) + mTonic->index;

//         // If the value exceeds our note range, bring it up/down an octave until it fits
//         while (arpNotes[i] > 60){
//             arpNotes[i] -= 12;
//         }
//         while (arpNotes[i] < 0){
//             arpNotes[i] += 12;
//         }
//     }

//     UpdateArpString();
// }

// // Called every time a clock pulse is received
// void OnClockPulseIn(){
//     clockCount++;
//     if (clockCount >= clockInDivToInt[mClockDiv->Value()]){
//         clockCount = 0;

//         UpdateArpStep();
//     }
// }

// // Updates the arp traversal values based on the current pattern
// void UpdateArpTraversal(){
//     arpTraversal = std::vector<int>{};
//     int chordLen = static_cast<int>(voicingToScaleDegrees[mVoicing->Value()].size());

//     if (mPattern->Value() == "Down") {
//         for (int i = chordLen-1; i >= 0; i--) {
//             arpTraversal.push_back(i);
//         }

//     } else if (mPattern->Value() == "U+D In") {
//         for (int i = 0; i < chordLen; i++) {
//             arpTraversal.push_back(i);
//         }
//                 for (int i = chordLen-1; i > 0; i--) {
//             arpTraversal.push_back(i);
//         }

//     } else if (mPattern->Value() == "U+D Ex") {
//         for (int i = 0; i < chordLen-1; i++) {
//             arpTraversal.push_back(i);
//         }
//         for (int i = chordLen-1; i > 0; i--) {
//             arpTraversal.push_back(i);
//         }

//     } else if (mPattern->Value() == "Random") {// TODO implement
//         // -1 will represent "random value"
//         arpTraversal.push_back(-1);

//     } else { // mPattern->Value() == "Up"
//         for (int i = 0; i < chordLen; i++) {
//             arpTraversal.push_back(i);
//         }
//     }    

//     arpTraversalIndex = 0;
// }

// // Called every time the arp steps to the next note
// //
// // TODO: modify for other patterns besides up
// void UpdateArpStep()
// {
//     int semi;

//     arpStep = arpTraversal[arpTraversalIndex];

//     if (arpStep < 0) {
//         // Random note
//         // TODO this random method is biased, created a stronger random function
//         // Maybe keep this one as a separate option (e.g. "BadRandom") if it produces interesting
//         // musical results
//         semi = arpNotes[rand() % arpNotes.size()];
//     } else {
//         semi = arpNotes[arpStep];
//     }

//     debugString = std::to_string(semi);

//     trigOut = true;
//     arpNoteDacOutput1 = SemitoneToDac(semi);
    
//     arpTraversalIndex++;
//     arpTraversalIndex = arpTraversalIndex % static_cast<int>(arpTraversal.size());

//     UpdateArpString();
// }

// // Updates the string used to display the arp
// void UpdateArpString(){
//     arpString = "";
//     int chordSize = static_cast<int>(voicingToScaleDegrees[mVoicing->Value()].size());

//     for(int i = 0; i < chordSize; i++){
//         if (i == arpStep){
//             arpString += std::to_string(arpNotes[i]);
//         } else {
//             arpString += "_";
//         }
//     }
// }

// Updates Patches' output values
void UpdateOutputs()
{
    patch.seed.dac.WriteValue(DacHandle::Channel::ONE, arpNoteDacOutput1);

    dsy_gpio_write(&patch.gate_output, trigOut);
    trigOut = false;
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

// Converts a float from a libdaisy.Param's .Process() method to an int value, rounded down
int ParamValueToInt(float f) {
    return static_cast<int>(f);
}