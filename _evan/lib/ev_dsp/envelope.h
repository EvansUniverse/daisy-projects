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
 */

#pragma once

#include <string>

#include "daisysp.h"
#include "daisy_patch.h"

using namespace daisy;
using namespace daisysp;

#define DEFAULT_MAX_ATTACK 500
#define DEFAULT_MAX_HOLD 2000
#define DEFAULT_MAX_RELEASE 1000

#define MAX_PARAM_LVL 1000

/* slope-intercept form helpers */

// @return y = mx + b
float getY(float x, float m, float b){
    return m*x+b;
}

// @return slope (m) = (y2-y1)/(x2-x1)
float getM(float x1, float x2, float y1, float y2){
    return (y2-y1)/(x2-x1);
}

// @return intercept (b) = y1 - m * x1
float getB(float x1, float y1, float m){
    return y1-m*x1;
}



namespace ev_dsp {
    enum EnvState {rising, holding, falling, off, tapering_on, tapering_off};

    // The AHR (attack, hold, release) envelope used by Musubi
    //
    // Usage: 
    // - Call tick() every audio block
    // - Call trigger() to trigger a new envelope
    // - Call getLevel() to see where the envelope is at
    class AhrEnvelope {
    public:
        // 0-1000
        uint16_t aContour;
        uint16_t rContour;

        uint16_t attack;
        uint16_t hold;
        uint16_t release;

        uint16_t maxAttack;
        uint16_t maxHold;
        uint16_t maxRelease;

        // How "high" the env currently is
        // min 0, max 1.0
        float level;

        // How "high" the env tapering is
        // min 0, max 1.0
        float taperLevel;
        //float prevTaperLevel;

        // If a new cycle triggers while the envelope running, store the old level value and 
        // stay there until the new envelope exceeds it. This prevents unwanted click noises
        // when envelopes cascade into one another and the sound is abruptly cut.
        bool cascade;

        bool retriggering;

        uint16_t taperOn;
        uint16_t taperOff;

        bool gateOn;

        EnvState state;

        AhrEnvelope(){
            AhrEnvelope(DEFAULT_MAX_ATTACK, DEFAULT_MAX_HOLD, DEFAULT_MAX_RELEASE);
        }

        // @param max attack, in ticks
        // @param max hold, in ticks
        // @param max release, in ticks
        AhrEnvelope(uint16_t a, uint16_t h, uint16_t r){
            maxAttack = a;
            maxHold = h;
            maxRelease = r;
            attack = 0;
            hold = 0;
            release = 0;
            index = 0;
            fullIndex = 0;
            state = off;
            level = 0;
            cascade = false;
            retriggering = false;
            gateOn = false;
            taperOn = 0;
            taperOff = 0;
            setAttackContour(500);
            setReleaseContour(500);

            // param = (coefficient * level)^2
            aCoeff = std::sqrt(maxAttack) / MAX_PARAM_LVL;
            hCoeff = std::sqrt(maxHold) / MAX_PARAM_LVL;
            rCoeff = std::sqrt(maxRelease) / MAX_PARAM_LVL;
        }

        // @param max attack, in ticks
        // @param max hold, in ticks
        // @param max release, in ticks
        void setAhr(uint16_t a, uint16_t h, uint16_t r){ // TODO factor into constructor
            maxAttack = a;
            maxHold = h;
            maxRelease = r;
            attack = 0;
            hold = 0;
            release = 0;

            aCoeff = std::sqrt(maxAttack) / MAX_PARAM_LVL;
            hCoeff = std::sqrt(maxHold) / MAX_PARAM_LVL;
            rCoeff = std::sqrt(maxRelease) / MAX_PARAM_LVL;

            setAttackContour(500);
            setReleaseContour(500);

            setState(off);
        }

        // Trigger a new envelope
        void trigger(){
            if (state != off && cascade) {
                prevLvl = level;
                retriggering = true;
                setState(rising); // TODO consider going to tapering_on instead just for consistent note latency 

            } else if (state != off && !cascade){
                prevLvl = level;
                retriggering = true;
                setState(tapering_on);
                
            } else { // state == off
                retriggering = false;
                setState(tapering_on);
            }
        }

