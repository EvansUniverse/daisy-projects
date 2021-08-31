#include "daisysp.h"
#include "daisy_patch.h"
#include <string>
#include <array>

using namespace daisy;
using namespace daisysp;

DaisyPatch patch;

const int maxArpSteps = 10;
int  values[maxArpSteps];
bool trigs[maxArpSteps];
int  stepNumber;
bool trigOut;

const FontDef font = Font_7x10;
const int     fontWidth = 7;
const int     fontHeight = 10;

// Base class for menu item
class MenuItem {
  public:
    std::string name;
    std::string value;

    MenuItem(){
        name =  "N/A";
        value = "N/A";
    };

    MenuItem(std::string aName, std::string aValue){
        name = aName;
        value = aValue;
    };

    std::string Value(){
        return value;
    };
    void Increment(){};
    void Decrement(){};
};

// Menu item whos value is an element of a list of predefined strings
class MenuItemStr: public MenuItem {
  public:
    int index;
    std::vector<std::string> values;

    // aDefault must be a valid index in aValues
    MenuItemStr(std::string aName, std::vector<std::string> aValues, int aDefault){
        name = aName;
        values = aValues;
        index = aDefault;
    };

    std::string Value(){
        return  name + "   " + values[index];
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

// Menu item for a numeric value
class MenuItemInt: public MenuItem {
  public:
    int value;
    int max;
    int min;

    MenuItemInt(std::string aName, int aValue, int aMax, int aMin){
        name = aName;
        value = aValue;
        max = aMax;
        min = aMin;
    };

    std::string Value(){
        return std::to_string(value);
    };

    void Increment(){};
    void Decrement(){};
};

std::array<MenuItem, 10> menuItems;

std::vector<std::string> allNotes {
    "A",
    "Bb",
    "B",
    "Cb",
    "C",
    "Db",
    "D",
    "Eb",
    "E",
    "F",
    "Gb",
    "G"
};

std::vector<std::string> allScales {
    "Major",
    // "Minor"
    // "Dorian",
    // "Phyrgian",
    // "Lydian",
    // "Mixolydian",
    // "Locrian",
};

std::vector<std::string> allVoicings {
    "Triad",
    // "7th",
    // "9th",
    // "11th",
    // "13th",
    // "6th",
    // "Sus2",
    // "Sus4",
    // "KB"
};

std::vector<std::string> allOrders {
    "Up",
    // "Down",
    // "Up+Down In",
    // "Up+Down Ex",
    // "Random"
};

std::vector<std::string> allRhythms {
    "Straight",
    // "Swing 25%",
    // "Swing 50%",
    // "Swing 75%",
    // "Swing 100%"
};

std::vector<std::string> allInversions {
    "None",
    // "Drop 2",
    // "Drop 3",
    // "Drop 4"
};

std::vector<std::string> allOctaves {
    "-4",
    "-3",
    "-2",
    "-1",
    "0",
    "+1",
    "+2",
    "+3",
    "+4"
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

int  menuPos;
bool isEditing;

void UpdateControls();
void UpdateOled();
void UpdateOutputs();

int main(void)
{
    // Initialize hardware
    patch.Init(); 

    // Initialize menu items
    menuItems[0] = MenuItemStr("Tonic",     allNotes,      0);
    menuItems[1] = MenuItemStr("Scales",    allScales,     0);
    menuItems[2] = MenuItemStr("Division",  allClockDivs,  0);
    menuItems[3] = MenuItemStr("Voicing",   allVoicings,   0);
    menuItems[4] = MenuItemStr("Order",     allOrders,     0);
    menuItems[5] = MenuItemStr("Rhythm",    allRhythms,    0);
    menuItems[6] = MenuItemStr("Inversion", allInversions, 0);
    menuItems[7] = MenuItemStr("Oct Rng",   allOctaves,    0);
    menuItems[8] = MenuItemStr("Oct",       allOctaves,    0);
    menuItems[9] = MenuItemStr("Clock In",  allClockDivs,  0);

    // Initialize variables
    stepNumber = 0;
    trigOut    = false;
    menuPos    = 0;
    isEditing  = false;

    for(int i = 0; i < maxArpSteps; i++)
    {
        values[i] = 0.f;
        trigs[i]  = false;
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

        if (menuPos > menuItems.size() - 1)
        {
            menuPos = menuItems.size() - 1;
        } else if (menuPos < 0) {
            menuPos = 0;
        }

        // menuPos = (menuPos % 10 + 10) % 10;
        // if(menuPos < 5)
        // {
        //     isEditing = patch.encoder.RisingEdge() ? true : false;
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
        stepNumber++;
        stepNumber %= 5;
        trigOut = trigs[stepNumber];
    }
}

// Display on Daisy Patch is 128x64p
// With 7x10 font, this means it's limited to:
//  * 18 chars horizontally (w/2p to spare)
//  * 6 chars vertically (w/4p to spare)
void UpdateOled()
{
    // Clear display
    patch.display.Fill(false);  

    // Draw debug info
    patch.display.SetCursor(0, 0);
    std::string debug_str = std::to_string(menuPos);
    char* debug_cstr = &debug_str[0];
    patch.display.WriteString(debug_cstr, font, true);

    // Draw a line across the top
    patch.display.DrawLine(0, 10, 128, 10, true);

    // Draw the cursor indicator
    patch.display.SetCursor(0, 11);
    std::string cursor_str = ">";
    char* cursor_cstr = &cursor_str[0];
    patch.display.WriteString(cursor_cstr, font, true);

    // Draw each menu item
    for(unsigned int i = menuPos; i < menuPos + 6; i = i + 1){
        patch.display.SetCursor(fontWidth, i * fontHeight + 11);
        if(i < menuItems.size())
        {
            std::string str = menuItems[i].Value();
            char* cstr = &str[0];
            patch.display.WriteString(cstr, font, true);
        }
    }

    // Write display buffer to OLED
    patch.display.Update();
}

void UpdateOutputs()
{
    patch.seed.dac.WriteValue(DacHandle::Channel::ONE,
                              round((values[stepNumber] / 12.f) * 819.2f));
    patch.seed.dac.WriteValue(DacHandle::Channel::TWO,
                              round((values[stepNumber] / 12.f) * 819.2f));

    dsy_gpio_write(&patch.gate_output, trigOut);
    trigOut = false;
}