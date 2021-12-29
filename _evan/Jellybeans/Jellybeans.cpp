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

// Change this to enable debug output
const bool DEBUG_MODE = false;
const std::string VERSION = "1.0.0";

DaisyPatch* patch;
PatchGui*   gui;
Arp*        arp;
Menu*       menu;
// Rhythm*     rhythm;

uint8_t  lastNote;
uint8_t  ppn;
uint16_t pulseCounter;
float    bassDac;

FontDef font       = Font_7x10;
uint8_t fontWidth  = 7;
uint8_t fontHeight = 10;

void updateControls();
void updateOled();
void updateOutputs();

/* Callback functions invoked whenever menu parameters are changed */

void cbPattern(){
    arp->setPattern(menu->getItem("Pattern")->getValue());  // TODO make the titles of each menuItem const
};

void cbVoicing(){
    arp->getChord()->setVoicing(menu->getItem("Voicing")->getValue());
    arp->updateTraversal();
};

void cbInversion(){
    arp->getChord()->setInversion(menu->getItem("Inversion")->getIndex());
    arp->updateTraversal();
};

void cbMode(){
    arp->getChord()->setMode(menu->getItem("Mode")->getValue());
    arp->updateTraversal();
    //cbBassOct();
};

// Compute a new bass CV value
void cbBassOct(){
    int semi = arp->getChord()->getRoot();
    semi += 12 * menu->getItem("Bass Oct")->getIndex();
    bassDac = semitoneToDac(semi);
};

void cbRoot(){
    arp->getChord()->setModeRoot(menu->getItem("Root")->getIndex());
    //arp->updateTraversal();
    //cbBassOct();
};

void cbOctave(){
    arp->getChord()->setOctave(menu->getItem("Octave")->getIndex());
    arp->updateTraversal();
    //cbBassOct();
};

void cbNoteIn(){
    arp->getChord()->setDegreeByNote(menu->getItem("Note In")->getIndex());
    arp->updateTraversal();
    cbBassOct();
};

void cbPPN(){
    ppn = std::stoi((menu->getItem("PPN")->getValue()));
    pulseCounter = 0;
};

// Callback function invoked whenever the timer ticks
// void cbRhythm(){
//     if (rhythm->getTick()/64 == 0){
//          arp->onClockPulse();
//     }
// };

int main(void) {
    // Initialize vars and objects
    patch  = new DaisyPatch();
    patch->Init();
    arp = new Arp();
    menu = new Menu();

    uint8_t numHeaders = 2;
    if (DEBUG_MODE){
        // Add an extra header for debug output
        numHeaders++;
    }
    gui = new PatchGui(patch, menu, &font, fontWidth, fontHeight, numHeaders);
    //rhythm = new Rhythm(false, cbRhythm);

    gui->drawStartupScreen("Jellybeans", VERSION, 1500);

    bassDac = 0.f;
    lastNote = 0;
    ppn = 1;
    pulseCounter = 0;

    // Initialize menu items
    menu->append("Pattern", "   ", arpPatterns,    0, cbPattern);
    menu->append("Voicing", "   ", voicings,       0, cbVoicing);
    menu->append("Inversion", " ", allInversions,  0, cbInversion);
    menu->append("PPN", "       ", allPPNs,        0, cbPPN); // Pulses per note
    menu->append("Octave", "    ", allOctaves,     0, cbOctave);
    menu->append("Root", "      ", allNotes,       0, cbRoot);
    menu->append("Mode", "      ", modes,          0, cbMode);
    // menu->append("Oct Rng", "   ", allOctaves,     0, cb);
    menu->append("Bass Oct", "  ", allBassOctaves, 0, cbBassOct);
    menu->append("Note In", "   ", allNotes5Oct,   0, cbNoteIn);
    
    // Initialize CV params
    gui->assignToCV("Pattern",   1);
    gui->assignToCV("Voicing",   2);
    gui->assignToCV("Inversion", 3);

    // "In case if you wondered, the fucking thing starts the circular DMA transfer
    // that receives ADC readings from knobs / CV inputs."
    //
    // Thanks, antisvin :P
    patch->StartAdc();

    // Main event loop
    while(1){
        updateControls();
        updateOled();
        updateOutputs();
        // rhythm->update();
    }
}

// Handle any input to Patches' hardware
void updateControls() {
    patch->ProcessAnalogControls();
    patch->ProcessDigitalControls();

    // Read v/oct from CTRL 4
    float ctrl = patch->GetKnobValue((DaisyPatch::Ctrl)3);
    uint8_t i = static_cast<uint8_t>(std::round(ctrl*60.f));

    // !!! HACK !!! (Currently disabled)
    // Voltage inputs from my Arturia Keystep were all a few hundredths
    // of a volt shy of what their volt/oct values should theoretically
    // be, resulting in everything to be a semitone flat. Not sure if this 
    // was due to Keystep's output or Patches' input. I'll have to figure 
    // out a more elegant solution later. Possibly adding a "trim" param
    // or a device calibration feature.
    //i--;

    // Check that a new cv value has been input, otherwise encoder input to
    // note in won't work. Might remove later and just let CTRL 4 handle it
    if(i !=  lastNote){
        lastNote = i;
        menu->getItem("Note In")->setIndex(i);
    }
    
    // GATE IN 1 -> clock pulse
    if(patch->gate_input[0].Trig()){
        pulseCounter++;
        if (pulseCounter == ppn){
            arp->onClockPulse();
        }
        pulseCounter = pulseCounter % ppn;  
    }
}

void updateOutputs()
{
    // Arp CV -> CV OUT 1
    patch->seed.dac.WriteValue(DacHandle::Channel::ONE, arp->getDacValue());

    // Arp Gate -> GATE OUT 1
    dsy_gpio_write(&patch->gate_output, arp->getTrig());

    // Bass CV -> CV OUT 2
    patch->seed.dac.WriteValue(DacHandle::Channel::TWO, bassDac);
}

void updateOled(){
    gui->updateControls();
    gui->setHeader(arp->toString(), 0);
    gui->setHeader(arp->getChord()->toString(), 1);

    // Keeping a few useful debug outputs here
    if (DEBUG_MODE){
        // gui->setHeader("CV1: " + std::to_string(static_cast<int>(bassDac)) + 
        //        " CV2: " + std::to_string(static_cast<int>(arp->getDacValue())), 2);
        // gui->setHeader(rhythm->toString(), 2); 
        // gui->setHeader(floatToString(debug, 3), 2); 
    }

    gui->render();
}