#include <thread>

#include <cmath>
#include <random>
#include <array>
#include <iostream>

#include "Tone.h"

#define PINK_MAX_RANDOM_ROWS   (30)
#define PINK_RANDOM_BITS       (24)
#define PINK_RANDOM_SHIFT      ((sizeof(int32_t)*8)-PINK_RANDOM_BITS)

#ifndef M_PI
#define M_PI            3.14159265358979323846
#endif

static double fRand(double fMin, double fMax) {
    double f = (double)rand() / RAND_MAX;
    return fMin + f * (fMax - fMin);
}

class PinkNoise {
    std::array<int32_t, PINK_MAX_RANDOM_ROWS> rows;
    int32_t runningSum;     // Used to optimize summing of generators
    int32_t index;          // Incremented each sample
    int32_t indexMask;      // Index wrapped by ANDing with this mask
    float scalar;           // Used to scale within range of -1.0 to +1.0
    uint32_t seed;

    uint32_t generateRandomNumber() {
        static uint32_t randSeed = seed;
        randSeed = (randSeed * 196314165) + 907633515;
        return randSeed;
    }
public:
    PinkNoise(int numRows) {
        numRows = std::min(PINK_MAX_RANDOM_ROWS, numRows);

        seed = std::chrono::system_clock::now().time_since_epoch().count();
        index = 0;
        indexMask = (1<<numRows) - 1;

        // Calculate maximum possible signed random value. Extra 1 for white noise always added.
        int32_t pmax = (numRows + 1) * (1<<(PINK_RANDOM_BITS-1));
        scalar = 1.0f / pmax;

        rows.fill(0);
        runningSum = 0;
    }

    float generate() {
        int32_t new_random;
        int32_t sum;
        float output;

        // Increment and mask index
        index = (index + 1) & indexMask;

        // If index is zero, don't update any random values.
        if (index != 0) {
            // Determine how many trailing zeros in PinkIndex.
            // This algorithm will hang if n==0 so test first.
            int numZeros = 0;
            int n = index;
            while((n & 1) == 0) {
                n = n >> 1;
                numZeros++;
            }
            // Replace the indexed ROWS random value.
            // Subtract and add back to RunningSum instead of adding all the random
            // values together. Only one changes each time.
            runningSum -= rows[numZeros];
            new_random = ((int32_t)generateRandomNumber()) >> PINK_RANDOM_SHIFT;
            runningSum += new_random;
            rows[numZeros] = new_random;
        }

        // Add extra white noise value.
        new_random = ((int32_t)generateRandomNumber()) >> PINK_RANDOM_SHIFT;
        sum = runningSum + new_random;
        // Scale to range of -1.0 to 0.9999.
        output = scalar * sum;

        return output;
    }
};

Tone::Tone() {
    v = 0.0;
}

void Tone::multi(float freq, uint32_t duration, std::vector<WaveForm> wave_forms, uint8_t volume, uint8_t attack, uint8_t decay, uint8_t sustain, uint8_t release) {
    ToneObject to;
    to.freq = (double)freq;
    uint64_t long_duration = (uint64_t)duration;

    to.samplesLeft = long_duration * FREQUENCY / 1000;
    to.waveForms = wave_forms;
    to.volume = (float)volume/(float)UINT8_MAX;

    const int Scale = to.samplesLeft / (10 * FREQUENCY / 1000);

    to.sustainLevel = (float)sustain/(float)UINT8_MAX;

    to.attackLeft = attack*Scale;
    to.attackStep = ((float)UINT8_MAX/(float)attack)/UINT8_MAX/Scale;

    if (sustain == 255) {
        to.decayLeft = 0;
    } else {
        to.decayLeft = decay*Scale;
        to.decayStep = (1.0-to.sustainLevel) * ((float)(UINT8_MAX)/(float)decay)/UINT8_MAX/Scale;
    }

    to.releaseLeft = release * Scale;
    to.releaseStep = to.sustainLevel * ((float)(UINT8_MAX)/(float)release)/UINT8_MAX/Scale;

    to.sustainLeft = to.samplesLeft - to.attackLeft - to.decayLeft - to.releaseLeft;

    tones.push_back(to);
}

void Tone::tone(float freq, uint32_t duration, WaveForm wave_form, uint8_t volume, uint8_t attack, uint8_t decay, uint8_t sustain, uint8_t release) {
    multi(freq, duration, {wave_form}, volume, attack, decay, sustain, release);
}


size_t Tone::generateSamples(float *stream, size_t length, float amplitude) {
    static PinkNoise pinknoise(16);
    size_t i = 0;

    std::array<float, 32> noise;

    for (size_t i = 0; i < noise.size(); i++) {
        noise[i] = fRand(-1.0, 1);
    }

    if (tones.empty()) {
        return 0;
    }

    while (i < length) {
        if (tones.empty()) {
            while (i < length) {
                stream[i] += 0;
                i++;
            }
            return false;
        }
        ToneObject& to = tones.front();

        size_t samples_todo = std::min(i + to.samplesLeft, length);
        to.samplesLeft -= samples_todo - i;

        while (i < samples_todo) {
            float attack = 1.0f;
            float decay = 1.0f;
            float sustain = 1.0f;
            float release = 1.0f;

            if (to.attackLeft > 0) {
                attack -= (to.attackLeft * to.attackStep);
                to.attackLeft--;
            } else if (to.decayLeft > 0) {
                decay = (to.decayLeft * to.decayStep) + to.sustainLevel;
                to.decayLeft--;
            } else if (to.sustainLeft > 0) {
                sustain = to.sustainLevel;
                to.sustainLeft--;
            } else if (to.releaseLeft > 0) {
                release = (to.releaseLeft * to.releaseStep);
                to.releaseLeft--;
            }

            float pos = std::fmod(v/FREQUENCY,1.0);
            v += to.freq;

            for (const auto wave_form : to.waveForms) {
                switch (wave_form) {
                    case WaveForm::Sawtooth:
                        stream[i] += attack * decay * sustain * release * to.volume * amplitude * (pos*2 - 1);
                        break;
                    case WaveForm::Triangle:
                        stream[i] += attack * decay * sustain * release * to.volume * amplitude * (1-std::fabs(pos-0.5)*4);
                        break;
                    case WaveForm::Square:
                        stream[i] += attack * decay * sustain * release * to.volume * amplitude * (std::sin(pos*2*M_PI) >= 0 ? 1.0 : -1.0);
                        break;
                    case WaveForm::Noise:
                        stream[i] += attack * decay * sustain * release * to.volume * amplitude * (noise[(int)(std::fmod(v/FREQUENCY,noise.size()))]);
                        break;
                    case WaveForm::PinkNoise:
                        stream[i] += attack * decay * sustain * release * to.volume * amplitude * (pinknoise.generate());
                        break;
                    case WaveForm::Silence:
                        stream[i] += 0.0f;
                        break;
                    case WaveForm::Sine:
                    default:
                        stream[i] += attack * decay * sustain * release * to.volume * amplitude * (std::sin(pos*2*M_PI));
                }
            }
            i++;
        }

        if (to.samplesLeft == 0) {
            tones.pop_front();
        }
    }

    return size();
}

size_t Tone::size() const {
    size_t length = 0;

    for (const auto &tone : tones) {
        length += tone.samplesLeft;
    }

    return length;
}

