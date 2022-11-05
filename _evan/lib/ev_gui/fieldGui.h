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

#include "daisysp.h"
#include "daisy_field.h"
#include "menu.h"

#include <string>

#define DEFAULT_RENDER_MODE_ID 0

using namespace daisy;

namespace ev_gui {
    class FieldGui {
    private:
        DaisyField* hw;
    
        FontDef* font;
        uint8_t fontWidth;
        uint8_t fontHeight;

        // Contains each line of text to be displayed in the popup
        //
        // The delimiter "_END_" should be placed in the element after the final;
        // any elements after this will be ignored. Any lines that would exceed the screen
        // size will also be ignored.
       std::vector<std::string> popupLines; // TODO play with size

        // Number of remaining refresh cycles until the popup will no longer be shown
        uint32_t popupRemainingCycles;

        // If true, the popup will be shown regardless
        bool popupHold;

        Menu* menu;

        std::vector<std::string> headers;

        void drawPopup();

        // If true, knobs will not be processed by the gui so that the user can assign
        // their own custom functionality to them
        bool knobsDisabled;

    public:
        const static uint8_t SCREEN_WIDTH = 128;
        const static uint8_t SCREEN_HEIGHT = 64;

        // Used for hysteresis & assigning cv values to parameters
        std::array<float,      8> knobVals;
        std::array<bool,       8> knobIsChanged;
        std::array<float,      4> cvVals;
        std::array<MenuItem*,  4> cvMenuItems;

        // Don't use this
        FieldGui(){};

        // @param field
        // @param menu
        // @param fontDef
        // @param fontWidth
        // @param fontHeight
        // @param numHeaders
        FieldGui(DaisyField*, Menu*, FontDef*, uint8_t, uint8_t, uint8_t);

        // Utility to perform a silly little dance where we set the cursor, 
        // convert a std::string to char*, and pass it to WriteString()
        //
        // @param string
        // @param x coordinate
        // @param y coordinate
        void drawString(std::string, uint8_t, uint8_t);

        // Draws a string inside a rectange at center screen. Call this **after**
        // render() to superimpose the message over the rest of the gui.
        void drawBoxMessage(std::string s);

        // @param main string, drawn inside a rectangle center screen
        // @param version string, drawn below (supply "" to display nothing)
        // @param duration (ms)
        void drawStartupScreen(std::string, std::string, uint32_t);
 
        // Updates menu position, selection, etc. based upon
        // input from Fieldes' controls.
        //
        // Call this in the "update controls" function of the main event loop
        void updateControls();

        // Renders a new frame on Field's screen
        //
        // Call this in the "update oled" function of the main event loop
        void render();

        // Assigns a menuitem to a cv
        // @param page - must be a valid page index
        // @param title - must be a valid title of a menuItem in the given page
        // @param cv value (1-4)
        void assignCV(uint8_t, std::string, uint8_t);

        // Unassigns a CV from any menuitems
        // @param cv value (1-4)
        void unassignCV(uint8_t);

        // Cycles forward a page, wraps around.
        void pageFwd();

        // Cycles back a page, wraps around.
        void pageBack();

        // Call once per tick, before rendering, to maintain the current popup
        // until this isn't called again
        void keepPopup();

        /* Setters */

        // @param header string
        // @param header number (0-indexed)
        void setHeader(std::string, uint8_t);

        // @param Popup lines
        // @param time (in refresh cycles) that popup will exist
        void setPopup(std::vector<std::string>, uint32_t);

        // @param if true, disable knob input
        void setKnobsDisabled(bool b){ knobsDisabled = b; };

        // @param page (0-indexed)
        void setPage(uint8_t i){ menu->setPage(i); };

        /* Getters */
        // @return current page (0-indexed)
        uint8_t getPage(){ return menu->getPage(); };
    };
}