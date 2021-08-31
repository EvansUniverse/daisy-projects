/* Copyright (C) 2020 Evan Pernu - All Rights Reserved
 * You may use, distribute and modify this code under the
 * terms of the GNU AGPLv3 license
 *
 * You should have received a copy of the GNU AGPLv3 license with
 * this file (LICENSE.md). If not, please write to: evanpernu@gmail.com, 
 * or visit: https://www.gnu.org/licenses/agpl-3.0.en.html
 */

#include "daisysp.h"
#include "daisy_patch.h"
#include <string>
#include <array>

using namespace daisy;
using namespace daisysp;

DaisyPatch patch;

const bool debugMode = false;

const int maxArpSteps = 3; // TODO determine a non-arbitrary number
int  arpValues[maxArpSteps];
bool arpTrigs[maxArpSteps];
int  arpStep;
bool trigOut;
float root;

const FontDef font = Font_7x10;
const int     fontWidth = 7;
const int     fontHeight = 10;

int  menuPos;
bool isEditing;

// Note that the indices of these elements also correspond to
// their semitone distance from C.
std::vector<std::string> allNotes {
    "C",
    "Db",
    "D",
    "Eb",
    "E",
    "F",
    "Gb",
    "G",
    "A",
    "Bb",
    "B",
    "Cb"
};

std::vector<std::string> allScales {
    "Major",
    "Minor",
    "Dorian",
    "Phyrgi",
    "Lydian",
    "Mixo",
    "Locri",
};

std::vector<std::string> allVoicings {
    "Triad",
    "7th",
    "9th",
    "11th",
    "13th",
    "6th",
    "Sus2",
    "Sus4",
    "KennyB"
};

std::vector<std::string> allOrders {
    "Up",
    "Down",
    "U+D In",
    "U+D Ex",
    "Random"
};

std::vector<std::string> allRhythms {
    "None",
    "Sw 25%",
    "Sw 50%",
    "Sw 75%",
    "Sw 100%"
};

std::vector<std::string> allInversions {
    "None",
    "Drop 2",
    "Drop 3",
    "Drop 4"
};

// Given the 1V/oct and 0-5V range of the CV out port,
// we are limited to a 5 octave register.
std::vector<std::string> allOctaves {
    "-1",
    "0",
    "+1",
    "+2",
    "+3",
};

std::vector<std::string> allClockDivs {
    "8",
    "4",
    "2",
    "1",
    "1/2",
    "1/4",
    "1/8",
    "1/16",
    "1/32",
    "1/64"
};

// Menu item whos value is an element of a list of predefined strings
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

        // Each menu item's displayName needs to be padded with
        // enough spaces to be in line with the end of the longest
        // name, plus 1 additional space. The longest menu item name
        // is currently "Inversion" with 10 chars.
        displayName = aName;
        for (unsigned int i = 0; i < 10 - name.length(); i++) {
            displayName += " ";
        }
    };

    std::string Value() {
        return  displayName + values[index];
    };

    void Increment(){
        index++;
        index = index % values.size();
    };

    void Decrement(){
        index--;
        if (index < 0){
            index = values.size() - 1;
        }
    };
};

std::array<MenuItem, 10> menuItems;

void UpdateControls();
void UpdateOled();
void UpdateOutputs();

int main(void)
{
    // Initialize hardware
    patch.Init(); 

    // Initialize menu items
    menuItems[0] = MenuItem("Tonic",     allNotes,      0);
    menuItems[1] = MenuItem("Scales",    allScales,     0);
    menuItems[2] = MenuItem("Division",  allClockDivs,  5);
    menuItems[3] = MenuItem("Voicing",   allVoicings,   0);
    menuItems[4] = MenuItem("Order",     allOrders,     0);
    menuItems[5] = MenuItem("Rhythm",    allRhythms,    0);
    menuItems[6] = MenuItem("Inversion", allInversions, 0);
    menuItems[7] = MenuItem("Oct Rng",   allOctaves,    5);
    menuItems[8] = MenuItem("Oct",       allOctaves,    5);
    menuItems[9] = MenuItem("Clock",     allClockDivs,  0);

    // Initialize variables
    arpStep   = 0;
    trigOut   = false;
    menuPos   = 0;
    isEditing = false;

    for(int i = 0; i < maxArpSteps; i++)
    {
        arpValues[i] = 0;
        arpTrigs[i]  = true;
    }



    // God only knows what this fucking thing does
    patch.StartAdc();

    // Main event loop
    while(1)
    {
        UpdateControls();
        UpdateOled();
        UpdateOutputs();
    }
}

