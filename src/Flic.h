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

#ifndef FLIC_H
#define FLIC_H

#include <cstdint>
#include <array>
#include <fstream>

#include <raylib-cpp.hpp>

class Flic {
    enum ChunkType : uint16_t {
        FLI_COLOR = 11,
        FLI_LC = 12,
        FLI_BLACK = 13,
        FLI_BRUN = 15,
        FLI_COPY = 16,
    };

    class Chunk {
        uint32_t length;
        ChunkType type;
    public:
        Chunk(std::ifstream &fh);
        inline const ChunkType getType() const {
            return type;
        }

        void readColours(std::ifstream &fh, std::array<raylib::Color, 256> &colours) const;
        void readByteRun(std::ifstream &fh, std::array<raylib::Color, 64000> &current_frame) const;
        void readDelta(std::ifstream &fh, std::array<raylib::Color, 64000> &current_frame) const;
        void readCopy(std::ifstream &fh, std::array<raylib::Color, 64000> &current_frame) const;

        ~Chunk();
    };

    class Frame {
        uint32_t length;
        uint16_t chunks;
    public:
        Frame(std::ifstream &fh);

        void extract(std::ifstream &fh, std::array<raylib::Color, 256> &colours, std::array<raylib::Color, 64000> &current_frame) const;

        ~Frame();
    };

    const std::string filename;

    uint32_t length = 0;

    uint16_t frames = 0;
    uint16_t width = 0;
    uint16_t height = 0;
    uint16_t depth = 0;
    uint16_t flags = 0;
    uint16_t speed = 0;

    std::vector<raylib::TextureUnmanaged> textures;
public:
    Flic(const std::string &filename);

    uint16_t getWidth() const {
        return width;
    }

    uint16_t getHeight() const {
        return height;
    }

    uint16_t getSpeed() const {
        return speed;
    }

    const std::vector<raylib::TextureUnmanaged> &getTextures() const {
        return textures;
    }

    const raylib::TextureUnmanaged &getTexture(size_t index) const {
        return textures[index];
    }

    const size_t getFrameCount() const {
        return textures.size();
    }

    ~Flic();
};

#endif //FLIC_H
