/* 
 * Copyright (C) 2022 Evan Pernu. Author: Evan Pernu
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
 *        つ ◕_◕༽つ
 *    
 *  ╔═╗┌┬┐┌─┐┬┌─┌─┐┌─┐┌─┐
 *  ║ ║│││├─┤├┴┐├─┤└─┐├┤ 
 *  ╚═╝┴ ┴┴ ┴┴ ┴┴ ┴└─┘└─┘
 *
 *        おまかせ
 *
 * =======================
 *
 * DJ inspired performance mixer for Electrosmith Daisy Patch
 *
 */

#include "daisysp.h"
#include "daisy_patch.h"
#include "resources.h"

#include <string>
#include <array>

#define NUM_INPUT_CHANNELS  4

// Indices of each send
#define CH_MAIN       0
#define CH_EXTERNAL   1
#define CH_REVERB     2
#define CH_DELAY      3
#define CH_EXTERNAL_2 4

// Arbitrary value used to detect a fresh boot. Needs to be different for every module.
#define FRESH_BOOT_VAL 69

/* Indices of certain settings in settings_ram */

// number of settings to be stored in QSPI, ergo size of settings_ram
#define SETTINGS_BUFF_SIZE 67

// Mode of operation
#define RAM_OP_MODE 0

// Used to determine if this is a fresh install of Omakase, in which case default
// settings will be written to QSPI.
#define RAM_IS_INIT 1 

// Menu settings for save/load
//
// These are the indexes withing settings_ram of channel 1's setting. 4-channel settings 
// are stored in blocks of 4; to access channels 2-4, add 1-3 to the index.
#define RAM_LEVEL       2 // TODO use abbreviated names like below
#define RAM_GAIN        6
#define RAM_SEND        10
#define RAM_REVERB      14
#define RAM_DELAY       18
#define RAM_DISTORTION  22
#define RAM_PAN         26
#define RAM_FILTER      30
#define RAM_DIST_TYPE   34
#define RAM_DIST_TONE   35
#define RAM_DIST_DRIVE  36
#define RAM_DIST_LVL    37
#define RAM_DLY_FDBK    38
#define RAM_DLY_TIME    39
#define RAM_DLY_HPF     40
#define RAM_DLY_VERB    41
#define RAM_VERB_FDBK   42
#define RAM_VERB_DAMP   43
#define RAM_VERB_HPF    44
#define RAM_VERB_DRY    45
#define RAM_TX_DLY      46
#define RAM_TX_VERB     47
#define RAM_TX_NOISE    48
#define RAM_TX_FILT     49
#define RAM_COMP_ATK    50
#define RAM_COMP_REL    51
#define RAM_COMP_THRESH 52
#define RAM_COMP_RATIO  53
#define RAM_MSTR_TONE   54
#define RAM_MSTR_TYPE   55
#define RAM_MSTR_DRIVE  56
#define RAM_MSTR_LVL    57
#define RAM_CMP_BPS     58
#define RAM_NOISE_B     59
#define RAM_NOISE_C     60
#define RAM_NOISE_LVL   61
#define RAM_SEND_2      62
#define RAM_SEND_CFG    66

// Page numbers for each setting
#define PAGE_LVL      0 // Zone A
#define PAGE_FILT     1
#define PAGE_DIST     2
#define PAGE_DLY      3
#define PAGE_VERB     4
#define PAGE_SEND     5
#define PAGE_SEND_2   6
#define PAGE_PAN      7
#define PAGE_GAIN     8
#define PAGE_TX       9

#define PAGE_DIST_CFG 10 // Zone B
#define PAGE_DLY_CFG  11
#define PAGE_VERB_CFG 12
#define PAGE_COMP     13
#define PAGE_NOISE    14
#define PAGE_MSTR     15
#define PAGE_SYS      16

#define MENU_NUM_PAGES 17

using namespace daisy;
using namespace daisysp;
using namespace omakase;
using namespace ev_gui;
using namespace ev_theory;
using namespace ev_dsp;

/*
 * Update this with each change
 */
const std::string VERSION = "1.0.0";

/*
 * Change this to enable debug output
 */
const bool DEBUG_MODE = false;
std::string debugStr = "DEBUG_STR"; // Use for debugging

// Some settings are stored in QSPI memory and persisted on startup
int16_t DSY_QSPI_BSS settings_qspi[SETTINGS_BUFF_SIZE];
int16_t settings_ram[SETTINGS_BUFF_SIZE];

// State objects
DaisyPatch* patch;
PatchGui*   gui;
Menu*       menu;

Channel channels[NUM_INPUT_CHANNELS];

// Audio outputs for each send bus
float   outsL[NUM_OUTS];
float   outsR[NUM_OUTS];

// Reverb/delay buffers are stored in external SDRAM where there's enough space for them
static ReverbSc DSY_SDRAM_BSS reverbScSend; //TODO
StereoReverb* reverbSend;

static ReverbSc DSY_SDRAM_BSS  reverbScMaster;
StereoReverb* reverbMaster;

DelayLine<float, StereoDelay::MAX_DELAY> DSY_SDRAM_BSS delayLineSendL; // TODO make static?
DelayLine<float, StereoDelay::MAX_DELAY> DSY_SDRAM_BSS delayLineSendR;
StereoDelay* delaySend;

DelayLine<float, StereoDelay::MAX_DELAY> DSY_SDRAM_BSS delayLineMasterL;
DelayLine<float, StereoDelay::MAX_DELAY> DSY_SDRAM_BSS delayLineMasterR;
StereoDelay* delayMaster;

StereoDjFilter* djfMaster;
Noise* noiseMaster;
Distortion* distoMaster;
StereoCompressor* compMaster;
StereoLimiter* limMaster;

// Font data
FontDef font       = Font_7x10;
uint8_t fontWidth  = 7;
uint8_t fontHeight = 10;

