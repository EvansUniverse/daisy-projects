/* 
 * Copyright (C) 2021 Evan Pernu - All Rights Reserved
 * You may use, distribute and modify this code under the
 * terms of the GNU AGPLv3 license.
 * 
 * You should have received a copy of the GNU AGPLv3 license with
 * this file (LICENSE.md). If not, please write to: evanpernu@gmail.com, 
 * or visit: https://www.gnu.org/licenses/agpl-3.0.en.html
 * 
 * Graphics librrary for Electrosmith Daisy Patch
 */

#include "daisysp.h"
#include "daisy_patch.h"
#include "menu.h"

#include <string>

using namespace daisy;

namespace patch_gui {

    /*
     * UI System for Daisy Patch. It looks like this:
     * ----------------------------
     * | Header string            |
     * |--------------------------|
     * | > MenuItem1      value   |
     * |   MenuItem2      value   |
     * |   MenuItem3      value   |
     * |   MenuItem4      value   |
     * |   MenuItem5      value   |
     * ----------------------------
     * 
     * The header string(s) show data above the menu
     * The menu displays a list of menu items and their values
     * The '>' marker shows the currently selected menu item
     * 
     * Rotating the encoder knob on Patch scrolls through the menu.
     * Pressing the encoder enters edit mode, where rotating the 
     * encoder changes the value of the selected menu item.
     * 
     * There can be more than one header string. The menu will be 
     * dynamically sized to accomodate them:
     * ----------------------------
     * | Header string 1          |
     * | Header string 2          |
     * |--------------------------|
     * | > MenuItem1      value   |
     * |   MenuItem2      value   |
     * |   MenuItem3      value   |
     * |   MenuItem4      value   |
     * ----------------------------
     * 
     * There's also a toggle-able debug mode. When enabled, It will
     *  be displayed at the bottom in lieu of the last menu item:
     * ----------------------------
     * | Header string            |
     * |--------------------------|
     * | > MenuItem1      value   |
     * |   MenuItem2      value   |
     * |   MenuItem3      value   |
     * |   MenuItem4      value   |
     * |--------------------------|
     * | Debug string             |
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

        std::string headerStr; // TODO make this a vect to accomodate multiple headers
        std::string debugStr;
        bool debug;

        // Used to assigning cv values to parameters
        std::array<Parameter*, 4> cvParams;
        std::array<uint16_t,   4> cvVals;
        std::array<MenuItem*,  4> cvMenuItems;
    public:
        // I'll probably use these later
        const uint8_t SCREEN_WIDTH = 128;
        const uint8_t SCREEN_HEIGHT = 64;

        // Don't use this
        PatchGui();

        // @param thePatch
        // @param FontDef
        // @param fontWidth
        // @param fontHeight
        PatchGui(DaisyPatch*, Menu*, FontDef*, uint8_t, uint8_t);

        // Utility to perform a silly little dance where we set the cursor, 
        // convert a std::string to char*, and pass it to WriteString()
        //
        // @param string
        // @param x coordinate
        // @param y coordinate
        void drawString(std::string, uint8_t, uint8_t);

        // Updates menu position, selection, etc. based upon
        // input from Patches' controls.
        //
        // Call this each frame.
        void updateControls();

        // Renders a new frame on Patches' screen
        //
        // Call this each frame.
        void render();

        // Call this to render the debug string without re-rendering the whole gui.
        // Useful for println debugging.
        void renderDebug();

        // Assigns a parameter to a cv value
        // @param title - must be a valid title of a menuItem in menu
        // @param cv value (1-4)
        void assignToCV(std::string, uint8_t);

        void setHeaderStr(std::string);
        void setDebugStr(std::string);
        void setDebug(bool);
    };
}