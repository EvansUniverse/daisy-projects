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

#include <algorithm>
#include <map>
#include <vector>
#include <string>

#include "theory.h"
#include "chord.h"

using namespace ev_theory;

DiatonicChord::DiatonicChord() : DiatonicChord(0, "Major", 0, "Triad", 0){}

DiatonicChord::DiatonicChord(
        uint8_t     theModeRoot,
        std::string theMode,
        uint8_t     theDegree,
        std::string theVoicing,
        uint8_t     theOctave
){
    degree    = theDegree;
    octave    = theOctave;
    mode      = theMode;
    modeRoot  = theModeRoot;
    voicing   = theVoicing;
    inversion = 0;
    updateChord();
}

/* Updaters */

void DiatonicChord::updateChord(){
    length = static_cast<uint8_t>(voicingToScaleDegrees.at(voicing).size());
    semis = std::vector<uint8_t>(length);

    uint8_t noteDegree;

    // Determine the semitone value of each note in semis
    for (uint8_t i = 0; i < length; i++){
        // Get scale degree
        noteDegree = (degree + voicingToScaleDegrees.at(voicing)[i]-1) % 7; 
        // Convert it to a semitone value
        semis[i] = scaleDegreeToNote(noteDegree, mode, modeRoot);
        // Add octave modifier and quantize to range
        //semis[i] = quantizeNoteToRange(semis[i] + 12 * octave);
        semis[i] = semis[i] + 12 * octave;
    }

    // Update root
    root = semis[0];

    // Calculate inversion
    for (uint8_t i = 0; i < inversion; i++){
        semis.push_back(semis.front() + 12); 
        semis.erase(semis.begin());
        semis.back() = quantizeNoteToRange(semis.back());
    }

    // For each note, make sure it's higher than the previous one, if  possible. 
    // Also make sure it's unidentical to the previous note by shifting it up an
    // octave. If shifting up would bring the note out of range, shift down instead.
    for (uint8_t i = 0; i < length; i++){
        while (i > 0 && semis[i] <= semis[i-1]) {
            if (semis[i] + 12 <= MAX_NOTE){
                semis[i] += 12;
            } else {
                break;
            }
        }
        if (i > 0 && semis[i] == semis[i-1] && semis[i] - 12 >= MIN_NOTE){
            semis[i] -= 12;
        }  
    }

    updateString();
}

void DiatonicChord::updateString(){
    string = scaleDegreeToNumeral();

    // // To be enabled for debug purposes: e.g. "C E G"
    // // for(uint8_t i : semis) 
    // //     string += allNotes[i % 12] + " ";

    // To be enabled for debug purposes
    // string = "o " + std::to_string(octave) + " r " + std::to_string(root) + " m " + std::to_string(modeRoot) + " d " + std::to_string(degree);
    // // e.g. "0 4 7"
    // for(uint8_t i : semis) 
    //     string += std::to_string(i) + " ";
}


/*  Getters */

uint8_t DiatonicChord::getNoteAt(uint8_t n){
    // If out of bounds, return the first note.
    // This behavior could be modified in the future.
    if (n >= length) {
        return semis[0];
    }
    return semis[n];
}

uint8_t DiatonicChord::getDegree(){
    return degree;
}

uint8_t DiatonicChord::getRoot(){
    return root;
}

uint8_t DiatonicChord::getLength(){
    return length;
}

uint8_t DiatonicChord::getOctave(){
    return octave;
}

std::string DiatonicChord::toString(){
    return string;
}

/* Setters */

void DiatonicChord::setDegreeByNote(uint8_t i){
    setOctave(quantize(i, mode, modeRoot) / 12);
    i = quantizeNoteToRange(i);
    
    degree = noteToScaleDegree(i, mode, modeRoot);
    updateChord();
}

void DiatonicChord::setDegree(uint8_t i){
    if (i >= scaleLen ){
        i = scaleLen - 1;
    }
    degree = i;
    updateChord();
}

void DiatonicChord::setModeRoot(uint8_t i){
    modeRoot = i % 12;
    updateChord();
}

void DiatonicChord::setMode(std::string s){
    mode = s;
    updateChord();
}

void DiatonicChord::setVoicing(std::string s){
    voicing = s;
    updateChord();
}

void DiatonicChord::setOctave(uint8_t i){
    octave = i;


    updateChord();
}

void DiatonicChord::setInversion(uint8_t i){
    inversion = i;
    updateChord();
}

/* Helpers */

std::string DiatonicChord::scaleDegreeToNumeral(){
    std::string res = allNotes[getRoot() % 12];

    // Get the mode degree
    uint8_t i;
    for(i = 0; i < modes.size(); i++){
        if (modes[i] == mode){
            break;
        } 
    }

    uint8_t majEquivalent = (degree + i) % 7;

    // Decided not to display numeral scale degrees but I'll leave this here in case I want it later
    // 'cause it was a pain to write.
    //
    // // toLower() or subtracting the ascii value would be more elegant but 
    // // this is more efficient.
    // if(majEquivalent == 0 || majEquivalent == 3 || majEquivalent == 4){
    //     // Capitalize
    //     switch (degree) {
    //     case 0:
    //         res = "I";
    //         break;
    //     case 1:
    //         res = "II";
    //         break;
    //     case 2:
    //         res = "III";
    //         break;
    //     case 3:
    //         res = "IV";
    //         break;
    //     case 4:
    //         res = "V";
    //         break;
    //     case 5:
    //         res = "VI";
    //         break;
    //     case 6:
    //         res = "VII";
    //         break;
    //     }   
    // } else {
    //     // Don't capitalize
    //     switch (degree) {
    //     case 0:
    //         res = "i";
    //         break;
    //     case 1:
    //         res = "ii";
    //         break;
    //     case 2:
    //         res = "iii";
    //         break;
    //     case 3:
    //         res = "iv";
    //         break;
    //     case 4:
    //         res = "v";
    //         break;
    //     case 5:
    //         res = "vi";
    //         break;
    //     case 6:
    //         res = "vii";
    //         break;
    //     }   
    // }

    if (voicing == "Power"){
        res += "5";
    } else if ((voicing == "Triad" || voicing == "Triad+") && majEquivalent == 6){
        res += "dim";
    } else if (voicing == "Sus2"){
        res += "sus2";
    } else if (voicing == "Sus4"){
        res += "sus4";
    } else if ((voicing == "7th" || voicing == "7th+" || voicing == "Shell 1" || voicing == "Shell 2") && majEquivalent == 4) {
        res += "7";
    } else if (majEquivalent == 0 || majEquivalent == 3 || majEquivalent == 4){
        res += "maj";
    } else if (majEquivalent == 1 || majEquivalent == 2 || majEquivalent == 5 || majEquivalent == 6){
        res += "min";
    }

    if ((voicing == "7th" || voicing == "7th+" || voicing == "Shell 1" || voicing == "Shell 2") && majEquivalent != 4){
         res += "7"; 
    } else if (voicing == "9th"){
        res += "9";
    } else if (voicing == "11th"){
        res += "11";
    } else if (voicing == "13th"){
        res += "13";
    } else if (voicing == "6th"){
        res += "6";
    }

    if (majEquivalent == 6 && (voicing != "Sus2" && voicing != "Sus4" && voicing != "Power" && voicing != "Shell 1" && voicing != "Shell 2" && voicing != "Triad" && voicing != "Triad+")) {
        res += "b5";
    }

    return res;
}