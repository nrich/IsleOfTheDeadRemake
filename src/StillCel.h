#ifndef STILLCEL_H
#define STILLCEL_H

#include <cstdint>
#include <array>

#include <raylib-cpp.hpp>

#include "Palette.h"

class StillCel {
    const std::string filename;
    raylib::TextureUnmanaged texture;

    uint16_t width;
    uint16_t height;
public:
    StillCel(const std::string &filename);

    const raylib::TextureUnmanaged &getTexture() const {
        return texture;
    }

    ~StillCel();
};

#endif //STILLCEL_H
