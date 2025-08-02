#include <iostream>
#include <format>
#include <cstring>

#include "Flic.h"

Flic::Chunk::Chunk(std::ifstream &fh) {
    fh.read((char *)&length, 4);
    fh.read((char *)&type, 2);
}

void Flic::Chunk::readColours(std::ifstream &fh, std::array<raylib::Color, 256> &colours) const {
    uint16_t packet_count;

    fh.read((char *)&packet_count, 2);

    for (int i = 0; i < packet_count; i++) {
        uint8_t skip;
        uint8_t change;

        fh.read((char *)&skip, 1);
        fh.read((char *)&change, 1);

        int adjusted_change = change == 0 ? 256 : (int)change;

        for (int i = (int)skip; i < adjusted_change; i++) {
            uint8_t r, g, b;

            fh.read((char *)&r, 1);
            fh.read((char *)&g, 1);
            fh.read((char *)&b, 1);

            colours[i] = raylib::Color((r * 255) / 63, (g * 255) / 63, (b * 255) / 63, 0xFF);
        }
    }
}

void Flic::Chunk::readByteRun(std::ifstream &fh, std::array<raylib::Color, 64000> &current_frame) const {
    int pos = 0;

    for (int i = 0; i < 200; i++) {
        uint8_t packet_count;

        fh.read((char *)&packet_count, 1);


        for (int j = 0; j < packet_count; j++) {
            int8_t size_count;

            fh.read((char *)&size_count, 1);

            if (size_count < 0) {
                int byte_count = -size_count;

                for (int k = 0; k < byte_count; k++) {
                    uint8_t b;

                    fh.read((char *)&b, 1);

                    current_frame[pos++] = b;
                }
            } else {
                uint8_t b;

                fh.read((char *)&b, 1);

                for (int k = 0; k < size_count; k++) {
                    current_frame[pos++] = b;
                }
            }
        }
    }
}

void Flic::Chunk::readDelta(std::ifstream &fh, std::array<raylib::Color, 64000> &current_frame) const {
    uint16_t line_skip;
    uint16_t lines_changed;

    fh.read((char *)&line_skip, 2);
    fh.read((char *)&lines_changed, 2);

    int offset = line_skip * 320;

    for (int i = 0; i < lines_changed; i++) {
        int pos = offset;
        uint8_t packet_count;

        fh.read((char *)&packet_count, 1);

        for (int j = 0; j < packet_count; j++) {
            uint8_t skip_count;
            int8_t size_count;

            fh.read((char *)&skip_count, 1);
            fh.read((char *)&size_count, 1);

            pos += skip_count;

            if (size_count < 0) {
                int byte_count = -size_count;
                uint8_t b;

                fh.read((char *)&b, 1);

                for (int k = 0; k < byte_count; k++) {
                    current_frame[pos++] = b;
                }
            } else if (size_count) {
                 for (int k = 0; k < size_count; k++) {
                    uint8_t b;

                    fh.read((char *)&b, 1);

                    current_frame[pos++] = b;
                }
            } else {
                // skip with no data
            }
        }

        offset += 320;
    }
}

void Flic::Chunk::readCopy(std::ifstream &fh, std::array<raylib::Color, 64000> &current_frame) const {
    for (int i = 0; i < 64000; i++) {
        uint8_t b;

        fh.read((char *)&b, 1);

        current_frame[i] = b;
    }
}


Flic::Chunk::~Chunk() {
}

Flic::Frame::Frame(std::ifstream &fh) {
    fh.read((char *)&length, 4);

    uint16_t magic;
    fh.read((char *)&magic, 2);

    if (magic != 0xF1FA && ((magic & 0xFA00) == 0xFA00)) {
        fh.seekg(-5, std::ios_base::cur);

        fh.read((char *)&length, 4);
        fh.read((char *)&magic, 2);
    }

    if (magic != 0xF1FA)
        throw std::domain_error("Incorrect Flic::Frame magic value " + std::format("{:#04x}", magic));

    fh.read((char *)&chunks, 2);

    std::array<char, 8> skip;
    fh.read(skip.data(), skip.size());
}

void Flic::Frame::extract(std::ifstream &fh, std::array<raylib::Color, 256> &colours, std::array<raylib::Color, 64000> &current_frame) const {
    for (int i = 0; i < chunks; i++) {
        Chunk chunk(fh);

        switch (chunk.getType()) {
            case ChunkType::FLI_COLOR:
                chunk.readColours(fh, colours);
                break;
            case ChunkType::FLI_LC:
                chunk.readDelta(fh, current_frame);
                break;
            case ChunkType::FLI_BLACK:
                current_frame.fill(0);
                break;
            case ChunkType::FLI_BRUN:
                chunk.readByteRun(fh, current_frame);
                break;
            case ChunkType::FLI_COPY:
                chunk.readCopy(fh, current_frame);
                break;
        }
    }
}

Flic::Frame::~Frame() {
}

Flic::Flic(const std::string &filename) : filename(filename) {
    std::ifstream fh(filename, std::ios::binary|std::ios::in);

    fh.read((char *)&length, 4);

    uint16_t magic;
    fh.read((char *)&magic, 2);

    if (magic != 0xAF11)
        throw std::domain_error("Incorrect Flic::Header magic value " + std::format("{:#04x}", magic));

    fh.read((char *)&frames, 2);
    fh.read((char *)&width, 2);
    fh.read((char *)&height, 2);
    fh.read((char *)&depth, 2);
    fh.read((char *)&flags, 2);
    fh.read((char *)&speed, 2);

    uint32_t next, frit;
    fh.read((char *)&next, 4);
    fh.read((char *)&frit, 4);

    std::array<char, 102> skip;
    fh.read(skip.data(), skip.size());

    std::array<raylib::Color, 256> colours;
    colours.fill(raylib::BLACK);
    std::array<raylib::Color, 64000> current_frame;
    current_frame.fill(0);

    for (int i = 0; i < frames; i++) {
        Frame frame(fh);

        frame.extract(fh, colours, current_frame);

        std::vector<uint8_t> pixels;

        for (int i = 0; i < width * height; i++) {
            auto pixel = colours[current_frame[i]];

            pixels.push_back(pixel.GetR());
            pixels.push_back(pixel.GetG());
            pixels.push_back(pixel.GetB());
            pixels.push_back(pixel.GetA());
        }

        uint8_t *bytes = new uint8_t[pixels.size()];
        std::memcpy(bytes, pixels.data(), pixels.size());

        auto image = raylib::Image(bytes, width, height, 1, PIXELFORMAT_UNCOMPRESSED_R8G8B8A8);
        textures.push_back(raylib::TextureUnmanaged(image));
    }
}

Flic::~Flic() {

}

