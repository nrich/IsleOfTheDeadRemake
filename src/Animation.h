#ifndef TONE_H
#define TONE_H

#include <cstdint>
#include <deque>
#include <optional>

#include <raylib-cpp.hpp>

#include "Flic.h"

class Animation {
    size_t frame = 0;

    Flic flic;
    std::optional<raylib::Sound> sound;
public:
    Animation(const std::string &flic_filename, const std::string &sound_filename);
    Animation(const std::string &flic_filename);

    bool play(const int scale);

    ~Animation();
};

#endif //TONE_H
