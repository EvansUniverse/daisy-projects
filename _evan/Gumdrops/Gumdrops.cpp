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

/*
 * Step: Contains information for one sequencer step (note, length, etc.)
 * Pattern: Contains a set of steps and whether or not each step is activated
 * Sequencer: Has a pattern, plays that pattern
 */

#include "daisysp.h"
#include "daisy_patch.h"
#include "resources.h"

#include <string>
#include <array>

#define NUM_BUTTONS 16
#define NUM_PATTERNS 16
#define GD_MAX_BPM 200
#define GD_MIN_BPM 40
#define PAGE_OFFSET (pageView - 1) * 16
#define HYSTERESIS_THRESH 0.005

/* Indices of certain settings in settings_ram */
#define SETTINGS_BUFF_SIZE 8
#define RAM_IS_INIT       0 // Used to detect a fresh install
#define RAM_PATTERN_INDEX 1 // Index of the currently active pattern
#define RAM_BPM           2
#define RAM_IN_TUNE       3
#define RAM_OUT_TUNE      4
#define RAM_MIDI_TX_CH    5
#define RAM_HUMANIZE      6
#define RAM_QUANTIZE_REC  7

#define MIN_TUNE -12
#define MAX_TUNE 12

using namespace daisy;
using namespace daisysp;
using namespace gumdrops;
using namespace ev_gui;
using namespace ev_theory;

/*
 * Update this with each change
 */
const std::string VERSION = "0.2.0dev1";

/*
 * Change this to enable output
 */
const bool DEBUG_MODE = true;

/*
 * Change this to re-initialize data on startup
 */
const bool RESET_DATA_ON_STARTUP = false;

// Base memory addresses for QSPI storage
//
// Note that Daisy can only erease data in blocks of 4kb, 32kb or 64kb;
// hence leaving 4096 bytes for settings. 
// See for more info: https://forum.electro-smith.com/t/persisting-data-to-from-flash/502/8
const uint32_t BASE_QSPI_ADDR_SETTINGS = 0x90000000;
const uint32_t BASE_QSPI_ADDR_DATA     = 0x90004096;

// State objects (QSPI)
uint8_t DSY_QSPI_BSS settings_qspi[SETTINGS_BUFF_SIZE];
Pattern DSY_QSPI_BSS patterns_qspi[NUM_PATTERNS];

// State objects (RAM)
DaisyField* hw;
FieldGui*   gui;
Menu*       menu;
Tempo*      tempo;
Sequencer*  seq;
Pattern     patterns_ram[NUM_PATTERNS];
uint8_t     settings_ram[SETTINGS_BUFF_SIZE];

// The index of the step the user most recently selected
uint16_t selectedStep;

// Steps currently being edited
std::vector<Step*> editSteps;

// Stores if each pad is currently pressed
bool isPressed[16];

// If true, a step is currently being edited i.e. held down by the user
// TODO I can probably remove this and replace it with `editSteps.size() > 0`
bool isEditingStep;

// Stores dac1 so it isn't re-calculated until a new note is played
uint16_t cachedDac1;

enum padModes {step, pattern}; // In the future: bank, project, menu

// The current pad mode
padModes padMode;

// The sequencer page currently being viewed
uint8_t pageView;

// If true, the note was playing last frame. Used to determine if a new
// MIDI note on message should be sent.
bool wasPlaying;

// If true, input is currently being recorded through CV/gate
bool isRecording;

// Store the current note to be used in MIDI note off messages
uint8_t curNote;

// Field's gate_in->Trig() seems to not work so I'll just implement my own
bool prevTrig;

// We're using CV4 like a trig in for this case
bool prevTrigCV4;

// Index of cued pattern, -1 if no pattern is cued
int8_t cuedPattern;

// Used to track note input
int16_t noteIn;
bool gateIn;

// Used for debug display
std::string debugString;

FontDef font       = Font_6x8;
uint8_t fontWidth  = 6;
uint8_t fontHeight = 8;

// Copy the values in settings_ram (volatile) to settings_qspi (non-volatile)
void saveSettingsToQSPI(){
    size_t size = sizeof(settings_qspi);
    hw->seed.qspi.Erase(BASE_QSPI_ADDR_SETTINGS, BASE_QSPI_ADDR_SETTINGS + size);
    hw->seed.qspi.Write(BASE_QSPI_ADDR_SETTINGS, size, (uint8_t*)settings_ram);
}

