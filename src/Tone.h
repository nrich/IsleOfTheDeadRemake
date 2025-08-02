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

#ifndef TONE_H
#define TONE_H

#include <cstdint>
#include <deque>
#include <vector>

const int FREQUENCY = 44100;

enum class WaveForm : uint8_t {
    Sine,
    Sawtooth,
    Triangle,
    Square,
    Noise,
    PinkNoise,
    Silence,
};

struct ToneObject {
    double freq;
    size_t samplesLeft;
    std::vector<WaveForm> waveForms;
    float volume;

    int attackLeft;
    float attackStep;

    int decayLeft;
    float decayStep;

    int sustainLeft;
    float sustainLevel;

    int releaseLeft;
    float releaseStep;
};

class Tone {
    double v;
    std::deque<ToneObject> tones;
public:
    Tone();
    ~Tone() {}
    void tone(float freq, uint32_t duration, WaveForm wave_form, uint8_t volume, uint8_t attack, uint8_t decay, uint8_t sustain, uint8_t release);
    void multi(float freq, uint32_t duration, std::vector<WaveForm> wave_forms, uint8_t volume, uint8_t attack, uint8_t decay, uint8_t sustain, uint8_t release);
    size_t generateSamples(float *stream, size_t length, float amplitude=1.0f);
    size_t size() const;
};

#endif //TONE_H
