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

#include "pattern.h"

#include <string>
#include <random>
#include <algorithm>

using namespace daisy;
using namespace daisysp;

namespace gumdrops {

    // "Plays" a pattern. Designed to be stored in RAM only.
    class Sequencer {
    private:
        // The number of "ticks" per bar
        const static uint16_t RESOLUTION = 256;

        // The active pattern
        Pattern* pattern;

        // The pattern that will be used once the bar completes
        Pattern* cuedPattern;

        // The active step that was most recently played
        Step* activeStep;

        // Percentage 0.0-1.0 of random microtiming that is applied to each step
        float humanize;

        std::string string;

        // Store uTime of current and next step for humanize/swing purposes
        int8_t curUTime;
        int8_t nextUTime;

        // How many ticks the currently input note has been playing for
        uint16_t noteRecordTicks;

        // The previous note received
        uint8_t prevNote;

        bool quantizeRec;

    public:
        // The step currently being recorded
        Step* recStep;

        // Remaining ticks that activeStep will be played
        int16_t remainingLen;

        // Tick index within the bar, up to 256
        uint16_t tickIndex;

        Sequencer(){
            string = "";
            tickIndex = 0;
            cuedPattern = NULL;
            activeStep = NULL;
            remainingLen = 0;
            recStep = NULL;
            prevNote = 255;
            noteRecordTicks = 0;
            quantizeRec = true;
        };

        // Start back at the beginning of the pattern
        void restart(){
            pattern->restart();
            tickIndex = 0;
        };

        // Call this every time the timer ticks (every 1/RESOLUTIONth).
        //
        // Indended to be used in tandem with ev_theory.tempo, as both have the same resolution of 256 and this will keep
        // them in sync.
        //
        // @param the current tick from ev_theory.tempo
        void tick(uint16_t curTick){
            tickIndex = curTick;
            uint16_t stepIndex = tickIndex % pattern->getTicksPerStep();
    
            if(noteRecordTicks > 0){
                noteRecordTicks++;
            }

            if (stepIndex == 0){
                pattern->advance();
                
                if(cuedPattern && pattern->getIndex() == 0 && tickIndex == 0){ 
                    // Pattern switch
                    // TODO add different pattern change behavior options e.g. instant
                    pattern = cuedPattern;
                    cuedPattern = NULL;
                    pattern->setIndex(0);
                }

                // // Set humanization
                // int8_t r = (rand() % 200 + 1) - 100; // Random coefficient -100 to 100
                // int t = pattern->getNextStep()->getUTime() + r * humanize;
                // t = std::min(t, 100);
                // t = std::max(t, -100);
                // curUTime = nextUTime;
                // nextUTime = t;
            }
    
            // TODO could calculate the uTime vars only when step advances to save on computations.

            // TODO temporary naive way
            if(pattern->getCurStep()->getIsActive() && stepIndex == 0) {
                activeStep = pattern->getCurStep();
                remainingLen = activeStep->getLength() - 1; // subtract 1 so that notes don't portamento unless they're over-length
            } else {
                remainingLen--;
            }

            // // (TOTAL DIVISION) * (PERCENTAGE UTIME) * (1/2 to limit uTime to half a step length)
            // int16_t uTime = pattern->getTicksPerStep() * (0.01 * curUTime) * 0.5;

            // // (TOTAL DIVISION) * (PERCENTAGE UTIME) * (1/2 to limit uTime to half a step length)
            // int16_t uTimeNext = pattern->getTicksPerStep() * (0.01 * nextUTime) * 0.5;

            // if(pattern->getNextStep()->getIsActive() && uTimeNext < 0 && stepIndex - uTimeNext == pattern->getTicksPerStep()){
            //     // The next step has negative microtiming and has started playing
            //     activeStep = pattern->getNextStep();
            //     remainingLen = activeStep->getLength() - 1; // subtract 1 so that notes don't portamento unless they're over-length

            // } else if(pattern->getCurStep()->getIsActive() && uTime == stepIndex){
            //     // The current step has started playing
            //     activeStep = pattern->getCurStep();
            //     remainingLen = activeStep->getLength() - 1; // subtract 1 so that notes don't portamento unless they're over-length
                
            // } else if(remainingLen > 0){
            //     // No new step. Decrement tick counter.
            //     // if(){ 
            //     //     // If next note will play next frame, turn off gate. TODO
            //     //     // This ensures each note gets a separate trig.
            //     //     remainingLen = 0
            //     // } else {
            //         remainingLen--;
            //     // }
            // }
        };

