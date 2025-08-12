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
#include <variant>

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

    struct Animation {
        raylib::Vector2 position;
        std::vector<std::optional<raylib::TextureUnmanaged>> frames;
        bool loop;
        size_t frameRate;

        bool draw(int scale) {
            size_t next_frame = count / frameRate; 
            count++;

            if (next_frame >= frames.size()) {
                if (!loop)
                    return true;

                count = 0;
                next_frame = 0;
            }

            if (frames[next_frame])
                frames[next_frame]->Draw(position * scale, 0.0f, scale);
            return false;
        }

        Animation(const raylib::Vector2 &position, const std::vector<std::optional<raylib::TextureUnmanaged>> &frames, bool loop=true, size_t frame_rate=6) : position(position), frames(frames), loop(loop), frameRate(frame_rate), count(0) {}
        Animation(const Animation &other) : position(other.position), frames(other.frames), loop(other.loop), frameRate(other.frameRate), count(other.count) {}
        Animation() : loop(true), frameRate(6), count(0) {}
private:
        size_t count;
    };

    std::unordered_map<uint16_t, Animation> animations;

    struct Layout {
        raylib::Vector2 position;
        std::variant<raylib::TextureUnmanaged, raylib::Vector2> data;
        Item item;

        std::string name;
        std::string description;
        std::string pickup;

        raylib::Rectangle Border() const {
            if (std::holds_alternative<raylib::TextureUnmanaged>(data)) {
                auto image = std::get<raylib::TextureUnmanaged>(data);

                return raylib::Rectangle(position.GetX(), position.GetY(), image.GetWidth(), image.GetHeight());
            } else {
                auto bounds = std::get<raylib::Vector2>(data);
                return raylib::Rectangle(position.GetX(), position.GetY(), bounds.GetX(), bounds.GetY());
            }
        }

        void draw(int scale) const {
            if (std::holds_alternative<raylib::TextureUnmanaged>(data)) {
                auto image = std::get<raylib::TextureUnmanaged>(data);

                image.Draw(position * scale, 0.0f, scale);
            }
        }
    };

    Panel *panel;
    raylib::TextureUnmanaged background;
    Scene(Panel *panel, const std::string &background_filename);

    std::vector<Layout> layouts;
    std::unordered_map<Input, State> navigation;

    virtual std::tuple<bool, std::string, DeathType> getItem(const Layout &layout);
    virtual std::tuple<bool, std::string, DeathType> useItemOnItem(Item source, Item destination);
    virtual std::tuple<bool, std::string, DeathType> useItemOnItem(Item source, Item destination, Player *player) {
        return useItemOnItem(source, destination);
    }

    virtual std::optional<Dialogue> talk(Player *player);
    virtual std::string useItem(Player *player, Item item);

    virtual void animationCompleted(Player *player, uint16_t animation_id);

    std::optional<Entrance> entrance = std::nullopt;

    void removeItemLayout(Item item) {
        layouts.erase(std::remove_if(std::begin(layouts), std::end(layouts), [item](const auto &layout) {
            return layout.item == item;
        }));
    }
public:
    virtual void draw(Player *player, int scale);
    virtual ~Scene();
};

class CrashedPlaneEntryScene : public Scene {
public:
    CrashedPlaneEntryScene(Panel *panel, const Entrance &new_entrance);
};

class CrashedPlaneLeftScene : public Scene {
public:
    CrashedPlaneLeftScene(Panel *panel);
};

class CrashedPlaneCockpitScene : public Scene {
protected:
    std::tuple<bool, std::string, DeathType> getItem(const Layout &layout);
    std::tuple<bool, std::string, DeathType> useItemOnItem(Item source, Item destination);
public:
    CrashedPlaneCockpitScene(Panel *panel);
};

class CrashedPlaneRightScene : public Scene {
public:
    CrashedPlaneRightScene(Panel *panel);
};

class CrashedPlaneExitScene : public Scene {
public:
    CrashedPlaneExitScene(Panel *panel, const Entrance &new_entrance);
};

class BunkerEntryScene : public Scene {
    std::tuple<bool, std::string, DeathType> getItem(const Layout &layout);
    std::tuple<bool, std::string, DeathType> useItemOnItem(Item source, Item destination);
    bool trapped = true;
public:
    BunkerEntryScene(Panel *panel, const Entrance &new_entrance);
};

class BunkerExitScene : public Scene {
public:
    BunkerExitScene(Panel *panel, const Entrance &new_entrance);
};

class BunkerLeftScene : public Scene {
    std::tuple<bool, std::string, DeathType> getItem(const Layout &layout);
    std::tuple<bool, std::string, DeathType> useItemOnItem(Item source, Item destination);
public:
    BunkerLeftScene(Panel *panel);
};

