/*
 * Copyright (C) 2023 Evan Pernu. Author: Evan Pernu
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
 * =======================
 *
 *          (^_^)
 *  _____             _   _
 * |     |_ _ ___ _ _| |_|_|
 * | | | | | |_ -| | | . | |
 * |_|_|_|___|___|___|___|_|
 *
 *           結び
 *
 * =======================
 *
 * Monosynth for Electrosmith Daisy Patch
 *
 */

#include "daisysp.h"
#include "daisy_patch.h"
#include "resources.h"

#include <string>
#include <array>
#include <cmath>

// Arbitrary value used to detect a fresh boot. Needs to be different for every module.
#define NOT_FRESH_BOOT_VAL 86

/* Indices of certain settings in settings_ram */

// Mode of operation
#define RAM_OP_MODE 0

// Used to determine if this is a fresh install, in which case default
// settings will be written to QSPI.
#define RAM_IS_INIT 1

// Menu settings for save/load
#define RAM_ENV_A       2
#define RAM_ENV_H       3
#define RAM_ENV_R       4
//#define RAM_ENV_      5 // Placeholder

#define RAM_ENV_A_SHAPE 6
#define RAM_ENV_R_SHAPE 7
#define RAM_ENV_CASCADE 8
#define RAM_ENV_VCA     9

#define RAM_LPF_FREQ    10
#define RAM_LPF_RES     11
#define RAM_LPF_ENV     12
#define RAM_DST_DRIVE   13

#define RAM_MFX_VERB_MIX  14
#define RAM_MFX_DJF       15
#define RAM_MFX_DELAY_MIX 16
#define RAM_MFX_DELAY_LEN 17

#define RAM_S_GATE      18
#define RAM_S_LPF_TYPE  19
#define RAM_S_ENV_1_LVL 20
#define RAM_S_ENV_2_LVL 21

#define RAM_S_LVL       22 // 22-25

#define RAM_S_PAN       26 // 26-29

#define RAM_S_VRB_HPF    30
#define RAM_S_VRB_LPF    31
// #define RAM_S_VRB_    32
#define RAM_S_VRB_PREDLY 33

#define RAM_S_DLY_HPF    34
#define RAM_S_DLY_LPF    35
#define RAM_S_DLY_ANALOG 36 // Not in use but I'll leave its skeleton here in case I think of something else to add later
#define RAM_S_DLY_PONG   37

#define RAM_S_DST_TYPE    38
#define RAM_S_DST_TONE    39
#define RAM_S_DST_PRE_LPF 40
//#define RAM_S_DST_      41

#define RAM_S_SYS_OUTS 42
// #define RAM_S_SYS_ 43 // Placeholder
// #define RAM_S_SYS_ 44 // Placeholder
// #define RAM_S_SYS_RESET 45 // Probably don't need this, implementation-wise

// number of settings to be stored in QSPI, ergo size of settings_ram
#define SETTINGS_BUFF_SIZE 43

// Page numbers for each setting
#define PAGE_EG2 0
#define PAGE_EG1 1
#define PAGE_LPF 2
#define PAGE_MFX 3
#define PAGE_S_LVL 4
#define PAGE_S_PAN 5
#define PAGE_S_MSC 6
#define PAGE_S_VRB 7
#define PAGE_S_DLY 8
#define PAGE_S_DST 9
#define PAGE_S_SYS 10

#define MENU_NUM_PAGES 11

#define NUM_CHANNELS 4

// TODO play with these numbers to get the most musical results
#define MAX_ATTACK 45000
#define MAX_HOLD 50000
#define MAX_RELEASE 60000
#define MAX_ENV_LENGTH 155000 // Sum of all env times

#define STRAIGHT_CONTOUR 500 // TODO for some reason my contour code is allergic to the number 500, might have to add special handling for it

#define MIN_LVL -1000
#define MAX_LVL 1000

using namespace daisy;
using namespace daisysp;
using namespace musubi;
using namespace ev_gui;
using namespace ev_theory;
using namespace ev_dsp;

/*
 * Update this with each code change
 */
const std::string VERSION = "1.0.0";

/*
 * Change this to enable debug behavior during development (shorter popups, etc.)
 */
const bool DEBUG_MODE = true;

/*
 * Change this to enable debug string during development
 */
const bool DEBUG_MSG = false;
std::string debugStr = "DEBUG_STR";

// Some settings are stored in QSPI memory and persisted on startup
int16_t DSY_QSPI_BSS settings_qspi[SETTINGS_BUFF_SIZE];
int16_t settings_ram[SETTINGS_BUFF_SIZE];

// State objects
DaisyPatch*  patch;
PatchGui*    gui;
Menu*        menu;
StereoVcf*  vcf;
DjFilter* djf;
Pan* pans[NUM_CHANNELS];
Distortion* dist;

// Output levels, multiplies the input signal at the start of the signal chain.
// 0.0 <= levels[x] <= 1.0
float levels[NUM_CHANNELS]{1, 1, 1, 1};

// Reverb/delay buffers are stored in external SDRAM where there's enough space for them
static ReverbSc DSY_SDRAM_BSS  reverbScMaster;
StereoReverb* reverb;

