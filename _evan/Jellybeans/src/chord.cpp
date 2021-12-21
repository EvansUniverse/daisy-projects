/* 
 * Copyright (C) 2021 Evan Pernu - All Rights Reserved
 * You may use, distribute and modify this code under the
 * terms of the GNU AGPLv3 license.
 * 
 * You should have received a copy of the GNU AGPLv3 license with
 * this file (LICENSE.md). If not, please write to: evanpernu@gmail.com, 
 * or visit: https://www.gnu.org/licenses/agpl-3.0.en.html
 */

#include <algorithm>
#include <map>
#include <vector>
#include <string>
//#include <iostream>

#include "theory.h"
#include "chord.h"

using namespace jellybeans;

DiatonicChord::DiatonicChord(){
    root = 1;
    mode = "Major";
    modeRoot = 0;
    voicing = "Triad";

    this->SetRoot(1);
    this->SetMode("Major");
    this->SetVoicing("Triad");

    myString ="aaaaaaaaaaaaaaaaaaaaaaaaaaaa";
  //  this->UpdateString();
}

DiatonicChord::DiatonicChord(int theRoot, int theModeRoot, std::string theMode, std::string theVoicing){
    root = theRoot;
    mode = theMode;
    modeRoot = theModeRoot;
    voicing = theVoicing;

    this->SetRoot(theRoot);
    this->SetMode(theMode);
    this->SetVoicing(theVoicing);

    myString ="chord";
    //this->UpdateString();
}

void DiatonicChord::UpdateChord(){
    // int degree;
    // int oct;
    // int chordLen = static_cast<int>(voicingToScaleDegrees.at("Triad").size());
    // //int scaleLen = static_cast<int>(modeToSemitones.at(mode).size());
    // semis = std::vector<int>(chordLen);

    // length = chordLen;

    // // For each note in the chord
    // for (int i = 0; i < chordLen; i++){
    //     // Get the degree
    //     degree = voicingToScaleDegrees.at(voicing)[i];

    //     // Offset by 1 since the values of the maps are 1-inedexed
    //     degree--;

    //     // Calculate the semitone value
    //     semis[i] = modeToSemitones.at(mode)[degree]; //+ 12 * (oct + mOct->index) + mTonic->index;

    //     // If the value exceeds our note range, bring it up/down an octave until it fits
    //     while (semis[i] > MAX_NOTE){
    //         semis[i] -= 12;
    //     }
    //     while (semis[i] < MIN_NOTE){
    //         semis[i] += 12;
    //     }
    // }

    // this->UpdateString();
}

void DiatonicChord::SetRoot(int theRoot){
    root = theRoot;
    UpdateChord();
}

// TODO this is untested
void DiatonicChord::SetRootByNote(int theRoot){
    // Quantize theRoot
    if(!isDiatonic(theRoot, mode)){
        theRoot == MAX_NOTE ? theRoot-- : theRoot++;
    }

    // Set root to the scale degree of theRoot
    std::vector<int> s = modeToSemitones.at(mode);
    theRoot = distance(s.begin(), std::find(s.begin(), s.end(), theRoot % 12)) + 1;
    UpdateChord();
}

void DiatonicChord::SetModeRoot(int theModeRoot){
    modeRoot = theModeRoot;
    UpdateChord();
}

void DiatonicChord::SetMode(std::string theMode){
    mode = theMode;
    UpdateChord();
}

void DiatonicChord::SetVoicing(std::string theVoicing){
    voicing = theVoicing;
    UpdateChord();
}

void DiatonicChord::Transpose(int i) {
    if (modeRoot + i < MIN_NOTE || modeRoot + i > MAX_NOTE){
        return;
    }
    modeRoot += i;
    UpdateChord();
}

int DiatonicChord::GetNoteAt(int n){
    if (n >= length) {
        return semis[0];
    }
    return semis[n];
}