// System popup messages
std::vector<std::string> msgPress      = std::vector<std::string>{"State loaded", "_END_"}; // TODO consider duration 0 or shorter
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
// Called when there's a fresh install of Omakase
void saveDefaultSettingsToQSPI(){
    settings_ram[RAM_OP_MODE] = 0;
    settings_ram[RAM_IS_INIT] = FRESH_BOOT_VAL;

    settings_ram[RAM_DIST_TYPE]  = DIST_DEFAULT_TYPE;
    settings_ram[RAM_DIST_TONE]  = AFX_MIN_LEVEL;
    settings_ram[RAM_DIST_DRIVE] = AFX_MIN_LEVEL;
    settings_ram[RAM_DIST_LVL]   = AFX_MAX_LEVEL;

    settings_ram[RAM_DLY_FDBK]   = 500;
    settings_ram[RAM_DLY_TIME]   = 500;
    settings_ram[RAM_DLY_HPF]    = 200;
    settings_ram[RAM_DLY_VERB]   = AFX_MIN_LEVEL;

    settings_ram[RAM_VERB_FDBK]  = 800;
    settings_ram[RAM_VERB_DAMP]  = AFX_MAX_LEVEL;
    settings_ram[RAM_VERB_HPF]   = 500;
    settings_ram[RAM_VERB_DRY]   = AFX_MAX_LEVEL;

    settings_ram[RAM_TX_DLY]     = AFX_MIN_LEVEL;
    settings_ram[RAM_TX_VERB]    = AFX_MIN_LEVEL;
    settings_ram[RAM_TX_NOISE]   = AFX_MAX_LEVEL - DjFilter::DJF_MIN_LEVEL;
    settings_ram[RAM_TX_FILT]    = 0 - DjFilter::DJF_MIN_LEVEL;

    settings_ram[RAM_COMP_ATK]    = 0;
    settings_ram[RAM_COMP_REL]    = 0;
    settings_ram[RAM_COMP_THRESH] = 0 - COMP_MIN_THRESH;
    settings_ram[RAM_COMP_RATIO]  = 0;

    settings_ram[RAM_MSTR_TONE]   = AFX_MIN_LEVEL;
    settings_ram[RAM_MSTR_TYPE]   = DIST_DEFAULT_TYPE;
    settings_ram[RAM_MSTR_DRIVE]  = AFX_MIN_LEVEL;
    settings_ram[RAM_MSTR_LVL]    = 1000;

    settings_ram[RAM_CMP_BPS]      = 0;
    settings_ram[RAM_NOISE_LVL]    = 500;

    settings_ram[RAM_SEND_CFG]     = 0;

    for(uint8_t i = 0; i < NUM_INPUT_CHANNELS; i++){
        settings_ram[RAM_LEVEL + i]      = AFX_MIN_LEVEL;
        settings_ram[RAM_FILTER + i]     = 0 - DjFilter::DJF_MIN_LEVEL;
        settings_ram[RAM_DISTORTION + i] = AFX_MIN_LEVEL;
        settings_ram[RAM_DELAY + i]      = AFX_MIN_LEVEL;
        settings_ram[RAM_REVERB + i]     = AFX_MIN_LEVEL;
        settings_ram[RAM_SEND + i]       = AFX_MIN_LEVEL;
        settings_ram[RAM_SEND_2 + i]     = AFX_MIN_LEVEL;
        settings_ram[RAM_PAN + i]        = DEFAULT_PAN - MIN_PAN;
        settings_ram[RAM_GAIN + i]       = 1000;
    }
    saveSettingsToQSPI();
}

void cbOpMode(){
    settings_ram[RAM_OP_MODE] = menu->getItem("Op Mode")->getIndex();
    saveSettingsToQSPI();
};

// Gain
void cbGain(uint8_t ch){
    channels[ch].setGain(menu->getItem(ch, PAGE_GAIN)->getIndex());
    settings_ram[RAM_GAIN + ch] = menu->getItem(ch, PAGE_GAIN)->getIndex() + 1000;
};
void cbGain1(){ cbGain(0); }; // TODO this design pattern is ugly, maybe modify menu to take a function with an arg so I don't have to do this
void cbGain2(){ cbGain(1); };
void cbGain3(){ cbGain(2); };
void cbGain4(){ cbGain(3); };

// Level
void cbLvl(uint8_t ch){
    channels[ch].setLevel(CH_MAIN, menu->getItem(ch, PAGE_LVL)->getIndex());
    settings_ram[RAM_LEVEL + ch] = menu->getItem(ch, PAGE_LVL)->getIndex();
};
void cbLvl1(){ cbLvl(0); };
void cbLvl2(){ cbLvl(1); };
void cbLvl3(){ cbLvl(2); };
void cbLvl4(){ cbLvl(3); };

// External send 1
void cbSend(uint8_t ch){
   channels[ch].setLevel(CH_EXTERNAL, menu->getItem(ch, PAGE_SEND)->getIndex());
   settings_ram[RAM_SEND + ch] = menu->getItem(ch, PAGE_SEND)->getIndex();
};
void cbSend1(){ cbSend(0); };
void cbSend2(){ cbSend(1); };
void cbSend3(){ cbSend(2); };
void cbSend4(){ cbSend(3); };

// External send 2
void cbSendTwo(uint8_t ch){
   channels[ch].setLevel(CH_EXTERNAL_2, menu->getItem(ch, PAGE_SEND_2)->getIndex());
   settings_ram[RAM_SEND_2 + ch] = menu->getItem(ch, PAGE_SEND_2)->getIndex();
};
void cbSendTwo1(){ cbSendTwo(0); };
void cbSendTwo2(){ cbSendTwo(1); };
void cbSendTwo3(){ cbSendTwo(2); };
void cbSendTwo4(){ cbSendTwo(3); };

// Reverb send
void cbReverb(uint8_t ch){
   channels[ch].setLevel(CH_REVERB, menu->getItem(ch, PAGE_VERB)->getIndex());
   settings_ram[RAM_REVERB + ch] = menu->getItem(ch, PAGE_VERB)->getIndex();
};
void cbReverb1(){ cbReverb(0); };
void cbReverb2(){ cbReverb(1); };
void cbReverb3(){ cbReverb(2); };
void cbReverb4(){ cbReverb(3); };

void cbVerbFdbk(){
    settings_ram[RAM_VERB_FDBK] = menu->getItem(0, PAGE_VERB_CFG)->getIndex();
    reverbSend->sc->SetFeedback(std::max(settings_ram[RAM_VERB_FDBK] * .001, 0.2));
}