DelayLine<float, StereoDelay::MAX_DELAY> DSY_SDRAM_BSS delayLineMasterL;
DelayLine<float, StereoDelay::MAX_DELAY> DSY_SDRAM_BSS delayLineMasterR;
StereoDelay* delay;

// Font data
FontDef font       = Font_7x10;
uint8_t fontWidth  = 7;
uint8_t fontHeight = 10;

// 0.5 -> 1.0 is positive
// 0.5 -> 0.0 is negative
// 0.5 is no env
float filterEnv;

float env1Scale;
float env2Scale;

bool gateOut;

std::vector<std::string> msgPress      = std::vector<std::string>{"State loaded", "_END_"};
std::vector<std::string> msgShortHold  = std::vector<std::string>{"State saved", "_END_"};
std::vector<std::string> msgLongHoldA  = std::vector<std::string>{"Settings menu", "_END_"};
std::vector<std::string> msgLongHoldB  = std::vector<std::string>{"Exit Settings", "_END_"};
std::vector<std::string> msgFreshBoot  = std::vector<std::string>{"Performing", "initialization", "pls reboot", "after :)", "_END_"};
std::vector<std::string> msgReset      = std::vector<std::string>{"Reset? Save", "state, and", "reboot to", "confirm.", "_END_"};

// Copy the values in settings_ram (volatile) to settings_qspi (non-volatile)
void saveSettingsToQSPI(){
    size_t size = sizeof(settings_qspi);
    size_t address = (size_t)settings_qspi;
    patch->seed.qspi.Erase(address, address + size);
    patch->seed.qspi.Write(address, size, (uint8_t*)settings_ram);
}

void loadQSPISettingsToRAM(){
    memcpy(settings_ram, settings_qspi, sizeof(settings_ram));
}

// Saves default settings to QSPI storage
// Called when there's a fresh install of Musubi
void saveDefaultSettingsToQSPI(){
    settings_ram[RAM_OP_MODE] = 0; // TODO get rid of it
    settings_ram[RAM_IS_INIT] = NOT_FRESH_BOOT_VAL;

    settings_ram[RAM_ENV_A]       = 100;
    settings_ram[RAM_ENV_H]       = 200;
    settings_ram[RAM_ENV_R]       = 200;

    settings_ram[RAM_ENV_A_SHAPE] = 500;
    settings_ram[RAM_ENV_R_SHAPE] = 500;
    settings_ram[RAM_ENV_CASCADE] = 0;
    settings_ram[RAM_ENV_VCA]     = 1;

    settings_ram[RAM_LPF_ENV]     = 500;
    settings_ram[RAM_DST_DRIVE]   = 0;
    settings_ram[RAM_LPF_RES]     = 75;
    settings_ram[RAM_LPF_FREQ]    = 1000;

    settings_ram[RAM_MFX_VERB_MIX]  = 0;
    settings_ram[RAM_MFX_DJF]       = 0;
    settings_ram[RAM_MFX_DELAY_MIX] = 0;
    settings_ram[RAM_MFX_DELAY_LEN] = 200;

    settings_ram[RAM_S_VRB_HPF]    = 300;
    settings_ram[RAM_S_VRB_LPF]    = 50;
    settings_ram[RAM_S_VRB_PREDLY] = 10;

    settings_ram[RAM_S_DLY_HPF]    = 300;
    settings_ram[RAM_S_DLY_LPF]    = 50;
    settings_ram[RAM_S_DLY_ANALOG] = 0;
    settings_ram[RAM_S_DLY_PONG]   = 0;

    settings_ram[RAM_S_DST_TYPE]    = 0;
    settings_ram[RAM_S_DST_TONE]    = 0;
    settings_ram[RAM_S_DST_PRE_LPF] = 0;

    settings_ram[RAM_S_GATE]      = 0;
    settings_ram[RAM_S_LPF_TYPE]  = 0;
    settings_ram[RAM_S_ENV_1_LVL] = 1000;
    settings_ram[RAM_S_ENV_2_LVL] = 1000;

    settings_ram[RAM_S_SYS_OUTS]  = 0;

    for(uint8_t i = 0; i < NUM_CHANNELS; i++){
        settings_ram[RAM_S_PAN + i] = DEFAULT_PAN - MIN_PAN;
        settings_ram[RAM_S_LVL + i] = MIN_LVL - MIN_LVL;
    }
    
    settings_ram[RAM_S_LVL] = 0 - MIN_LVL; // Because inputs are normalized on Patch (lame!), channels not being used must be muted. Mute 2-4 by default.

    saveSettingsToQSPI();
}