        // Stop all noise and reset the envelope
        void mute(){
            setState(off);
        }

        // Call this every audio block
        void tick(){
            float l;
            switch(state){
            case tapering_on:
                index++;
                if (index > taperOn){
                    taperLevel = 1.0f;
                    setState(rising);
                    break;
                }

                if(!cascade){
                    // Prevents clicks
                    level = 0;
                }

                if(retriggering){
                    // Level will not ever reach a full 0 when retriggering; this prevents clicks
                    taperLevel = 1.f;
                    level = max(.1f, (1.f - ((float) index)/((float) taperOn)) * prevLvl);
                } else {
                    taperLevel = ((float) index)/((float) taperOn);
                    level = taperLevel * 0.05f;
                }

                break;

            case rising:
                index++;
                fullIndex++;
                if (index >= attack){
                    setState(holding);
                    break;
                }

                if ((float) index <= aX[0]) {
                    l = getY(index, aM[0], aB[0]);
                } else if ((float) index <= aX[1]) {
                    l = getY(index, aM[1], aB[1]);
                } else if ((float) index <= aX[2]) {
                    l = getY(index, aM[2], aB[2]);
                } else {
                    l = getY(index, aM[3], aB[3]);
                }

                if (cascade){
                    level = max(l, prevLvl);
                } else if (retriggering){
                    // Level will not ever reach a full 0 when retriggering; this prevents clicks
                    level = max(l, .1f);
                } else if (taperOn > 0) {
                    // If tapering is enabled, we will already be at 0.05f
                    level = max(l, 0.05f);
                } else {
                    level = l;
                }
                break;

            case holding:
                if (index < hold){
                    // Prevent possible integer overflow, if user were to sustain the note
                    // for an arbitrarily long time and index kept incrementing.
                    index++;
                    fullIndex++;
                }

                if (!gateOn && index >= hold){
                    setState(falling);
                    break;
                }

                level = 1.f;
                break;

            case falling:
                index++;
                fullIndex++;
                if (index >= release){
                    setState(tapering_off);
                    break;
                }

                //level = 1.f - ((float) index / (float) release);float l;
                if ((float) index <= rX[0]) {
                    level = getY(index, rM[0], rB[0]);
                } else if ((float) index <= rX[1]) {
                    level = getY(index, rM[1], rB[1]);
                } else if ((float) index <= rX[2]) {
                    level = getY(index, rM[2], rB[2]);
                } else {
                    level = getY(index, rM[3], rB[3]);
                }
                break;
            case tapering_off:
                index++;
                if (index > taperOff){
                    // if(retriggering){
                    //     setState(tapering_on);
                    // } else {
                        setState(off);
                    //}
                    break;
                }
                taperLevel = 1.f - ((float) index)/((float) taperOff); 
                break;
            case off:
                break;
            }
        }

        // Sets attack based on "level" aka knob position
        //
        // @param a value between 0 and 1000
        void setAttack(uint16_t i){
            // attack = (aCoeff * i)^2
            attack = (aCoeff * i) * (aCoeff * i);
            computeAttackContour();
        }

        // Sets hold based on "level" aka knob position
        //
        // @param a value between 0 and 1000
        void setHold(uint16_t i){
            // hold = (hCoeff * i)^2
            hold = (hCoeff * i) * (hCoeff * i);
        }

        // Sets release based on "level" aka knob position
        //
        // @param a value between 0 and 1000
        void setRelease(uint16_t i){
            // release = (rCoeff * i)^2
            release = (rCoeff * i) * (rCoeff * i);
            computeReleaseContour();
        }

        // Sets attack contour based on "level" aka knob position
        //
        // @param a value between 0 and 1000. 
        //        0->499: more->less concave,
        //        500: linear,
        //        501->1000: less->more convex.
        void setAttackContour(uint16_t i){
            aContour = i;
            computeAttackContour();
        }

