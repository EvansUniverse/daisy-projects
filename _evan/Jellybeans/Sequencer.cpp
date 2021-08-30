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

//const char *menuItems[10] = {
std::array<std::string, 10> menuItems = {
    // TODO shorten names if screen domain doesn't permint.
    "Tonic",
    "Scale",
    "Division",
    "Voicing",
    "Order",
    "Rhythm",
    "Inversion",
    "Oct Range",
    "Oct",
    "Clock"
};
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
    patch.display.WriteString(debug_cstr, Font_7x10, true);

    // Draw a line across the top
    patch.display.DrawLine(0, 10, 128, 10, true);

    // Draw the cursor indicator
    patch.display.SetCursor(0, 11);
    std::string cursor_str = ">";
    char* cursor_cstr = &cursor_str[0];
    patch.display.WriteString(cursor_cstr, Font_7x10, true);

    // Draw each menu item
    for(unsigned int i = menuPos; i < menuPos + 6; i = i + 1){
        patch.display.SetCursor(7, i * 10 + 11);
        if(i < menuItems.size())
        {
            char* cstr = &menuItems[i][0];
            patch.display.WriteString(cstr, Font_7x10, true);
        }
    }

    // std::string str  = "!";
    // char*       cstr = &str[0];
    // patch.display.SetCursor(25 * stepNumber, 45);
    // patch.display.WriteString(cstr, Font_7x10, true);

    // //values and trigs
    // for(int i = 0; i < 5; i++)
    // {
    //     sprintf(cstr, "%d", values[i]);
    //     patch.display.SetCursor(i * 25, 10);
    //     patch.display.WriteString(cstr, Font_7x10, true);

    //     str = trigs[i % 5] ? "*" : "-";
    //     patch.display.SetCursor(i * 25, 30);
    //     patch.display.WriteString(cstr, Font_7x10, true);
    // }

    // //cursor
    // str = isEditing ? "@" : "o";
    // patch.display.SetCursor((menuPos % 5) * 25, (menuPos > 4) * 20);
    // patch.display.WriteString(cstr, Font_7x10, true);

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