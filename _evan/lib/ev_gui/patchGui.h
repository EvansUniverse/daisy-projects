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

namespace patch_gui {

    /*
     * Simple UI System for Daisy Patch with a scrollable menu.
     * It looks like this:
     * ----------------------------
     * | Header 1        Header 2 |
     * |--------------------------|
     * | > MenuItem1      value   |
     * |   MenuItem2      value   |
     * |   MenuItem3      value   |
     * |   MenuItem4      value   |
     * |   MenuItem5      value   |
     * ----------------------------
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
     * ----------------------------
     * | Header 1        Header 2 |
     * | Header 3        Header 4 |
     * |--------------------------|
     * | > MenuItem1      value   |
     * |   MenuItem2      value   |
     * |   MenuItem3      value   |
     * |   MenuItem4      value   |
     * ----------------------------
     * 
     * Supply "" as a header to omit it.
     * ----------------------------
     * | Header 1                 |
     * | Header 3        Header 4 |
     * |--------------------------|
     * | > MenuItem1      value   |
     * |   MenuItem2      value   |
     * |   MenuItem3      value   |
     * |   MenuItem4      value   |
     * ----------------------------
     */
    class PatchGui {
    private:
        DaisyPatch* patch;
        Menu* menu;        
        bool isEditing;
    
        FontDef* font;
        uint8_t fontWidth;
        uint8_t fontHeight;

        std::vector<std::string> headers;

        // Used to assigning cv values to parameters
        std::array<Parameter*, 4> cvParams;
        std::array<uint16_t,   4> cvVals;
        std::array<MenuItem*,  4> cvMenuItems;
    public:
        // I'll probably use these later
        const static uint8_t SCREEN_WIDTH = 128;
        const static uint8_t SCREEN_HEIGHT = 64;

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
        // @param title - must be a valid title of a menuItem in menu
        // @param cv value (1-4)
        void assignToCV(std::string, uint8_t);

        // @param header string
        // @param header number (0-indexed)
        void setHeader(std::string, uint8_t);
    };
}