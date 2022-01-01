/* 
 * Copyright (C) 2021 Evan Pernu. Author: Evan Pernu
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
 * Jellybeans is a diatonic quanitizing arpeggiator designed
 * for the Electrosmith Daisy Patch eurorack module.
 */

#include "daisysp.h"
#include "daisy_patch.h"
#include "resources.h"

#include <string>
#include <array>

using namespace daisy;
using namespace daisysp;
using namespace jellybeans;
using namespace patch_gui;
using namespace ev_theory;

/*
 * Update this with each change
 */
const std::string VERSION = "1.3.0";

/*
 * Change this to enable debug output
 */
const bool DEBUG_MODE = false;

DaisyPatch* patch;
PatchGui*   gui;
Arp*        arp;
Menu*       menu;
Rhythm*     rhythm;

// Previous note in's semitone value
uint8_t lastNote;

// Used to track time divisions
// TODO factor this out into a rhythm sequencer library
uint16_t divMax;
uint16_t divCounter;

// DAC output value of bass note
float bassDac;

// Offset applied to inbound notes (semitones)
int8_t       inTune;
const int8_t MIN_IN_TUNE = -12;
const int8_t MAX_IN_TUNE =  12;

// Offsets applied to outbound note CVs (cents)
float        arpOutTune;
float        bassOutTune;
const int8_t MIN_OUT_TUNE = -100;
const int8_t MAX_OUT_TUNE =  100;
int16_t      bassOctMod;
int16_t      arpOctMod;
const int8_t MIN_OCT_MOD  = -4;
const int8_t MAX_OCT_MOD  =  4;

// Tracks the blinking icon next to bpm
int       blink;
const int BLINK_FRAMES = 35;

FontDef font       = Font_7x10;
uint8_t fontWidth  = 7;
uint8_t fontHeight = 10;

void updateControls();
void updateOled();
void updateOutputs();

// Compute a new bass CV value
void updateBassNote(){
    int semi = arp->getChord()->getRoot();
    semi += SEMIS_PER_OCT * menu->getItem("Bass Oct")->getIndex();
    bassDac = semitoneToDac(semi);
};

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
    updateBassNote();
};

void cbRoot(){
    arp->getChord()->setModeRoot(menu->getItem("Root")->getIndex());
    arp->updateTraversal();
    updateBassNote();
};

void cbArpOct(){
    arpOctMod = semitoneToDac(menu->getItem("Arp Oct")->getIndex() * SEMIS_PER_OCT);
};

void cbBassOct(){
    bassOctMod = semitoneToDac(menu->getItem("Bass Oct")->getIndex() * SEMIS_PER_OCT);
};

void cbNoteIn(){
    arp->getChord()->setDegreeByNote(menu->getItem("Note In")->getIndex());
    arp->updateTraversal();
    updateBassNote();
};

void cbClockDiv(){
    if(menu->getItem("Clock")->getIndex() == 0){
        // Internal: timing is determined by clock divisions
        divMax = clockDivTo256ths.at((menu->getItem("Clock Div")->getValue()));
    } else {
        // External: timing is determined by pulses per note.
        // Fractional clock values will just be set to 1.
        divMax = std::max(clockDivTo256ths.at(menu->getItem("Clock Div")->getValue())/256, 1);
    }
    divCounter = 0;
};

void cbClockMode(){
    bool internal = (menu->getItem("Clock")->getIndex() == 0);
    rhythm->setClock(internal);
    cbClockDiv();
};

void cbBPM(){
    rhythm->setBPM(menu->getItem("BPM")->getIndex());
}

void cbInTune(){
    inTune = menu->getItem("In Tune")->getIndex();
}

void cbArpOutTune(){
    arpOutTune = centsToDac(menu->getItem("Arp Tune")->getIndex());
}

void cbBassOutTune(){
    bassOutTune = centsToDac(menu->getItem("Bass Tune")->getIndex());
}

// Invoked whenever the timer ticks
void cbRhythm(){
    divCounter++;
    divCounter = divCounter % divMax;
    if(divCounter == 0){
       arp->onClockPulse();
    }
};

