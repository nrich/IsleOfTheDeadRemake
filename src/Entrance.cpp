#include <map>
#include <unordered_map>
#include <array>
#include <exception>
#include <fstream>
#include <cstring>
#include <iostream>
#include <algorithm>

#include "Entrance.h"


/*
std::unordered_map<std::string, std::vector<Entrance>> Entrance::Parse(const std::string &filename) {
    std::ifstream fh(filename, std::ios::binary|std::ios::in); 

    std::unordered_map<std::string, std::vector<Entrance>> entrance_table;

    int count = 0;
    while (!fh.eof()) {
        std::vector<Entrance> entrances;

        std::string current_map_name;
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
                Entrance entrance(x, y, flags);
                entrances.push_back(entrance);

                std::transform(std::begin(map_name), std::end(map_name), std::begin(map_name), [](unsigned char c){ return std::tolower(c); });

                current_map_name = "maps/" + std::string(map_name.data());
                std::cout << count << " " << current_map_name << " " << x<< " " << y << " " << flags << "\n";
                count++;
            }
        }

        if (current_map_name.size()) {
            entrance_table[current_map_name] = entrances;
        }
    }

//    exit(0);
    return entrance_table;
}
*/

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

                std::cout << count << " " << full_map_name << " " << x<< " " << y << " " << flags << "\n";
                count++;
            }
        }
    }

//    exit(0);
    return entrances;
}
