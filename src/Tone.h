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
