#include <filesystem>
#include <fstream>
#include <vector>
#include <iostream>
#include <format>
#include <cstring>

#include "StillCel.h"

StillCel::StillCel(const std::string &filename) : filename(filename) {
    std::ifstream fh(filename, std::ios::binary|std::ios::in);

    std::vector<uint8_t> pixels;

    uint16_t magic;
    std::array<char, 26> skip;

    fh.read((char *)&magic, 2);

    if (magic != 0x9119)
        throw std::domain_error("Incorrect magic value " + std::format("{:#04x}", magic));

    fh.read((char *)&width, 2);
    fh.read((char *)&height, 2);
    fh.read(skip.data(), skip.size());

    Palette palette(fh);

    while (!fh.eof()) {
        uint8_t index;

        fh.read((char *)&index, 1);

        auto pixel = palette[index];

        pixels.push_back(pixel.GetR());
        pixels.push_back(pixel.GetG());
        pixels.push_back(pixel.GetB());
        pixels.push_back(pixel.GetA());
    }

    auto bytes = new uint8_t[pixels.size()];
    std::memcpy(bytes, pixels.data(), pixels.size());

    auto image = raylib::Image(bytes, width, height, 1, PIXELFORMAT_UNCOMPRESSED_R8G8B8A8);
    //image = image.Copy();
//    image = image.RotateCCW();

    texture = raylib::TextureUnmanaged(image);
}

StillCel::~StillCel() {

}
