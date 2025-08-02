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

#ifndef MIDI_H
#define MIDI_H

#include <cstdint>
#include <string>
#include <vector>
#include <array>

struct Event {
    uint32_t delay;
    uint8_t type;
    uint8_t channel;
    std::vector<uint8_t> data;

    Event(uint32_t delay, uint8_t type, uint8_t channel) : delay(delay), type(type), channel(channel) {
    }

    Event(const Event &other) {
        delay = other.delay;
        type = other.type;
        channel = other.channel;
        data = other.data;
    }
};

struct Track {
    std::vector<Event> events;

    Track() {
    }

    Track(const Track &other) {
        events = other.events;
    }
};

class MIDI {
    uint16_t ticks;
    std::vector<Track> tracks;
public:
    MIDI(const std::string &filename);
    MIDI(const MIDI &other);

    const std::vector<Track> &getTracks() const {
        return tracks;
    }
    const uint16_t getTicks() const {
        return ticks;
    }
    ~MIDI();
};

#endif //MIDI_H