void loadQSPISettingsToRAM(){
    memcpy(settings_ram, reinterpret_cast<void*>(BASE_QSPI_ADDR_SETTINGS), sizeof(settings_ram));
}

// Writes a set of default settings to QSPI storage
// Called when QSPI storage is empty (fresh install)
void saveDefaultSettingsToQSPI(){
    settings_ram[RAM_IS_INIT]        = 42;
    settings_ram[RAM_PATTERN_INDEX]  = 0;
    settings_ram[RAM_BPM]            = 120;
    settings_ram[RAM_IN_TUNE]        = 0 - MIN_TUNE;
    settings_ram[RAM_OUT_TUNE]       = 0 - MIN_TUNE;
    settings_ram[RAM_MIDI_TX_CH]     = 0;
    settings_ram[RAM_HUMANIZE]       = 0;
    settings_ram[RAM_QUANTIZE_REC]   = 1;
    saveSettingsToQSPI();
}

// Saves pattern data to QSPI
void saveDataToQSPI(){
    size_t size = sizeof(patterns_qspi);
    hw->seed.qspi.Erase(BASE_QSPI_ADDR_DATA, BASE_QSPI_ADDR_DATA + size);
    hw->seed.qspi.Write(BASE_QSPI_ADDR_DATA, size, (uint8_t*)patterns_ram);
}

// Loads pattern data from QSPI
void loadQSPIDataToRAM(){
    memcpy(patterns_ram, reinterpret_cast<void*>(BASE_QSPI_ADDR_DATA), sizeof(patterns_ram));
}

// Writes a set of default pattern data to QSPI storage
// Called when QSPI storage is empty (fresh install)
void saveDefaultDataToQSPI(){
    for(uint8_t i = 0; i < NUM_PATTERNS; i++){
        patterns_ram[i] = *(new Pattern(16, clockDivTo256ths.at("1/16")));
    }
    patterns_ram[0].setIsNew(false);
    saveDataToQSPI();
}

void loadMenuValsFromPattern(Pattern* p){
    for(uint8_t i = 0; i < clockDivs.size(); i++){
        if(clockDivs[i] == conv256thToClockDiv.at(p->getTicksPerStep())){
            menu->getItem("Clock Div")->setIndex(i);
        }
    }

    menu->getItem("Page")->setIndex(1);
    static_cast<MenuItemInt*>(menu->getItem("Page"))->setMax(p->getMaxSteps()/16);

    menu->getItem("Max Pages")->setIndex(p->getMaxSteps()/16);
}

// Populates the step edit menu with values of the currently selected step
void populateStepEditMenu(Step* s){

    // Set "Length" to the nearest value corresponding to stepLengths
    for(uint8_t i = 0; i < stepLengths.size(); i++){
        if(s->getLength() <= stepLengthsToTicks.at(stepLengths[i])){
            menu->getItem("Length")->setIndex(i);
            break;
        }
    }

    //menu->getItem("Slide")->setIndex(static_cast<uint8_t>(s->getSlide()));
    menu->getItem("Note")->setIndex(s->getNote());
    menu->getItem("Octave")->setIndex(s->getOctave());
    menu->getItem("CV2")->setIndex(s->getCV2());
    //menu->getItem("uTime")->setIndex(s->getUTime());
}

void updateLeds(){
    hw->led_driver.SetAllTo(0.f);

    switch (padMode){
    // LEDs show pattern data
    // - Step currently being played: 100%
    // - Activated steps: 60%
    // - Empty steps: 0%
    case step:
        for (uint8_t i = 0; i < NUM_BUTTONS; i++){
            if (seq->getPattern()->getStepAt(i + PAGE_OFFSET)->getIsActive()){
                hw->led_driver.SetLed(stepLeds[i], 0.6f);
            } else {
                hw->led_driver.SetLed(stepLeds[i], 0.f);
            }
        }
        if(seq->getPattern()->getIndex() >= PAGE_OFFSET && seq->getPattern()->getIndex() <= (PAGE_OFFSET + 16)){ // TODO bad math here
            hw->led_driver.SetLed(stepLeds[seq->getPattern()->getIndex() % 16], 1.0f);
        }
        break;
    // LEDs show available patterns
    // - Currently selected pattern: 100%
    // - Currently populated patterns: 40%
    // - Empty patterns: 0%
    case pattern:
        for (uint8_t i = 0; i < NUM_BUTTONS; i++){
            if(!patterns_ram[i].getIsNew()){
                hw->led_driver.SetLed(stepLeds[i], 0.4f);
            } else {
                hw->led_driver.SetLed(stepLeds[i], 0.f);
            }
        }
        hw->led_driver.SetLed(stepLeds[settings_ram[RAM_PATTERN_INDEX]], 1.0f);
        break;
    }

    // Light up knob led based on currently playing page
    hw->led_driver.SetLed(knobLeds[seq->getPattern()->getIndex()/16], 0.3f);

    hw->led_driver.SwapBuffersAndTransmit();
}

