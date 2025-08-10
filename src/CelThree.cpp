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

#include <filesystem>
#include <fstream>
#include <vector>
#include <iostream>
#include <cstring>

#include "CelThree.h"

CelThree::CelThree(const std::string &filename, const Palette &palette) : filename(filename) {
    std::ifstream fh(filename, std::ios::binary|std::ios::in);

    std::vector<uint8_t> pixels;

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

    auto image = raylib::Image(bytes, 64, 64, 1, PIXELFORMAT_UNCOMPRESSED_R8G8B8A8);
    image = image.RotateCW();
    image = image.FlipHorizontal();

    texture = raylib::TextureUnmanaged(image); 
    texture.SetWrap(TEXTURE_WRAP_CLAMP);
}

CelThree::~CelThree() {

}
