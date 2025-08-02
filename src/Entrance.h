#ifndef ENTRANCE_H
#define ENTRANCE_H

#include <cstdint>
#include <unordered_map>

#include <raylib-cpp.hpp>

enum Direction : uint32_t {
    NORTH = 0,
    WEST = 90,
    SOUTH = 180,
    EAST = 270,
};

class Entrance {
    std::string name;

    uint32_t x;
    uint32_t y;
    Direction direction;

public:
    Entrance(const std::string &map_name, uint32_t x, uint32_t y, uint32_t direction) : name(map_name), x(x), y(y), direction((Direction)direction) {
    }

    inline const std::string getName() const {
        return name;
    }

    inline const uint32_t X() const {
        return x;
    }

    inline const uint32_t Y() const {
        return y;
    }

    inline const Direction getDirection() const {
        return direction;
    }

public:
    static std::vector<Entrance> Parse(const std::string &filename);
};

#endif //ENTRANCE_H
