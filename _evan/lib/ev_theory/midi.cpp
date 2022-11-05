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
#include "daisy_seed.h"
#include "midi.h"

using namespace daisy;
using namespace ev_theory;

namespace ev_theory {

    void MIDISendNoteOn(MidiUartHandler* m, uint8_t channel, uint8_t note, uint8_t velocity) {
        uint8_t data[3] = { 0 };
        data[0] = (channel & 0x0F) + 0x90; // limit channel byte, add status byte
        data[1] = (note + 24) & 0x7F;      // remove MSB on data
        data[2] = velocity & 0x7F;

        m->SendMessage(data, 3);
    }

    void MIDISendNoteOff(MidiUartHandler* m, uint8_t channel, uint8_t note) {
        uint8_t data[3] = { 0 };
        data[0] = (channel & 0x0F) + 0x80;
        data[1] = (note + 24) & 0x7F;
        data[2] = 0x0;

        m->SendMessage(data, 3);
    }

    // void MIDISendAllNotesOff(MidiUartHandler* m, uint8_t channel) {
    //     uint8_t data[3] = { 0 };
    //     data[0] = (channel & 0x0F) + 0xB0;
    //     data[1] = 0x0;
    //     data[2] = 0x0;

    //     m->SendMessage(data, 3);
    // }
} // namespace ev_theory