int main(void) {
    // Initialize vars and objects
    patch = new DaisyPatch();
    patch->Init();
    arp   = new Arp();
    menu  = new Menu();

    gui = new PatchGui(patch, menu, &font, fontWidth, fontHeight, 4);
    rhythm = new Rhythm(true, cbRhythm);

    if (!DEBUG_MODE){
        // Boot screen gets annoying during development
        gui->drawStartupScreen("Jellybeans", VERSION, 1500);
    }

    bassDac    = 0.f;
    lastNote   = 0;
    divMax     = clockDivTo256ths.at("1/4");
    divCounter = 0;
    blink      = 0;
    arpOctMod  = 0.f;
    bassOctMod = 0.f;

    // Initialize menu items
    menu->append("Pattern", "   ", arpPatterns,    0, cbPattern);
    menu->append("Voicing", "   ", voicings,       0, cbVoicing);
    menu->append("Inversion", " ", allInversions,  0, cbInversion);
    menu->append("Clock Div", " ", clockDivs,      5, cbClockDiv);
    menu->append("Arp Oct", "   ", MIN_OCT_MOD, MAX_OCT_MOD, 0, cbArpOct);
    menu->append("Root", "      ", allNotes,       0, cbRoot);
    menu->append("Mode", "      ", modes,          0, cbMode);
    menu->append("Bass Oct", "  ", MIN_OCT_MOD, MAX_OCT_MOD, 0, cbBassOct);
    menu->append("BPM", "       ", Rhythm::MIN_BPM, Rhythm::MAX_BPM, 120, cbBPM);
    menu->append("Clock", "     ", clockModes,     0, cbClockMode);
    menu->append("Note In", "   ", allNotes5Oct,   0, cbNoteIn);
    menu->append("In Tune", "   ", MIN_IN_TUNE,  MAX_IN_TUNE,  0, cbInTune);
    menu->append("Arp Tune", "  ", MIN_OUT_TUNE, MAX_OUT_TUNE, 0, cbArpOutTune);
    menu->append("Bass Tune", " ", MIN_OUT_TUNE, MAX_OUT_TUNE, 0, cbBassOutTune);
    
    // Initialize CV params
    gui->assignToCV("Pattern",   1);
    gui->assignToCV("Voicing",   2);
    gui->assignToCV("Inversion", 3);

    // "Prime" menu items
    cbBPM();
    cbInTune();
    cbArpOutTune();
    cbBassOutTune();

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
        rhythm->update();
    }
}

// Handle any input to Patches' hardware
void updateControls() {
    patch->ProcessAnalogControls();
    patch->ProcessDigitalControls();

    // Read v/oct from CTRL 4
    float ctrl = patch->GetKnobValue((DaisyPatch::Ctrl)3);
    uint8_t i = static_cast<uint8_t>(std::round(ctrl*60.f) + inTune);
    //i = quantizeNoteToRange(i); // TODO this may be redundant

    // Check that a new cv value has been input, otherwise encoder input to
    // note in won't work. Might remove later and just let CTRL 4 handle it
    if(i !=  lastNote){
        lastNote = i;
        menu->getItem("Note In")->setIndex(i);
    }
    
    // GATE IN 1 -> clock pulse
    blink--;
    if(patch->gate_input[0].Trig()){
        blink = BLINK_FRAMES;
        rhythm->pulse();
    }
}

void updateOutputs()
{
    // Arp CV -> CV OUT 1
    patch->seed.dac.WriteValue(DacHandle::Channel::ONE, 
            prepareDacValForOutput(arp->getDacValue() + arpOutTune + arpOctMod));

    // Arp Gate -> GATE OUT 1
    dsy_gpio_write(&patch->gate_output, arp->getTrig());

    // Bass CV -> CV OUT 2
    patch->seed.dac.WriteValue(DacHandle::Channel::TWO, 
            prepareDacValForOutput(bassDac + bassOutTune + bassOctMod));
}

void updateOled(){
    gui->updateControls();

    // Display a blinking '.' on every clock pulse in
    std::string h2 = "";
    if (blink > 0) {
        h2 += ".";
    }
    h2 += rhythm->bpmToString();

    // Set headers
    gui->setHeader(arp->toString(), 0);
    gui->setHeader(h2, 1);
    gui->setHeader(arp->getChord()->toString(), 2);
//    gui->setHeader(allNotes5Oct.at(lastNote), 3);

    // Keeping a few useful debug outputs here in case I need them later
    if (DEBUG_MODE){
        // gui->setHeader("CV1: " + std::to_string(static_cast<int>(bassDac)) + 
        //        " CV2: " + std::to_string(static_cast<int>(arp->getDacValue())), 2);
        //gui->setHeader(rhythm->toString(), 2); 
        //gui->setHeader(std::to_string(divCounter) + " " + std::to_string(divMax), 2); 
        //gui->setHeader(std::to_string(divCounter) + " " + std::to_string(divMax), 2); 
       // gui->setHeader(std::to_string(menu->getItem("BPM")->getIndex()), 2);
      // gui->setHeader(std::to_string(arp->getChord()->getOctave()), 2);
    }

    gui->render();
}