        // Sets release contour based on "level" aka knob position
        //
        // @param a value between 0 and 1000. 
        //        0->499: more->less concave,
        //        500: linear,
        //        501->1000: less->more convex.
        void setReleaseContour(uint16_t i){
            rContour = i;
            computeReleaseContour();
        }

        // @param the number of samples that the envelope will "taper" on for
        void setTaperOn(uint16_t i){
            taperOn = i;
        }

        // @param the number of samples that the envelope will "taper" off for
        void setTaperOff(uint16_t i){
            taperOff = i;
        }

        // @return 0.0 - 1.0, multiply audio output by this value to prevent clicking
        float getTaperLevel(){
            return taperLevel;
        }

        // When cascade is on, an envelope will not reset itself if another one is triggered while it's still cycling
        void setCascade(bool b){
            cascade = b;
        }

        std::string toString(){ 
            return "A:" + std::to_string(attack) + " H:" + std::to_string(hold) + " R:" + std::to_string(release);
        }

        std::string debugString(){ 
            return std::to_string(state);
        }

        // @return true if the envelope is currently cycling
        bool isCycling(){
            return state != off;
        }

        // @ return level (0.0 - 1.0)
        float getLevel(){
            return level;
        }

        // @param If true, the user is still sustaining the note. Envelope will continue to advance
        // to the end of the "holding" stage, will remain there until the note is released,
        // then proceed to the "falling" stage.
        void setGate(bool b){
            gateOn = b;
        }

    // If I were a better man, I would put "private:" here, but I be directly accessing variables

        // How far along the env segment is in ticks
        uint32_t index;

        // How far along the total envelope is in ticks
        uint32_t fullIndex;

        // Coefficients used for exponential knob behavior
        float aCoeff;
        float hCoeff;
        float rCoeff;

        // Track previous env lvl for cascade behavior
        float prevLvl;

        // variables used for computing curve approximation, we cache them to avoid extra computation
        // a = attack
        // r = release
        // m = slope
        // b = intercept
        // x = x value where that function ends and the next begins
        // array indices 1-4 correspond to linear functions 1-4 (i, m, n, j)
        float aM[4];
        float aB[4];
        float aX[4];

        float rM[4];
        float rB[4];
        float rX[4];

        // @return the x-coordinate of the intersect point of a line in the attack env
        // @param 0-3
        float getAttackX(uint8_t i){
            return aX[i];
        }

        // @return the y-coordinate of the intersect point of a line in the attack env
        // @param 0-3
        float getAttackY(uint8_t i){
            return getY(aX[i], aM[i], aB[i]);
        }

        // @return the x-coordinate of the intersect point of a line in the release env
        // @param 0-3
        float getReleaseX(uint8_t i){
            return rX[i];
        }

        // @return the y-coordinate of the intersect point of a line in the release env
        // @param 0-3
        float getReleaseY(uint8_t i){
            return getY(rX[i], rM[i], rB[i]);
        }