void cbVerbDamp(){
    settings_ram[RAM_VERB_DAMP] = menu->getItem(1, PAGE_VERB_CFG)->getIndex();
    reverbSend->sc->SetLpFreq(settings_ram[RAM_VERB_DAMP] * .001 * 18000.0);
}

void cbVerbHpf(){
    settings_ram[RAM_VERB_HPF] = menu->getItem(2, PAGE_VERB_CFG)->getIndex();
    reverbSend->setHpf(settings_ram[RAM_VERB_HPF] * .001 * 10000.f); // anything above 10k wouldnt be useful
}

void cbVerbDry(){
    // Stub function
}

// Delay send
void cbDelay(uint8_t ch){
   channels[ch].setLevel(CH_DELAY, menu->getItem(ch, PAGE_DLY)->getIndex());
   settings_ram[RAM_DELAY + ch] = menu->getItem(ch, PAGE_DLY)->getIndex();
};
void cbDelay1(){ cbDelay(0); };
void cbDelay2(){ cbDelay(1); };
void cbDelay3(){ cbDelay(2); };
void cbDelay4(){ cbDelay(3); };

void cbDlyFdbk(){
    settings_ram[RAM_DLY_FDBK] = menu->getItem(0, PAGE_DLY_CFG)->getIndex();
    delaySend->setFeedback(settings_ram[RAM_DLY_FDBK] * .001);
}

void cbDlyTime(){
    settings_ram[RAM_DLY_TIME] = menu->getItem(1, PAGE_DLY_CFG)->getIndex();
    delaySend->setTime(settings_ram[RAM_DLY_TIME]);
    delayMaster->setTime(settings_ram[RAM_DLY_TIME]);
}

void cbDlyHpf(){
    settings_ram[RAM_DLY_HPF] = menu->getItem(2, PAGE_DLY_CFG)->getIndex();
    delaySend->setHpf(settings_ram[RAM_DLY_HPF] * .001 * 9000.f); // anything above 9k wouldnt be useful
}

void cbDlyVerb(){
    // Stub function
}

// Distortion
void cbDistortion(uint8_t ch){
   channels[ch].dist->setDrive(menu->getItem(ch, PAGE_DIST)->getIndex());
   settings_ram[RAM_DISTORTION + ch] = menu->getItem(ch, PAGE_DIST)->getIndex();
};
void cbDistortion1(){ cbDistortion(0); };
void cbDistortion2(){ cbDistortion(1); };
void cbDistortion3(){ cbDistortion(2); };
void cbDistortion4(){ cbDistortion(3); };

void cbDistType(){
    settings_ram[RAM_DIST_TYPE] = menu->getItem(0, PAGE_DIST_CFG)->getIndex();
    for(uint8_t ch = 0; ch < NUM_INPUT_CHANNELS; ch++){
        channels[ch].dist->setType(settings_ram[RAM_DIST_TYPE]);
    }
}

void cbDistTone(){
    settings_ram[RAM_DIST_TONE] = menu->getItem(1, PAGE_DIST_CFG)->getIndex();
    for(uint8_t ch = 0; ch < NUM_INPUT_CHANNELS; ch++){
        channels[ch].dist->setTone(settings_ram[RAM_DIST_TONE]);
    }
}

void cbDistDrive(){
    // Stub function
}

void cbDistLvl(){
    settings_ram[RAM_DIST_LVL] = menu->getItem(3, PAGE_DIST_CFG)->getIndex();
    for(uint8_t ch = 0; ch < NUM_INPUT_CHANNELS; ch++){
        channels[ch].dist->setLevel(settings_ram[RAM_DIST_LVL]);
    }
}

// DJ filter
void cbDjf(uint8_t ch){
   channels[ch].djf->setLevel(menu->getItem(ch, PAGE_FILT)->getIndex());
   settings_ram[RAM_FILTER + ch] = menu->getItem(ch, PAGE_FILT)->getIndex() - DjFilter::DJF_MIN_LEVEL;
};
void cbDjf1(){ cbDjf(0); };
void cbDjf2(){ cbDjf(1); };
void cbDjf3(){ cbDjf(2); };
void cbDjf4(){ cbDjf(3); };

// Pan
void cbPan(uint8_t ch){
   channels[ch].pan->setPan(menu->getItem(ch, PAGE_PAN)->getIndex());
   settings_ram[RAM_PAN + ch] = menu->getItem(ch, PAGE_PAN)->getIndex() - MIN_PAN;
};
void cbPan1(){ cbPan(0); };
void cbPan2(){ cbPan(1); };
void cbPan3(){ cbPan(2); };
void cbPan4(){ cbPan(3); };

// Scales wet/dry, feedback, hpf
void cbTxDelay(){
    settings_ram[RAM_TX_DLY] = menu->getItem(0, PAGE_TX)->getIndex();
    delayMaster->feedback = std::max(settings_ram[RAM_TX_DLY] * .001 * 0.6, 0.2);
    delayMaster->setHpf(std::max(settings_ram[RAM_TX_DLY] * .001 * 4000, 1000.0));
}

void cbTxReverb(){
    settings_ram[RAM_TX_VERB] = menu->getItem(1, PAGE_TX)->getIndex();
    reverbMaster->sc->SetFeedback(std::max(settings_ram[RAM_TX_VERB] * .001 * 0.99999, 0.85));
    reverbMaster->setHpf(std::max(settings_ram[RAM_TX_VERB] * .001 * 500, 0.0));
}

void cbTxNoise(){
   noiseMaster->djf->setLevel(menu->getItem(2, PAGE_TX)->getIndex());
   settings_ram[RAM_TX_NOISE] = menu->getItem(2, PAGE_TX)->getIndex() - DjFilter::DJF_MIN_LEVEL;
}

void cbTxFilter(){
   djfMaster->setLevel(menu->getItem(3, PAGE_TX)->getIndex());
   settings_ram[RAM_TX_FILT] = menu->getItem(3, PAGE_TX)->getIndex() - DjFilter::DJF_MIN_LEVEL;
}