void loadQSPISettingsToState(){
    loadQSPISettingsToRAM();

    // Note that setIndex() automatically calls the MenuItem's callback
    menu->getItem(0, PAGE_EG1)->setIndex(settings_ram[RAM_ENV_A]);
    menu->getItem(1, PAGE_EG1)->setIndex(settings_ram[RAM_ENV_H]);
    menu->getItem(2, PAGE_EG1)->setIndex(settings_ram[RAM_ENV_R]);

    menu->getItem(0, PAGE_EG2)->setIndex(settings_ram[RAM_ENV_A_SHAPE]);
    menu->getItem(1, PAGE_EG2)->setIndex(settings_ram[RAM_ENV_R_SHAPE]);
    menu->getItem(2, PAGE_EG2)->setIndex(settings_ram[RAM_ENV_CASCADE]);
    menu->getItem(3, PAGE_EG2)->setIndex(settings_ram[RAM_ENV_VCA]);

    menu->getItem(0, PAGE_LPF)->setIndex(settings_ram[RAM_DST_DRIVE]);
    menu->getItem(1, PAGE_LPF)->setIndex(settings_ram[RAM_LPF_ENV]);
    menu->getItem(2, PAGE_LPF)->setIndex(settings_ram[RAM_LPF_RES]);
    menu->getItem(3, PAGE_LPF)->setIndex(settings_ram[RAM_LPF_FREQ]);

    menu->getItem(0, PAGE_MFX)->setIndex(settings_ram[RAM_MFX_VERB_MIX]);
    menu->getItem(1, PAGE_MFX)->setIndex(settings_ram[RAM_MFX_DJF] + DjFilter::DJF_MIN_LEVEL);
    menu->getItem(2, PAGE_MFX)->setIndex(settings_ram[RAM_MFX_DELAY_MIX]);
    menu->getItem(3, PAGE_MFX)->setIndex(settings_ram[RAM_MFX_DELAY_LEN]);

    menu->getItem(0, PAGE_S_MSC)->setIndex(settings_ram[RAM_S_GATE]);
    menu->getItem(1, PAGE_S_MSC)->setIndex(settings_ram[RAM_S_LPF_TYPE]);
    menu->getItem(2, PAGE_S_MSC)->setIndex(settings_ram[RAM_S_ENV_1_LVL]);
    menu->getItem(3, PAGE_S_MSC)->setIndex(settings_ram[RAM_S_ENV_2_LVL]);
    
    menu->getItem(0, PAGE_S_VRB)->setIndex(settings_ram[RAM_S_VRB_HPF]);
    menu->getItem(1, PAGE_S_VRB)->setIndex(settings_ram[RAM_S_VRB_LPF]);
    menu->getItem(3, PAGE_S_VRB)->setIndex(settings_ram[RAM_S_VRB_PREDLY]);

    menu->getItem(0, PAGE_S_DLY)->setIndex(settings_ram[RAM_S_DLY_HPF]);
    menu->getItem(1, PAGE_S_DLY)->setIndex(settings_ram[RAM_S_DLY_LPF]);
    menu->getItem(2, PAGE_S_DLY)->setIndex(settings_ram[RAM_S_DLY_ANALOG]);
    menu->getItem(3, PAGE_S_DLY)->setIndex(settings_ram[RAM_S_DLY_PONG]);

    menu->getItem(0, PAGE_S_DST)->setIndex(settings_ram[RAM_S_DST_TYPE]);
    menu->getItem(1, PAGE_S_DST)->setIndex(settings_ram[RAM_S_DST_TONE]);
    menu->getItem(2, PAGE_S_DST)->setIndex(settings_ram[RAM_S_DST_PRE_LPF]);

    menu->getItem(0, PAGE_S_SYS)->setIndex(settings_ram[RAM_S_SYS_OUTS]);

    for(uint8_t ch = 0; ch < NUM_CHANNELS; ch++){
        menu->getItem(ch, PAGE_S_LVL)->setIndex(settings_ram[RAM_S_LVL + ch] + MIN_LVL);
        menu->getItem(ch, PAGE_S_PAN)->setIndex(settings_ram[RAM_S_PAN + ch] + MIN_PAN);
    }
}

/* Envelope */

void cbEnvAttack(){
    settings_ram[RAM_ENV_A] = menu->getItem(0, PAGE_EG1)->getIndex();
    vcf->env->setAttack(settings_ram[RAM_ENV_A]);
}

void cbEnvHold(){
    settings_ram[RAM_ENV_H] = menu->getItem(1, PAGE_EG1)->getIndex();
    vcf->env->setHold(settings_ram[RAM_ENV_H]);
}

void cbEnvRelease(){
    settings_ram[RAM_ENV_R] = menu->getItem(2, PAGE_EG1)->getIndex();
    vcf->env->setRelease(settings_ram[RAM_ENV_R]);
}

void cbAContour(){
    settings_ram[RAM_ENV_A_SHAPE] = menu->getItem(0, PAGE_EG2)->getIndex();
    vcf->env->setAttackContour(settings_ram[RAM_ENV_A_SHAPE]);
}


void cbRContour(){
    settings_ram[RAM_ENV_R_SHAPE] = menu->getItem(1, PAGE_EG2)->getIndex();
    vcf->env->setReleaseContour(settings_ram[RAM_ENV_R_SHAPE]);
}

void cbCascade(){
    settings_ram[RAM_ENV_CASCADE] = menu->getItem(2, PAGE_EG2)->getIndex();
    vcf->env->setCascade(settings_ram[RAM_ENV_CASCADE]);
}

void cbEnvVca(){
    settings_ram[RAM_ENV_VCA] = menu->getItem(3, PAGE_EG2)->getIndex();
    vcf->setVca(settings_ram[RAM_ENV_VCA]);
}

