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

#include <cstdint>
#include <cstdlib>
#include <vector>
#include <fstream>
#include <string>
#include <iterator>
#include <algorithm>
#include <vector>
#include <iomanip>
#include <memory>
#include <filesystem>
#include <chrono>
#include <thread>

#include <cmdline.h>
#include <raylib-cpp.hpp>

#include "Palette.h"
#include "CelThree.h"
#include "StillCel.h"
#include "Flic.h"
#include "Fnt.h"
#include "World.h"
#include "Player.h"
#include "Strings.h"
#include "Inventory.h"
#include "Panel.h"
#include "Scene.h"
#include "Voc.h"
#include "Flic.h"
#include "MusicPlayer.h"
#include "Animation.h"

static void draw_world(Player *player, MusicPlayer *music_player, raylib::Window &window, const int scale) {
    static uint64_t frame_count = 0;

    frame_count += 1;

    auto *world = player->getWorld();
    auto *level = world->getCurrentLevel();
    auto *map = level->getMap();

    player->update(frame_count);

    for (const auto &segment : map->getSegments()) {
        Entity *entity = world->getEntity(segment.id);

        if (entity) {
            entity->update(player, frame_count);
        }
    }

    level->draw(player, window, frame_count, scale);
} 

static void draw_map(Player *player, raylib::Window &window, const int scale) {
    auto *world = player->getWorld();
    auto *level = world->getCurrentLevel();
    auto *map = level->getMap();

    uint64_t player_input = player->getInput();

    if (player_input & Input::ViewMap) {
        player_input &= ~Input::ViewMap;
        player->setState(State::World);
    }

    player->setInput(player_input);

    raylib::Camera2D camera(
        raylib::Vector2(scale * 10.0f, scale * 10.0f),
        raylib::Vector2(),
        0.0f
    );

    auto player_position = player->getPosition();
    auto player_angles = player->getAngles();
    auto player_position_angled = (raylib::Vector2(0, -10).Rotate(DEG2RAD * player_angles.GetX())) + player_position;

    BeginDrawing();
    {
        window.ClearBackground(raylib::BLACK);

        camera.BeginMode();
        {
            for (const auto &segment : map->getSegments()) {
                Entity *entity = world->getEntity(segment.id);

                if (entity) {
                    switch (entity->getType()) {
                        case SegmentType::Unknown:
                            DrawLineEx(raylib::Vector2(segment.x1, segment.y1) * 0.4 * scale, raylib::Vector2(segment.x2, segment.y2) * 0.4 * scale, scale, raylib::GRAY);
                            break;
                        case SegmentType::Wall:
                            DrawLineEx(raylib::Vector2(segment.x1, segment.y1) * 0.4 * scale, raylib::Vector2(segment.x2, segment.y2) * 0.4 * scale, scale, raylib::YELLOW);
                            break;
                        case SegmentType::Door:
                            DrawLineEx(raylib::Vector2(segment.x1, segment.y1) * 0.4 * scale, raylib::Vector2(segment.x2, segment.y2) * 0.4 * scale, scale, raylib::BLUE);
                            break;
                        case SegmentType::Monster:
                            DrawLineEx(raylib::Vector2(segment.x1, segment.y1) * 0.4 * scale, raylib::Vector2(segment.x2, segment.y2) * 0.4 * scale, scale, raylib::RED);
                            break;
                        case SegmentType::Item:
                            DrawLineEx(raylib::Vector2(segment.x1, segment.y1) * 0.4 * scale, raylib::Vector2(segment.x2, segment.y2) * 0.4 * scale, scale, raylib::WHITE);
                            break;
                        case SegmentType::Prop:
                            DrawLineEx(raylib::Vector2(segment.x1, segment.y1) * 0.4 * scale, raylib::Vector2(segment.x2, segment.y2) * 0.4 * scale, scale, raylib::GREEN);
                            break;
                        case SegmentType::Trap:
                            DrawLineEx(raylib::Vector2(segment.x1, segment.y1) * 0.4 * scale, raylib::Vector2(segment.x2, segment.y2) * 0.4 * scale, scale, raylib::PURPLE);
                            break;
                    }
                }
            }

            DrawLineEx(player_position * 0.4 * scale, player_position_angled * 0.4 * scale, scale, raylib::BLUE);
            DrawCircleV(player_position * 0.4 * scale, 0.4 * scale * 3, raylib::BLUE); 
        }
        camera.EndMode();
    }
    EndDrawing();
}

