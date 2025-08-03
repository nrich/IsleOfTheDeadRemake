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

#ifndef WORLD_H
#define WORLD_H

#include <vector>
#include <cstdint>
#include <array>
#include <memory>

#include <raylib-cpp.hpp>

#include "Game.h"
#include "Map.h"
#include "Level.h"
#include "Entrance.h"
#include "Entity.h"
#include "MusicPlayer.h"

class World {
    std::unordered_map<std::string, Level> levels;
    std::vector<Entrance> entrances;
    std::string currentMap;

    void spawnEntityForSegment(const std::string &map_filename, const Segment &segment);

    void spawnPassage(const Segment &segment);
    void spawnRoomEntry(const Segment &segment);

    std::unordered_map<uint64_t, std::unique_ptr<Entity>> entities;
    MusicPlayer *musicPlayer;
public:
    World(MusicPlayer *music_player, const std::vector<LevelSettings> &level_settings, const std::string &entrance_filename); 

    Level *getCurrentLevel() {
        return &levels.at(currentMap);
    }

    Level *setCurrentLevel(const std::string &map_name) {
        Level &level = levels.at(map_name);
        musicPlayer->play(level.getLevelMusic());
        currentMap = map_name;
        return &levels.at(map_name);
    }

    Entity *getEntity(uint64_t id) {
        try {
            return entities.at(id).get();
        } catch (std::out_of_range &e) {
            return nullptr;
        }
    }

    const Entrance &getEntrance(size_t index) const {
        return entrances[index];
    }

    std::pair<raylib::Vector2, float> getBounds() const;

    raylib::Vector2 findSpawn() const;
};

#endif //WORLD_H