void cbCompAtk(){
    settings_ram[RAM_COMP_ATK] = menu->getItem(0, PAGE_COMP)->getIndex();
    compMaster->setAttack(compAtksToFloat[settings_ram[RAM_COMP_ATK]]);
}

void cbCompRel(){
    settings_ram[RAM_COMP_REL] = menu->getItem(1, PAGE_COMP)->getIndex();
    compMaster->setRelease(compRelsToFloat[settings_ram[RAM_COMP_REL]]);
}

void cbCompThresh(){
    compMaster->setThresh(menu->getItem(2, PAGE_COMP)->getIndex());
    settings_ram[RAM_COMP_THRESH] = menu->getItem(2, PAGE_COMP)->getIndex() - COMP_MIN_THRESH;
}

void cbCompRatio(){
    settings_ram[RAM_COMP_RATIO] = menu->getItem(3, PAGE_COMP)->getIndex();
    compMaster->setRatio(compRatiosToFloat[settings_ram[RAM_COMP_RATIO]]);
}

void cbMstrGain(){
    settings_ram[RAM_MSTR_TONE] = menu->getItem(0, PAGE_MSTR)->getIndex();
    distoMaster->setTone(settings_ram[RAM_MSTR_TONE]);
}

void cbMstrDistType(){
    settings_ram[RAM_MSTR_TYPE] = menu->getItem(1, PAGE_MSTR)->getIndex();
    distoMaster->setType(settings_ram[RAM_MSTR_TYPE]);
}

void cbMstrDrive(){
    settings_ram[RAM_MSTR_DRIVE] = menu->getItem(2, PAGE_MSTR)->getIndex();
    distoMaster->setDrive(settings_ram[RAM_MSTR_DRIVE]);
}

void cbMstrLvl(){
    settings_ram[RAM_MSTR_LVL] = menu->getItem(3, PAGE_MSTR)->getIndex() + 1000;
}

void cbSysReset(){
    if(menu->getItem(0, PAGE_SYS)->getIndex() == 0){
        settings_ram[RAM_IS_INIT] = 68;
        gui->setPopup(msgReset, 500, false);
    } else {
        settings_ram[RAM_IS_INIT] = 69;
    }
}

void cbSendCfg(){
    settings_ram[RAM_SEND_CFG] = menu->getItem(1, PAGE_SYS)->getIndex();
    if(menu->getItem(1, PAGE_SYS)->getIndex() == 0){
        // 1 stereo send
        menu->setPageName(PAGE_SEND, "Ext Send");
        gui->setIsHidden(PAGE_SEND_2, true);
    } else {
        // 2 mono sends
        menu->setPageName(PAGE_SEND, "Ext Send 1");
        gui->setIsHidden(PAGE_SEND_2, false);
    }
}

void cbSys3(){
    // Stub function
}

void cbSys4(){
    // Stub function
}

void cbCompBypass(){
    settings_ram[RAM_CMP_BPS] = menu->getItem(0, PAGE_NOISE)->getIndex();
}

void cbMisc2(){
    // Stub function
}

void cbMisc3(){
    // Stub function
}

void cbNoiseLvl(){
    settings_ram[RAM_NOISE_LVL] = menu->getItem(3, PAGE_NOISE)->getIndex();
}

void loadQSPISettingsToState(){
    loadQSPISettingsToRAM();

    menu->getItem(0, PAGE_DIST_CFG)->setIndex(settings_ram[RAM_DIST_TYPE]);
    menu->getItem(1, PAGE_DIST_CFG)->setIndex(settings_ram[RAM_DIST_TONE]);
    menu->getItem(2, PAGE_DIST_CFG)->setIndex(settings_ram[RAM_DIST_DRIVE]);
    menu->getItem(3, PAGE_DIST_CFG)->setIndex(settings_ram[RAM_DIST_LVL]);

    menu->getItem(0, PAGE_DLY_CFG)->setIndex(settings_ram[RAM_DLY_FDBK]);
    menu->getItem(1, PAGE_DLY_CFG)->setIndex(settings_ram[RAM_DLY_TIME]);
    menu->getItem(2, PAGE_DLY_CFG)->setIndex(settings_ram[RAM_DLY_HPF]);
    menu->getItem(3, PAGE_DLY_CFG)->setIndex(settings_ram[RAM_DLY_VERB]);

    menu->getItem(0, PAGE_VERB_CFG)->setIndex(settings_ram[RAM_VERB_FDBK]);
    menu->getItem(1, PAGE_VERB_CFG)->setIndex(settings_ram[RAM_VERB_DAMP]);
    menu->getItem(2, PAGE_VERB_CFG)->setIndex(settings_ram[RAM_VERB_HPF]);
    menu->getItem(3, PAGE_VERB_CFG)->setIndex(settings_ram[RAM_VERB_DRY]);

    menu->getItem(0, PAGE_TX)->setIndex(settings_ram[RAM_TX_DLY]);
    menu->getItem(1, PAGE_TX)->setIndex(settings_ram[RAM_TX_VERB]);
    menu->getItem(2, PAGE_TX)->setIndex(settings_ram[RAM_TX_NOISE] + DjFilter::DJF_MIN_LEVEL);
    menu->getItem(3, PAGE_TX)->setIndex(settings_ram[RAM_TX_FILT] + DjFilter::DJF_MIN_LEVEL);

    menu->getItem(0, PAGE_MSTR)->setIndex(settings_ram[RAM_MSTR_TONE]);
    menu->getItem(1, PAGE_MSTR)->setIndex(settings_ram[RAM_MSTR_DRIVE]);
    menu->getItem(2, PAGE_MSTR)->setIndex(settings_ram[RAM_MSTR_TYPE]);
    menu->getItem(3, PAGE_MSTR)->setIndex(settings_ram[RAM_MSTR_LVL] - 1000);

    menu->getItem(0, PAGE_COMP)->setIndex(settings_ram[RAM_COMP_ATK]);
    menu->getItem(1, PAGE_COMP)->setIndex(settings_ram[RAM_COMP_REL]);
    menu->getItem(2, PAGE_COMP)->setIndex(settings_ram[RAM_COMP_THRESH] + COMP_MIN_THRESH);
    menu->getItem(3, PAGE_COMP)->setIndex(settings_ram[RAM_COMP_RATIO]);

    menu->getItem(0, PAGE_NOISE)->setIndex(settings_ram[RAM_CMP_BPS]);
    menu->getItem(3, PAGE_NOISE)->setIndex(settings_ram[RAM_NOISE_LVL]);

    menu->getItem(1, PAGE_SYS)->setIndex(settings_ram[RAM_SEND_CFG]);

    for(uint8_t ch = 0; ch < NUM_INPUT_CHANNELS; ch++){
        // Note that setIndex() automatically calls the callback
        menu->getItem(ch, PAGE_LVL )->setIndex(settings_ram[RAM_LEVEL + ch]);
        menu->getItem(ch, PAGE_FILT)->setIndex(settings_ram[RAM_FILTER + ch] + DjFilter::DJF_MIN_LEVEL);
        menu->getItem(ch, PAGE_DIST)->setIndex(settings_ram[RAM_DISTORTION + ch]);
        menu->getItem(ch, PAGE_DLY )->setIndex(settings_ram[RAM_DELAY + ch]);
        menu->getItem(ch, PAGE_VERB)->setIndex(settings_ram[RAM_REVERB + ch]);
        menu->getItem(ch, PAGE_SEND)->setIndex(settings_ram[RAM_SEND + ch]);
        menu->getItem(ch, PAGE_SEND_2)->setIndex(settings_ram[RAM_SEND_2 + ch]);
        menu->getItem(ch, PAGE_PAN )->setIndex(settings_ram[RAM_PAN + ch] + MIN_PAN);
        menu->getItem(ch, PAGE_GAIN)->setIndex(settings_ram[RAM_GAIN + ch] - 1000);
    }

    // un-catch knobs once loaded
    gui->setAllIsCaught(false);
}