static void play_death_anim(Player *player, raylib::Window &window, const int scale) {
    DeathType death_type = player->getDeathType();

    static auto acid_anim = Animation("fli/acid.fli");
    static auto rifle_anim = Animation("fli/rflekill.fli", "sound/rifle.voc");
    static auto snare_anim = Animation("fli/snare.fli");
    static auto wire_anim = Animation("fli/wirekill.fli", "sound/explode.voc");
    static auto zombie_anim = Animation("fli/death.fli", "sound/aarh4.voc");

    BeginDrawing();
    {
        window.ClearBackground(raylib::BLACK);

        switch (death_type) {
            case DeathType::Acid:
                if (acid_anim.play(scale))
                    player->setState(State::Laugh);
                break;
            case DeathType::Rifle:
                if (rifle_anim.play(scale))
                    player->setState(State::Laugh);
                break;
            case DeathType::Snare:
                if (snare_anim.play(scale))
                    player->setState(State::Laugh);
                break;
            case DeathType::Wire:
                if (wire_anim.play(scale))
                    player->setState(State::Laugh);
                break;
            case DeathType::Zombie:
            default:
                if (zombie_anim.play(scale))
                    player->setState(State::Laugh);
                break;
        }
    }
    EndDrawing();
}

static void play_laugh_anim(Player *player, raylib::Window &window, const int scale) {
    static auto laugh_anim = Animation("fli/memnabha.fli", "sound/dielaugh.voc");

    BeginDrawing();
    {
        window.ClearBackground(raylib::BLACK);

        if (laugh_anim.play(scale)) {
            World *world = player->getWorld();
            world->setCurrentLevel("maps/01.map");
            auto spawn_point = world->findSpawn();

            player->setState(State::World);
            player->respawn(spawn_point);
        }
    }
    EndDrawing();
}

static void play_title_anim(Player *player, raylib::Window &window, const int scale, MusicPlayer *music_player) {
    static auto intro_anim = Animation("fli/dead.fli", "sound/laugh.voc");
    music_player->play("music/out3fm.mid");

    BeginDrawing();
    {
        window.ClearBackground(raylib::BLACK);

        if (intro_anim.play(scale)) {
            World *world = player->getWorld();
            world->setCurrentLevel("maps/01.map");
            auto spawn_point = world->findSpawn();
            player->setState(State::World);
            player->respawn(spawn_point, true);
        }
    }
    EndDrawing();
}

static void play_lab2_anim(Player *player, raylib::Window &window, const int scale, char anim_index) {
    static auto lab2a_anim = Animation("fli/babea.fli");
    static auto lab2b_anim = Animation("fli/babeb.fli");
    static auto lab2c_anim = Animation("fli/babec.fli");
    static auto lab2d_anim = Animation("fli/babed.fli");
    static auto lab2e_anim = Animation("fli/babee.fli");
    static auto lab2f_anim = Animation("fli/babef.fli");

    BeginDrawing();
    {
        window.ClearBackground(raylib::BLACK);

        bool anim_finished = false;
        switch (anim_index) {
            case 'A':
            case 'a':
                anim_finished = lab2a_anim.play(scale);
                break;
            case 'B':
            case 'b':
                anim_finished = lab2b_anim.play(scale);
                break;
            case 'C':
            case 'c':
                anim_finished = lab2c_anim.play(scale);
                break;
            case 'D':
            case 'd':
                anim_finished = lab2d_anim.play(scale);
                break;
            case 'E':
            case 'e':
                anim_finished = lab2e_anim.play(scale);
                break;
            case 'F':
            case 'f':
                anim_finished = lab2f_anim.play(scale);
                break;
        }

        if (anim_finished) {
            player->setState(State::Lab2);
        }
    }
    EndDrawing();
}

