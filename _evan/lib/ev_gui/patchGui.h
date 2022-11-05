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
#include "daisy_patch.h"
#include "menu.h"

#include <string>

using namespace daisy;

namespace ev_gui {
    const static float   HYSTERESIS_THRESH = 0.005;
    const static uint8_t SCREEN_WIDTH = 128;
    const static uint8_t SCREEN_HEIGHT = 64;

    /*
     * Simple UI System for Daisy Patch. It has two possible modes of operation:
     *
     * ========== Mode 1: Pageless mode, scrollable menu. ==========
     *
     * This mode is automatically used when the constructor is given a menu with 
     * 1 page. It looks like this:
     *  --------------------------
     * | Header 1        Header 2 |
     * |--------------------------|
     * | > MenuItem1      value   |
     * |   MenuItem2      value   |
     * |   MenuItem3      value   |
     * |   MenuItem4      value   |
     * |   MenuItem5      value   |
     *  --------------------------
     * 
     * The header string(s) show data above the menu.
     * The menu displays a list of menu items and their values.
     * The '>' marker shows the currently selected menu item.
     * 
     * Rotating the encoder knob on Patch scrolls through the menu.
     * Pressing the encoder enters edit mode, where rotating the 
     * encoder changes the value of the selected menu item.
     * 
     * There can be more than one header string. The scrollable menu
     * will be resized accordingly:
     *  --------------------------
     * | Header 1        Header 2 |
     * | Header 3        Header 4 |
     * |--------------------------|
     * | > MenuItem1      value   |
     * |   MenuItem2      value   |
     * |   MenuItem3      value   |
     * |   MenuItem4      value   |
     *  --------------------------
     * 
     * Supply "" as a header to omit it.
     *  --------------------------
     * | Header 1                 |
     * | Header 3        Header 4 |
     * |--------------------------|
     * | > MenuItem1      value   |
     * |   MenuItem2      value   |
     * |   MenuItem3      value   |
     * |   MenuItem4      value   |
     *  --------------------------
     * 
     * ========== Mode 2: Page mode, pages with 4 params each ==========
     *
     * This mode is automatically used when the constructor is given a menu with 
     * more than 1 page. It looks like this:
     *  --------------------------
     * | Header 1        Header 2 |
     * | Header 3        Header 4 |
     * |--------------------------|
     * |   MenuItem1      value   |
     * |   MenuItem2      value   |
     * |   MenuItem3      value   |
     * |   MenuItem4      value   |
     *  --------------------------
     * 
     * The header string(s) show data above the menu. The menu displays a list
     * of menu items and their values. There are (up to) 4 menu items per page, 1
     * for each CV knob. There are also 2 zones (collections of pages) that can be
     * switched between, which is ideal for something like a hidden settings menu.
     * 
     * -Rotating the encoder on Patch scrolls through the pages.
     * -Quickly pressing, briefly holding, and long holding the encoder all have
     *  assignable callback functions.
     * -Rotating the CV knobs adjust the 4 menu parameters.
     * 
     * There is room for 4 header strings in 2 rows.
     */
    class PatchGui {
    private:
        DaisyPatch* patch;
        Menu* menu;        
        bool isEditing;

        // If true, in page mode (see above)
        bool pageMode;
    
        FontDef* font;
        uint8_t fontWidth;
        uint8_t fontHeight;

        std::vector<std::string> headers;

        // Used to assigning cv values to parameters & hysteresis
        std::array<Parameter*, 4> cvParams;
        std::array<float,      4> cvVals;
        std::array<MenuItem*,  4> cvMenuItems;

        // Tracks whether or not each CV has been "caught" for knob catch behavior
        std::array<bool, 4> cvIsCaught;
        bool cvCatch;

        // Callback functions called & popup messages displayed during various encoder presses
        std::function<void ()> cbPress;
        std::function<void ()> cbShortHold;
        std::function<void ()> cbLongHold;

        std::vector<std::string> msgLongHold;
        std::vector<std::string> msgPress;
        std::vector<std::string> msgShortHold;

        uint32_t popupTimePress;
        uint32_t popupTimeShortHold;
        uint32_t popupTimeLongHold;

        bool popupTypePress;
        bool popupTypeShortHold;
        bool popupTypeLongHold;
        
        bool didHold;
        float timeHeld;

        // If 0, there is only 1 zone
        uint8_t zoneB;
        bool isZoneB;

        // Contains each line of text to be displayed in the popup
        //
        // The delimiter "_END_" should be placed in the element after the final 
        // (the setter will automatically do this) and any elements after this will 
        // be ignored. Any lines that would exceed the screen  size will also be ignored.
        std::vector<std::string> popupLines;