void updateOled(){
    // Clear screen
    hw->display.Fill(false);  

    // Blink every 1/4 note (agnostic of the sequencer state, for debugging)
    // TODO come back when you aren't sleep deprived and find a cleaner solution, ya goofball.
    std::string blinkStr = "";
    if ((tempo->getTick() > 0 && tempo->getTick() < 10) ||
        (tempo->getTick() > 64 && tempo->getTick() < 74) ||
        (tempo->getTick() > 128 && tempo->getTick() < 138) ||
        (tempo->getTick() > 192 && tempo->getTick() < 202))
        {
        blinkStr = "o";
    }

    if(isRecording){
        // Display isRecording popup until recording is stopped
        gui->keepPopup();
    }

    std::string header;
    if(tempo->getIsRunning()){
        header = "BPM: " + tempo->bpmToString() + blinkStr;
    } else {
        header = "STOPPED";
    }

    gui->setHeader(header, 0);
    if(DEBUG_MODE){
        gui->setHeader("", 1);
        gui->setHeader(debugString, 2);
    }
    gui->render();
}

void updateControls() {
    hw->ProcessAllControls();

    bool wasEditingStep = isEditingStep;
    isEditingStep = false;

    /* Process CV/Gate in */

    // TODO: it looks like the displayed value of ctrrl is inconsistent. Watch the value of ctrl on the first key press down vs first release. It stabilizes after that, on the second keypress. 
    // GOTTEM: the issue isn't with step->note. On the first keypress down, i and ctrl arew wrong (but i does match step->note) until the key is released up. Then, the second press, it's right.
    // It's possible that the correct voltage change is getting send a teeny bit aftter the trigger signal. If this is the case, maybe we need some sort of logic to change the note mid-play if the cv changes.

    // CV 1 & Gate in -> recording
    gateIn = hw->gate_in.State();

    float ctrl = hw->GetCvValue(0);
    int16_t i = static_cast<int16_t>(std::round(ctrl*60.f));// + settings_ram[RAM_IN_TUNE]); // TODO rename
    //i = quantizeNoteToRange(i);
    noteIn = semitoneToDac(i);

    if(isRecording){
        if (prevTrig != gateIn) {
            seq->updateRec(i);
            if(seq->recStep != NULL){
                debugString =  floatToString(ctrl, 6) + " " + std::to_string(i) + " " + std::to_string(seq->recStep->note);
            }
        }
        prevTrig = gateIn;
    }
    

    // CV IN 4 -> clock pulse
    // Gumdrops treats CV4 like a trig in. If the value is above ~3.5v, the gate is high.
    bool cv4Gate = hw->GetCvValue(3) > 0.7f;
    bool trig = (prevTrigCV4 != cv4Gate);
    if (trig) {
        prevTrigCV4 = cv4Gate;
    }
    if(trig && cv4Gate){
        tempo->pulse();
    }


    /* Process LED buttons */
    for (uint8_t i = 0; i < NUM_BUTTONS; i++){
        // Because for some stupid fucking reason, LED_KEY_B7 actually lights 
        // up the key labelled  A7 on Field and vice versa.
        uint8_t j = (i + 8) % 16;

        if (hw->KeyboardRisingEdge(j) && !isPressed[i]){
            // Executes once when a key is pressed
            isPressed[i] = true; 
            switch (padMode){
            case step:
                selectedStep = i + PAGE_OFFSET;
                seq->getPattern()->getStepAt(selectedStep)->toggleIsActive();
                break;
            case pattern:
                cuedPattern = i;
                seq->cuePattern(&patterns_ram[i]);
                break;
            }
        } else if (hw->KeyboardFallingEdge(j)){
            // Executes once when a key is released
            isPressed[i] = false; 
        }

        if (isPressed[i] && padMode == step){ isEditingStep = true; }
    }

    /* Process Knobs */

    if(isEditingStep){
        // Show step edit menu if a note is held down
        gui->setPage(2);
    } else if (gui->getPage() == 2){
        // Revert to main menu when notes are released
        gui->setPage(0);
    }

    // Determine which steps to edit
    editSteps = {};
    if(isEditingStep){
        for(uint8_t i=0; i < NUM_BUTTONS; i++){
            if(isPressed[i]){
                editSteps.push_back(seq->getPattern()->getStepAt(i + PAGE_OFFSET));
            }
        }
    }

    if(!wasEditingStep && isEditingStep && editSteps.size() > 0){
        populateStepEditMenu(editSteps[0]);
    }

    // Don't deactivate a step if it's edited while held
    for(uint8_t i = 0; i < 8; i++){
        if(isEditingStep && gui->knobIsChanged[i]){
            seq->getPattern()->getStepAt(selectedStep)->setIsActive(true);
        }
    }

    /* Process SW buttons */
    
    // if(hw->sw[0].Pressed() && hw->sw[1].RisingEdge()){
    //     // Holding SW 1 + Pressing SW 2 starts recording

    //     // Page back since pressing SW 1 cycled page
    //     gui->pageBack();

    //     if(!isRecording){
    //         isRecording = true;
    //         gui->setPopup(std::vector<std::string>{"Recording...", " ", "Press SW 1 to stop."}, 1);
    //     }
    // } else 
    if(hw->sw[0].RisingEdge()){
        // Pressing SW1 1 saves data
        gui->setPopup(std::vector<std::string>{"Saving data..."}, 400);
        // Since saveDataToQSPI() takes a sec, update oled immediately to display popup wihtout latency
        updateOled();
        saveSettingsToQSPI();
        saveDataToQSPI();
    } else if(hw->sw[1].Pressed() && hw->sw[0].RisingEdge()){
        // Holding SW 2 + Pressing SW 1 either stops recording or cycles menu
        if(isRecording){
            isRecording = false;
            seq->stopRec();
        } else {  
            gui->pageFwd();

            // Don't display note edit menu
            if(gui->getPage() >= 2){
                gui->setPage(0);
            }
        }


    }
    if (hw->sw[1].Pressed() && !hw->sw[0].Pressed()){
        // Holding SW 2 shows pattern select mode
        padMode = pattern;
    } else {
        padMode = step;
    }

    gui->updateControls();
}