// TODO make this more accurate/robust (i.e. "A minor" instead of "A triad")
void DiatonicChord::UpdateString(){
   // std::string s = aallNotes5Oct[1];
    // for some reason, this code generates an error :
    // {"token":26,"outOfBandRecord":[],"resultRecords":{"resultClass":"done","results":[["stack",[["frame",[["level","0"],["addr","0x0800b814"],["func","HardFault_Handler"],["file","src/sys/system.cpp"],["fullname","E:\\Daisy\\DaisyExamples\\libDaisy\\src\\sys\\system.cpp"],["line","121"],["arch","armv7e-m"]]],["frame",[["level","1"],["addr","0xfffffff9"],["func","<signal handler called>"]]],["frame",[["level","2"],["addr","0x080178ca"],["func","memcpy"],["arch","armv7e-m"]]],["frame",[["level","3"],["addr","0x08005a40"],["func","std::char_traits<char>::copy"],["file","e:\\daisy\\daisytoolchain-0.3.1\\windows\\arm-none-eabi\\include\\c++\\10.2.1\\bits/char_traits.h"],["fullname","e:\\daisy\\daisytoolchain-0.3.1\\windows\\arm-none-eabi\\include\\c++\\10.2.1\\bits\\char_traits.h"],["line","395"],["arch","armv7e-m"]]],["frame",[["level","4"],["addr","0x08005a40"],["func","std::__cxx11::basic_string<char, std::char_traits<char>, std::allocator<char> >::_S_copy"],["file","e:\\daisy\\daisytoolchain-0.3.1\\windows\\arm-none-eabi\\include\\c++\\10.2.1\\bits/basic_string.h"],["fullname","e:\\daisy\\daisytoolchain-0.3.1\\windows\\arm-none-eabi\\include\\c++\\10.2.1\\bits\\basic_string.h"],["line","351"],["arch","armv7e-m"]]],["frame",[["level","5"],["addr","0x08005a40"],["func","std::__cxx11::basic_string<char, std::char_traits<char>, std::allocator<char> >::_S_copy"],["file","e:\\daisy\\daisytoolchain-0.3.1\\windows\\arm-none-eabi\\include\\c++\\10.2.1\\bits/basic_string.h"],["fullname","e:\\daisy\\daisytoolchain-0.3.1\\windows\\arm-none-eabi\\include\\c++\\10.2.1\\bits\\basic_string.h"],["line","346"],["arch","armv7e-m"]]],["frame",[["level","6"],["addr","0x08005a40"],["func","std::__cxx11::basic_string<char, std::char_traits<char>, std::allocator<char> >::_S_copy_chars"],["file","e:\\daisy\\daisytoolchain-0.3.1\\windows\\arm-none-eabi\\include\\c++\\10.2.1\\bits/basic_string.h"],["fullname","e:\\daisy\\daisytoolchain-0.3.1\\windows\\arm-none-eabi\\include\\c++\\10.2.1\\bits\\basic_string.h"],["line","393"],["arch","armv7e-m"]]],["frame",[["level","7"],["addr","0x08005a40"],["func","std::__cxx11::basic_string<char, std::char_traits<char>, std::allocator<char> >::_M_construct<char*>"],["file","e:\\daisy\\daisytoolchain-0.3.1\\windows\\arm-none-eabi\\include\\c++\\10.2.1\\bits/basic_string.tcc"],["fullname","e:\\daisy\\daisytoolchain-0.3.1\\windows\\arm-none-eabi\\include\\c++\\10.2.1\\bits\\basic_string.tcc"],["line","225"],["arch","armv7e-m"]]],["frame",[["level","8"],["addr","0x08005a40"],["func","std::__cxx11::basic_string<char, std::char_traits<char>, std::allocator<char> >::_M_construct_aux<char*>"],["file","e:\\daisy\\daisytoolchain-0.3.1\\windows\\arm-none-eabi\\include\\c++\\10.2.1\\bits/basic_string.h"],["fullname","e:\\daisy\\daisytoolchain-0.3.1\\windows\\arm-none-eabi\\include\\c++\\10.2.1\\bits\\basic_string.h"],["line","247"],["arch","armv7e-m"]]],["frame",[["level","9"],["addr","0x08005a40"],["func","std::__cxx11::basic_string<char, std::char_traits<char>, std::allocator<char> >::_M_construct<char*>"],["file","e:\\daisy\\daisytoolchain-0.3.1\\windows\\arm-none-eabi\\include\\c++\\10.2.1\\bits/basic_string.h"],["fullname","e:\\daisy\\daisytoolchain-0.3.1\\windows\\arm-none-eabi\\include\\c++\\10.2.1\\bits\\basic_string.h"],["line","266"],["arch","armv7e-m"]]],["frame",[["level","10"],["addr","0x08005a40"],["func","std::__cxx11::basic_string<char, std::char_traits<char>, std::allocator<char> >::basic_string"],["file","e:\\daisy\\daisytoolchain-0.3.1\\windows\\arm-none-eabi\\include\\c++\\10.2.1\\bits/basic_string.h"],["fullname","e:\\daisy\\daisytoolchain-0.3.1\\windows\\arm-none-eabi\\include\\c++\\10.2.1\\bits\\basic_string.h"],["line","451"],["arch","armv7e-m"]]],["frame",[["level","11"],["addr","0x08005a40"],["func","jellybeans::DiatonicChord::UpdateString"],["file","src/chord.cpp"],["fullname","E:\\Daisy\\daisy-projects\\_evan\\Jellybeans\\src\\chord.cpp"],["line","204"],["arch","armv7e-m"]]],["frame",[["level","12"],["addr","0x08005a40"],["func","jellybeans::DiatonicChord::DiatonicChord"],["file","src/chord.cpp"],["fullname","E:\\Daisy\\daisy-projects\\_evan\\Jellybeans\\src\\chord.cpp"],["line","33"],["arch","armv7e-m"]]],["frame",[["level","13"],["addr","0x0800492e"],["func","jellybeans::Arp::Arp"],["file","src/arp.cpp"],["fullname","E:\\Daisy\\daisy-projects\\_evan\\Jellybeans\\src\\arp.cpp"],["line","16"],["arch","armv7e-m"]]],["frame",[["level","14"],["addr","0x0800428e"],["func","__static_initialization_and_destruction_0"],["file","Jellybeans.cpp"],["fullname","E:\\Daisy\\daisy-projects\\_evan\\Jellybeans\\Jellybeans.cpp"],["line","78"],["arch","armv7e-m"]]],["frame",[["level","15"],["addr","0x08017868"],["func","__libc_init_array"],["arch","armv7e-m"]]],["frame",[["level","16"],["addr","0x080009a4"],["func","Reset_Handler"],["file","../../libdaisy/core/startup_stm32h750xx.c"],["fullname","E:\\Daisy\\daisy-projects\\libdaisy\\core\\startup_stm32h750xx.c"],["line","1553"],["arch","armv7e-m"]]]]]]}}
    myString =  allNotes5Oct[root] + " " + voicing;
}