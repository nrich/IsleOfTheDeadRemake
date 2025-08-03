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

#ifndef SCENE_H
#define SCENE_H

#include <cstdint>
#include <array>
#include <exception>
#include <string>
#include <unordered_map>
#include <utility>
#include <optional>

#include <raylib-cpp.hpp>

#include "Panel.h"
#include "StillCel.h"
#include "Entrance.h"
#include "Voc.h"

class Scene {
protected:
    struct Dialogue {
        std::string text;
        std::optional<raylib::Color> colour;
        raylib::Sound *sound = nullptr;
    };

    struct Layout {
        raylib::Vector2 position;
        raylib::TextureUnmanaged image;
        Item item;

        std::string name;
        std::string description;
        std::string pickup;

        raylib::Rectangle Border() const {
            return raylib::Rectangle(position.GetX(), position.GetY(), image.GetWidth(), image.GetHeight());
        }

        void draw(int scale) const {
            image.Draw(position * scale, 0.0f, scale);
        }
    };

    Panel *panel;
    raylib::TextureUnmanaged background;
    Scene(Panel *panel, const std::string &background_filename);

    std::vector<Layout> layouts;
    std::unordered_map<Input, State> navigation;

    virtual std::tuple<bool, std::string, DeathType> getItem(const Layout &layout);
    virtual std::tuple<bool, std::string, DeathType> useItemOnItem(Item source, Item destination);
    virtual std::optional<Dialogue> talk(Player *player);

    std::optional<Entrance> entrance = std::nullopt;
public:
    virtual void draw(Player *player, int scale);
    virtual ~Scene();
};

class CrashedPlaneEntryScene : public Scene {
public:
    CrashedPlaneEntryScene(Panel *panel, const Entrance &new_entrance);
    ~CrashedPlaneEntryScene();
};

class CrashedPlaneLeftScene : public Scene {
public:
    CrashedPlaneLeftScene(Panel *panel);
    ~CrashedPlaneLeftScene();
};

class CrashedPlaneCockpitScene : public Scene {
protected:
    std::tuple<bool, std::string, DeathType> getItem(const Layout &layout);
    std::tuple<bool, std::string, DeathType> useItemOnItem(Item source, Item destination);
public:
    CrashedPlaneCockpitScene(Panel *panel);
    ~CrashedPlaneCockpitScene();
};

class CrashedPlaneRightScene : public Scene {
public:
    CrashedPlaneRightScene(Panel *panel);
    ~CrashedPlaneRightScene();
};

class CrashedPlaneExitScene : public Scene {
public:
    CrashedPlaneExitScene(Panel *panel, const Entrance &new_entrance);
    ~CrashedPlaneExitScene();
};

class BunkerEntryScene : public Scene {
    std::tuple<bool, std::string, DeathType> getItem(const Layout &layout);
    std::tuple<bool, std::string, DeathType> useItemOnItem(Item source, Item destination);
    bool trapped = true;
public:
    BunkerEntryScene(Panel *panel, const Entrance &new_entrance);
    ~BunkerEntryScene();
};

class BunkerExitScene : public Scene {
public:
    BunkerExitScene(Panel *panel, const Entrance &new_entrance);
    ~BunkerExitScene();
};

class BunkerLeftScene : public Scene {
    std::tuple<bool, std::string, DeathType> getItem(const Layout &layout);
    std::tuple<bool, std::string, DeathType> useItemOnItem(Item source, Item destination);
public:
    BunkerLeftScene(Panel *panel);
    ~BunkerLeftScene();
};

class BunkerRightScene : public Scene {
    std::tuple<bool, std::string, DeathType> getItem(const Layout &layout);
    std::tuple<bool, std::string, DeathType> useItemOnItem(Item source, Item destination);
public:
    BunkerRightScene(Panel *panel);
    ~BunkerRightScene();
};

class VillageGateShaman : public Scene {
    std::vector<Dialogue> script;
    std::optional<Dialogue> talk(Player *player);
    size_t dialogueIndex = 0;
    bool pass = false;
public:
    VillageGateShaman(Panel *panel, const Entrance &new_entrance);
};

class VillageGateChief : public Scene {
    std::vector<Dialogue> script;
    std::optional<Dialogue> talk(Player *player);
    size_t dialogueIndex = 0;
    bool pass = false;
public:
    VillageGateChief(Panel *panel, const Entrance &new_entrance);
};

#endif //SCENE_H