// Converts a semitone value to data that can be supplied to Daisy Seed's DAC
// for CV, using the function patch.seed.dac.WriteValue()
//
// In Daisy Seed's DAC, 0=0v and 4095=5v. 4095/5=819, meaning 819 per volt/octave.
// Therefore tthe number 819 is significant here.
float semitoneToDac(int semi) {
    round((semi / 12.f) * 819.2f);
}

void UpdateControls()
{
    patch.ProcessAnalogControls();
    patch.ProcessDigitalControls();

    //encoder
    //can we simplify the menu logic?
    if(!isEditing)
    {
        // Update menu position
        menuPos += patch.encoder.Increment();

        if (menuPos > (int) menuItems.size() - 1)
        {
            menuPos = (int) menuItems.size() - 1;
        } else if (menuPos <= 0) {
            menuPos = 0;
        }

        // menuPos = (menuPos % 10 + 10) % 10;
        // if(menuPos < 5)
        // {
        isEditing = patch.encoder.RisingEdge() ? true : false;
        // }
        // else
        // {
        //     trigs[menuPos % 5] = patch.encoder.RisingEdge()
        //                              ? !trigs[menuPos % 5]
        //                              : trigs[menuPos % 5];
        // }
    }

    else
    {
        // values[menuPos] += patch.encoder.Increment();
        // values[menuPos] = values[menuPos] < 0.f ? 0.f : values[menuPos];
        // values[menuPos] = values[menuPos] > 60.f ? 60.f : values[menuPos];
        int inc = patch.encoder.Increment();
        if (inc > 0){
            menuItems[menuPos].Increment();
        } else if (inc < 0){
            menuItems[menuPos].Decrement();
        }

        isEditing = patch.encoder.RisingEdge() ? false : true;
    }

    //gate in
    if(patch.gate_input[0].Trig() || patch.gate_input[1].Trig())
    {
        arpStep++;
        arpStep %= 5;
        trigOut = arpTrigs[arpStep];
    }
}

// Utility to perform a silly little dance where we set the cursor, 
// convert a std::string to char*, and pass it to WriteString()
void DrawString(std::string str, int x, int y){
    patch.display.SetCursor(x, y);
    char* cstr = &str[0];
    patch.display.WriteString(cstr, font, true);
}

// Display on Daisy Patch is 128x64p
// With 7x10 font, this means it's limited to:
//  * 18 chars horizontally (w/2p to spare)
//  * 6 chars vertically (w/4p to spare)
void UpdateOled()
{
    // Clear display
    patch.display.Fill(false);  

    // Draw the top bar
    if (debugMode){
        // Debug mode - displays debug values for development
        DrawString(std::to_string(menuPos) + " " + std::to_string(isEditing), 0, 0);
    } else {
        // Normal mode - displays various info
    }
    patch.display.DrawLine(0, 10, 128, 10, true);

    // Draw the cursor indicator
    DrawString(">", 0, 11);

    // Draw each menu item
    for(int i = menuPos; i < menuPos + 6; i = i + 1){
        if (i < (int) menuItems.size()){
            DrawString(menuItems[i].Value(), fontWidth, (i - menuPos) * fontHeight + 11);
        }    
    }

    // Write display buffer to OLED
    patch.display.Update();
}

void UpdateOutputs()
{
    // patch.seed.dac.WriteValue(DacHandle::Channel::ONE, round((arpValues[arpStep] / 12.f) * 819.2f));

    dsy_gpio_write(&patch.gate_output, trigOut);
    trigOut = false;
}