void cbGateType(){
    settings_ram[RAM_S_GATE] = menu->getItem(0, PAGE_S_MSC)->getIndex();
}

void cbEnv1Lvl(){
    settings_ram[RAM_S_ENV_1_LVL] = menu->getItem(2, PAGE_S_MSC)->getIndex();
    env1Scale = settings_ram[RAM_S_ENV_1_LVL] * .001f;
}

void cbEnv2Lvl(){
    settings_ram[RAM_S_ENV_2_LVL] = menu->getItem(3, PAGE_S_MSC)->getIndex();
    env2Scale = settings_ram[RAM_S_ENV_2_LVL] * .001f;
}

/* Filter */

void cbFiltFreq(){
    settings_ram[RAM_LPF_FREQ] = menu->getItem(3, PAGE_LPF)->getIndex();
    if(menu->getItem(3, PAGE_EG1)->getIndex() != settings_ram[RAM_LPF_FREQ]){
        menu->getItem(3, PAGE_EG1)->setIndex(settings_ram[RAM_LPF_FREQ]);
    }
    vcf->lpf->setFreq(settings_ram[RAM_LPF_FREQ]);
}

void cbEnvFiltFreq(){
    // Since there are 2 occurences osf this parameter, we will treat the other one
    // as the "source of truth".
    menu->getItem(3, PAGE_LPF)->setIndex(menu->getItem(3, PAGE_EG1)->getIndex());
}

void cbFiltRes(){
    settings_ram[RAM_LPF_RES] = menu->getItem(2, PAGE_LPF)->getIndex();
    vcf->lpf->setResonance(settings_ram[RAM_LPF_RES]);
}

void cbFiltEnv(){
    settings_ram[RAM_LPF_ENV] = menu->getItem(1, PAGE_LPF)->getIndex();
    vcf->setLpfEnv(settings_ram[RAM_LPF_ENV]);
}

void cbFiltType(){
    settings_ram[RAM_S_LPF_TYPE] = menu->getItem(1, PAGE_S_MSC)->getIndex();
    vcf->lpf->setType(static_cast<LpfType>(settings_ram[RAM_S_LPF_TYPE]));
}

/* DJF */

void cbMfxDjf(){
   djf->setLevel(menu->getItem(0, PAGE_MFX)->getIndex());
   settings_ram[RAM_MFX_DJF] = menu->getItem(0, PAGE_MFX)->getIndex() - DjFilter::DJF_MIN_LEVEL;
}

/* Distortion */

void cbFiltDrive(){
    settings_ram[RAM_DST_DRIVE] = menu->getItem(0, PAGE_LPF)->getIndex();
    dist->setDrive(settings_ram[RAM_DST_DRIVE]); // TODO rename var
}

void cbDistType(){
    settings_ram[RAM_S_DST_TYPE] = menu->getItem(0, PAGE_S_DST)->getIndex();
    dist->setType(static_cast<DistType>(settings_ram[RAM_S_DST_TYPE]));
}

void cbDistTone(){
    settings_ram[RAM_S_DST_TONE] = menu->getItem(1, PAGE_S_DST)->getIndex();
    dist->setTone(settings_ram[RAM_S_DST_TONE]);
}

void cbDistPreLpf(){
    settings_ram[RAM_S_DST_PRE_LPF] = menu->getItem(2, PAGE_S_DST)->getIndex();
}

/* Reverb */

void cbVerbLvl(){
    settings_ram[RAM_MFX_VERB_MIX] = menu->getItem(1, PAGE_MFX)->getIndex();
    reverb->setLevel(settings_ram[RAM_MFX_VERB_MIX]);
}

void cbVerbHpf(){
    settings_ram[RAM_S_VRB_HPF] = menu->getItem(0, PAGE_S_VRB)->getIndex();
    reverb->setHpf(settings_ram[RAM_S_VRB_HPF]);
}

void cbVerbLpf(){
    settings_ram[RAM_S_VRB_LPF] = menu->getItem(1, PAGE_S_VRB)->getIndex();
    reverb->setLpf(settings_ram[RAM_S_VRB_LPF]);
}

void cbVerbPredelay(){
    settings_ram[RAM_S_VRB_PREDLY] = menu->getItem(3, PAGE_S_VRB)->getIndex();
    reverb->setPredelay(settings_ram[RAM_S_VRB_PREDLY]);
}

/* Delay */

void cbDelayLvl(){
    settings_ram[RAM_MFX_DELAY_MIX] = menu->getItem(2, PAGE_MFX)->getIndex();
    delay->setLevel(settings_ram[RAM_MFX_DELAY_MIX]);
}

void cbMfxDelayTime(){
    settings_ram[RAM_MFX_DELAY_LEN] = menu->getItem(3, PAGE_MFX)->getIndex();
    delay->setTime(settings_ram[RAM_MFX_DELAY_LEN]);
}

void cbDelayHpf(){
    settings_ram[RAM_S_DLY_HPF] = menu->getItem(0, PAGE_S_DLY)->getIndex();
    delay->setHpf(settings_ram[RAM_S_DLY_HPF]);
}