void updateOutputs() {
    /* MIDI */
    
    if(!wasPlaying && seq->isPlaying()) {
        // Note just started
        wasPlaying = true;
        curNote = seq->getCurStep()->getNote();
        MIDISendNoteOn(&(hw->midi), settings_ram[RAM_MIDI_TX_CH], curNote, 127); 

    } else if (wasPlaying && !seq->isPlaying()){
        // Note just ended
        wasPlaying = false;
        MIDISendNoteOff(&(hw->midi), settings_ram[RAM_MIDI_TX_CH], curNote);
    }


    /* CV & Gate */
    if(gateIn){
        // True -> GATE OUT
        dsy_gpio_write(&hw->gate_out, true);

        // v/oct in -> CV OUT 1
        cachedDac1 = noteIn;

        // TODO consider using the prepareforoutput function since Daisy can't output a  CV value
        // of 0. I'll leave it out for nn
        hw->seed.dac.WriteValue(DacHandle::Channel::ONE, prepareDacValForOutput(noteIn));
    } else {
        // Sequencer Gate -> GATE OUT
        dsy_gpio_write(&hw->gate_out, seq->isPlaying());

        // Sequencer note v/oct -> CV OUT 1
        // Don't change pitch unless a new, active note is playing.
        uint16_t curDac1 = seq->getCurStep()->getDac1();// + settings_ram[RAM_OUT_TUNE] + MIN_TUNE;
        if(seq->isPlaying() && seq->getCurStep()->getIsActive() && (curDac1 != cachedDac1)){
            cachedDac1 = curDac1;
        }
        hw->seed.dac.WriteValue(DacHandle::Channel::ONE, prepareDacValForOutput(cachedDac1));
    }

    // Sequencer CV 2 -> CV OUT 2
    // Unlike pitch, CV2 value should not run past the duration of its note.
    hw->seed.dac.WriteValue(DacHandle::Channel::TWO, prepareDacValForOutput(
            seq->getCurStep()->getDac2()));
}

void updateState(){
    if(cuedPattern != -1 && !seq->hasCuedPattern()){
        settings_ram[RAM_PATTERN_INDEX] = cuedPattern;
        saveSettingsToQSPI();
        loadMenuValsFromPattern(&patterns_ram[settings_ram[RAM_PATTERN_INDEX]]);
        cuedPattern = -1;
    }
}

