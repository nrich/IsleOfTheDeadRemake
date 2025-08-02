#ifndef CELTHREE_H
#define CELTHREE_H

#include <cstdint>
#include <array>

#include <raylib-cpp.hpp>

#include "Palette.h"

class CelThree {
    const std::string filename;
    raylib::TextureUnmanaged texture;
public:
    CelThree(const std::string &filename, const Palette &palette);

    const raylib::TextureUnmanaged &getTexture() const {
        return texture;
    }

    ~CelThree();
};

#endif //CELTHREE_H
