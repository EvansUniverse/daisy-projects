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
 * General purpose MIDI library designed to work with ev_theory
 *
 * TODO: This library was created because LibDaisy currently does not support 
 *       MIDI out. They have plans to implement it in the future, see here:
 *       https://github.com/electro-smith/libDaisy/issues/486
 *       When this is implemented in LibDaisy, this library should either
 *       be removed or refactored to use the new LibDaisy functions
 * 
 * TODO: Consider moving this somewhere out of ev_theory
 */

#pragma once

#include "daisy_seed.h"
using namespace daisy;

namespace ev_theory {

    // Shoutout to MakingSoundMachines for this function :)
    // https://forum.electro-smith.com/t/how-to-use-libdaisy-for-midi-out/2346/8?u=evan
    //
    // @param daisy's midi handler
    // @param midi channel (0-15)
    // @param midi note (0-127)
    // @param velocity (0-127)
    void MIDISendNoteOn(MidiUartHandler*, uint8_t, uint8_t, uint8_t);

    // @param daisy's midi handler
    // @param midi channel (0-15)
    // @param midi note (0-127)
    void MIDISendNoteOff(MidiUartHandler*, uint8_t, uint8_t);
    
    // // @param daisy's midi handler
    // // @param midi channel (0-15)
    // void MIDISendAllNotesOff(MidiUartHandler* m, uint8_t channel) {
    //     uint8_t data[3] = { 0 };
    //     data[0] = (channel & 0x0F) + 0xB0;
    //     data[1] = 0x0;
    //     data[2] = 0x0;

    //     m->SendMessage(data, 3);
    // }
} // namespace ev_theory