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