// Invoked whenever tempo ticks
void cbTempo(){
    seq->tick(tempo->getTick());
};

void cbBPM(){
    settings_ram[RAM_BPM] = menu->getItem("Int. BPM")->getIndex();
    saveSettingsToQSPI();
    tempo->setBPM(static_cast<float>(settings_ram[RAM_BPM]));
}

void cbPage(){
    pageView = static_cast<uint8_t>(menu->getItem("Page")->getIndex());
}

void cbMaxPages(){
    seq->getPattern()->setMaxSteps(
        16 * static_cast<uint8_t>(menu->getItem("Max Pages")->getIndex()));
    static_cast<MenuItemInt*>(menu->getItem("Page"))->setMax(menu->getItem("Max Pages")->getIndex());
}

void cbClockDiv(){
    seq->getPattern()->setTicksPerStep(
        clockDivTo256ths.at(menu->getItem("Clock Div")->getValue()));
}

// void cbInTune(){
//     settings_ram[RAM_IN_TUNE] = menu->getItem("In Tune")->getIndex();
//     saveSettingsToQSPI();
// }

void cbOutTune(){
    settings_ram[RAM_OUT_TUNE] = menu->getItem("Out Tune")->getIndex();
    saveSettingsToQSPI();
}

void cbMidiTxCh(){
    settings_ram[RAM_MIDI_TX_CH] = menu->getItem("MIDI Tx Ch")->getIndex();
    saveSettingsToQSPI();
}

void cbHumanize(){
    settings_ram[RAM_HUMANIZE] = menu->getItem("Humanize")->getIndex();
    seq->setHumanize(settings_ram[RAM_HUMANIZE] * 0.01f);
    saveSettingsToQSPI();
}

// void cbQuantizeRec(){
//     settings_ram[RAM_QUANTIZE_REC] = menu->getItem("Quantize")->getIndex();
//     seq->setQuantizeRec(settings_ram[RAM_QUANTIZE_REC] > 0);
//     saveSettingsToQSPI();
// }

void cbStepLength(){
    for(uint8_t i = 0; i < editSteps.size(); i++){
        editSteps[i]->setLength(
            stepLengthsToTicks.at(menu->getItem("Length")->getValue())-1);
    }
}

void cbStepNote(){
    for(uint8_t i = 0; i < editSteps.size(); i++){
        editSteps[i]->setNote(menu->getItem("Note")->getIndex());
    }
}

void cbStepOctave(){
    for(uint8_t i = 0; i < editSteps.size(); i++){
        editSteps[i]->setOctave(menu->getItem("Octave")->getIndex());
    }
}

void cbStepCV2(){
    for(uint8_t i = 0; i < editSteps.size(); i++){
        // TODO fix
        editSteps[i]->setCV2(menu->getItem("CV2")->getIndex());
    }
}

// void cbStepUTime(){
//     for(uint8_t i = 0; i < editSteps.size(); i++){
//         editSteps[i]->setUTime(menu->getItem("uTime")->getIndex());
//     }
// }

// void cbStepSlide(){
//     for(uint8_t i = 0; i < editSteps.size(); i++){
//         editSteps[i]->setSlide(static_cast<bool>(menu->getItem("Slide")->getIndex()));
//     }
// }


