#include <filesystem>
#include <fstream>
#include <exception>
#include <iostream>

#include "Palette.h"

Palette::Palette(const std::string &filename) {
    std::filesystem::path filepath = filename;

    if (std::filesystem::file_size(filepath) != 768) {
        throw std::invalid_argument(filename + " does not look like a VGA palette file"); 
    }

    std::ifstream fh(filename, std::ios::binary|std::ios::in);

    for (size_t i = 0; i < 256; i += 1) {
        uint8_t r, g, b, a;

        fh.read((char *)&r, 1);
        fh.read((char *)&g, 1);
        fh.read((char *)&b, 1);

        if (i == 1) {
            a = 0x00;
        } else {
            a = 0xFF;
        }

        colours[i] = raylib::Color((r * 255) / 63, (g * 255) / 63, (b * 255) / 63, a);
    }
}

Palette::Palette(std::ifstream &fh) {
    for (size_t i = 0; i < 256; i += 1) {
        uint8_t r, g, b, a;

        fh.read((char *)&r, 1);
        fh.read((char *)&g, 1);
        fh.read((char *)&b, 1);

        if (i == 1) {
            a = 0x00;
        } else {
            a = 0xFF;
        }

        colours[i] = raylib::Color((r * 255) / 63, (g * 255) / 63, (b * 255) / 63, a);
    }
}

Palette::Palette(const std::array<uint8_t, 768> &data) {
    for (size_t i = 0, c = 0; i < 256; i += 1, c += 3) {
        uint8_t r, g, b, a;

        r = data[c];
        g = data[c+1];
        b = data[c+2];

        if (i == 1) {
            a = 0x00;
        } else {
            a = 0xFF;
        }

        colours[i] = raylib::Color((r * 255) / 63, (g * 255) / 63, (b * 255) / 63, a);
    }
}

Palette::~Palette() {
}