void cbDelayLpf(){
    settings_ram[RAM_S_DLY_LPF] = menu->getItem(1, PAGE_S_DLY)->getIndex();
    delay->setLpf(settings_ram[RAM_S_DLY_LPF]);
}

void cbDelayAnalog(){

}

void cbDelayPingPong(){
    settings_ram[RAM_S_DLY_PONG] = menu->getItem(3, PAGE_S_DLY)->getIndex();
    delay->setPingPong(settings_ram[RAM_S_DLY_PONG]);
}

/* Mixing */

void cbLvl(uint8_t ch){
    int16_t l = menu->getItem(ch, PAGE_S_LVL)->getIndex();
    if(l < -950) {
        // Mute any values sufficiently close to 0, just to keep the signal clean
        l = -1000;
    }
    levels[ch] = 1.f + (l * .001);
    settings_ram[RAM_S_LVL + ch] = menu->getItem(ch, PAGE_S_LVL)->getIndex() - MIN_LVL;
};
void cbLvl1(){ cbLvl(0); };
void cbLvl2(){ cbLvl(1); };
void cbLvl3(){ cbLvl(2); };
void cbLvl4(){ cbLvl(3); };

void cbPan(uint8_t ch){
    pans[ch]->setPan(menu->getItem(ch, PAGE_S_PAN)->getIndex());
    settings_ram[RAM_S_PAN + ch] = menu->getItem(ch, PAGE_S_PAN)->getIndex() - MIN_PAN;
};
void cbPan1(){ cbPan(0); };
void cbPan2(){ cbPan(1); };
void cbPan3(){ cbPan(2); };
void cbPan4(){ cbPan(3); };

/* System */

void cbSysReset(){
    if(menu->getItem(3, PAGE_S_SYS)->getIndex() == 1){
        settings_ram[RAM_IS_INIT] = NOT_FRESH_BOOT_VAL-1;
        gui->setPopup(msgReset, 500, false);
    } else {
        settings_ram[RAM_IS_INIT] = NOT_FRESH_BOOT_VAL;
    }
}

void cbOutput(){
   settings_ram[RAM_S_SYS_OUTS] = menu->getItem(0, PAGE_S_SYS)->getIndex();
}

void cbEmpty(){}

void updateControls() {
    patch->ProcessAnalogControls();
    patch->ProcessDigitalControls();

    // GATE IN 1 -> GATE OUT (passthrough) 
    gateOut = patch->gate_input[0].State();

    // GATE IN 1 -> envelope trig
    if(patch->gate_input[0].Trig()){
        vcf->env->trigger();
        reverb->trigger();
    }

    vcf->env->gateOn = settings_ram[RAM_S_GATE] && patch->gate_input[0].State();
}

void updateOutputs() {
    // GATE IN 1 -> GATE OUT (passthrough) 
    dsy_gpio_write(&patch->gate_output, gateOut);

    // Env level -> CV OUT 1 & CV OUT 2
    uint16_t dac = (uint16_t) (vcf->env->getLevel() * MAX_DAC_VALUE);
    patch->seed.dac.WriteValue(DacHandle::Channel::ONE, dac * env1Scale);
    patch->seed.dac.WriteValue(DacHandle::Channel::TWO, dac * env2Scale);
}

void drawEnvPage(){
    // Draw line under headers
    uint16_t y = 2*fontHeight + 1;
    //patch->display.DrawLine(0, y, SCREEN_WIDTH, y, true);

    // Draw attack segment, which consists of 4 line segments that we will extract from env
    y+=4;
    uint16_t maxY = SCREEN_HEIGHT-y;
    uint16_t eX1, eY1;
    uint16_t eX2 = 0;
    uint16_t eY2 = SCREEN_HEIGHT;

    for (uint8_t i = 0; i < 4; i++){
        eX1 = eX2;
        eY1 = eY2;
        eX2 = (uint16_t) SCREEN_WIDTH * (vcf->env->getAttackX(i) / (float) MAX_ENV_LENGTH);
        eY2 = (uint16_t) SCREEN_HEIGHT - vcf->env->getAttackY(i) * maxY;
        if (eY2 < y){
            // Prevent invalid eY2 value when attack = 0
            eY2 = y;
        }
        patch->display.DrawLine(eX1, eY1, eX2, eY2, true);
    }

    // Draw hold segment
    uint16_t x1 = SCREEN_WIDTH * ((float) vcf->env->attack / (float) MAX_ENV_LENGTH);
    uint16_t x2 = x1 + SCREEN_WIDTH * ((float) vcf->env->hold / (float) MAX_ENV_LENGTH);
    patch->display.DrawLine(x1, y, x2, y, true);

    // Draw release segment, which consists of 4 line segments that we will extract from env
    if(vcf->env->release == 0){
        // Prevent invalid eY2 value when release = 0
        patch->display.DrawLine(x2, y, x2, SCREEN_HEIGHT, true);
    } else {
        eX2 = x2;
        eY2 = SCREEN_HEIGHT-maxY;

        for (uint8_t i = 0; i < 4; i++){
            eX1 = eX2;
            eY1 = eY2;
            eX2 = x2 + (uint16_t) SCREEN_WIDTH * (vcf->env->getReleaseX(i) / (float) MAX_ENV_LENGTH);
            eY2 = (uint16_t) SCREEN_HEIGHT - vcf->env->getReleaseY(i) * maxY;

            patch->display.DrawLine(eX1, eY1, eX2, eY2, true);
        }
    }

    // Draw env location line
    if (vcf->env->fullIndex > 0){
        uint8_t x = (uint8_t) ((vcf->env->fullIndex / (float) MAX_ENV_LENGTH) * SCREEN_WIDTH);
        patch->display.DrawLine(x, y-4, x, SCREEN_HEIGHT, true);
    }
}