        // Number of remaining refresh cycles until the popup will no longer be shown
        uint32_t popupRemainingCycles;

        // If true, the popup will be shown regardless
        bool popupHold;

        // Type of the current popup
        // True for banner, false for center. If I add more types, I'll probably change this to an enum.
        bool popupType;
        
    public:
        // If true, draw a little loading bar at the top that shows how long the encoder's been held
        bool drawHoldBar;

        // Don't use this
        PatchGui(){};

        // @param thePatch
        // @param fontDef
        // @param fontWidth
        // @param fontHeight
        // @param numHeaders
        PatchGui(DaisyPatch*, Menu*, FontDef*, uint8_t, uint8_t, uint8_t);

        // Utility to perform a silly little dance where we set the cursor, 
        // convert a std::string to char*, and pass it to WriteString()
        //
        // @param string
        // @param x coordinate
        // @param y coordinate
        void drawString(std::string, uint8_t, uint8_t);

        // @param main string, drawn inside a rectangle center screen
        // @param version string, drawn below (supply "" to display nothing)
        // @param duration (ms)
        void drawStartupScreen(std::string, std::string, uint32_t);

        // Updates menu position, selection, etc. based upon
        // input from Patches' controls.
        //
        // Call this each frame.
        void updateControls();

        // Renders a new frame on Patches' screen
        //
        // Call this each frame.
        void render();

        // Assigns a parameter to a cv value
        // @param a valid title of a menuItem in menu
        // @param cv value (1-4)
        void assignToCV(std::string, uint8_t);

        // Assigns a parameter to a cv value
        // @param pointer to a menuItem in menu
        // @param cv value (1-4)
        void assignToCV(MenuItem*, uint8_t);

        // Assigns the 4 CVs to the first 4 items in the current page
        void assignCVsToPage();

        // @param header string
        // @param header number (0-indexed)
        void setHeader(std::string, uint8_t);

        // @param encoder press callback function
        // @param popup messag displayed when a press happens
        // @param duration (in refresh cycles) the popup is displayed
        // @param popup type (true for banner, false for center)
        void setCbPress(std::function<void ()>, std::vector<std::string>, uint32_t, bool);

        // Sets a callback with no message displayed
        //
        // @param encoder press callback function
        void setCbPress(std::function<void ()>);

        // @param encoder short hold callback function
        // @param popup messagdisplayed when a short hold happens
        // @param duration (in refresh cycles) the popup is displayed
        // @param popup type (true for banner, false for center)
        void setCbShortHold(std::function<void ()>, std::vector<std::string>, uint32_t, bool);

        // Sets a callback with no message displayed
        //
        // @param encoder press callback function
        void setCbShortHold(std::function<void ()>);

        // @param encoder long hold callback function
        // @param popup messagdisplayed when a long hold happens
        // @param duration (in refresh cycles) the popup is displayed
        // @param popup type (true for banner, false for center)
        void setCbLongHold(std::function<void ()>, std::vector<std::string>, uint32_t, bool);

        // Sets a callback with no message displayed
        //
        // @param encoder press callback function
        void setCbLongHold(std::function<void ()>);

        void setMsgLongHold(std::vector<std::string> v){ msgLongHold = v; };

        // @param if true, enable CV knob catch behavior (they must be turned to the param's
        // current value before being able to alter its value). Off by default.
        void setCvCatch(bool b){ cvCatch = b; };

        void setAllIsCaught(bool b){
            for(uint8_t i = 0; i < 4; i++){
                cvIsCaught[i] = b;
            }
        };

        // Cycles forward a page, wraps around.
        void pageFwd();

        // Cycles back a page, wraps around.
        void pageBack();

        // @param page number (0-indexed)
        // @param behavior if hidden page is encountered: true=skip forward false=skip backward
        void setPage(uint8_t, bool);

        // @param page number (0-indexed)
        void setPage(uint8_t i){ setPage(i, true); };

        // @param menu index at which zone B will start
        void setZoneB(uint8_t i){ zoneB = i; };

        // @param index
        // @param isHidden
        void setIsHidden(uint8_t, bool);

        // @param Popup lines
        // @param time (in refresh cycles) that popup will exist
        // @param if true, it will be a banner popup (1 line at top of screen). 
        // If false, it will be a center popup (up to 4 lines in center screen)
        void setPopup(std::vector<std::string>, uint32_t, bool);

        // Call once per tick, before rendering, to maintain the current popup
        // until this isn't called again
        void keepPopup();

        // Toggles between zones A and B
        void changeZones();

        // @return isZoneB
        bool getIsZoneB(){ return isZoneB; };
    };
} // namespace ev_gui