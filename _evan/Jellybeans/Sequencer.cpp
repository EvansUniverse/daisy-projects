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

// Menu item
// By default, the value can be any string
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

    void Increment(){};
    void Decrement(){};
};

// Menu item whos value is an element of a list of predefined strings
class MenuItemStr: public MenuItem {
  public:
    int index;
    std::vector<std::string> values;

    MenuItemStr(std::string aName, std::vector<std::string> aValues){
        name = aName;
        values = aValues;
        index = 0;
    };

    std::string Value(){
        return values[index];
    };

    void Increment(){};
    void Decrement(){};
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
//  = {
//     "Tonic",
//     "Scalse",
//     "Division",
//     "Voicing",
//     "Order",
//     "Rhythm",
//     "Inversion",
//     "Oct Range",
//     "Oct",
//     "Clock"
// };
//const int menuLen = sizeof(menuItems)/sizeof(menuItems[0]);

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
    menuItems[0] = 

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
        // menuPos = (menuPos % 100 + 100) % 100;

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
        values[menuPos] += patch.encoder.Increment();
        values[menuPos] = values[menuPos] < 0.f ? 0.f : values[menuPos];
        values[menuPos] = values[menuPos] > 60.f ? 60.f : values[menuPos];
        isEditing       = patch.encoder.RisingEdge() ? false : true;
    }

    //gate in
    if(patch.gate_input[0].Trig() || patch.gate_input[1].Trig())
    {
        stepNumber++;
        stepNumber %= 5;
        trigOut = trigs[stepNumber];
    }
}

// Display is 128x64
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
            char* cstr = &menuItems[i][0];
            patch.display.WriteString(cstr, font, true);
        }
    }


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