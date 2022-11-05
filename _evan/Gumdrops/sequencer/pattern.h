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
#include "step.h"

using namespace daisy;
using namespace daisysp;

// Ticks per bar
const static uint16_t RESOLUTION = 256;

// Total number of steps a pattern can hold.
static const uint8_t PATTERN_MAX_SIZE = 128;

// Default step values
static const bool     DEFAULT_ISACTIVE = false;
static const uint16_t DEFAULT_LENGTH   = 15;
static const int16_t  DEFAULT_NOTE     = 0;
static const uint8_t  DEFAULT_OCT      = 0;

namespace gumdrops {
    // A pattern of steps. Holds a set of metadata and an array of steps.
    // It's designed this way to be writeable to persistent storage.
    class Pattern {
    protected:
        // Index of the final step in the current pattern
        uint8_t maxSteps;

        // Index of the current step
        uint8_t index;

        Step steps[PATTERN_MAX_SIZE];

        // If true, this pattern is new and has not been initialized yet
        bool isNew;

        // The number of 1/RESOLUTIONths that it takes for a step to advance
        // TODO we'll encounter bugs when switching this value mid-run, i'll have to
        // implement some sort of "round when division changes" function when I want that to work.
        uint16_t ticksPerStep;
    public:
        // Don't use this
        Pattern(){};

        // @param maxSteps
        // @param ticksPerStep
        Pattern(uint8_t theMaxSteps, uint16_t theTPS){
            maxSteps = theMaxSteps;
            ticksPerStep = theTPS;
            index = 0;
            isNew = true;

            // Populate steps
            for(uint8_t i = 0; i < PATTERN_MAX_SIZE; i++){
                steps[i] = *(new Step(DEFAULT_ISACTIVE, DEFAULT_LENGTH, DEFAULT_NOTE, DEFAULT_OCT));
            }
        }

        // Advance to the next step
        void advance(){
            index++;
            index = index % maxSteps;
        };

        // Reset the index
        void restart(){ index = 0; };

        /* Setters */

        void setIsNew(bool b){ isNew = b; };
        void setTicksPerStep(uint16_t i){ ticksPerStep = i; };
        void setIndex(uint8_t i){ index = i; };

        void setMaxSteps(uint8_t i){
            maxSteps = i;
            //index = index % maxSteps;
        };

        /* Getters */
        
        Step* getCurStep(){ return &steps[index]; };
        Step* getNextStep(){ return getStepAt((index+1) % maxSteps); };
        Step* getStepAt(uint8_t i){ return &steps[i]; };
        uint8_t getIndex(){ return index; };
        uint8_t getMaxSteps(){ return maxSteps; };
        bool getIsNew(){ return isNew; };
        uint16_t getTicksPerStep(){ return ticksPerStep; };
    };
} // namespace gumdrops