void drawFilterPage(){
    uint8_t x, y, y2, maxX;
    
    // Draw knob for drive
    gui->drawKnob(10, 19, 7, settings_ram[RAM_DST_DRIVE]/1000.f);

    // Draw an env indicator at the bottom of the screen, at the point which the env would take the filter
    maxX = SCREEN_WIDTH - 16;
    if(vcf->getEnvLvl() != 500){
        x = (uint8_t) ((vcf->getEnvLvl()/1000.f) * maxX);
        patch->display.DrawLine(x, SCREEN_HEIGHT-6, x, SCREEN_HEIGHT, true);
        patch->display.DrawLine(x+1, SCREEN_HEIGHT-5, x+1, SCREEN_HEIGHT, true);
        patch->display.DrawLine(x-1, SCREEN_HEIGHT-5, x-1, SCREEN_HEIGHT, true);
    }

    // Draw filter
    x = (uint8_t) ((vcf->lpf->getTrueLevel()/1000.f) * maxX);
    y = SCREEN_HEIGHT - 20;
    patch->display.DrawLine(0, y, x, y, true);
    y2 = y - (settings_ram[RAM_LPF_RES]/1000.f) * 20;
    patch->display.DrawLine(x, y, x+4, y2, true);
    x += 4;
    patch->display.DrawLine(x, y2, x+4, SCREEN_HEIGHT, true);
}

void drawMfxPage(){
    // Draw DJ filter
    gui->drawKnob(14, 30, 12, settings_ram[RAM_MFX_DJF]/2000.f);

    // Draw verb
    gui->drawKnob(47, 50, 12, settings_ram[RAM_MFX_VERB_MIX]/1000.f);

    // Draw delay
    gui->drawKnob(81, 30, 12, settings_ram[RAM_MFX_DELAY_MIX]/1000.f);

    // Draw delay time
    gui->drawKnob(114, 50, 12, settings_ram[RAM_MFX_DELAY_LEN]/1000.f);
}

const std::string EG1_OPTS = "Atk  Hld  Rel  LPF";
const std::string EG2_OPTS = "Atk  Rel  Chn  VCA";
const std::string LPF_OPTS = "Drv  Env  Res  Frq";
const std::string MFX_OPTS = "DJF  Vrb  Dly  Tim";

void updateOled(){
    gui->updateControls();
    gui->setHeader(menu->getPageName(menu->getPage()), 0); // TODO rm when all pages have animations

    // Clear display
    patch->display.Fill(false);

    // Draw page-specific parameters & animations
    switch(menu->getPage()){
    case PAGE_EG1:{
        gui->drawString(EG1_OPTS, 0, 0);

        // Draw a horizontal bar for LPF
        uint8_t y = fontHeight+1;
        uint8_t x = 101;
        uint8_t x2 = x + ((uint8_t) ((menu->getItem(3, PAGE_EG1)->getIndex()/1000.f) * 27.f)); 
        patch->display.DrawRect(x, y, x2, y + 3, true, true);

        drawEnvPage();
        break;}
    case PAGE_EG2:
        gui->drawString(EG2_OPTS, 0, 0);
        gui->drawString(menu->getItem(2, PAGE_EG2)->getValue(), SCREEN_WIDTH_HALF, fontHeight+1);
        gui->drawString(menu->getItem(3, PAGE_EG2)->getValue(), SCREEN_WIDTH_3_QUARTERS, fontHeight+1);
        drawEnvPage();
        break;
    case PAGE_LPF:
        gui->drawString(LPF_OPTS, 0, 0);
        drawFilterPage();
        break;
    case PAGE_MFX:
        gui->drawString(MFX_OPTS, 0, 0);
        drawMfxPage();
        break;
    default:
        gui->render();
        break;
    }

    // Draw popups & loading bar
    gui->drawOverlays();

    // Draw debug info over whatever's beneath it
    if(DEBUG_MSG){
        debugStr = floatToString(vcf->env->taperLevel, 4);
        uint8_t dx = 0;
        uint8_t dy = 0;
        patch->display.DrawRect(dx, dy, dx+SCREEN_WIDTH, dy+12, false, true);
        gui->drawString(debugStr, dx+1, dy+1);
    }

    // Write display buffer to OLED
    patch->display.Update();
}