// TODO rm if not needed
void updateControls() {
    patch->ProcessAnalogControls();
    patch->ProcessDigitalControls();
}

// TODO rm if not needed
void updateOutputs() {
   
}

void updateOled(){
    gui->updateControls();

    // Set headers
    gui->setHeader(menu->getPageName(menu->getPage()), 0);

    // Keeping a few useful debug outputs here in case I need them later
    if (DEBUG_MODE){
        gui->setHeader(debugStr, 2);
    }

    gui->render();
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

static void AudioCallback(AudioHandle::InputBuffer in,
        AudioHandle::OutputBuffer out, size_t size){

    // Iterate through buffer
    for(size_t i = 0; i < size; i++){

        // Zero out the audio outs
        for(uint8_t bus = 0; bus < NUM_OUTS; bus++){
            outsL[bus] = 0.f;
            outsR[bus] = 0.f;
        }

        // Process each channel, sum them to each audio out
        for(uint8_t ch = 0; ch < NUM_INPUT_CHANNELS; ch++){
            channels[ch].processAudio(in[ch][i]);
            for(uint8_t bus = 0; bus < NUM_OUTS; bus++){
                outsL[bus] += channels[ch].outsL[bus];
                outsR[bus] += channels[ch].outsR[bus];
            }
        }
        float fxL, fxR, wet;

        /* Send FX */

        // External send(s) --> OUT 3, OUT 4
        if(settings_ram[RAM_SEND_CFG] == 0){
            // 1 Stereo
            out[2][i] = outsL[CH_EXTERNAL];
            out[3][i] = outsR[CH_EXTERNAL];
        } else {
            // 2 Mono
            out[2][i] = outsL[CH_EXTERNAL]/2 + outsR[CH_EXTERNAL]/2;
            out[3][i] = outsL[CH_EXTERNAL_2]/2 + outsR[CH_EXTERNAL_2]/2;
        }

        // Delay send
        delaySend->processAudio(fxL, fxR, outsL[CH_DELAY], outsR[CH_DELAY]);
        outsL[CH_MAIN] += fxL;
        outsR[CH_MAIN] += fxR; // TODO decide if I want to feed this straight into the reverb or if I want to process delay, process reverb, then sum em after. Or maybe add a delay send option later.

        // Reverb send
        reverbSend->processAudio(fxL, fxR, outsL[CH_REVERB], outsR[CH_REVERB]);
        outsL[CH_MAIN] = outsL[CH_MAIN] + fxL;
        outsR[CH_MAIN] = outsR[CH_MAIN] + fxR;

        /* Transition FX */

        // Delay
        wet = settings_ram[RAM_TX_DLY] * .001; // TODO add more elegant wet/dry to audioEffect
        delayMaster->processAudio(fxL, fxR, outsL[CH_MAIN], outsR[CH_MAIN]); 
        outsL[CH_MAIN] = fxL * wet + outsL[CH_MAIN] * (1.f - wet);
        outsR[CH_MAIN] = fxR * wet + outsR[CH_MAIN] * (1.f - wet);

        // Reverb
        wet = settings_ram[RAM_TX_VERB] * .001;
        reverbMaster->processAudio(fxL, fxR, outsL[CH_MAIN], outsR[CH_MAIN]); 
        outsL[CH_MAIN] = fxL * wet + outsL[CH_MAIN] * (1.f - wet);
        outsR[CH_MAIN] = fxR * wet + outsR[CH_MAIN] * (1.f - wet);

        // Noise sweep
        fxL = noiseMaster->processAudio() * settings_ram[RAM_NOISE_LVL] * 0.001f;
        outsL[CH_MAIN] += fxL;
        outsR[CH_MAIN] += fxL;

        // Filter
        djfMaster->processAudio(fxL, fxR, outsL[CH_MAIN], outsR[CH_MAIN]); 
        outsL[CH_MAIN] = fxL;
        outsR[CH_MAIN] = fxR;

        /* Mastering chain */
        // Distorion
        outsL[CH_MAIN] = distoMaster->processAudio(outsL[CH_MAIN]);
        outsR[CH_MAIN] = distoMaster->processAudio(outsR[CH_MAIN]);

        // Compressor
        if(settings_ram[RAM_CMP_BPS] == 1){
            compMaster->processAudio(fxL, fxR, outsL[CH_MAIN], outsR[CH_MAIN]); 
            outsL[CH_MAIN] = fxL;
            outsR[CH_MAIN] = fxR;
        }

        // Limiter
        // limMaster->processAudio(fxL, fxR, outsL[CH_MAIN], outsR[CH_MAIN]); 
        // outsL[CH_MAIN] = fxL;
        // outsR[CH_MAIN] = fxR;

        // Level
        outsL[CH_MAIN] = outsL[CH_MAIN] * (1.f + (settings_ram[RAM_MSTR_LVL] - 1000) * .001f);
        outsR[CH_MAIN] = outsR[CH_MAIN] * (1.f + (settings_ram[RAM_MSTR_LVL] - 1000) * .001f);

        // Main channel --> OUT 1, OUT 2
        out[0][i] = outsL[CH_MAIN];
        out[1][i] = outsR[CH_MAIN];
    } 
}


int main(void) {
    // Initialize state objects
    patch = new DaisyPatch();
    patch->Init();
    float sampleRate = patch->AudioSampleRate();

    menu = new Menu(MENU_NUM_PAGES);

    gui = new PatchGui(patch, menu, &font, fontWidth, fontHeight, 4);
    gui->setCvCatch(true);
    gui->setCbPress(cbPress, msgPress, 100, true);
    gui->setCbShortHold(cbShortHold, msgShortHold, 100, true);
    gui->setCbLongHold(cbLongHold, msgLongHoldA, 100, true); 
    gui->setZoneB(PAGE_DIST_CFG);
    gui->drawHoldBar = true;

    reverbSend   = new StereoReverb(sampleRate, &reverbScSend);
    reverbMaster = new StereoReverb(sampleRate, &reverbScMaster);

    delayLineSendL.Init();
    delayLineSendR.Init();
    delayLineMasterL.Init();
    delayLineMasterR.Init();
    delaySend   = new StereoDelay(sampleRate, &delayLineSendL, &delayLineSendR);
    delayMaster = new StereoDelay(sampleRate, &delayLineMasterL, &delayLineMasterR);

    djfMaster   = new StereoDjFilter(sampleRate);
    noiseMaster = new Noise(sampleRate);
    distoMaster = new Distortion(sampleRate);

    compMaster = new StereoCompressor(sampleRate);
    compMaster->setRatio(800); // TODO play with these vals
    compMaster->setAttack(2);
    compMaster->setRelease(10);

    limMaster = new StereoLimiter();

    for(uint8_t i = 0; i < NUM_INPUT_CHANNELS; i++){
        channels[i] = *(new Channel(sampleRate));
    }

    loadQSPISettingsToRAM();

    // If the RAM_IS_INIT setting isn't FRESH_BOOT_VAL, we know that Omakase doesn't have settings
    // here and we should use default settings.
    if (settings_ram[RAM_IS_INIT] != FRESH_BOOT_VAL){
        uint16_t t;
        if(DEBUG_MODE){
            // Long popup gets annoying during development, don't display in debug mode
            t = 300;
        } else {
            t = 1500;
        }
        gui->setPopup(msgFreshBoot, t, false);
        saveDefaultSettingsToQSPI();
    } 

    if (!DEBUG_MODE){
        // Boot screen gets annoying during development, don't display it in debug mode
        gui->drawStartupScreen("Omakase", VERSION, 750);
    }

    /* Zone A (Performance) */
    menu->setPageName(PAGE_GAIN, "Gain");
    menu->append(PAGE_GAIN,     "Gain 1", "   ", -1000, 1000, settings_ram[RAM_GAIN]   - 1000, cbGain1);
    menu->append(PAGE_GAIN,     "Gain 2", "   ", -1000, 1000, settings_ram[RAM_GAIN+1] - 1000, cbGain2);
    menu->append(PAGE_GAIN,     "Gain 3", "   ", -1000, 1000, settings_ram[RAM_GAIN+2] - 1000, cbGain3);
    menu->append(PAGE_GAIN,     "Gain 4", "   ", -1000, 1000, settings_ram[RAM_GAIN+3] - 1000, cbGain4);

    menu->setPageName(PAGE_LVL, "Level");
    menu->append(PAGE_LVL,      "Level 1", "  ", AFX_MIN_LEVEL, AFX_MAX_LEVEL, settings_ram[RAM_LEVEL],   cbLvl1);
    menu->append(PAGE_LVL,      "Level 2", "  ", AFX_MIN_LEVEL, AFX_MAX_LEVEL, settings_ram[RAM_LEVEL+1], cbLvl2);
    menu->append(PAGE_LVL,      "Level 3", "  ", AFX_MIN_LEVEL, AFX_MAX_LEVEL, settings_ram[RAM_LEVEL+2], cbLvl3);
    menu->append(PAGE_LVL,      "Level 4", "  ", AFX_MIN_LEVEL, AFX_MAX_LEVEL, settings_ram[RAM_LEVEL+3], cbLvl4);

    menu->setPageName(PAGE_FILT, "DJ Filter");
    menu->append(PAGE_FILT,     "Filter 1", " ", DjFilter::DJF_MIN_LEVEL, DjFilter::DJF_MAX_LEVEL, settings_ram[RAM_FILTER]   + DjFilter::DJF_MIN_LEVEL, cbDjf1);
    menu->append(PAGE_FILT,     "Filter 2", " ", DjFilter::DJF_MIN_LEVEL, DjFilter::DJF_MAX_LEVEL, settings_ram[RAM_FILTER+1] + DjFilter::DJF_MIN_LEVEL, cbDjf2);
    menu->append(PAGE_FILT,     "Filter 3", " ", DjFilter::DJF_MIN_LEVEL, DjFilter::DJF_MAX_LEVEL, settings_ram[RAM_FILTER+2] + DjFilter::DJF_MIN_LEVEL, cbDjf3);
    menu->append(PAGE_FILT,     "Filter 4", " ", DjFilter::DJF_MIN_LEVEL, DjFilter::DJF_MAX_LEVEL, settings_ram[RAM_FILTER+3] + DjFilter::DJF_MIN_LEVEL, cbDjf4);
    menu->getItem(0, PAGE_FILT)->setNotch(0, 50);
    menu->getItem(1, PAGE_FILT)->setNotch(0, 50);
    menu->getItem(2, PAGE_FILT)->setNotch(0, 50);
    menu->getItem(3, PAGE_FILT)->setNotch(0, 50);

    menu->setPageName(PAGE_VERB, "Reverb");
    menu->append(PAGE_VERB,     "Reverb 1", " ", AFX_MIN_LEVEL, AFX_MAX_LEVEL, settings_ram[RAM_REVERB],   cbReverb1);
    menu->append(PAGE_VERB,     "Reverb 2", " ", AFX_MIN_LEVEL, AFX_MAX_LEVEL, settings_ram[RAM_REVERB+1], cbReverb2);
    menu->append(PAGE_VERB,     "Reverb 3", " ", AFX_MIN_LEVEL, AFX_MAX_LEVEL, settings_ram[RAM_REVERB+2], cbReverb3);
    menu->append(PAGE_VERB,     "Reverb 4", " ", AFX_MIN_LEVEL, AFX_MAX_LEVEL, settings_ram[RAM_REVERB+3], cbReverb4);
   
    menu->setPageName(PAGE_DLY, "Delay");
    menu->append(PAGE_DLY,      "Delay 1", "  ", AFX_MIN_LEVEL, AFX_MAX_LEVEL, settings_ram[RAM_DELAY],   cbDelay1);
    menu->append(PAGE_DLY,      "Delay 2", "  ", AFX_MIN_LEVEL, AFX_MAX_LEVEL, settings_ram[RAM_DELAY+1], cbDelay2);
    menu->append(PAGE_DLY,      "Delay 3", "  ", AFX_MIN_LEVEL, AFX_MAX_LEVEL, settings_ram[RAM_DELAY+2], cbDelay3);
    menu->append(PAGE_DLY,      "Delay 4", "  ", AFX_MIN_LEVEL, AFX_MAX_LEVEL, settings_ram[RAM_DELAY+3], cbDelay4);

    menu->setPageName(PAGE_DIST, "Distortion");
    menu->append(PAGE_DIST,     "Disto 1", "  ", AFX_MIN_LEVEL, AFX_MAX_LEVEL, settings_ram[RAM_DISTORTION],   cbDistortion1);
    menu->append(PAGE_DIST,     "Disto 2", "  ", AFX_MIN_LEVEL, AFX_MAX_LEVEL, settings_ram[RAM_DISTORTION+1], cbDistortion2);
    menu->append(PAGE_DIST,     "Disto 3", "  ", AFX_MIN_LEVEL, AFX_MAX_LEVEL, settings_ram[RAM_DISTORTION+2], cbDistortion3);
    menu->append(PAGE_DIST,     "Disto 4", "  ", AFX_MIN_LEVEL, AFX_MAX_LEVEL, settings_ram[RAM_DISTORTION+3], cbDistortion4);

    //menu->setPageName(PAGE_SEND, "Ext Send");
    menu->append(PAGE_SEND,     "Ch1", "      ", AFX_MIN_LEVEL, AFX_MAX_LEVEL, settings_ram[RAM_SEND],   cbSend1);
    menu->append(PAGE_SEND,     "Ch2", "      ", AFX_MIN_LEVEL, AFX_MAX_LEVEL, settings_ram[RAM_SEND+1], cbSend2);
    menu->append(PAGE_SEND,     "Ch3", "      ", AFX_MIN_LEVEL, AFX_MAX_LEVEL, settings_ram[RAM_SEND+2], cbSend3);
    menu->append(PAGE_SEND,     "Ch4", "      ", AFX_MIN_LEVEL, AFX_MAX_LEVEL, settings_ram[RAM_SEND+3], cbSend4);

    menu->setPageName(PAGE_SEND_2, "Ext Send 2");
    menu->append(PAGE_SEND_2,     "Ch 1", "      ", AFX_MIN_LEVEL, AFX_MAX_LEVEL, settings_ram[RAM_SEND_2],   cbSendTwo1);
    menu->append(PAGE_SEND_2,     "Ch 2", "      ", AFX_MIN_LEVEL, AFX_MAX_LEVEL, settings_ram[RAM_SEND_2+1], cbSendTwo2);
    menu->append(PAGE_SEND_2,     "Ch 3", "      ", AFX_MIN_LEVEL, AFX_MAX_LEVEL, settings_ram[RAM_SEND_2+2], cbSendTwo3);
    menu->append(PAGE_SEND_2,     "Ch 4", "      ", AFX_MIN_LEVEL, AFX_MAX_LEVEL, settings_ram[RAM_SEND_2+3], cbSendTwo4);

    menu->setPageName(PAGE_PAN, "Pan");
    menu->append(PAGE_PAN,      "Pan 1", "    ", MIN_PAN, MAX_PAN, settings_ram[RAM_PAN]   + MIN_PAN, cbPan1);
    menu->append(PAGE_PAN,      "Pan 2", "    ", MIN_PAN, MAX_PAN, settings_ram[RAM_PAN+1] + MIN_PAN, cbPan2);
    menu->append(PAGE_PAN,      "Pan 3", "    ", MIN_PAN, MAX_PAN, settings_ram[RAM_PAN+2] + MIN_PAN, cbPan3);
    menu->append(PAGE_PAN,      "Pan 4", "    ", MIN_PAN, MAX_PAN, settings_ram[RAM_PAN+3] + MIN_PAN, cbPan4);
    menu->getItem(0, PAGE_PAN)->setNotch(0, 5);
    menu->getItem(1, PAGE_PAN)->setNotch(0, 5);
    menu->getItem(2, PAGE_PAN)->setNotch(0, 5);
    menu->getItem(3, PAGE_PAN)->setNotch(0, 5);

    menu->setPageName(PAGE_TX, "Transition FX");
    menu->append(PAGE_TX,       "Delay" , "   ", AFX_MIN_LEVEL, AFX_MAX_LEVEL, settings_ram[RAM_TX_DLY],   cbTxDelay);  // Wet/dry, maybe increases feedback too. Might put a ~500hz HPF on this
    menu->append(PAGE_TX,       "Reverb", "   ", AFX_MIN_LEVEL, AFX_MAX_LEVEL, settings_ram[RAM_TX_VERB],  cbTxReverb); // Wet/dry, maybe increases feedback too. Might put a ~500hz HPF on this
    menu->append(PAGE_TX,       "Noise", "    ", DjFilter::DJF_MIN_LEVEL, DjFilter::DJF_MAX_LEVEL, settings_ram[RAM_TX_NOISE]  + DjFilter::DJF_MIN_LEVEL, cbTxNoise);  // TODO gotta figure out how to keep the level for this correct 
    menu->append(PAGE_TX,       "Filter", "   ", DjFilter::DJF_MIN_LEVEL, DjFilter::DJF_MAX_LEVEL, settings_ram[RAM_TX_FILT] + DjFilter::DJF_MIN_LEVEL,   cbTxFilter);
    menu->getItem(3, PAGE_TX)->setNotch(0, 50);

    /* Zone B (settings) */

    menu->setPageName(PAGE_DIST_CFG, "Distortion");
    menu->append(PAGE_DIST_CFG, "Type", "     ", distAlgos,         settings_ram[RAM_DIST_TYPE],  cbDistType);
    menu->append(PAGE_DIST_CFG, "Tone", "     ", 0, AFX_MAX_LEVEL,  settings_ram[RAM_DIST_TONE],  cbDistTone);
    menu->append(PAGE_DIST_CFG, "", "         ", blank,             0,                            cbDistDrive); // maybe this can set a scalar that multiplies the existing drive value?
    menu->append(PAGE_DIST_CFG, "", "   ",  blank, 0,   cbDistLvl);

    menu->setPageName(PAGE_DLY_CFG, "Delay");
    menu->append(PAGE_DLY_CFG,  "Fdbk", "     ", AFX_MIN_LEVEL, AFX_MAX_LEVEL, settings_ram[RAM_DLY_FDBK], cbDlyFdbk);
    menu->append(PAGE_DLY_CFG,  "Time", "     ", AFX_MIN_LEVEL, AFX_MAX_LEVEL, settings_ram[RAM_DLY_TIME], cbDlyTime); // adjusts tx delay time too
    menu->append(PAGE_DLY_CFG,  "HPF", "      ", AFX_MIN_LEVEL, AFX_MAX_LEVEL, settings_ram[RAM_DLY_HPF],  cbDlyHpf);
    menu->append(PAGE_DLY_CFG,  "", "         ", blank,         0,                                         cbDlyVerb); // TODO noise level?

    menu->setPageName(PAGE_VERB_CFG, "Reverb");
    menu->append(PAGE_VERB_CFG, "Fdbk", "     ", AFX_MIN_LEVEL, AFX_MAX_LEVEL, settings_ram[RAM_VERB_FDBK], cbVerbFdbk);
    menu->append(PAGE_VERB_CFG, "Damp", "     ", AFX_MIN_LEVEL, AFX_MAX_LEVEL, settings_ram[RAM_VERB_DAMP], cbVerbDamp);
    menu->append(PAGE_VERB_CFG, "HPF", "      ", AFX_MIN_LEVEL, AFX_MAX_LEVEL, settings_ram[RAM_VERB_HPF],  cbVerbHpf);
    menu->append(PAGE_VERB_CFG, "", "      ",    blank, 0,  cbVerbDry); 

    menu->setPageName(PAGE_COMP, "Compressor");
    menu->append(PAGE_COMP,     "Attack", "   ", compAtks,                         settings_ram[RAM_COMP_ATK],   cbCompAtk);
    menu->append(PAGE_COMP,     "Release", "  ", compRels,                         settings_ram[RAM_COMP_REL],   cbCompRel);
    menu->append(PAGE_COMP,     "Thresh", "   ", COMP_MIN_THRESH, COMP_MAX_THRESH, settings_ram[RAM_COMP_THRESH] + COMP_MIN_THRESH, cbCompThresh);
    menu->append(PAGE_COMP,     "Ratio", "    ", compRatios,                       settings_ram[RAM_COMP_RATIO], cbCompRatio);

    menu->setPageName(PAGE_NOISE, "Misc");
    menu->append(PAGE_NOISE,    "Cmp Byps", " ", yesOrNo, 0, cbCompBypass); 
    menu->append(PAGE_NOISE,    "", "         ", blank,   0,  cbMisc2); 
    menu->append(PAGE_NOISE,    "", "         ", blank,   0,  cbMisc3); 
    menu->append(PAGE_NOISE,    "Noise Lvl", "   ", AFX_MIN_LEVEL, AFX_MAX_LEVEL, settings_ram[RAM_NOISE_LVL],  cbNoiseLvl);
    
    menu->setPageName(PAGE_MSTR, "Master");
    menu->append(PAGE_MSTR,     "Tone", "     ", AFX_MIN_LEVEL, AFX_MAX_LEVEL, settings_ram[RAM_MSTR_TONE],  cbMstrGain);
    menu->append(PAGE_MSTR,     "Type", "     ", distAlgos,                    settings_ram[RAM_MSTR_TYPE],  cbMstrDistType); 
    menu->append(PAGE_MSTR,     "Drive", "    ", AFX_MIN_LEVEL, AFX_MAX_LEVEL, settings_ram[RAM_MSTR_DRIVE], cbMstrDrive);
    menu->append(PAGE_MSTR,     "Output", "   ", -1000, 1000,                  settings_ram[RAM_MSTR_LVL] - 1000, cbMstrLvl);

    menu->setPageName(PAGE_SYS, "System");
    menu->append(PAGE_SYS,      "Reset", "    ", yesOrNo , 1,                          cbSysReset);
    menu->append(PAGE_SYS,      "Sends", "    ", sendCfgs, settings_ram[RAM_SEND_CFG], cbSendCfg); 
    menu->append(PAGE_SYS,      "", "         ", blank,    0,                          cbSys3); 
    menu->append(PAGE_SYS,      "", "         ", blank,    0,                          cbSys4); 

    loadQSPISettingsToState();

    patch->StartAdc();
    patch->StartAudio(AudioCallback);

    gui->setPage(0); // Required to bind the CV knobs to the first page's params
    
    // Main event loop
    while(1){
        updateControls();
        updateOled();
        updateOutputs();
    }
}