class BunkerRightScene : public Scene {
    std::tuple<bool, std::string, DeathType> getItem(const Layout &layout);
    std::tuple<bool, std::string, DeathType> useItemOnItem(Item source, Item destination);
public:
    BunkerRightScene(Panel *panel);
};

class VillageGateShamanScene : public Scene {
    std::vector<Dialogue> script;
    std::optional<Dialogue> talk(Player *player);
    size_t dialogueIndex = 0;
    bool pass = false;
public:
    VillageGateShamanScene(Panel *panel, const Entrance &new_entrance);
};

class VillageGateChiefScene : public Scene {
    std::vector<Dialogue> script;
    std::optional<Dialogue> talk(Player *player);
    size_t dialogueIndex = 0;
    bool pass = false;
public:
    VillageGateChiefScene(Panel *panel, const Entrance &new_entrance);
};

class TempleEntranceScene : public Scene {
    std::tuple<bool, std::string, DeathType> getItem(const Layout &layout);
    std::tuple<bool, std::string, DeathType> useItemOnItem(Item source, Item destination);

    void animationCompleted(Player *player, uint16_t animation_id);
    bool opened = false;
public:
    TempleEntranceScene(Panel *panel, const Entrance &new_entrance);
    void draw(Player *player, int scale);
};

class OracleScene : public Scene {
    std::vector<Dialogue> script;
    size_t dialogueIndex = 0;
    bool sacrifice = false;

    std::optional<Dialogue> talk(Player *player);
    std::tuple<bool, std::string, DeathType> useItemOnItem(Item source, Item destination);
public:
    OracleScene(Panel *panel, const Entrance &new_entrance);
};

class ToiletScene : public Scene {
public:
    ToiletScene(Panel *panel, const Entrance &new_entrance);
};

class ShowerScene : public Scene {
public:
    ShowerScene(Panel *panel, const Entrance &new_entrance);
};

class DevelopersScene : public Scene {
    std::optional<Dialogue> talk(Player *player);
public:
    DevelopersScene(Panel *panel, const Entrance &new_entrance);
};

class VillageEyesScene : public Scene {
    std::optional<Dialogue> talk(Player *player);

    std::vector<Dialogue> script;
    size_t dialogueIndex = 0;
public:
    VillageEyesScene(Panel *panel, const Entrance &new_entrance);
};

class RocketLauncherScene : public Scene {
    enum class WireCut {
        Green,
        Red,
        Blue,
    };

    WireCut cut = WireCut::Green;

    std::tuple<bool, std::string, DeathType> getItem(const Layout &layout);
    std::tuple<bool, std::string, DeathType> useItemOnItem(Item source, Item destination, Player *player);
public:
    RocketLauncherScene(Panel *panel, const Entrance &new_entrance);
};

class PlaneCockpitScene : public Scene {
    std::string useItem(Player *player, Item item);
public:
    PlaneCockpitScene(Panel *panel, const Entrance &new_entrance);
};

class PlaneGalleyScene : public Scene {
public:
    PlaneGalleyScene(Panel *panel);
};

class LabZombieScene : public Scene {
public:
    LabZombieScene(Panel *panel, const Entrance &new_entrance);
};

class LabCompanionScene : public Scene {
public:
    LabCompanionScene(Panel *panel, const Entrance &new_entrance);
    std::tuple<bool, std::string, DeathType> getItem(const Layout &layout);
    std::string useItem(Player *player, Item item);
};

class MirrorScene : public Scene {
public:
    MirrorScene(Panel *panel, const Entrance &new_entrance);
};

class ShamanScene : public Scene {
    std::array<std::vector<Dialogue>, 3> scripts;
    std::optional<Dialogue> talk(Player *player);
    size_t dialogueIndex = 0;
    bool pass = false;
    size_t script = 0;
    bool gaveSmokes = false;

    std::tuple<bool, std::string, DeathType> useItemOnItem(Item source, Item destination);
    std::string useItem(Player *player, Item item);
    std::tuple<bool, std::string, DeathType> getItem(const Layout &layout);
public:
    ShamanScene(Panel *panel, const Entrance &new_entrance);
};

class ChiefScene : public Scene {
    std::array<std::vector<Dialogue>, 3> scripts;
    std::optional<Dialogue> talk(Player *player);
    size_t dialogueIndex = 0;
    size_t script = 0;

    std::tuple<bool, std::string, DeathType> getItem(const Layout &layout);
public:
    ChiefScene(Panel *panel, const Entrance &new_entrance);
};

#endif //SCENE_H