static void AudioCallback(AudioHandle::InputBuffer hwIn,
        AudioHandle::OutputBuffer hwOut, size_t size){

    // Iterate through the audio buffer
   for(size_t i = 0; i < size; i++){
        vcf->env->tick();

        float fxL, fxR;
        float outL = 0.f;
        float outR = 0.f;

        // Read in audio from buffer
        for(uint8_t ch = 0; ch < NUM_CHANNELS; ch++){ 
            float o;
            // In order to leave headroom for send effects, we lower the volume of the input a bit
            o = hwIn[ch][i] * levels[ch];

            pans[ch]->processAudio(fxL, fxR, o);
            outL += fxL; // TODO maybe get rid of these, gotta confirm that all processAudio functions don't modify the input var
            outR += fxR;
        }

        // Distortion (pre-filter)
        if(settings_ram[RAM_S_DST_PRE_LPF]){
            fxL = dist->processAudio(outL);
            fxR = dist->processAudio(outR);
            outL = fxL;
            outR = fxR;
        }

        // VCF
        vcf->processAudio(fxL, fxR, outL, outR);
        outL = fxL;
        outR = fxR;

        // Distortion (post-filter)
        if(!settings_ram[RAM_S_DST_PRE_LPF]){
            fxL = dist->processAudio(outL);
            fxR = dist->processAudio(outR);
            outL = fxL;
            outR = fxR;
        }

        // DJ filter
        djf->processAudio(fxL, fxR, outL, outR); 
        outL = fxL;
        outR = fxR;

        // Delay
        delay->processAudio(fxL, fxR, outL, outR); 
        outL += fxL;
        outR += fxR;

        // Reverb
        reverb->processAudio(fxL, fxR, outL, outR); 
        outL = fxL;
        outR = fxR;
        
        // AUDIO OUT 1/2 = output L/R
        hwOut[0][i] = outL;
        hwOut[1][i] = outR;
        // AUDIO OUT 3/4 = output L/R
        hwOut[2][i] = outL;
        hwOut[3][i] = outR;
    }
}

void cbPress(){
    loadQSPISettingsToState();
}

void cbShortHold(){
    saveSettingsToQSPI();
}

void cbLongHold(){
    gui->changeZones();
    if(gui->getIsZoneB()){
        gui->setMsgLongHold(msgLongHoldB);
    } else {
        gui->setMsgLongHold(msgLongHoldA);
    }
}

