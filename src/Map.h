#ifndef MAP_H
#define MAP_H

#include <cstdint>
#include <array>
#include <exception>
#include <string>

#include <raylib-cpp.hpp>

#include "Segment.h"

class World;

class Map {
    const std::string filename;
    std::vector<Segment> segments;

    uint16_t x;
    uint16_t y;
    uint16_t width;
    uint16_t height;
public:
    Map(const std::string &filename);

    const std::vector<Segment> &getSegments() const {
        return segments;
    }

    uint16_t getX() const {
        return x;
    }

    uint16_t getY() const {
        return y;
    }

    uint16_t getWidth() const {
        return width;
    }

    uint16_t getHeight() const {
        return height;
    }

    const std::string &getFilename() const {
        return filename;
    }

    void sortSegments(const raylib::Camera3D *camera, World *world);

    ~Map();
};

#endif //MAP_H
