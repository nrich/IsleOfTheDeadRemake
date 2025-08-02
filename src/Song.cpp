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

#include <iostream>
#include <stack>
#include <unordered_map>

#include "Song.h"

const static std::array<float, 128> frequencies {
    8.18f,
    8.66f,
    9.18f,
    9.72f,
    10.30f,
    10.91f,
    11.56f,
    12.25f,
    12.98f,
    13.75f,
    14.57f,
    15.43f,
    16.35f,
    17.32f,
    18.35f,
    19.45f,
    20.60f,
    21.83f,
    23.12f,
    24.50f,
    25.96f,
    27.50f,
    29.14f,
    30.87f,
    32.70f,
    34.65f,
    36.71f,
    38.89f,
    41.20f,
    43.65f,
    46.25f,
    49.00f,
    51.91f,
    55.00f,
    58.27f,
    61.74f,
    65.41f,
    69.30f,
    73.42f,
    77.78f,
    82.41f,
    87.31f,
    92.50f,
    98.00f,
    103.83f,
    110.00f,
    116.54f,
    123.47f,
    130.81f,
    138.59f,
    146.83f,
    155.56f,
    164.81f,
    174.61f,
    185.00f,
    196.00f,
    207.65f,
    220.00f,
    233.08f,
    246.94f,
    261.63f,
    277.18f,
    293.66f,
    311.13f,
    329.63f,
    349.23f,
    369.99f,
    392.00f,
    415.30f,
    440.00f,
    466.16f,
    493.88f,
    523.25f,
    554.37f,
    587.33f,
    622.25f,
    659.26f,
    698.46f,
    739.99f,
    783.99f,
    830.61f,
    880.00f,
    932.33f,
    987.77f,
    1046.50f,
    1108.73f,
    1174.66f,
    1244.51f,
    1318.51f,
    1396.91f,
    1479.98f,
    1567.98f,
    1661.22f,
    1760.00f,
    1864.66f,
    1975.53f,
    2093.00f,
    2217.46f,
    2349.32f,
    2489.02f,
    2637.02f,
    2793.83f,
    2959.96f,
    3135.96f,
    3322.44f,
    3520.00f,
    3729.31f,
    3951.07f,
    4186.01f,
    4434.92f,
    4698.64f,
    4978.03f,
    5274.04f,
    5587.65f,
    5919.91f,
    6271.93f,
    6644.88f,
    7040.00f,
    7458.62f,
    7902.13f,
    8372.02f,
    8869.84f,
    9397.27f,
    9956.06f,
    10548.08f,
    11175.30f,
    11839.82f,
    12543.85f,
};

Song::Song(const MIDI &midi) : midi(midi) {
    generateTones();
}

void Song::generateBuffer(void *buffer, uint32_t frames) {
    size_t remaining = 0;

    for (auto &channel : channels) {
        for (auto &[note, tone] : channel) {
            remaining += tone.generateSamples((float *)buffer, frames * 2, 1.0);
        }
    }

    if (!remaining) {
        generateTones();
    }
}

void Song::generateTones() {
    for (auto &channel : channels) {
        channel.clear();
    }

    uint16_t ticks = midi.getTicks();

    float us_per_beat = 500000.0f;
    float tick = us_per_beat / ticks;

    uint64_t current_tick = 0;
    uint32_t current_delay = 0;

    std::array<std::unordered_map<uint8_t, std::vector<std::tuple<uint32_t, uint32_t, uint8_t, Instrument>>>, 16> voices;
    std::array<std::array<std::stack<std::pair<uint32_t, uint8_t>>, 128>, 16> note_offsets;

    Instrument current_instrument = Instrument::Default;

    for (const auto &track : midi.getTracks()) {
        current_tick = 0;
        current_delay = 0;

        for (const auto &event : track.events) {
            switch (event.type) {
                case 0x00: {
                        uint32_t delay = tick * event.delay;
                        uint8_t note = event.data[0];

                        current_tick += delay;
                        current_delay += event.delay;

                        if (note_offsets[event.channel][note].empty())
                            throw std::runtime_error("Note " + std::to_string((int)note) + " not started");

                        auto [delay_start, velocity] = note_offsets[event.channel][note].top();
                        note_offsets[event.channel][note].pop();

                        voices[event.channel][note].push_back(std::make_tuple(delay_start, current_delay, velocity, current_instrument));
                    }
                    break;
                case 0x01: {
                        uint32_t delay = tick * event.delay;

                        uint8_t note = event.data[0];
                        uint8_t velocity = event.data[1];

                        current_tick += delay;
                        current_delay += event.delay;

                        if (!note_offsets[event.channel][note].empty())
                            throw std::runtime_error("Note " + std::to_string((int)note) + " already started");

                        note_offsets[event.channel][note].push(std::make_pair(current_delay, velocity));
                    }
                    break;
                case 0xFF:
                    if (event.channel == 0x51) {
                        uint32_t new_tempo = event.data[2] | (event.data[1] << 8) | (event.data[0] << 16);

                        us_per_beat = new_tempo;
                        tick = us_per_beat / ticks;
                    } else if (event.channel == 0x03 || event.channel == 0x04) {
                        std::string instrument = "";

                        for (const auto c : event.data) {
                            instrument.push_back(c);
                        }

                        if (instrument == "synth brass") {
                            current_instrument = Instrument::SynthBrass;
                        } else if (instrument == "bass") {
                            current_instrument = Instrument::Bass;
                        } else if (instrument == "guitar") {
                            current_instrument = Instrument::Guitar;
                        } else if (instrument == "choir") {
                            current_instrument = Instrument::Choir;
                        } else if (instrument == "mute guitar") {
                            current_instrument = Instrument::MuteGuitar;
                        } else if (instrument == "lead guitar") {
                            current_instrument = Instrument::LeadGuitar;
                        } else if (instrument == "strings") {
                            current_instrument = Instrument::Strings;
                        } else {
                            std::cerr << "Unknown instrument " << instrument << "\n";
                            current_instrument = Instrument::Default;
                        }
                    }
                    break;
                default:
                    break;
            }
        }
    }

    for (size_t i = 0; i < 16; i++) {
        for (const auto &[note, notes] : voices[i]) {
            uint32_t previous = 0;
            float freqency = frequencies[note];

            for (const auto &[start, stop, velocity, instrument] : notes) {
                uint32_t silence_ticks = start - previous;
                uint32_t silence_delay = tick * silence_ticks;

                channels[i][note].tone(freqency, silence_delay/500, WaveForm::Sine, 0, 0, 0, 0, 0);

                uint32_t play_ticks = stop - start;
                uint32_t play_delay = tick * play_ticks;

                switch (instrument) {
                    case Instrument::Choir:
                        channels[i][note].multi(freqency, play_delay/500, {WaveForm::Sine}, velocity, 80, 60, 40, 60);
                        break;
                    case Instrument::SynthBrass:
                        channels[i][note].multi(freqency, play_delay/500, {WaveForm::Square}, velocity, 10, 5, 10, 5);
                        break;
                    default:
                        channels[i][note].multi(freqency, play_delay/500, {WaveForm::Triangle, WaveForm::Square}, velocity, 40, 20, 128, 40);
                        break;
                }

                previous = stop;
            }
        }
    }
}

Song::~Song() {

}

