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

#include <fstream>
#include <cstring>
#include <iostream>
#include <exception>
#include <bit>
#include <algorithm>

#include "MIDI.h"

template<std::integral T>
constexpr T byteswap(T value) noexcept
{
    static_assert(std::has_unique_object_representations_v<T>, 
                  "T may not have padding bits");
    auto value_representation = std::bit_cast<std::array<std::byte, sizeof(T)>>(value);
    std::ranges::reverse(value_representation);
    return std::bit_cast<T>(value_representation);
}

enum {
    NoteOff             = 0x00,
    NoteOn              = 0x01,
    PolyphonicKey       = 0x02,
    Controller          = 0x03,
    InstrumentChange    = 0x04,
    ChannelPressure     = 0x05,
    PitchBend           = 0x06,
    SystemExclusive     = 0x07,
};

MIDI::MIDI(const std::string &filename) {
    std::ifstream fh(filename, std::ios::binary|std::ios::in);

    std::array<char, 4> signature;
    static const std::array<char, 4> header_validator = {'M', 'T', 'h', 'd'};
    static const std::array<char, 4> track_validator = {'M', 'T', 'r', 'k'};

    fh.read(signature.data(), signature.size());

    if (signature != header_validator)
        throw std::domain_error("Invalid MIDI file header");

    uint32_t header_length = 0;
    uint16_t midi_type = 0;
    uint16_t num_tracks = 0;

    fh.read((char *)&header_length, 4);
    fh.read((char *)&midi_type, 2);
    fh.read((char *)&num_tracks, 2);
    fh.read((char *)&ticks, 2);

    if constexpr (std::endian::native == std::endian::little) {
        header_length = byteswap(header_length);
        midi_type = byteswap(midi_type);
        num_tracks = byteswap(num_tracks);
        ticks = byteswap(ticks);
    }

    uint8_t last_event_type = 0x00;

    while (!fh.eof()) {
        if (tracks.size() > num_tracks)
            break;

        std::array<char, 4> signature;
        fh.read(signature.data(), signature.size());

        if (signature != track_validator)
            throw std::domain_error("Invalid MIDI track header");

        uint32_t block_length = 0;
        fh.read((char *)&block_length, 4);
        if constexpr (std::endian::native == std::endian::little) {
            block_length = byteswap(block_length);
        }

        std::vector<uint8_t> block;
        block.resize(block_length);
        fh.read((char *)block.data(), block_length);

        Track track;

        for (size_t i = 0; i < block.size();) {
            uint32_t delay = 0;

            for (int j = 0; j < 4; j++, i++) {
                delay |= block[i] & 0x7F;
                if (block[i] & 0x80) {
                    delay <<= 7;
                } else {
                    i += 1;
                    break;
                }
            }

            uint8_t event_type = block[i];
            if (event_type < 0x80) {
                event_type = last_event_type;
            } else {
                i += 1;
            }

            last_event_type = event_type;

            if (event_type == 0xFF) {
                // meta event
                Event event(delay, event_type, block[i++]);

                uint32_t meta_event_data_length = 0;
                for (int j = 0; j < 4; j++, i++) {
                    meta_event_data_length |= block[i] & 0x7F;
                    if (block[i] & 0x80) {
                        meta_event_data_length <<= 7;
                    } else {
                        i += 1;
                        break;
                    }
                }

                event.data.reserve(meta_event_data_length);
                for (size_t j = 0; j < meta_event_data_length; j++) {
                    event.data.push_back(block[i++]);
                }

                track.events.push_back(event);
            } else {
                uint8_t event_number = (event_type & 0x7F) >> 4;
                uint8_t event_channel = (event_type & 0x0F);

                Event event(delay, event_number, event_channel);

                switch (event_number) {
                    case NoteOff : {
                            event.data.push_back(block[i++]);
                            event.data.push_back(block[i++]);
                        }
                        break;
                    case NoteOn: {
                            event.data.push_back(block[i++]);
                            uint8_t velocity = block[i++];
                            event.data.push_back(velocity);
                            if (velocity == 0) {
                                event.type = NoteOff;
                            }
                        }
                        break;
                    case PolyphonicKey: {
                            event.data.push_back(block[i++]);
                            event.data.push_back(block[i++]);
                        }
                        break;
                    case Controller: {
                            event.data.push_back(block[i++]);
                            event.data.push_back(block[i++]);
                        }
                        break;
                    case InstrumentChange: {
                            event.data.push_back(block[i++]);
                        }
                        break;
                    case ChannelPressure: {
                            event.data.push_back(block[i++]);
                        }
                        break;
                    case PitchBend: {
                            //uint8_t lsb = block[i++];
                            //uint8_t msb = block[i++];

                            //uint16_t pitch = (msb << 7) | lsb;
                            event.data.push_back(block[i++]);
                            event.data.push_back(block[i++]);
                        }
                        break;
                    case SystemExclusive: {
                            uint32_t data_length = 0;

                            for (int j = 0; j < 4; j++, i++) {
                                data_length |= block[i] & 0x7F;
                                if (block[i] & 0x80) {
                                    data_length <<= 7;
                                } else {
                                    i += 1;
                                    break;
                                }
                            }

                            event.data.reserve(data_length);
                            for (size_t j = 0; j < data_length; j++) {
                                event.data.push_back(block[i++]);
                            }
                        }
                        break;
                }

                track.events.push_back(event);
            }
        }
        tracks.push_back(track);
    }
}

MIDI::MIDI(const MIDI &other) {
    tracks = other.tracks; 
    ticks = other.ticks;
}

MIDI::~MIDI() {

}

