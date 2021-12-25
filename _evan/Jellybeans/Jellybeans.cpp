/* 
 * Copyright (C) 2021 Evan Pernu - All Rights Reserved
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
 * 
 * Jellybeans is an arpeggiator eurorack module designed for the 
 * Electrosmith Daisy Patch platform.
 */

#include "daisysp.h"
#include "daisy_patch.h"
#include "src/resources.h"

#include <string>
#include <array>

using namespace daisy;
using namespace daisysp;
using namespace jellybeans;
using namespace patch_gui;

DaisyPatch* patch;
PatchGui* gui;
Arp* arp;
Menu* menu;

FontDef font = Font_7x10;
int fontWidth = 7;
int fontHeight = 10;

void updateControls();
void updateOled();
void updateOutputs();

/* Callback functions invoked whenever menu parameters are changed */

void cb(){};

void cbPattern(){
    arp->setPattern(menu->getItem("Pattern")->getValue());  // TODO make the titles of each menuItem const
};

void cbVoicing(){
    arp->getChord()->setVoicing(menu->getItem("Voicing")->getValue());
    arp->updateTraversal();
};

void cbMode(){
    arp->getChord()->setMode(menu->getItem("Mode")->getValue());
    arp->updateTraversal();
};

void cbRoot(){
    arp->getChord()->setRoot(menu->getItem("Root")->getIndex());
    arp->updateTraversal();
};

void cbOctave(){
    arp->getChord()->setOctave(menu->getItem("Octave")->getIndex());
    arp->updateTraversal();
};

int main(void) {
    patch = new DaisyPatch();
    patch->Init();
    arp  = new Arp();
    menu = new Menu();
    gui  = new PatchGui(patch, menu, &font, fontWidth, fontHeight);

    gui->setDebug(true); // Uncomment this line to enable debug output

    // Initialize menu items
    // Note that the positions of items 0-3 need to remain fixed
    menu->append("Pattern", "   ", arpPatterns,    0, cbPattern);
    menu->append("Division", "  ", allClockInDivs, 0, cb); // Disabled
    menu->append("Voicing", "   ", voicings,       0, cbVoicing);
    menu->append("Inversion", " ", allInversions,  0, cb); // Disabled
    menu->append("Root", "      ", allNotes,       0, cbRoot);
    menu->append("Mode", "      ", modes,          0, cbMode);
    menu->append("Rhythm", "    ", emptyVect,      0, cb); // Disabled
    menu->append("Oct Rng", "   ", allOctaves,     0, cb); // Disabled
    menu->append("Octave", "    ", allOctaves,     0, cbOctave);
    menu->append("Clock In", "  ", allClockInDivs, 0, cb); // Disabled

    // Initialize CV params
    gui->assignToCV("Pattern",   1);
    gui->assignToCV("Division",  2);
    gui->assignToCV("Voicing",   3);
    gui->assignToCV("Inversion", 4);

    patch->StartAdc();

    // Main event loop
    while(1){
        updateControls();
        updateOled();
        updateOutputs();
    }
}

// Handle any input to Patches' controls
void updateControls() {
    patch->ProcessAnalogControls();
    patch->ProcessDigitalControls();

    // Update step with respect to clock
    // Accept input from either GATE IN
    //
    // Currently, we'll just do 1 step per clock pulse
    if(patch->gate_input[0].Trig() || patch->gate_input[1].Trig()){
        arp->onClockPulse();
    }
}

void updateOutputs()
{
    patch->seed.dac.WriteValue(DacHandle::Channel::ONE, arp->getDacValue());
    dsy_gpio_write(&patch->gate_output, arp->getTrig());
}

void updateOled(){
    gui->updateControls();
    gui->setHeaderStr(arp->toString());
    gui->setDebugStr(arp->getChord()->toString()); // Uncomment this line and change string to set debug output
    gui->render();
}