int main(void) {
    /* Init hardware */
    hw = new DaisyField();
    hw->Init(); // Don't try to use SDRAM until after this Init function

    /* Load state from QSPI */
    loadQSPISettingsToRAM();
    if (settings_ram[RAM_IS_INIT] != 42 || RESET_DATA_ON_STARTUP){
        // If the RAM_IS_INIT isn't 42, we know that there are no settings
        // in QSPI and we should generate default settings.
        saveDefaultSettingsToQSPI();
        saveDefaultDataToQSPI();
    }
    loadQSPIDataToRAM();

    /* GUI */
    menu  = new Menu(3);

    // Page 1 (step mode)
    menu->append(0, "Page", "       ", 1,          8,          1,                     cbPage);
    menu->append(0, "Max Pages", "  ", 1,          8,          1,                     cbMaxPages);
    menu->append(0, "Clock Div", "  ", clockDivs,              0,                     cbClockDiv);
    menu->append(0, "Int. BPM", "   ", GD_MIN_BPM, GD_MAX_BPM, settings_ram[RAM_BPM], cbBPM);
    //menu->append(0, "Humanize", "   ", 0, 100, settings_ram[RAM_HUMANIZE],            cbHumanize);

    // Page 2 (menu mode)
    //menu->append(1, "Quantize", "   ", offOrOn,                settings_ram[RAM_QUANTIZE_REC],        cbQuantizeRec);
    //menu->append(1, "In Tune", "    ", MIN_TUNE,   MAX_TUNE,   settings_ram[RAM_IN_TUNE] + MIN_TUNE,  cbInTune);
    menu->append(1, "Out Tune", "   ", MIN_TUNE,   MAX_TUNE,   settings_ram[RAM_OUT_TUNE] + MIN_TUNE, cbOutTune);
    menu->append(1, "MIDI Tx Ch", " ", 0,          15,         settings_ram[RAM_MIDI_TX_CH],          cbMidiTxCh);
    // MIDI Rx Ch
    // MIDI Rx Md
    // Pnt SW Md
    // Clear Ptn
    // Clear all

    // Page 3 (step edit mode)
    menu->append(2, "Length", "     ", stepLengths, 2, cbStepLength);
    menu->append(2, "Note", "       ", allNotes,    0, cbStepNote);
    menu->append(2, "Octave", "     ", 0,    4,     0, cbStepOctave);
    menu->append(2, "CV2", "        ", 0,    100,   0, cbStepCV2);
    //menu->append(2, "uTime", "      ", -100, 100,   0, cbStepUTime);
    //menu->append(2, "Slide", "      ", offOrOn,     0, cbStepSlide);

    uint8_t numHeaders = 2;
    if(DEBUG_MODE){ numHeaders = 3; };
    gui = new FieldGui(hw, menu, &font, fontWidth, fontHeight, numHeaders); // TODO swap numHeaders with a quick n dirty ternary
    
    /* Init state objects & vars */
    seq = new Sequencer();
    seq->setPattern(&(patterns_ram[settings_ram[RAM_PATTERN_INDEX]]));
    seq->restart();

    // "Prime" menu items
    loadMenuValsFromPattern(seq->getPattern());
    cbBPM();
    // cbOutTune();
    // cbMidiTxCh();
 
    tempo = new Tempo(true, cbTempo);
    tempo->setBPM(static_cast<float>(settings_ram[RAM_BPM]));

    // Populate isPressed
    for(size_t i = 0; i < 16; i++){
        isPressed[i] = false;
    }

    padMode = step;
    pageView = 1;
    wasPlaying = false;
    curNote = 0;
    isRecording = false;
    cuedPattern = -1;

    editSteps = {};

    /* Boot screen */
    if (!DEBUG_MODE){
        // TODO add knob leds
        gui->drawStartupScreen("Gumdrops", VERSION, 1);
        for (uint8_t i = 0; i < 8; i++){
            hw->led_driver.SetLed(stepLeds[i], 1.f);
            hw->led_driver.SetLed(stepLeds[i + 8], 1.f);
            hw->led_driver.SwapBuffersAndTransmit();
            hw->seed.DelayMs(80);
        }
        for (uint8_t i = 0; i < 8; i++){
            hw->led_driver.SetLed(stepLeds[i], 0.f);
            hw->led_driver.SetLed(stepLeds[i + 8], 0.f);
            hw->led_driver.SwapBuffersAndTransmit();
            hw->seed.DelayMs(80);
        }
    }

    /* Populate control state */
    hw->StartAdc();
    hw->ProcessAllControls();
    for(uint8_t i = 0; i < 8; i++){
        gui->knobVals[i] = hw->knob[i].Process(); 
    }

    /* Start MIDI */
    //hw->midi.StartReceive();

    /* Main event loop */
    while(1){
        updateControls();
        tempo->update();
        updateOled();
        updateOutputs();
        updateLeds();
        updateState();

        // TODO maybe replace this with an updateMidi() fn?
        //
        // while(hw.midi.HasEvents()){
        //     HandleMidi(hw.midi.PopEvent());
        // }

        //debugString = std::to_string(patterns_ram[settings_ram[RAM_PATTERN_INDEX]].getStepAt(2)->getNote());
        //debugString = std::to_string(patterns_ram[settings_ram[RAM_PATTERN_INDEX]].getIndex());
        //debugString = std::to_string(patterns_ram[settings_ram[RAM_PATTERN_INDEX]].getStepAt(0)->getIsActive());
        //debugString = "Han shot first";
        //debugString = std::to_string(seq->getPattern()->getMaxSteps());
        //debugString = std::to_string(cachedDac1);
        //debugString = std::to_string(cuedPattern) + " " +  std::to_string(seq->hasCuedPattern());
        debugString = tempo->toString();
    }
}
