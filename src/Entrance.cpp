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
#include <iostream>
#include <algorithm>

#include "Entrance.h"

std::vector<Entrance> Entrance::Parse(const std::string &filename) {
    std::ifstream fh(filename, std::ios::binary|std::ios::in); 

    std::vector<Entrance> entrances;

    int count = 0;
    while (!fh.eof()) {
        for (int i = 0; i < 10; i++) {
            uint32_t x;
            uint32_t y;
            uint32_t flags;
            std::array<char, 16> map_name;

            fh.read((char *)&x, 4);
            fh.read((char *)&y, 4);
            fh.read((char *)&flags, 4);
            fh.read((char *)map_name.data(), map_name.size());

            if (map_name[0]) {

                std::transform(std::begin(map_name), std::end(map_name), std::begin(map_name), [](unsigned char c){ return std::tolower(c); });

                auto full_map_name = "maps/" + std::string(map_name.data());

                Entrance entrance(full_map_name, x, y, flags);
                entrances.push_back(entrance);

                //std::cout << count << " " << full_map_name << " " << x<< " " << y << " " << flags << "\n";
                count++;
            }
        }
    }

    return entrances;
}