        void computeAttackContour(){
            // To save on computational complexity (and also because I'm a lazy bastard),
            // the contour function actually creates loose approximation of exponential/logarithmic behavior
            // with linear functions. 
            //
            // For example, to approximate the exponential curve of [y=x^2], one could draw 2 lines, from
            // (0, 0) to (.6, .4) and from (.6, .4) to (1, 1).
            //
            // We will do a similar thing here, but using 4 lines:
            //   1. A linear attack envelope segment could be imagined as a line segment from (0,0) to (attack, 1.0), 
            //      or modeled by the function [f(x)=x/attack].
            //   2. Imagine an "inverse" line, [g(x)=attack*-x]
            //   3. The contour amount (0.0 - 1.0) determines where along this inverse line the first 2 lines of our "curve"
            //      will intersect, at the point (contour*attack, g(contour*attack)), which we will refer to as (Ix, Iy)
            //   4. We compute 2 linear functions for our curve:
            //      - i(x), which intersects (0,0) and (Ix, Iy)
            //      - j(x), which intersects (Ix, Iy) and (1,1)
            //   5. We need to "smooth out" our curve, so we will then compute 2 more linear functions:
            //      - m(x), which intersects i(x) at 1/2 of the way to (Ix, Iy) and j(x) at 1/4 of the way to (1,1)
            //      - n(x), which intersects i(x) at 3/4 of the way to (Ix, Iy) and j(x) at 1/2 of the way to (1,1)
            //   6. The envelope segment's "curve" will consist of these 4 functions

            float c = ((float) aContour)/1000.f;
            
            // g(x)
            float gM = 1.f/attack;
            float gB = 1;

            // Intersect point
            float x = c*attack;
            float y = getY(-x, gM, gB);

            // i(x)
            aM[0] = getM(0, x, 0, y);
            aB[0] = 0;
            aX[0] = x*.5f;

            // j(x)
            aM[3] = getM(x, attack, y, 1);
            aB[3] = getB(x, y, aM[3]);
            aX[3] = attack;

            // m(x)
            float x1 = aX[0];
            float y1 = getY(x1, aM[0], aB[0]);
            float x2 = (attack-x)*.25f + x;
            float y2 = getY(x2, aM[3], aB[3]);
            aM[1] = getM(x1, x2, y1, y2);
            aB[1] = getB(x1, y1, aM[1]);

            // n(x)
            x1 = x*.75f;
            y1 = getY(x1, aM[0], aB[0]);
            x2 = (attack-x)*.5f + x;
            y2 = getY(x2, aM[3], aB[3]);
            aM[2] = getM(x1, x2, y1, y2);
            aB[2] = getB(x1, y1, aM[2]);
            aX[2] = x2;

            // m(x) ends where it intersects with n(x)
            // 
            // m(x) = m1*x+b1
            // n(x) = m2*x+b2
            // x = (b2-b1)/(m1-m2)
            if(c == 0.5f){
                // Special case for straight (no) contour where m1 == m2.
                // Set it aX[1] manually to avoid dividing by z0.
                // The value aX[0]+1 was chosen arbitrarily, all 4 lines are the same
                // so it doesn't really matter.
                aX[1] = aX[0]+1;
            } else {
                aX[1] = (aB[2]-aB[1])/(aM[1]-aM[2]);
            }
            
        }

        void computeReleaseContour(){
            // Same as computeAttackContour but f(x) and g(x) are swapped; the curve
            // will start at (0,1) and end at (release,0)
            //
            // See computeAttackContour for detailed comments.

            float c = ((float) rContour)/1000.f;
            
            // g(x)
            float gM = 1.f/release;
            float gB = 0;

            // Intersect point
            float x = c*release;
            float y = getY(x, gM, gB);

            // i(x)
            rM[0] = getM(0, x, 1, y);
            rB[0] = 1;
            rX[0] = x*.5f;

            // j(x)
            rM[3] = getM(x, release, y, 0);
            rB[3] = getB(x, y, rM[3]);
            rX[3] = release;

            // m(x)
            float x1 = rX[0];
            float y1 = getY(x1, rM[0], rB[0]);
            float x2 = (release-x)*.25f + x;
            float y2 = getY(x2, rM[3], rB[3]);
            rM[1] = getM(x1, x2, y1, y2);
            rB[1] = getB(x1, y1, rM[1]);

            // n(x)
            x1 = x*.75f;
            y1 = getY(x1, rM[0], rB[0]);
            x2 = (release-x)*.5f + x;
            y2 = getY(x2, rM[3], rB[3]);
            rM[2] = getM(x1, x2, y1, y2);
            rB[2] = getB(x1, y1, rM[2]);
            rX[2] = x2;

            // m(x) ends where it intersects with n(x)
            if(c == 0.5f){
                // Avoid dividing by 0
                rX[1] = rX[0]+1;
            } else {
                rX[1] = (rB[2]-rB[1])/(rM[1]-rM[2]);
            }
        }

        // Transition to the given state
        void setState(EnvState s){
            index = 0;
            state = s;

            if(s == rising){
                fullIndex = 0;
            } else if (s == off){
                level = 0.f;
                fullIndex = 0;
            }
        }
    };
} // namespace ev_dsp