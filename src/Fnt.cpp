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

#include <map>
#include <unordered_map>
#include <array>
#include <exception>
#include <fstream>
#include <cstring>

#include "Fnt.h"

static std::unordered_map<char, raylib::TextureUnmanaged> font_map;

raylib::TextureUnmanaged Fnt::Get(const char c) {
    if (c == ' ')
        return Fnt::Get('_');

    if (c < '!' || c > 'z')
        throw std::out_of_range("Unknown character code " + std::to_string((int)c));

    return font_map[c];
}

void Fnt::ExtractFonts(const std::string &filename) {
    std::ifstream fh(filename, std::ios::binary|std::ios::in); 

    uint8_t height;
    uint8_t width;
    uint16_t skip;
    uint16_t char_count;
    std::array<char, 18> title;

    fh.read((char *)&height, 1);
    fh.read((char *)&width, 1);
    fh.read((char *)&skip, 2);
    fh.read((char *)&char_count, 2);
    fh.read(title.data(), title.size());

    for (int i = 0; i < char_count; i++) {
        std::vector<uint8_t> pixels;

        for (int j = 0; j < height*width; j++) {
            uint8_t b;

            fh.read((char *)&b, 1);

            if (b) {
                pixels.push_back(0xFF);
                pixels.push_back(0xFF);
                pixels.push_back(0xFF);
                pixels.push_back(0xFF);
            } else {
                pixels.push_back(0x00);
                pixels.push_back(0x00);
                pixels.push_back(0x00);
                pixels.push_back(0x00);
            }
        }

        auto bytes = new uint8_t[pixels.size()];
        std::memcpy(bytes, pixels.data(), pixels.size());

        auto image = raylib::Image(bytes, width, height, 1, PIXELFORMAT_UNCOMPRESSED_R8G8B8A8);

        auto texture = raylib::TextureUnmanaged(image);

        font_map[(char)(i + '!')] = texture;
    }
}

void Fnt::Write(const std::string &text, int x, int y, int scale, std::optional<raylib::Color> tint, float rotation) {
    if (!tint)
        tint = raylib::Color(0xCF, 0xCD, 0x50, 0xFF);

    for (size_t i = 0; i < text.size(); i++) {
        auto texture = Fnt::Get(text[i]);
        texture.Draw(Vector2(x + (i * scale * 6), y), rotation, scale, *tint);
    }
}
