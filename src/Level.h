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

#ifndef LEVEL_H
#define LEVEL_H

#include <vector>
#include <cstdint>
#include <array>
#include <memory>
#include <optional>
#include <iostream>

#include <raylib-cpp.hpp>

#include "Game.h"
#include "Map.h"
#include "Entrance.h"
#include "Entity.h"

struct LevelSettings {
    const std::string filename;
    const Sky sky;
    const Ground ground;
    const std::string music;
};

class Level {
    class Grid {
        std::vector<uint8_t> data;
        uint16_t width;
        uint16_t height;
    public:
        Grid() : width(0), height(0) {
        }

        Grid(uint16_t width, uint16_t height) : width(width), height(height) {
            for (size_t i = 0; i < (width*height); i++) {
                data.push_back(0);
            }
        }

        uint8_t &operator()(int x, int y) {
            return data[(y * width) + x];
        }

        uint16_t getWidth() const {
            return width;
        }

        uint16_t getHeight() const {
            return height;
        }
    };

    struct Node {
        uint16_t x;
        uint16_t y;

        int f;
        int g;
        int h;

        Node(int x, int y) : x(x), y(y), f(0), g(0), h(0) {
        }

        bool operator>(const Node &other) const {
            return f > other.f;
        }

        bool operator==(const Node &other) const {
            return x == other.x && y == other.y;
        }
    };

    class Graph {
        std::vector<std::optional<Node>> nodes;

        uint16_t width;
        uint16_t height;
    public:
        Graph(const Grid &grid) : width(grid.getWidth()), height(grid.getHeight()) {
            for (size_t i = 0; i < (width*height); i++) {
                nodes.push_back(std::nullopt);
            }
        }

        void set(int x, int y, const Node &node) {
            nodes[(y * width) + x] = node;
        }

        Node get(int x, int y) const {
            auto node_if = nodes[(y * width) + x];

            if (!node_if) {
                throw std::out_of_range("Invalid graph node index (" + std::to_string(x) + "," + std::to_string(y));
            }

            return *node_if;
        }

        std::optional<Node> find(int x, int y) const {
            auto node_if = nodes[(y * width) + x];
            return node_if;
        }

        uint16_t getWidth() const {
            return width;
        }

        uint16_t getHeight() const {
            return height;
        }
    };

    Map map;
    Grid grid;

    std::vector<Node> findPathNodes(const Node &start, const Node &goal);

    raylib::Color sky;
    raylib::Color ground;

    std::string music;
public:
    Level(const LevelSettings &level_settings); 
    Map *getMap() {
        return &map;
    }

    std::string getLevelMusic() const {
        return music;
    }

    void draw(Player *player, raylib::Window &window, const uint64_t frame_count, const int scale);

    std::vector<raylib::Vector2> findPath(const raylib::Vector2 &start, const raylib::Vector2 &goal);

    ~Level();
};

#endif //LEVEL_H
