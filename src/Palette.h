#ifndef PALETTE_H
#define PALETTE_H

#include <cstdint>
#include <array>
#include <exception>
#include <string>

#include <raylib-cpp.hpp>

class Palette {
    std::array<raylib::Color, 256> colours;
public:
    Palette(const std::string &filename);
    Palette(const std::array<uint8_t, 768> &data);
    Palette(std::ifstream &fh);

    const raylib::Color operator[](const int index) const {
        if (index < 0 || index > 255)
            throw std::out_of_range(std::string("Index outside 0 <= ") + std::to_string(index) + " <= 255");
        return colours[index];
    }

    ~Palette();
};

#endif //PALETTE_H