int main(int argc, char *argv[]) {
    cmdline::parser argparser;
    argparser.add<std::string>("datadir", 'd', "Data directory", false, "./");
    argparser.add<std::string>("map", 'm', "map file", false, "");
    argparser.add<int>("scale", 's', "render scale", false, 2);
    argparser.parse_check(argc, argv);

    SetTraceLogLevel(LOG_WARNING);

    std::string map_file = argparser.get<std::string>("map");
    std::string datadir = argparser.get<std::string>("datadir");
    const int scale = argparser.get<int>("scale");

    std::filesystem::current_path(datadir);

    SetConfigFlags(FLAG_MSAA_4X_HINT|FLAG_WINDOW_RESIZABLE);
    raylib::Window window(320*scale, 200*scale, "Isle of the Dead remake" + std::string(" (v") + std::string(VERSION) + ")");
    SetTargetFPS(60);

    raylib::AudioDevice audiodevice;

    raylib::Mouse::SetScale(1.0f/scale, 1.0f/scale);

    Fnt::ExtractFonts("system.fnt");
    Strings::Extract("iodex1.exe");
    Panel panel;

    MusicPlayer music_player;

    World world(&music_player, {
        LevelSettings("maps/01.map", Sky::Day, Ground::Dirt, "music/out4fm.mid"),
        LevelSettings("maps/02.map", Sky::Day, Ground::Dirt, "music/out4fm.mid"),
        LevelSettings("maps/03.map", Sky::Day, Ground::Dirt, "music/out4fm.mid"),
        LevelSettings("maps/04.map", Sky::Day, Ground::Dirt, "music/out4fm.mid"),
        LevelSettings("maps/05.map", Sky::Day, Ground::Dirt, "music/out4fm.mid"),
        LevelSettings("maps/06.map", Sky::Day, Ground::Dirt, "music/out4fm.mid"),
        LevelSettings("maps/07.map", Sky::Day, Ground::Dirt, "music/out4fm.mid"),
        LevelSettings("maps/08.map", Sky::Cave, Ground::Cave, "music/out3fm.mid"),
        LevelSettings("maps/09.map", Sky::Cave, Ground::Cave, "music/out3fm.mid"),
        LevelSettings("maps/10.map", Sky::Cave, Ground::Cave, "music/out3fm.mid"),
        LevelSettings("maps/11.map", Sky::Day, Ground::Dirt, "music/out4fm.mid"),
        LevelSettings("maps/12.map", Sky::Day, Ground::Dirt, "music/out4fm.mid"),
        LevelSettings("maps/13.map", Sky::Day, Ground::Dirt, "music/out4fm.mid"),
        LevelSettings("maps/14.map", Sky::Day, Ground::Dirt, "music/out4fm.mid"),
        LevelSettings("maps/15.map", Sky::Cave, Ground::Cave, "music/out3fm.mid"),
        LevelSettings("maps/16.map", Sky::Day, Ground::Dirt, "music/out4fm.mid"),
        LevelSettings("maps/17.map", Sky::Day, Ground::Dirt, "music/out4fm.mid"),
        LevelSettings("maps/18.map", Sky::Day, Ground::Dirt, "music/out4fm.mid"),
        LevelSettings("maps/19.map", Sky::Day, Ground::Dirt, "music/out4fm.mid"),
        LevelSettings("maps/20.map", Sky::Day, Ground::Dirt, "music/out4fm.mid"),
        LevelSettings("maps/21.map", Sky::Day, Ground::Dirt, "music/out4fm.mid"),
        LevelSettings("maps/22.map", Sky::Day, Ground::Dirt, "music/out4fm.mid"),
        LevelSettings("maps/23.map", Sky::Day, Ground::Dirt, "music/out4fm.mid"),
        LevelSettings("maps/24.map", Sky::Day, Ground::Dirt, "music/out4fm.mid"),
        LevelSettings("maps/25.map", Sky::Day, Ground::Dirt, "music/out4fm.mid"),
        LevelSettings("maps/26.map", Sky::Day, Ground::Dirt, "music/out4fm.mid"),
        LevelSettings("maps/27.map", Sky::Day, Ground::Dirt, "music/out4fm.mid"),
        LevelSettings("maps/28.map", Sky::Day, Ground::Dirt, "music/out4fm.mid"),
        LevelSettings("maps/29.map", Sky::Day, Ground::Dirt, "music/out4fm.mid"),
        LevelSettings("maps/30.map", Sky::Day, Ground::Dirt, "music/out4fm.mid"),
        LevelSettings("maps/31.map", Sky::Day, Ground::Dirt, "music/out4fm.mid"),
    }, "entrance.tbl");

    Inventory inventory(&panel);

    auto crashed_plane_entry_scene = std::make_unique<CrashedPlaneEntryScene>(&panel, world.getEntrance(5));
    auto crashed_plane_left_scene = std::make_unique<CrashedPlaneLeftScene>(&panel);
    auto crashed_plane_cockpit_scene = std::make_unique<CrashedPlaneCockpitScene>(&panel);
    auto crashed_plane_right_scene = std::make_unique<CrashedPlaneRightScene>(&panel);
    auto crashed_plane_exit_scene = std::make_unique<CrashedPlaneExitScene>(&panel, world.getEntrance(5));

    auto bunker_entry_scene = std::make_unique<BunkerEntryScene>(&panel, world.getEntrance(11));
    auto bunker_exit_scene = std::make_unique<BunkerExitScene>(&panel, world.getEntrance(11));
    auto bunker_left_scene = std::make_unique<BunkerLeftScene>(&panel);
    auto bunker_right_scene = std::make_unique<BunkerRightScene>(&panel);

    auto village_gate_chief_scene = std::make_unique<VillageGateChiefScene>(&panel, world.getEntrance(83));
    auto village_gate_shaman_scene = std::make_unique<VillageGateShamanScene>(&panel, world.getEntrance(100));
    auto village_eyes_scene = std::make_unique<VillageEyesScene>(&panel, Entrance("maps/23.map", 200, 330, Direction::WEST));
    auto toilet_scene = std::make_unique<ToiletScene>(&panel, Entrance("maps/23.map", 200, 330, Direction::WEST));
    auto shower_scene = std::make_unique<ShowerScene>(&panel, Entrance("maps/23.map", 200, 330, Direction::WEST));
    auto developers_scene = std::make_unique<DevelopersScene>(&panel, Entrance("maps/23.map", 200, 330, Direction::WEST));

    auto temple_entrance_scene = std::make_unique<TempleEntranceScene>(&panel, Entrance("maps/22.map", 200, 330, Direction::WEST));
    auto oracle_scene = std::make_unique<OracleScene>(&panel, Entrance("maps/22.map", 200, 330, Direction::WEST));

    auto rocket_launcher_scene = std::make_unique<RocketLauncherScene>(&panel, Entrance("maps/22.map", 200, 330, Direction::WEST));
    auto plane_cockpit_scene = std::make_unique<PlaneCockpitScene>(&panel, Entrance("maps/22.map", 200, 330, Direction::WEST));
    auto plane_galley_scene = std::make_unique<PlaneGalleyScene>(&panel);
    auto lab_zombie_scene = std::make_unique<LabZombieScene>(&panel, Entrance("maps/22.map", 200, 330, Direction::WEST));
    auto lab_companion_scene = std::make_unique<LabCompanionScene>(&panel, Entrance("maps/22.map", 200, 330, Direction::WEST));
    auto mirror_scene = std::make_unique<MirrorScene>(&panel, Entrance("maps/22.map", 200, 330, Direction::WEST));

    Player player(&world);

    //player.death(DeathType::Zombie);

    if (map_file.size()) {
        world.setCurrentLevel(map_file);
        auto spawn_point = world.findSpawn();

        player.setState(State::World);
        player.respawn(spawn_point, true);
        player.addItem(Item::Machete);
        player.addItem(Item::GoldMedal1);
        player.addItem(Item::GoldMedal2);
        player.addItem(Item::DeadWolf);
    } else {
        player.setState(State::Title);
    }

    player.setState(State::Lab2);
    //player.setState(State::TempleEntrance);

    while (!window.ShouldClose()) {
        uint64_t player_input = player.getInput();

        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            player_input |= Input::PrimaryAction;
        } else if (IsMouseButtonReleased(MOUSE_LEFT_BUTTON)) {
            player_input &= ~Input::PrimaryAction;
        }

        if (IsKeyPressed(KEY_SPACE)) {
            player_input |= Input::PrimaryAction;
        } else if (IsKeyReleased(KEY_SPACE)) {
            player_input &= ~Input::PrimaryAction;
        }

        if (IsKeyPressed(KEY_M)) {
            player_input |= Input::ViewMap;
        } else if (IsKeyReleased(KEY_M)) {
            player_input &= ~Input::ViewMap;
        }

        if (IsKeyPressed(KEY_I)) {
            player_input |= Input::ViewInventory;
        } else if (IsKeyReleased(KEY_I)) {
            player_input &= ~Input::ViewInventory;
        }

        if (IsKeyPressed(KEY_W)) {
            player_input |= Input::StepForward;
        } else if (IsKeyReleased(KEY_W)) {
            player_input &= ~Input::StepForward;
        }

        if (IsKeyPressed(KEY_S)) {
            player_input |= Input::StepBack;
        } else if (IsKeyReleased(KEY_S)) {
            player_input &= ~Input::StepBack;
        }

        if (IsKeyPressed(KEY_A)) {
            player_input |= Input::StepLeft;
        } else if (IsKeyReleased(KEY_A)) {
            player_input &= ~Input::StepLeft;
        }

        if (IsKeyPressed(KEY_D)) {
            player_input |= Input::StepRight;
        } else if (IsKeyReleased(KEY_D)) {
            player_input &= ~Input::StepRight;
        }

        if (IsKeyPressed(KEY_UP)) {
            player_input |= Input::LookUp;
        } else if (IsKeyReleased(KEY_UP)) {
            player_input &= ~Input::LookUp;
        }

        if (IsKeyPressed(KEY_DOWN)) {
            player_input |= Input::LookDown;
        } else if (IsKeyReleased(KEY_DOWN)) {
            player_input &= ~Input::LookDown;
        }

        if (IsKeyPressed(KEY_LEFT)) {
            player_input |= Input::TurnLeft;
        } else if (IsKeyReleased(KEY_LEFT)) {
            player_input &= ~Input::TurnLeft;
        }

        if (IsKeyPressed(KEY_RIGHT)) {
            player_input |= Input::TurnRight;
        } else if (IsKeyReleased(KEY_RIGHT)) {
            player_input &= ~Input::TurnRight;
        }

        if (IsKeyPressed(KEY_ONE)) {
            player_input |= Input::EquipMachete;
        } else if (IsKeyReleased(KEY_ONE)) {
            player_input &= ~Input::EquipMachete;
        }

        if (IsKeyPressed(KEY_TWO)) {
            player_input |= Input::EquipRifle;
        } else if (IsKeyReleased(KEY_TWO)) {
            player_input &= ~Input::EquipRifle;
        }

        if (IsKeyPressed(KEY_THREE)) {
            player_input |= Input::EquipShotgun;
        } else if (IsKeyReleased(KEY_THREE)) {
            player_input &= ~Input::EquipShotgun;
        }

        if (IsKeyPressed(KEY_FOUR)) {
            player_input |= Input::EquipUzi;
        } else if (IsKeyReleased(KEY_FOUR)) {
            player_input &= ~Input::EquipUzi;
        }

        player.setInput(player_input);

        switch (player.getState()) {
            case State::World: 
                draw_world(&player, &music_player, window, scale);
                break;

            case State::CrashedPlaneEntry:
                music_player.stop();
                crashed_plane_entry_scene->draw(&player, scale);
                break;
            case State::CrashedPlaneLeft:
                music_player.stop();
                crashed_plane_left_scene->draw(&player, scale);
                break;
            case State::CrashedPlaneCockpit:
                music_player.stop();
                crashed_plane_cockpit_scene->draw(&player, scale);
                break;
            case State::CrashedPlaneRight:
                music_player.stop();
                crashed_plane_right_scene->draw(&player, scale);
                break;
            case State::CrashedPlaneExit:
                music_player.stop();
                crashed_plane_exit_scene->draw(&player, scale);
                break;

            case State::BunkerEntry:
                music_player.stop();
                bunker_entry_scene->draw(&player, scale);
                break;
            case State::BunkerExit:
                music_player.stop();
                bunker_exit_scene->draw(&player, scale);
                break;
            case State::BunkerLeft:
                music_player.stop();
                bunker_left_scene->draw(&player, scale);
                break;
            case State::BunkerRight:
                bunker_right_scene->draw(&player, scale);
                break;

            case State::VillageGate1:
                music_player.stop();
                village_gate_shaman_scene->draw(&player, scale);
                break;
            case State::VillageGate2:
                music_player.stop();
                village_gate_chief_scene->draw(&player, scale);
                break;
            case State::VillageEyes:
                music_player.stop();
                village_eyes_scene->draw(&player, scale);
                break;
            case State::Toilet:
                music_player.stop();
                toilet_scene->draw(&player, scale);
                break;
            case State::Shower:
                music_player.stop();
                shower_scene->draw(&player, scale);
                break;
            case State::Developers:
                music_player.stop();
                developers_scene->draw(&player, scale);
                break;

            case State::TempleEntrance:
                music_player.stop();
                temple_entrance_scene->draw(&player, scale);
                break;
            case State::Oracle:
                music_player.stop();
                oracle_scene->draw(&player, scale);
                break;

            case State::RocketLauncher:
                music_player.stop();
                rocket_launcher_scene->draw(&player, scale);
                break;
            case State::PlaneCockpit:
                music_player.stop();
                plane_cockpit_scene->draw(&player, scale);
                break;
            case State::PlaneGalley:
                music_player.stop();
                plane_galley_scene->draw(&player, scale);
                break;

            case State::Lab1:
                music_player.stop();
                lab_zombie_scene->draw(&player, scale);
                break;
            case State::Lab2:
                music_player.stop();
                lab_companion_scene->draw(&player, scale);
                break;
            case State::Mirror:
                music_player.stop();
                mirror_scene->draw(&player, scale);
                break;

            case State::Lab2A:
                play_lab2_anim(&player, window, scale, 'A');
                break;
            case State::Lab2B:
                play_lab2_anim(&player, window, scale, 'B');
                break;
            case State::Lab2C:
                play_lab2_anim(&player, window, scale, 'C');
                break;
            case State::Lab2D:
                play_lab2_anim(&player, window, scale, 'D');
                break;
            case State::Lab2E:
                play_lab2_anim(&player, window, scale, 'E');
                break;
            case State::Lab2F:
                play_lab2_anim(&player, window, scale, 'F');
                break;

            case State::Title:
                play_title_anim(&player, window, scale, &music_player);
                break;
            case State::Dead:
                play_death_anim(&player, window, scale);
                break;
            case State::Laugh:
                play_laugh_anim(&player, window, scale);
                break;

            case State::Map:
                draw_map(&player, window, scale);
                break;
            case State::Inventory: 
                inventory.draw(&player, scale);
                break;
        }
    }

    exit (0);
}
