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
#include <exception>
#include <iostream>
#include <algorithm>
#include <functional>
#include <unordered_map>

#include "Map.h"
#include "Entity.h"
#include "World.h"

struct MapSegment {
    uint16_t count;
    uint16_t _flags0;

    uint16_t x1;
    uint16_t _flags1;

    uint16_t y1;
    uint16_t _flags2;

    uint16_t x2;
    uint16_t _flags3;

    uint16_t y2;
    uint16_t _flags4;

    uint16_t footer;
    uint16_t _flags5;
};

size_t hash_map_segment(const MapSegment &map_segment) {
    uint64_t map_segment_data = ((uint64_t)(map_segment.x1) << 48) | ((uint64_t)map_segment.y1 << 32) | ((uint64_t)map_segment.x2 << 16) | (uint64_t)map_segment.y2;
    return std::hash<uint64_t>{}(map_segment_data);
}

Map::Map(const std::string &filename) : filename(filename), x(0), y(0), width(0), height(0) {
    std::ifstream fh(filename, std::ios::binary|std::ios::in);

    MapSegment map_segment;

    fh.read((char *)&map_segment, sizeof(map_segment));

    size_t map_hash_id = std::hash<std::string>{}(filename);

    size_t segment_id = map_hash_id ^ hash_map_segment(map_segment);
    segments.push_back(Segment(segment_id, map_segment.x1, map_segment.y1, map_segment.x2, map_segment.y2, map_segment.footer, map_segment._flags5, map_segment.count));

    size_t segment_count = map_segment.count-1;

    for (size_t i = 0; i < segment_count; i += 1) {
        fh.read((char *)&map_segment, sizeof(map_segment));
        size_t segment_id = map_hash_id ^ hash_map_segment(map_segment);

        if (map_segment._flags5) {
            //std::cerr << filename << " " << map_segment.footer << " " << map_segment._flags5 << "\n";
        }

        segments.push_back(Segment(segment_id, map_segment.x1, map_segment.y1, map_segment.x2, map_segment.y2, map_segment.footer, map_segment._flags5, map_segment.count));

        x = std::min(x, std::min(map_segment.x1, map_segment.x2));
        y = std::min(y, std::min(map_segment.y1, map_segment.y2));
        width = std::max(width, std::max(map_segment.x1, map_segment.x2));
        height = std::max(height, std::max(map_segment.y1, map_segment.y2));
    }

    std::cout << filename << " " << fh.tellg() << "\n";
}

void Map::sortSegments(const raylib::Camera3D *camera, World *world) {
    std::sort(std::begin(segments), std::end(segments), [camera, world](const Segment &l, const Segment &r) {
        auto camera_position = camera->GetPosition();
        camera_position.y = 0;


        if (l.texture != 63 && r.texture != 63) {
            if (l.texture < 100 && r.texture >= 100)
                return true;

            if (r.texture < 100 && l.texture >= 100)
                return false;
        }

        Entity *le = world->getEntity(l.id);
        Entity *re = world->getEntity(r.id);

        Vector3 l_segment_position(0.0f, 0.0f, 0.0f);
        Vector3 r_segment_position(0.0f, 0.0f, 0.0f);

        if (!le) {
            uint16_t min_x = std::min(l.x1, l.x2);
            uint16_t max_x = std::max(l.x1, l.x2);
            uint16_t min_y = std::min(l.y1, l.y2);
            uint16_t max_y = std::max(l.y1, l.y2);

            l_segment_position = Vector3((max_x-min_x)/2 + min_x, 0, (max_y - min_y)/2 + min_y);
        } else {
            auto position_if = le->getPosition();

            if (position_if) {
                l_segment_position = Vector3(position_if->x, 0, position_if->y);
            } else {
                uint16_t min_x = std::min(l.x1, l.x2);
                uint16_t max_x = std::max(l.x1, l.x2);
                uint16_t min_y = std::min(l.y1, l.y2);
                uint16_t max_y = std::max(l.y1, l.y2);

                l_segment_position = Vector3((max_x-min_x)/2 + min_x, 0, (max_y - min_y)/2 + min_y);
            }
        }

        if (!re) {
            uint16_t min_x = std::min(r.x1, r.x2);
            uint16_t max_x = std::max(r.x1, r.x2);
            uint16_t min_y = std::min(r.y1, r.y2);
            uint16_t max_y = std::max(r.y1, r.y2);

            r_segment_position = Vector3((max_x-min_x)/2 + min_x, 0, (max_y - min_y)/2 + min_y);
        } else {
            auto position_if = re->getPosition();

            if (position_if) {
                r_segment_position = Vector3(position_if->x, 0, position_if->y);
            } else {
                uint16_t min_x = std::min(r.x1, r.x2);
                uint16_t max_x = std::max(r.x1, r.x2);
                uint16_t min_y = std::min(r.y1, r.y2);
                uint16_t max_y = std::max(r.y1, r.y2);

                r_segment_position = Vector3((max_x-min_x)/2 + min_x, 0, (max_y - min_y)/2 + min_y);
            }
        }

        float l_dist = Vector3Distance(camera_position, l_segment_position);
        float r_dist = Vector3Distance(camera_position, r_segment_position);

        return r_dist < l_dist;
    }); 
}

Map::~Map() {

}
