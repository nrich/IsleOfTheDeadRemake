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
    static auto bat_anim = Animation("fli/batkill.fli");
    static auto companion_anim = Animation("fli/zombabe.fli");
    static auto doc_anim = Animation("fli/memkill.fli");
    static auto fence_anim = Animation("fli/zap.fli", "sound/zap.voc");
    static auto launch_anim = Animation("fli/launch.fli");
    static auto missile_anim = Animation("fli/mslekill.fli");
    static auto nurse_anim = Animation("fli/bzap.fli", "sound/zap.voc");
    static auto plane_anim = Animation("fli/launch.fli");
    static auto rifle_anim = Animation("fli/rflekill.fli", "sound/rifle.voc");
    static auto snare_anim = Animation("fli/snare.fli");
    static auto suicide_anim = Animation("fli/suicide.fli");
    static auto wire_anim = Animation("fli/wirekill.fli", "sound/explode.voc");
    static auto zombie_anim = Animation("fli/death.fli", "sound/aarh4.voc");

    BeginDrawing();
    {
        window.ClearBackground(raylib::BLACK);

        bool anim_finished = false;
        switch (death_type) {
            case DeathType::Acid:
                anim_finished = acid_anim.play(scale);
                break;
            case DeathType::Bat:
                anim_finished = bat_anim.play(scale);
                break;
            case DeathType::Companion:
                anim_finished = companion_anim.play(scale);
                break;
            case DeathType::Doc:
                anim_finished = doc_anim.play(scale);
                break;
            case DeathType::Fence:
                anim_finished = fence_anim.play(scale);
                break;
            case DeathType::Launch:
                anim_finished = launch_anim.play(scale);
                break;
            case DeathType::Missile:
                anim_finished = missile_anim.play(scale);
                break;
            case DeathType::Nurse:
                anim_finished = nurse_anim.play(scale);
                break;
            case DeathType::Plane:
                anim_finished = plane_anim.play(scale);
                break;
            case DeathType::Rifle:
                anim_finished = rifle_anim.play(scale);
                break;
            case DeathType::Snare:
                anim_finished = snare_anim.play(scale);
                break;
            case DeathType::Suicide:
                anim_finished = suicide_anim.play(scale);
                break;
            case DeathType::Wire:
                anim_finished = wire_anim.play(scale);
                break;
            case DeathType::Zombie:
            default:
                anim_finished = zombie_anim.play(scale);
        }

        if (anim_finished)
            player->setState(State::Laugh);
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
    static raylib::Sound way_out_basement = raylib::Sound(Voc::Load("sound/wayout.voc"));
    static raylib::Sound help = raylib::Sound(Voc::Load("sound/helpbabe.voc"));

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
            switch (anim_index) {
                case 'F':
                case 'f':
                    way_out_basement.Play();
                    break;
                default:
                    help.Play();
                    break;
            }
            player->setState(State::Lab2);
        }
    }
    EndDrawing();
}

static void play_ending_anim(Player *player, raylib::Window &window, const int scale, int ending_index) {
    static auto the_end0_anim = Animation("fli/theend.fli");
    static auto the_end1_anim = Animation("fli/theend1.fli");
    static auto the_end2_anim = Animation("fli/theend2.fli");

    BeginDrawing();
    {
        window.ClearBackground(raylib::BLACK);

        bool anim_finished = false;
        switch (ending_index) {
            case 0:
                anim_finished = the_end0_anim.play(scale);
                break;
            case 1:
                anim_finished = the_end1_anim.play(scale);
                break;
            case 2:
                anim_finished = the_end2_anim.play(scale);
                break;
        }

        if (anim_finished) {
            player->setState(State::Title);
        }
    }
    EndDrawing();
}

static void play_doc_transform_anim(Player *player, raylib::Window &window, const int scale) {
    static auto transform_anim = Animation("fli/memgro.fli");

    BeginDrawing();
    {
        if (transform_anim.play(scale)) {
            player->setState(State::World);
        }
    }
    EndDrawing();
}