int main(void) {
    // Initialize state objects
    patch = new DaisyPatch();
    patch->Init();
    float sampleRate = patch->AudioSampleRate();
    gateOut = false;

    menu = new Menu(MENU_NUM_PAGES);

    gui = new PatchGui(patch, menu, &font, fontWidth, fontHeight, 4);
    gui->setCbPress(cbPress, msgPress, 100, true);
    gui->setCbShortHold(cbShortHold, msgShortHold, 100, true);
    gui->setCbLongHold(cbLongHold, msgLongHoldA, 100, true); 
    gui->drawHoldBar = true;
    gui->setZoneB(PAGE_S_LVL);

    vcf = new StereoVcf(sampleRate);
    vcf->env->setAhr(MAX_ATTACK, MAX_HOLD, MAX_RELEASE);

    reverb = new StereoReverb(sampleRate, &reverbScMaster);

    delayLineMasterL.Init();
    delayLineMasterR.Init();
    delay = new StereoDelay(sampleRate, &delayLineMasterL, &delayLineMasterR);

    djf = new DjFilter(sampleRate);

    dist = new Distortion(sampleRate);

    for(uint8_t i = 0; i < NUM_CHANNELS; i++){
        pans[i] = new Pan();
    }

    
    loadQSPISettingsToRAM();

    // Draw startup message(s)
    uint16_t t;
    if(DEBUG_MODE){
        // Long popup gets annoying during development, shorten in debug mode
        t = 300;
    } else {
        t = 1500;
    }

    // If the RAM_IS_INIT setting isn't NOT_FRESH_BOOT_VAL, we know that we don't have settings
    // here and we should use default settings.
    if (settings_ram[RAM_IS_INIT] != NOT_FRESH_BOOT_VAL){
        gui->setPopup(msgFreshBoot, t, false);
        saveDefaultSettingsToQSPI();
    }

    gui->drawStartupScreen("Musubi", VERSION, 750);

    /* Zone A (main) */

    //menu->setPageName(PAGE_EG2, "Envelope 2");
    menu->append(PAGE_EG2, "", "", 1,     999,  0,    cbAContour);
    menu->append(PAGE_EG2, "", "", 1,     999,  0,    cbRContour);
    menu->append(PAGE_EG2, "", "", cascadeOpts, 0,    cbCascade);
    menu->append(PAGE_EG2, "", "", onOrOff,     1,    cbEnvVca);

    //menu->setPageName(PAGE_EG1, "Envelope 1");
    menu->append(PAGE_EG1, "", "", 0,     1000, 0,    cbEnvAttack);
    menu->append(PAGE_EG1, "", "", 0,     1000, 0,    cbEnvHold);
    menu->append(PAGE_EG1, "", "", 0,     1000, 0,    cbEnvRelease);
    menu->append(PAGE_EG1, "", "", 0,     1000, 1000, cbEnvFiltFreq);

    //menu->setPageName(PAGE_LPF, "Filter");
    menu->append(PAGE_LPF, "", "", 0,     1000, 0,    cbFiltDrive);
    menu->append(PAGE_LPF, "", "", 0,     1000, 500,  cbFiltEnv);
    menu->append(PAGE_LPF, "", "", 0,     1000, 0,    cbFiltRes);
    menu->append(PAGE_LPF, "", "", 0,     1000, 1000, cbFiltFreq); // Same knob as freq on PAGE_EG1, makes better UI
    menu->getItem(1, PAGE_LPF)->setNotch(500, 20);

    // menu->setPageName(PAGE_MFX, "Filter");
    menu->append(PAGE_MFX, "", "", -1000, 1000, 0,    cbMfxDjf);
    menu->append(PAGE_MFX, "", "", 0,     1000, 0,    cbVerbLvl);
    menu->append(PAGE_MFX, "", "", 0,     1000, 0,    cbDelayLvl);
    menu->append(PAGE_MFX, "", "", 0,     1000, 0,    cbMfxDelayTime);
    menu->getItem(0, PAGE_MFX)->setNotch(0, 50);

    /* Zone B (settings) */

    menu->setPageName(PAGE_S_LVL, "Levels");
    menu->append(PAGE_S_LVL, "1", "        ", MIN_LVL, MAX_LVL, 0, cbLvl1);
    menu->append(PAGE_S_LVL, "2", "        ", MIN_LVL, MAX_LVL, 0, cbLvl2);
    menu->append(PAGE_S_LVL, "3", "        ", MIN_LVL, MAX_LVL, 0, cbLvl3);
    menu->append(PAGE_S_LVL, "4", "        ", MIN_LVL, MAX_LVL, 0, cbLvl4);

    menu->setPageName(PAGE_S_PAN, "Panning");
    menu->append(PAGE_S_PAN, "1", "        ", MIN_PAN, MAX_PAN, 0, cbPan1);
    menu->append(PAGE_S_PAN, "2", "        ", MIN_PAN, MAX_PAN, 0, cbPan2);
    menu->append(PAGE_S_PAN, "3", "        ", MIN_PAN, MAX_PAN, 0, cbPan3);
    menu->append(PAGE_S_PAN, "4", "        ", MIN_PAN, MAX_PAN, 0, cbPan4);

    menu->setPageName(PAGE_S_MSC, "Misc");
    menu->append(PAGE_S_MSC, "Gate In",  " ", gateInOpts, 0, cbGateType);
    menu->append(PAGE_S_MSC, "LPF Type", " ", lpfOpts, 0, cbFiltType);
    menu->append(PAGE_S_MSC, "Env1 Lvl", " ", 0, 1000, 0, cbEnv1Lvl);
    menu->append(PAGE_S_MSC, "Env2 Lvl", " ", 0, 1000, 0, cbEnv2Lvl);

    menu->setPageName(PAGE_S_VRB, "Reverb");
    menu->append(PAGE_S_VRB, "HPF", "      ", 0, 1000, 0, cbVerbHpf);
    menu->append(PAGE_S_VRB, "LPF", "      ", 0, 1000, 0, cbVerbLpf);
    menu->append(PAGE_S_VRB, "", " ",       blankOpts, 0, cbEmpty);
    menu->append(PAGE_S_VRB, "Predelay", " ", 0, 1000, 0, cbVerbPredelay);

    menu->setPageName(PAGE_S_DLY, "Delay");
    menu->append(PAGE_S_DLY, "HPF", "      ", 0, 1000, 0, cbDelayHpf);
    menu->append(PAGE_S_DLY, "LPF", "      ", 0, 1000, 0, cbDelayLpf);
    menu->append(PAGE_S_DLY, "", "",          blankOpts, 0, cbDelayAnalog);
    menu->append(PAGE_S_DLY, "PingPong", " ", yesOrNo, 0, cbDelayPingPong);

    menu->setPageName(PAGE_S_DST, "Distortion");
    menu->append(PAGE_S_DST, "Type", "     ", distAlgos, 0, cbDistType);
    menu->append(PAGE_S_DST, "Tone", "     ", 0, 1000,   0, cbDistTone);
    menu->append(PAGE_S_DST, "Pre-LPF", "  ", yesOrNo,   0, cbDistPreLpf);
    menu->append(PAGE_S_DST, "", "",          blankOpts, 0, cbEmpty);

    menu->setPageName(PAGE_S_SYS, "System");
    menu->append(PAGE_S_SYS, "Output", "  ",  outputOpts, 0, cbOutput);
    menu->append(PAGE_S_SYS, "", " ",         blankOpts, 0, cbEmpty); 
    menu->append(PAGE_S_SYS, "", " ",         blankOpts, 0, cbEmpty);
    menu->append(PAGE_S_SYS, "Reset", "   ",  yesOrNo, 1, cbSysReset);

    loadQSPISettingsToState();

    patch->StartAdc();
    patch->StartAudio(AudioCallback);

    patch->DelayMs(100);
    patch->ProcessAnalogControls();
    gui->setInitialPage(PAGE_EG1);

    // Main event loop
    while(1){
        updateControls();
        updateOled();
        updateOutputs();
    }
}