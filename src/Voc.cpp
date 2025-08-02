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

#include <exception>
#include <fstream>
#include <cstring>
#include <array>
#include <iostream>

#include "Voc.h"

raylib::Wave Voc::Load(const std::string &filename) {
    std::ifstream fh(filename, std::ios::binary|std::ios::in);

    std::array<char, 19> identifier;
    static const std::array<char, 19> identifier_validator = {'C', 'r', 'e', 'a', 't', 'i', 'v', 'e', ' ', 'V', 'o', 'i', 'c', 'e', ' ', 'F', 'i', 'l', 'e'};

    fh.read(identifier.data(), identifier.size());

    if (identifier != identifier_validator)
        throw std::domain_error("Invalid VOC file identifier found in file header");

    uint8_t validate_eof;
    fh.read((char *)&validate_eof, 1);

    if (validate_eof != 0x1A)
        throw std::domain_error("Invalid VOC file header EOF");

    uint16_t data_offset;
    fh.read((char *)&data_offset, 2);

    if (data_offset != 0x1A)
        throw std::domain_error("Invalid VOC file data offset");

    uint16_t version;
    uint16_t version_checksum;

    fh.read((char *)&version, 2);
    fh.read((char *)&version_checksum, 2);

    while (!fh.eof()) {
        uint8_t block_type;
        fh.read((char *)&block_type, 1);

        if (block_type == 9) {
            uint32_t size = 0;
            fh.read((char *)&size, 3);

            uint32_t samples_per_second = 0;
            fh.read((char *)&samples_per_second, 4);

            uint8_t bits_per_sample = 0;
            fh.read((char *)&bits_per_sample, 1);

            uint8_t num_channels = 0;
            fh.read((char *)&num_channels, 1);

            uint16_t sound_format = 0;
            fh.read((char *)&sound_format, 2);

            uint32_t padding = 0;
            fh.read((char *)&padding, 4);

            uint8_t *pcm_data = new uint8_t[size-11];

            fh.read((char *)pcm_data, size-11);

            raylib::Wave wav(size-11, samples_per_second, bits_per_sample, num_channels, pcm_data);

            return wav;
        } else {
            throw std::domain_error("Unsupported VOC block type: " + std::to_string(block_type));
        }
    }

    throw std::runtime_error("Error decoding VOC file " + filename);
}
