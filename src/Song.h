/******************************************************************************

Copyright (C) 2025 Neil Richardson (nrich@neiltopia.com)

This program is free software: you can redistribute it and/or modify it under
the terms of the GNU General Public License as published by the Free Software
Foundation, version 3.

This program is distributed in the hope that it will be useful, but WITHOUT
ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more
details.

You should have received a copy of the GNU General Public License along with
this program. If not, see <https://www.gnu.org/licenses/>.

******************************************************************************/

#ifndef SONG_H
#define SONG_H

#include <cstdint>
#include <string>
#include <map>
#include <memory>
#include <array>

#include <raylib-cpp.hpp>

#include "MIDI.h"
#include "Tone.h"

class Song {
    enum class Instrument {
        Default,
        Bass,
        Guitar,
        SynthBrass,
        Choir,
        MuteGuitar,
        LeadGuitar,
        Strings,
    };

    std::array<std::map<uint8_t, Tone>, 16> channels;
    const MIDI midi;
public:
    Song(const MIDI &midi);

    void generateBuffer(void *buffer, uint32_t frames);
    void generateTones();

    ~Song();
};

#endif //SONG_H
