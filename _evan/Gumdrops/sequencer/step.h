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

#include "../../lib/ev_theory/theory.h"

using namespace daisy;
using namespace daisysp;
using namespace ev_theory;

        // Maximum values, enforced by setters
static const uint8_t STEP_MAX_NOTE = 11;
static const uint8_t STEP_MAX_OCTAVE = 4;

namespace gumdrops {

    // Step containing note, rhythm, and CV data
    class Step {
    private:
        // True if the step is currently activated
        bool isActive;

        // Microtiming, between -100 and 100
        int8_t uTime;

        // The duration, in (1/RESOLUTION)ths of the parent Seq object, the step is played
        uint16_t length;

        // If true, this step is untouched. Lets the user know if they need to implement
        // any "populate a new step with certain defaults" behavior.
        bool isNew;

        uint8_t octave;

        uint16_t cv2;

        bool slide;

        // Micro-timing (u as in μ)
        // Offsets (in ticks) the time that the step is triggered
        //
        // TODO for some wierd reason, this line causes a runtime crash but the above 
        // declaration of uTime doesn't
        //int8_t uTime;
        
        // DAC values cached so that they only have to be computed upon changing
        uint16_t dac1;
        uint16_t dac2;

        void updateDac1(){
            dac1 = semitoneToDac(note + (12 * octave));
        };
        void updateDac2(){
            // (max DAC value of 4095)/(max voltage value of 5) = 819
            dac2 = (uint16_t)std::round(819 * cv2);
        };

    public:
        Step(){};

        // Semitone value 0-11, valid index of theory.h::allNotes
        int16_t note;


        // Sets reasonable default values for everything not specified in the params.
        //
        // @param isActive
        // @param length
        // @param note
        // @param octave
        Step(bool b, uint16_t l, uint16_t n, uint8_t o){
            setIsActive(b);
            setLength(l);
            setNote(n);
            setOctave(o);
            setCV2(0.f);
            setUTime(0);
            setSlide(false);
        };

        //Step* clone() const { return new Step (*this); }; 

        // Convenience function
        void toggleIsActive(){ isActive = !isActive; };

        /* Setters */

        void setIsActive(bool b){   isActive = b; };
        void setIsNew(bool b){      isNew = b;    };
        void setLength(uint16_t i){ length = i;   };
        void setDac1(uint16_t i){   dac1 = i;     };
        void setDac2(uint16_t i){   dac2 = i;     };
        void setUTime(int8_t i){    uTime = i;    };
        void setSlide(bool b){      slide = b;    };

        // @param semitone value 0-11
        void setNote(int16_t i){
            note = i;// % (STEP_MAX_NOTE + 1);
            updateDac1();
        };

        // @param octave 0-4
        void setOctave(uint8_t i){
            octave = std::min(i, STEP_MAX_OCTAVE);
            updateDac1();
        };

        // @param voltage value of cv2 0.0-5.0
        void setCV2(float f){
            cv2 = std::min(f, 5.f);
            cv2 = std::max(f, 0.f);
            updateDac2();
        };

        /* Getters */

        bool     getIsActive(){ return isActive; };
        bool     getIsNew(){    return isNew;    };
        uint16_t getLength(){   return length;   };
        int16_t  getNote(){     return note;     }; 
        uint8_t  getOctave(){   return octave;   };
        float    getCV2(){      return cv2;      };
        uint16_t getDac1(){     return dac1;     };
        uint16_t getDac2(){     return dac2;     };
        int8_t   getUTime(){    return uTime;    };
        bool     getSlide(){    return slide;    };

        std::string toString(){
            return std::to_string(isActive) + ", "  + std::to_string(length)
                + ", " + std::to_string(note) + ", " + std::to_string(octave) 
                + ", " + std::to_string(cv2);
        }
    };
} // namespace gumdrops