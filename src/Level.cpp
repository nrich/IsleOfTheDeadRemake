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

#include <queue>
#include <array>
#include <unordered_map>
#include <tuple>

#include "Fnt.h"
#include "Level.h"
#include "Player.h"

Level::Level(const LevelSettings &level_settings) : map(level_settings.filename), music(level_settings.music) {
    size_t width = map.getWidth() / 10 + 1;
    size_t height = map.getHeight() / 10 + 1;

    grid = Grid(width, height);

    for (const auto &segment : map.getSegments()) {
        if (segment.texture >= 100)
            continue;

        size_t x = segment.x1 / 10;
        size_t y = segment.y1 / 10;

        grid(x, y) = 1;
    }

    switch (level_settings.sky) {
        case Sky::Day:
            sky = raylib::Color(0x0C, 0x14, 0x51, 0xFF);
            break;
        case Sky::Cave:
            sky = raylib::Color(0x30, 0x20, 0x20, 0xFF);
            break;
        case Sky::Basement:
            sky = raylib::Color(0x94, 0x94, 0x94, 0xFF);
            break;
        case Sky::Mansion:
            sky = raylib::Color(0x59, 0x48, 0x2C, 0xFF);
            break;

    }

    switch (level_settings.ground) {
        case Ground::Dirt:
            ground = raylib::Color(0x79, 0x61, 0x3C, 0xFF);
            break;
        case Ground::Cave:
            ground = raylib::Color(0x49, 0x30, 0x30, 0xFF);
            break;
        case Ground::Basement:
            ground = raylib::Color(0x55, 0x55, 0x55, 0xFF);
            break;
        case Ground::Carpet:
            ground = raylib::Color(0x38, 0x04, 0x04, 0xFF);
            break;
    }
}

void Level::draw(Player *player, raylib::Window &window, const uint64_t frame_count, const int scale) {
    auto *world = player->getWorld();
    auto *level = world->getCurrentLevel();
    auto *map = level->getMap();
    auto *camera = player->getCamera();

    BeginDrawing();
    {
        window.ClearBackground(sky);

        map->sortSegments(camera, world);

        camera->BeginMode();
        {
            DrawPlane(Vector3(0.0, 0.0, 0.0), Vector2(1000, 1000), ground);

            rlColor4ub(0xFF, 0xFF, 0xFF, 0xFF);

            for (const auto &segment : map->getSegments()) {
                auto entity = world->getEntity(segment.id);

                if (entity) {
                    entity->draw(camera, frame_count);
                }

                rlDrawRenderBatchActive();
            }
        }
        camera->EndMode();

        player->drawWeapon(frame_count, scale);

        Fnt::Write(map->getFilename(), 0, 0, scale);
        Fnt::Write(std::to_string(player->getHealth()), 0, (200-10) * scale, scale);
    }
    EndDrawing();
}

std::vector<Level::Node> Level::findPathNodes(const Level::Node &start, const Level::Node &goal) {
    Graph graph(grid);

    const static std::vector<std::tuple<int, int, int>> directions = {
        std::make_tuple(-1, 0, 10),
        std::make_tuple(-1, 1, 14),

        std::make_tuple(0, 1, 10),
        std::make_tuple(1, 1, 14),

        std::make_tuple(1, 0, 10),
        std::make_tuple(1, -1, 14),

        std::make_tuple(0, -1, 10),
        std::make_tuple(-1, -1, 14),
    };

    std::priority_queue<Level::Node, std::vector<Level::Node>, std::greater<Level::Node>> open_list;
    std::vector<std::vector<bool>> closed_list(grid.getHeight(), std::vector<bool>(grid.getWidth(), false));

    open_list.push(start);

    while (!open_list.empty()) {
        auto current = open_list.top();
        open_list.pop();

        if (current == goal) {
            std::vector<Level::Node> path;

            while (current != start) {
                path.push_back(current);
                current = graph.get(current.x, current.y);
            }

            path.push_back(start);
            std::reverse(std::begin(path), std::end(path));
            return path;
        }

        closed_list[current.y][current.x] = true;

        for (size_t i = 0; i < directions.size(); i++) {
            auto [direction_x, direction_y, direction_cost] = directions[i];

            int new_x = current.x + direction_x;
            int new_y = current.y + direction_y;

            if (new_x >= 0 && new_x < grid.getWidth() && new_y >= 0 && new_y < grid.getHeight()) {
                if (!grid(new_x, new_y) && !closed_list[new_y][new_x]) {
                    int new_g = current.g + direction_cost;

                    auto node_if = graph.find(new_x, new_y);

                    if (node_if) {
                        Level::Node neighbour = *node_if;

                        if (new_g < neighbour.g) {
                            neighbour.g = new_g;
                            neighbour.h = std::abs(new_x - goal.x) * 10 + std::abs(new_y - goal.y) * 10;
                            neighbour.f = neighbour.g + neighbour.h;
                            graph.set(new_x, new_y, current);

                            std::vector<Level::Node> temp_nodes;

                            while (!open_list.empty()) {
                                auto temp = open_list.top();
                                open_list.pop();

                                if (temp != neighbour) {
                                    temp_nodes.push_back(temp);
                                }
                            }

                            for (const auto &node : temp_nodes) {
                                open_list.push(node);
                            }

                            open_list.push(neighbour);
                        }
                    } else {
                        Level::Node neighbour(new_x, new_y);

                        neighbour.g = new_g;
                        neighbour.h = std::abs(new_x - goal.x) * 10 + std::abs(new_y - goal.y) * 10;
                        neighbour.f = neighbour.g + neighbour.h;

                        graph.set(new_x, new_y, current);

                        open_list.push(neighbour);
                    }
                }
            }
        }
    }

    return {};
}

std::vector<raylib::Vector2> Level::findPath(const raylib::Vector2 &start, const raylib::Vector2 &goal) {
    std::vector<raylib::Vector2> path;

    uint8_t goal_blocked = grid(goal.x / 10, goal.y / 10);
    grid(goal.x / 10, goal.y / 10) = 0;

    auto path_nodes = findPathNodes(Level::Node(start.x / 10, start.y / 10), Level::Node(goal.x / 10, goal.y / 10));

    int skew_x = GetRandomValue(2, 8);
    int skew_y = GetRandomValue(2, 8);

    for (const auto &node : path_nodes) {
        path.push_back(raylib::Vector2(node.x * 10.0f + skew_x, node.y * 10.0f + skew_y));
    }

    grid(goal.x / 10, goal.y / 10) = goal_blocked;

    return path;
}

Level::~Level() {

}