static void play_doc_die_anim(Player *player, raylib::Window &window, const int scale) {
    static auto die_anim = Animation("fli/membom.fli");
    static raylib::Sound countdown_sound = raylib::Sound(Voc::Load("sound/15min.voc"));

    BeginDrawing();
    {
        if (die_anim.play(scale)) {
            countdown_sound.Play();
            player->setState(State::World);
            player->setFlag(Flag::BombCountdown);
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
        LevelSettings("maps/19.map", Sky::Day, Ground::Dirt, "music/drum.mid"),
        LevelSettings("maps/20.map", Sky::Day, Ground::Dirt, "music/out4fm.mid"),
        LevelSettings("maps/21.map", Sky::Day, Ground::Dirt, "music/out4fm.mid"),
        LevelSettings("maps/22.map", Sky::Day, Ground::Dirt, "music/out4fm.mid"),
        LevelSettings("maps/23.map", Sky::Day, Ground::Dirt, "music/out4fm.mid"),
        LevelSettings("maps/24.map", Sky::Day, Ground::Dirt, "music/out4fm.mid"),
        LevelSettings("maps/25.map", Sky::Mansion, Ground::Carpet, "music/out1fm.mid"),
        LevelSettings("maps/26.map", Sky::Mansion, Ground::Carpet, "music/out1fm.mid"),
        LevelSettings("maps/27.map", Sky::Basement, Ground::Basement, "music/out4fm.mid"),
        LevelSettings("maps/28.map", Sky::Cave, Ground::Cave, "music/out4fm.mid"),
        LevelSettings("maps/29.map", Sky::Day, Ground::Dirt, "music/out4fm.mid"),
        LevelSettings("maps/30.map", Sky::Day, Ground::Sea, "music/out4fm.mid"),
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

    auto village1_eyes_lu_scene = std::make_unique<VillageEyesScene>(&panel, Entrance("maps/23.map", 220, 170, Direction::WEST));
    auto toilet_scene = std::make_unique<ToiletScene>(&panel, Entrance("maps/23.map", 220, 250, Direction::WEST));
    auto village1_eyes_ld_scene = std::make_unique<VillageEyesScene>(&panel, Entrance("maps/23.map", 220, 330, Direction::WEST));
    auto village1_eyes_ru_scene = std::make_unique<VillageEyesScene>(&panel, Entrance("maps/23.map", 300, 170, Direction::EAST));
    auto village1_eyes_r_scene = std::make_unique<VillageEyesScene>(&panel, Entrance("maps/23.map", 300, 250, Direction::EAST));
    auto village1_eyes_rd_scene = std::make_unique<VillageEyesScene>(&panel, Entrance("maps/23.map", 300, 330, Direction::EAST));
    auto shaman_scene = std::make_unique<ShamanScene>(&panel, Entrance("maps/23.map", 250, 280, Direction::NORTH));

    auto village2_eyes_lu_scene = std::make_unique<VillageEyesScene>(&panel, Entrance("maps/19.map", 150, 190, Direction::WEST));
    auto village2_eyes_l_scene = std::make_unique<VillageEyesScene>(&panel, Entrance("maps/19.map", 130, 280, Direction::WEST));
    auto shower_scene = std::make_unique<ShowerScene>(&panel, Entrance("maps/19.map", 110, 320, Direction::NORTH));
    auto village2_eyes_ru_scene = std::make_unique<VillageEyesScene>(&panel, Entrance("maps/19.map", 220, 160, Direction::EAST));
    auto developers_scene = std::make_unique<DevelopersScene>(&panel, Entrance("maps/19.map", 230, 240, Direction::EAST));
    auto village2_eyes_rd_scene = std::make_unique<VillageEyesScene>(&panel, Entrance("maps/19.map", 260, 320, Direction::EAST));
    auto chief_scene = std::make_unique<ChiefScene>(&panel, Entrance("maps/19.map", 180, 150, Direction::SOUTH));

    auto temple_entrance_scene = std::make_unique<TempleEntranceScene>(&panel, Entrance("maps/22.map", 200, 330, Direction::EAST));
    auto oracle_scene = std::make_unique<OracleScene>(&panel, Entrance("maps/22.map", 200, 330, Direction::EAST));

    auto rocket_launcher_scene = std::make_unique<RocketLauncherScene>(&panel, Entrance("maps/18.map", 140, 390, Direction::EAST));
    auto plane_cockpit_scene = std::make_unique<PlaneCockpitScene>(&panel, Entrance("maps/18.map", 140, 130, Direction::NORTH));
    auto plane_galley_scene = std::make_unique<PlaneGalleyScene>(&panel);
    auto lab_zombie_scene = std::make_unique<LabZombieScene>(&panel, world.getEntrance(104));
    auto lab_companion_scene = std::make_unique<LabCompanionScene>(&panel, world.getEntrance(112));
    auto mirror_scene = std::make_unique<MirrorScene>(&panel, Entrance("maps/26.map", 250, 160, Direction::WEST));

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
        player.addItem(Item::Book);
        player.addItem(Item::Smokes);
        player.addItem(Item::Shotgun);
        player.addItem(Item::Companion);
        player.addItem(Item::Drug);
        player.addItem(Item::Antidote);
        player.addItem(Item::WireClipper);
    } else {
        player.setState(State::Title);
    }

    //player.setState(State::Shaman);
    //player.setState(State::TempleEntrance);
    //player.setState(State::PlaneCockpit);
    //player.setState(State::RocketLauncher);
    //player.setState(State::Oracle);
    player.setState(State::VillageGate2);
    player.setFlag(Flag::VisitedVillage);

    while (!window.ShouldClose()) {
        uint64_t player_input = player.getInput();

        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            player_input |= Input::PrimaryAction;
        } else if (IsMouseButtonReleased(MOUSE_LEFT_BUTTON)) {
            player_input &= ~Input::PrimaryAction;
        }

        if (IsMouseButtonPressed(MOUSE_RIGHT_BUTTON)) {
            player_input |= Input::Use;
        } else if (IsMouseButtonReleased(MOUSE_RIGHT_BUTTON)) {
            player_input &= ~Input::Use;
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

        if (IsKeyPressed(KEY_E)) {
            player_input |= Input::Use;
        } else if (IsKeyReleased(KEY_E)) {
            player_input &= ~Input::Use;
        }

        if (IsKeyPressed(KEY_Q)) {
            std::cerr << "POSITION " << player.getPosition().ToString() << "\n";
        }

        player.setInput(player_input);

        if (player.showInventory()) {
            inventory.draw(&player, scale);
            continue;
        }

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

            case State::Village1EyesLU:
                music_player.stop();
                village1_eyes_lu_scene->draw(&player, scale);
                break;
            case State::Toilet:
                music_player.stop();
                toilet_scene->draw(&player, scale);
                break;
            case State::Village1EyesLD:
                music_player.stop();
                village1_eyes_ld_scene->draw(&player, scale);
                break;
            case State::Village1EyesRU:
                music_player.stop();
                village1_eyes_ru_scene->draw(&player, scale);
                break;
            case State::Village1EyesR:
                music_player.stop();
                village1_eyes_r_scene->draw(&player, scale);
                break;
            case State::Village1EyesRD:
                music_player.stop();
                village1_eyes_rd_scene->draw(&player, scale);
                break;

            case State::Village2EyesLU:
                music_player.stop();
                village2_eyes_lu_scene->draw(&player, scale);
                break;
            case State::Village2EyesL:
                music_player.stop();
                village2_eyes_l_scene->draw(&player, scale);
                break;
            case State::Shower:
                music_player.stop();
                shower_scene->draw(&player, scale);
                break;
            case State::Village2EyesRU:
                music_player.stop();
                village2_eyes_ru_scene->draw(&player, scale);
                break;
            case State::Developers:
                music_player.stop();
                developers_scene->draw(&player, scale);
                break;
            case State::Village2EyesRD:
                music_player.stop();
                village2_eyes_rd_scene->draw(&player, scale);
                break;

            case State::Shaman:
                music_player.stop();
                shaman_scene->draw(&player, scale);
                break;
            case State::Chief:
                music_player.stop();
                chief_scene->draw(&player, scale);
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

            case State::DocTransform:
                play_doc_transform_anim(&player, window, scale);
                break;

            case State::DocDie:
                play_doc_die_anim(&player, window, scale);
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

            case State::Ending:
                play_ending_anim(&player, window, scale, 0);
                break;
            case State::Ending1:
                play_ending_anim(&player, window, scale, 1);
                break;
            case State::Ending2:
                play_ending_anim(&player, window, scale, 2);
                break;

            case State::Map:
                draw_map(&player, window, scale);
                break;
        }
    }

    exit (0);
}