        /* Updaters */

        void updateString(){ 
            string = "i: " + std::to_string(tickIndex);
        };

        // Call this when a gate in signal change has been received
        // @param note value 0-59 (if this update is ending a current note, this value won't matter)
        void updateRec(int16_t noteIdx){
           // bool portamento = noteIdx != prevNote;

            if(noteRecordTicks > 0){// || portamento) {
                // Note just ended
                if(quantizeRec){
                    // Quantize end of note
                    noteRecordTicks -= (noteRecordTicks % pattern->getTicksPerStep()); 

                    if(tickIndex > pattern->getTicksPerStep()/2){
                        // Note occurs more than halfway through the step
                        noteRecordTicks += pattern->getTicksPerStep();
                    }

                    // Enforce minimum length of 1 step
                    noteRecordTicks = std::max(pattern->getTicksPerStep(), noteRecordTicks);
                }

                recStep->setLength(noteRecordTicks);
                noteRecordTicks = 0;

            } else if(noteRecordTicks == 0){// || portamento) { 
                // Note just started
                noteRecordTicks = 1;

                recStep = pattern->getCurStep();

                // TODO re-add

                // if(tickIndex > pattern->getTicksPerStep()/2){
                //     // Note occurs more than halfway through the step; consider it part of the next step
                //     recStep = pattern->getNextStep();
                //     recStep->setUTime(static_cast<int8_t>(-100 * 
                //         (pattern->getTicksPerStep() - tickIndex) / static_cast<float>(pattern->getTicksPerStep())));

                // } else {
                //     // Note occurs less than halfway through the step; consider it part of the current step
                //     recStep = pattern->getCurStep();
                //     recStep->setUTime(static_cast<int8_t>(100 * 
                //         (tickIndex) / static_cast<float>(pattern->getTicksPerStep())));
                // }

                //recStep->setNote(noteIdx % 12);
                recStep->note = noteIdx;// % 12;
                recStep->setOctave(noteIdx / 12);
                recStep->setIsActive(true);

                if(quantizeRec){
                    recStep->setUTime(0);
                }
            }
        }

        // Call this when recording has stopped
        void stopRec(){
            if(noteRecordTicks > 0){
                updateRec(0);
            }
        };

        /* Setters */

        // The given pattern will start playing after the current pattern finishes
        // @param pattern
        void cuePattern(Pattern* p){
            cuedPattern = p;
            p->setIsNew(false);
        };
        
        // The given pattern will start playing immediately
        // @param pattern
        void setPattern(Pattern* p){
            pattern = p;
            p->setIsNew(false);
        };

        // @param percentage 0.0-1.0
        void setHumanize(float f){
            humanize = f;
        };

        // @param if true, quantize recorded notes
        void setQuantizeRec(bool b){
            quantizeRec = b;
        };

        /* Getters */

        Pattern* getPattern(){ return pattern; };

        std::string toString(){
            updateString();
            return string;
        };

        // @return true if there is a note actively playing during the current tick
        bool isPlaying(){ return remainingLen > 0; };

        Step* getCurStep(){ return activeStep; }

        bool hasCuedPattern(){
            return cuedPattern != NULL;
        }
    };

    // TODO move this to theory
    const std::vector<std::string> offOrOn {
        "Off",
        "On",
    };

} // namespace gumdrops