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

#include <iostream>
#include <algorithm>
#include <functional>

#include "Entrance.h"
#include "Map.h"
#include "World.h"
#include "Palette.h"
#include "CelThree.h"
#include "Entity.h"
#include "Monster.h"

static std::unordered_map<std::string, raylib::TextureUnmanaged> cel3_texture;

static raylib::TextureUnmanaged load_cel3_texture(const std::string &filename) {
    static Palette palette("cels3/palette.pal");

    if (cel3_texture.contains(filename))
        return cel3_texture[filename];

    const auto cel3 = CelThree(filename, palette);
    auto texture = cel3.getTexture();

    cel3_texture[filename] = texture;

    return texture;
}

static std::vector<raylib::TextureUnmanaged> load_cel3_texture(const std::vector<std::string> &filenames) {
    std::vector<raylib::TextureUnmanaged> textures;

    for (const auto &filename : filenames) {
        auto texture = load_cel3_texture(filename);
        textures.push_back(texture);
    }

    return textures;
}

World::World(MusicPlayer *music_player, const std::vector<LevelSettings> &level_settings_list, const std::string &entrance_filename) : musicPlayer(music_player) {
    entrances = Entrance::Parse(entrance_filename); 

    for (const auto &level_settings : level_settings_list) {
        auto level_data = levels.emplace(level_settings.filename, Level(level_settings));

        std::cout << level_settings.filename << "\n";

        if (level_data.second) {
            auto level = level_data.first;
            for (const auto &segment : level->second.getMap()->getSegments()) {
                spawnEntityForSegment(level_settings.filename, segment);
            }
        }
    }

    currentMap = level_settings_list[0].filename;
}

void World::spawnPassage(const Segment &segment) {
    static const auto tree_closed_entry = load_cel3_texture("cels3/rent1.cel");
    static const auto tree_opened_entry = load_cel3_texture("cels3/jtocave.cel");

    static const auto cave_entry = load_cel3_texture("cels3/cav2cave.cel");
    static const auto jungle_entry = load_cel3_texture("cels3/cavetoj.cel");

//    static const auto temple_door = load_cel3_texture("cels3/tmpdoor1.cel");

    switch (segment.id) {
        // 01.map
        case 0x20a8dba56cca7792:
            // 02.map
            entities.emplace(segment.id, std::make_unique<Barricade>(&segment, tree_closed_entry, tree_opened_entry, entrances[7], DamageType::Machete));
            break;
        case 0x21e8dae56d8a76d2:
            // 05.map
            entities.emplace(segment.id, std::make_unique<Barricade>(&segment, tree_closed_entry, tree_opened_entry, entrances[22], DamageType::Machete));
            break;
        case 0x20dadbaf6ca27786:
            // 06.map
            entities.emplace(segment.id, std::make_unique<Barricade>(&segment, tree_closed_entry, tree_opened_entry, entrances[30], DamageType::Machete));
            break;

        // 02.map
        case 0x6651cdbd548a56e7:
            // 01.map
            entities.emplace(segment.id, std::make_unique<Barricade>(&segment, tree_closed_entry, tree_opened_entry, entrances[4], DamageType::Machete));
            //break;
        case 0x67e9ccd555045785:
            // 03.map
            entities.emplace(segment.id, std::make_unique<Barricade>(&segment, tree_closed_entry, tree_opened_entry, entrances[13], DamageType::Machete));
            break;
        case 0x6629cd9554c456c5:
            // 13.map
            entities.emplace(segment.id, std::make_unique<Barricade>(&segment, tree_closed_entry, tree_opened_entry, entrances[59], DamageType::Machete));
            break;
        case 0x6705cc8755e657dd:
            // 14.map
            entities.emplace(segment.id, std::make_unique<Barricade>(&segment, tree_closed_entry, tree_opened_entry, entrances[63], DamageType::Machete));
            break;

        // 03.map
        case 0x7b4fdbfa39fe6beb:
            // 02.map
            entities.emplace(segment.id, std::make_unique<Barricade>(&segment, tree_closed_entry, tree_opened_entry, entrances[10], DamageType::Machete));
            break;
        case 0x7a27da9238a06a9d:
            // 04.map
            entities.emplace(segment.id, std::make_unique<Barricade>(&segment, tree_closed_entry, tree_opened_entry, entrances[18], DamageType::Machete));
            break;
        case 0x7b5bdbdc39f46bd3:
            // 16.map
            entities.emplace(segment.id, std::make_unique<Barricade>(&segment, tree_closed_entry, tree_opened_entry, entrances[70], DamageType::Machete));
            break;
        case 0x7a79da9c38c86a89:
            // 17.map
            entities.emplace(segment.id, std::make_unique<Barricade>(&segment, tree_closed_entry, tree_opened_entry, entrances[70], DamageType::Machete));
            break;

        // 04.map
        case 0x2eb12e47b2f2ba92:
            // 03.map
            entities.emplace(segment.id, std::make_unique<Barricade>(&segment, tree_closed_entry, tree_opened_entry, entrances[16], DamageType::Machete));
            break;
        case 0x2eef2e59b2b6ba86:
            // 20.map
            entities.emplace(segment.id, std::make_unique<Barricade>(&segment, tree_closed_entry, tree_opened_entry, entrances[88], DamageType::Machete));
            break;
        case 0x2fc72f71b3a8bba4:
            // 21.map
            entities.emplace(segment.id, std::make_unique<Barricade>(&segment, tree_closed_entry, tree_opened_entry, entrances[93], DamageType::Machete));
            break;

        // 05.map
        case 0x6a7aeeeaa11d2129:
            // 01.map
            entities.emplace(segment.id, std::make_unique<Barricade>(&segment, tree_closed_entry, tree_opened_entry, entrances[2], DamageType::Machete));
            break;
        case 0x6bdaefcea091200d:
            // 06.map
            entities.emplace(segment.id, std::make_unique<Barricade>(&segment, tree_closed_entry, tree_opened_entry, entrances[28], DamageType::Machete));
            break;
        case 0x6a4ceff0a12b2033:
            // 07.map
            entities.emplace(segment.id, std::make_unique<Barricade>(&segment, tree_closed_entry, tree_opened_entry, entrances[34], DamageType::Machete));
            break;
        case 0x6bceef00a09120c3:
            // 08.map
            entities.emplace(segment.id, std::make_unique<Passage>(&segment, cave_entry, entrances[40]));
            break;

        // 06.map
        case 0x6b58b8342e63d32e:
            // 01.map
            entities.emplace(segment.id, std::make_unique<Barricade>(&segment, tree_closed_entry, tree_opened_entry, entrances[3], DamageType::Machete));
            break;
        case 0x6a70b91e2f51d21e:
            // 05.map
            entities.emplace(segment.id, std::make_unique<Barricade>(&segment, tree_closed_entry, tree_opened_entry, entrances[25], DamageType::Machete));
            break;
        case 0x6b8eb83a2eb5d340:
            // 07.map
            entities.emplace(segment.id, std::make_unique<Barricade>(&segment, tree_closed_entry, tree_opened_entry, entrances[35], DamageType::Machete));
            break;
        case 0x6bcab8a62ef1d390:
            // 10.map
            entities.emplace(segment.id, std::make_unique<Passage>(&segment, cave_entry, entrances[47]));
            break;
        case 0x6bf8b8902ec3d3a6:
            // 11.map
            entities.emplace(segment.id, std::make_unique<Barricade>(&segment, tree_closed_entry, tree_opened_entry, entrances[52], DamageType::Machete));
            break;

        // 07.map
        case 0xe151a5cc1050d739:
            // 05.map
            entities.emplace(segment.id, std::make_unique<Barricade>(&segment, tree_closed_entry, tree_opened_entry, entrances[26], DamageType::Machete));
            break;
        case 0xe06da4141156d6ef:
            // 06.map
            entities.emplace(segment.id, std::make_unique<Barricade>(&segment, tree_closed_entry, tree_opened_entry, entrances[29], DamageType::Machete));
            break;
        case 0xe0a1a4321180d6c7:
            // 11.map
            entities.emplace(segment.id, std::make_unique<Barricade>(&segment, tree_closed_entry, tree_opened_entry, entrances[51], DamageType::Machete));
            break;
        case 0xe139a4381002d6c7:
            // 21.map
            entities.emplace(segment.id, std::make_unique<Barricade>(&segment, tree_closed_entry, tree_opened_entry, entrances[94], DamageType::Machete));
            break;

        // 08.map
        case 0xe69efdb1ef0bbd7a:
            // 05.map
            entities.emplace(segment.id, std::make_unique<Barricade>(&segment, jungle_entry, cave_entry, entrances[23], DamageType::Machete));
            break;
        case 0xe61cfc39ef87bcc8:
            // 09.map
            entities.emplace(segment.id, std::make_unique<Passage>(&segment, cave_entry, entrances[44]));
            break;
        case 0xe6cefd97ef5bbd5c:
            // 10.map
            entities.emplace(segment.id, std::make_unique<Passage>(&segment, cave_entry, entrances[48]));
            break;

        // 09.map
        case 0x3b9232a816164198:
            // 08.map
            entities.emplace(segment.id, std::make_unique<Barricade>(&segment, jungle_entry, cave_entry, entrances[41], DamageType::Machete));
            break;
        case 0x3b2c33ea169e40e0:
            // 10,map
            entities.emplace(segment.id, std::make_unique<Passage>(&segment, cave_entry, entrances[49]));
            break;

        // 10.map
        case 0x2c6b2a6405ab26b2:
            // 06.map
            entities.emplace(segment.id, std::make_unique<Barricade>(&segment, jungle_entry, cave_entry, entrances[32], DamageType::Machete));
            break;
        case 0x2ce32b3e05192712:
            // 08.map
            entities.emplace(segment.id, std::make_unique<Passage>(&segment, cave_entry, entrances[42]));
            break;
        case 0x2c2d2aec05db26c0:
            // 09.map
            entities.emplace(segment.id, std::make_unique<Passage>(&segment, cave_entry, entrances[45]));
            break;

        // 11.map
        case 0x16e971ccb01c0d9c:
            // 06.map
            entities.emplace(segment.id, std::make_unique<Barricade>(&segment, tree_closed_entry, tree_opened_entry, entrances[31], DamageType::Machete));
            break;
        case 0x162b7144b0e40dee:
            // 07.map
            entities.emplace(segment.id, std::make_unique<Barricade>(&segment, tree_closed_entry, tree_opened_entry, entrances[36], DamageType::Machete));
            break;
        case 0x176771c2b1920d72:
            // 07.map
            entities.emplace(segment.id, std::make_unique<Barricade>(&segment, tree_closed_entry, tree_opened_entry, entrances[37], DamageType::Machete));
            break;
        case 0x163f71a8b0ca0d14:
            // 12.map
            entities.emplace(segment.id, std::make_unique<Barricade>(&segment, tree_closed_entry, tree_opened_entry, entrances[56], DamageType::Machete));
            break;

        // 12.map
        case 0xcf604ea6aea76e9d:
            // 11.map
            entities.emplace(segment.id, std::make_unique<Barricade>(&segment, tree_closed_entry, tree_opened_entry, entrances[54], DamageType::Machete));
            break;

        // 13.map
        case 0x52482393c6e3e3d0:
            // 02.map
            entities.emplace(segment.id, std::make_unique<Barricade>(&segment, tree_closed_entry, tree_opened_entry, entrances[8], DamageType::Machete));
            break;
        case 0x52e222e5c649e2b6:
            // 14.map
            entities.emplace(segment.id, std::make_unique<Barricade>(&segment, tree_closed_entry, tree_opened_entry, entrances[64], DamageType::Machete));
            break;
        case 0x52ae2393c605e324:
            // 14.map
            entities.emplace(segment.id, std::make_unique<Barricade>(&segment, tree_closed_entry, tree_opened_entry, entrances[65], DamageType::Machete));
            break;
        case 0x52122245c6b9e216:
            // 15.map
            entities.emplace(segment.id, std::make_unique<Passage>(&segment, cave_entry, entrances[67]));
            break;

        // 14.map
        case 0xb9047aac95a6f43d:
            // 02.map
            entities.emplace(segment.id, std::make_unique<Barricade>(&segment, tree_closed_entry, tree_opened_entry, entrances[9], DamageType::Machete));
            break;
        case 0xb8d27ab8948af437:
            // 13.map
            entities.emplace(segment.id, std::make_unique<Barricade>(&segment, tree_closed_entry, tree_opened_entry, entrances[60], DamageType::Machete));
            break;
        case 0xb89e7b2a94c6f541:
            // 13.map
            entities.emplace(segment.id, std::make_unique<Barricade>(&segment, tree_closed_entry, tree_opened_entry, entrances[61], DamageType::Machete));
            break;
        case 0xb94e7a189516f497:
            // 15.map
            entities.emplace(segment.id, std::make_unique<Passage>(&segment, cave_entry, entrances[68]));
            break;

        // 15.map
        case 0x29ffb0a225bdc9f2:
            // 13.map
            entities.emplace(segment.id, std::make_unique<Passage>(&segment, cave_entry, Entrance("maps/13.map", 80, 170, 270)));
            break;
        case 0x28d1b0942493c9c4:
            // 14.map
            entities.emplace(segment.id, std::make_unique<Passage>(&segment, cave_entry, Entrance("maps/14.map", 320, 170, 90)));
            break;
 
        // 16.map
        case 0x867b5289b921129:
            // 03.map
            entities.emplace(segment.id, std::make_unique<Barricade>(&segment, tree_closed_entry, tree_opened_entry, entrances[14], DamageType::Machete));
            break;
        case 0x85bb5fe9b9211ff:
            // 29.map
            entities.emplace(segment.id, std::make_unique<Barricade>(&segment, tree_closed_entry, tree_opened_entry, entrances[122], DamageType::Machete));
            break;

        // 17.map
        case 0x64371b1fc0564805:
            // 03.map
            entities.emplace(segment.id, std::make_unique<Barricade>(&segment, tree_closed_entry, tree_opened_entry, entrances[15], DamageType::Machete));
            break;
        case 0x65331bf3c1444813:
            // 20.map
            entities.emplace(segment.id, std::make_unique<Barricade>(&segment, tree_closed_entry, tree_opened_entry, entrances[89], DamageType::Machete));
            break;

        // 18.map
        case 0xdfae5cff3e42436e:
            // 16.map
            break;
        case 0xdfe25c8d3e74432a:
            // 17.map
            break;

        // 19.map
        case 0xe902a21ddb690d8a:
            // 17.map
            entities.emplace(segment.id, std::make_unique<Passage>(&segment, cave_entry, entrances[75]));
            break;

        // 20.map
        case 0x8ff8e10da10d8828:
            // 04.map
            entities.emplace(segment.id, std::make_unique<Barricade>(&segment, tree_closed_entry, tree_opened_entry, entrances[19], DamageType::Machete));
            break;
        case 0x8e4ce113a0a3883c:
            // 17.map
            entities.emplace(segment.id, std::make_unique<Barricade>(&segment, tree_closed_entry, tree_opened_entry, entrances[77], DamageType::Machete));
            break;
        case 0x8e12e12ba0e7881e:
            // 21.map
            entities.emplace(segment.id, std::make_unique<Barricade>(&segment, tree_closed_entry, tree_opened_entry, entrances[95], DamageType::Machete));
            break;

        // 21.map
        case 0xddbbb06349f2ef08:
            // 04.map
            entities.emplace(segment.id, std::make_unique<Barricade>(&segment, tree_closed_entry, tree_opened_entry, entrances[20], DamageType::Machete));
            break;
        case 0xddafb1bf49eceede:
            // 07.map
            entities.emplace(segment.id, std::make_unique<Barricade>(&segment, tree_closed_entry, tree_opened_entry, entrances[38], DamageType::Machete));
            break;
        case 0xdd11b1cd4952eebc:
            // 20.map
            entities.emplace(segment.id, std::make_unique<Barricade>(&segment, tree_closed_entry, tree_opened_entry, entrances[90], DamageType::Machete));
            break;
        case 0xddddb1814964eeea:
            // 22.map
            entities.emplace(segment.id, std::make_unique<Barricade>(&segment, tree_closed_entry, tree_opened_entry, entrances[98], DamageType::Machete));
            break;

        // 22.map
        case 0x10308b1b23661b81:
            // 21.map
            entities.emplace(segment.id, std::make_unique<Barricade>(&segment, tree_closed_entry, tree_opened_entry, entrances[96], DamageType::Machete));
            break;

        // 28.map
            // 27.map
            entities.emplace(segment.id, std::make_unique<Barricade>(&segment, jungle_entry, cave_entry, entrances[112], DamageType::Machete));
            break;

        // 29.map
        case 0x1db0d5af45726024:
            // 16.map
            entities.emplace(segment.id, std::make_unique<Barricade>(&segment, tree_closed_entry, tree_opened_entry, entrances[72], DamageType::Machete));
            break;
    }
}

void World::spawnEntityForSegment(const std::string &map_filename, const Segment &segment) {
    static const auto beach = load_cel3_texture({
        "cels3/beach1.cel",
        "cels3/beach2.cel",
        "cels3/beach3.cel",
        "cels3/beach2.cel",
    });

    // Walls
    static const auto trees1 = load_cel3_texture("cels3/rtrees1.cel");
    static const auto trees2 = load_cel3_texture("cels3/rtrees2.cel");

    static const auto rcave1 = load_cel3_texture("cels3/rcave1.cel");
    static const auto rcave2 = load_cel3_texture("cels3/rcave2.cel");
    static const auto rcave3 = load_cel3_texture("cels3/rcave3.cel");
    static const auto rcave4 = load_cel3_texture("cels3/rcave4.cel");

    static const auto hut_wall_north = load_cel3_texture("cels3/nhut.cel");
    static const auto hut_wall_west = load_cel3_texture("cels3/whut.cel");
    static const auto hut_wall_east = load_cel3_texture("cels3/ehut.cel");
    static const auto hut_wall_south = load_cel3_texture("cels3/shut.cel");

    static const auto boowall_north = load_cel3_texture("cels3/nboowall.cel");
    static const auto boowall_west = load_cel3_texture("cels3/wboowall.cel");
    static const auto boowall_east = load_cel3_texture("cels3/eboowall.cel");
    static const auto boowall_south = load_cel3_texture("cels3/sboowall.cel");

    static const auto boogate = load_cel3_texture("cels3/boogate.cel");

    static const auto mansion_window_light = load_cel3_texture("cels3/mansext1.cel");
    static const auto mansion_window_dark = load_cel3_texture("cels3/mansext2.cel");
    static const auto mansion_wall_light = load_cel3_texture("cels3/mansext7.cel");
    static const auto mansion_wall_dark = load_cel3_texture("cels3/mansext8.cel");

    static const auto hanger_dark = load_cel3_texture("cels3/hngar01.cel");
    static const auto hanger_light = load_cel3_texture("cels3/hngar02.cel");

    static const auto compound_wall_light = load_cel3_texture("cels3/compnd51.cel");
    static const auto compound_wall_dark = load_cel3_texture("cels3/compnd53.cel");
    static const auto compound_window_light = load_cel3_texture("cels3/compnd54.cel");
    static const auto compound_window_dark = load_cel3_texture("cels3/compnd55.cel");

    static const auto husk1 = load_cel3_texture("cels3/husk1.cel");
    static const auto husk2 = load_cel3_texture("cels3/husk2.cel");
    static const auto husk3 = load_cel3_texture("cels3/husk3.cel");

    static const auto temple_wall_light = load_cel3_texture("cels3/tmpl2.cel");
    static const auto temple_wall_dark = load_cel3_texture("cels3/tmpl4.cel");
    static const auto temple_face_left = load_cel3_texture("cels3/tmpltex1.cel");
    static const auto temple_face_mid = load_cel3_texture("cels3/tmpltex2.cel");
    static const auto temple_face_right = load_cel3_texture("cels3/tmpltex3.cel");

    static const auto shack1 = load_cel3_texture("cels3/shack1.cel");
    static const auto shack2 = load_cel3_texture("cels3/shack2.cel");
    static const auto shack3 = load_cel3_texture("cels3/shack3.cel");

    static const auto fence = load_cel3_texture("cels3/fence.cel");
    static const auto jungle_fence = load_cel3_texture("cels3/jfence.cel");

    static const auto runway = load_cel3_texture("cels3/runway.cel");
    static const auto plane_fence_left = load_cel3_texture("cels3/fencepl1.cel");
    static const auto plane_fence_right = load_cel3_texture("cels3/fencepl2.cel");

    // Damageable/animated walls
    static const auto fireplace_left = load_cel3_texture({
        "cels3/firepl1a.cel",
        "cels3/firepl1b.cel",
        "cels3/firepl1c.cel",
    });

    static const auto fireplace_right = load_cel3_texture({
        "cels3/firepl2a.cel",
        "cels3/firepl2b.cel",
        "cels3/firepl2c.cel",
    });

    const auto bookshelf = load_cel3_texture("cels3/frnture4.cel");
    const auto bookshelf_damaged = load_cel3_texture("cels3/frnture3.cel");

    const auto tv = load_cel3_texture("cels3/frnture7.cel");
    const auto tv_damaged = load_cel3_texture("cels3/frnture7.cel");

    const auto portrait = load_cel3_texture("cels3/frnture5.cel");
    const auto portrait_damaged = load_cel3_texture("cels3/frnture6.cel");

    // Doors,passages,entries
    static const auto bunker_entry_closed = load_cel3_texture("cels3/bunkent1.cel");
    static const auto bunker_entry_opened = load_cel3_texture("cels3/bunkent2.cel");

    static const auto temple_door = load_cel3_texture({
        "cels3/tmpdoor1.cel",
        "cels3/tmpdoor2.cel",
        "cels3/tmpdoor3.cel",
        "cels3/tmpdoor4.cel",
    });

    static const auto plane_fire_left = load_cel3_texture({
        "cels3/fire1a.cel",
        "cels3/fire2a.cel",
        "cels3/fire3a.cel",
    });

    static const auto plane_fire_mid = load_cel3_texture({
        "cels3/fire1b.cel",
        "cels3/fire2b.cel",
        "cels3/fire3b.cel",
    });

    static const auto plane_fire_right = load_cel3_texture({
        "cels3/fire1c.cel",
        "cels3/fire2c.cel",
        "cels3/fire3c.cel",
    });

    const auto camp_gate_open = load_cel3_texture("cels3/cmpgate3.cel");

    // Items
    static const auto jacket = load_cel3_texture("cels3/jacket.cel");
    static const auto coconut = load_cel3_texture("cels3/food2c1.cel");
    static const auto banana = load_cel3_texture("cels3/food3c1.cel");
    static const auto aid_kit = load_cel3_texture("cels3/aidkitc1.cel");
    static const auto ammo_shells = load_cel3_texture("cels3/ammo1c1.cel");
    static const auto ammo_bullets = load_cel3_texture("cels3/ammo2c1.cel");
    static const auto ammo_clips = load_cel3_texture("cels3/ammo3c1.cel");
    static const auto shotgun = load_cel3_texture("cels3/sgunc1.cel");
    static const auto flower = load_cel3_texture("cels3/flowerc1.cel");
    static const auto crystal = load_cel3_texture("cels3/crystalc.cel");

    // Props
    static const auto trees = load_cel3_texture("cels3/rtrees.cel");
    static const auto barrel = load_cel3_texture("cels3/barrel.cel");
    static const auto bed = load_cel3_texture("cels3/bed.cel");
    static const auto dish = load_cel3_texture("cels3/dish.cel");
    static const auto driftwood = load_cel3_texture("cels3/driftwd.cel");
    static const auto labsink = load_cel3_texture("cels3/labsink.cel");
    static const auto labtable = load_cel3_texture("cels3/labtable.cel");
    static const auto table = load_cel3_texture("cels3/table.cel");
    static const auto vine = load_cel3_texture("cels3/vine.cel");
    static const auto tiki = load_cel3_texture({
        "cels3/tiki1.cel",
        "cels3/tiki2.cel",
        "cels3/tiki3.cel",
        "cels3/tiki4.cel",
        "cels3/tiki5.cel",
        "cels3/tiki6.cel",
    });

    const auto red_chair = load_cel3_texture("cels3/frnture2.cel");
    const auto red_chair_damaged = load_cel3_texture("cels3/frnture1.cel");

    // Traps
    static const auto pit = load_cel3_texture("cels3/pit.cel");
    static const auto snare = load_cel3_texture("cels3/snare.cel");
    static const auto trapdoor = load_cel3_texture("cels3/trapdoor.cel");

    // Monsters
    static const auto bat = load_cel3_texture({
        "cels3/rbat01.cel",
        "cels3/rbat02.cel",
        "cels3/rbat03.cel",
        "cels3/rbat04.cel",
        "cels3/rbat05.cel",
        "cels3/rbat06.cel",
        "cels3/rbat07.cel",
        "cels3/rbat08.cel",
        "cels3/rbat09.cel",
        "cels3/rbat10.cel",
        "cels3/rbat11.cel",
        "cels3/rbat12.cel",
        "cels3/rbat13.cel",
        "cels3/rbat14.cel",
        "cels3/rbat15.cel",
        "cels3/rbat16.cel",
        "cels3/rbat17.cel",
        "cels3/rbat18.cel",
        "cels3/rbat19.cel",
        "cels3/rbat20.cel",
        "cels3/rbat21.cel",
        "cels3/rbat22.cel",
        "cels3/rbat23.cel",
        "cels3/rbat24.cel",
        "cels3/rbat25.cel",
        "cels3/rbat26.cel",
        "cels3/rbat27.cel",
        "cels3/rbat28.cel",
        "cels3/rbat29.cel",
        "cels3/rbat30.cel",
        "cels3/rbat31.cel",
        "cels3/rbat32.cel",
        "cels3/rbat33.cel",
        "cels3/rbat34.cel",
        "cels3/rbat35.cel",
        "cels3/rbat36.cel",
        "cels3/rbat37.cel",
        "cels3/rbat38.cel",
    });

    static const auto cj = load_cel3_texture({
        "cels3/cj01.cel",
        "cels3/cj02.cel",
        "cels3/cj03.cel",
        "cels3/cj04.cel",
        "cels3/cj05.cel",
        "cels3/cj06.cel",
        "cels3/cj07.cel",
        "cels3/cj08.cel",
        "cels3/cj09.cel",
        "cels3/cj10.cel",
        "cels3/cj11.cel",
        "cels3/cj12.cel",
        "cels3/cj13.cel",
        "cels3/cj14.cel",
        "cels3/cj15.cel",
        "cels3/cj16.cel",
        "cels3/cj17.cel",
        "cels3/cj18.cel",
        "cels3/cj19.cel",
        "cels3/cj20.cel",
        "cels3/cj21.cel",
        "cels3/cj22.cel",
        "cels3/cj23.cel",
        "cels3/cj24.cel",
        "cels3/cj25.cel",
        "cels3/cj26.cel",
        "cels3/cj27.cel",
        "cels3/cj28.cel",
        "cels3/cj29.cel",
        "cels3/cj30.cel",
        "cels3/cj31.cel",
        "cels3/cj32.cel",
        "cels3/cj33.cel",
        "cels3/cj34.cel",
        "cels3/cj35.cel",
        "cels3/cj36.cel",
        "cels3/cj37.cel",
        "cels3/cj38.cel",
        "cels3/cj39.cel",
        "cels3/cj40.cel",
        "cels3/cj41.cel",
        "cels3/cj42.cel",
        "cels3/cj43.cel",
        "cels3/cj44.cel",
        "cels3/cj45.cel",
        "cels3/cj46.cel",
        "cels3/cj47.cel",
        "cels3/cj48.cel",
    });

    static const auto dude = load_cel3_texture({
        "cels3/dude01.cel",
        "cels3/dude02.cel",
        "cels3/dude03.cel",
        "cels3/dude04.cel",
        "cels3/dude05.cel",
        "cels3/dude06.cel",
        "cels3/dude07.cel",
        "cels3/dude08.cel",
        "cels3/dude09.cel",
        "cels3/dude10.cel",
        "cels3/dude11.cel",
        "cels3/dude12.cel",
        "cels3/dude13.cel",
        "cels3/dude14.cel",
        "cels3/dude15.cel",
        "cels3/dude16.cel",
        "cels3/dude17.cel",
        "cels3/dude18.cel",
        "cels3/dude19.cel",
        "cels3/dude20.cel",
        "cels3/dude21.cel",
        "cels3/dude22.cel",
        "cels3/dude23.cel",
        "cels3/dude24.cel",
        "cels3/dude25.cel",
        "cels3/dude26.cel",
        "cels3/dude27.cel",
        "cels3/dude28.cel",
        "cels3/dude29.cel",
        "cels3/dude30.cel",
        "cels3/dude31.cel",
        "cels3/dude32.cel",
        "cels3/dude33.cel",
        "cels3/dude34.cel",
        "cels3/dude35.cel",
        "cels3/dude36.cel",
        "cels3/dude37.cel",
        "cels3/dude38.cel",
        "cels3/dude39.cel",
        "cels3/dude40.cel",
    });

    static const auto doc = load_cel3_texture({
        "cels3/rdoc01.cel",
        "cels3/rdoc02.cel",
        "cels3/rdoc03.cel",
        "cels3/rdoc04.cel",
        "cels3/rdoc05.cel",
        "cels3/rdoc06.cel",
        "cels3/rdoc07.cel",
        "cels3/rdoc08.cel",
        "cels3/rdoc09.cel",
        "cels3/rdoc10.cel",
        "cels3/rdoc11.cel",
        "cels3/rdoc12.cel",
        "cels3/rdoc13.cel",
        "cels3/rdoc14.cel",
        "cels3/rdoc15.cel",
        "cels3/rdoc16.cel",
        "cels3/rdoc17.cel",
        "cels3/rdoc18.cel",
        "cels3/rdoc19.cel",
        "cels3/rdoc20.cel",
        "cels3/rdoc21.cel",
        "cels3/rdoc22.cel",
        "cels3/rdoc23.cel",
        "cels3/rdoc24.cel",
        "cels3/rdoc25.cel",
        "cels3/rdoc26.cel",
        "cels3/rdoc27.cel",
        "cels3/rdoc28.cel",
        "cels3/rdoc29.cel",
        "cels3/rdoc30.cel",
        "cels3/rdoc31.cel",
        "cels3/rdoc32.cel",
        "cels3/rdoc33.cel",
        "cels3/rdoc34.cel",
        "cels3/rdoc35.cel",
        "cels3/rdoc36.cel",
        "cels3/rdoc37.cel",
        "cels3/rdoc38.cel",
        "cels3/rdoc39.cel",
        "cels3/rdoc40.cel",
        "cels3/rdoc41.cel",
    });

    static const auto kid = load_cel3_texture({
        "cels3/kidsit.cel",
        "cels3/rkid01.cel",
        "cels3/rkid02.cel",
        "cels3/rkid03.cel",
        "cels3/rkid04.cel",
        "cels3/rkid05.cel",
        "cels3/rkid06.cel",
        "cels3/rkid07.cel",
        "cels3/rkid08.cel",
        "cels3/rkid09.cel",
        "cels3/rkid10.cel",
        "cels3/rkid11.cel",
        "cels3/rkid12.cel",
        "cels3/rkid13.cel",
        "cels3/rkid14.cel",
        "cels3/rkid15.cel",
        "cels3/rkid16.cel",
        "cels3/rkid17.cel",
        "cels3/rkid18.cel",
        "cels3/rkid19.cel",
        "cels3/rkid20.cel",
        "cels3/rkid21.cel",
        "cels3/rkid22.cel",
        "cels3/rkid23.cel",
        "cels3/rkid24.cel",
        "cels3/rkid25.cel",
        "cels3/rkid26.cel",
        "cels3/rkid27.cel",
        "cels3/rkid28.cel",
        "cels3/rkid29.cel",
        "cels3/rkid30.cel",
        "cels3/rkid31.cel",
        "cels3/rkid32.cel",
        "cels3/rkid33.cel",
    });

    static const auto harry = load_cel3_texture({
        "cels3/rharry01.cel",
        "cels3/rharry02.cel",
        "cels3/rharry03.cel",
        "cels3/rharry04.cel",
        "cels3/rharry05.cel",
        "cels3/rharry06.cel",
        "cels3/rharry07.cel",
        "cels3/rharry08.cel",
        "cels3/rharry09.cel",
        "cels3/rharry10.cel",
        "cels3/rharry11.cel",
        "cels3/rharry12.cel",
        "cels3/rharry13.cel",
        "cels3/rharry14.cel",
        "cels3/rharry15.cel",
        "cels3/rharry16.cel",
        "cels3/rharry17.cel",
        "cels3/rharry18.cel",
        "cels3/rharry19.cel",
        "cels3/rharry20.cel",
        "cels3/rharry21.cel",
        "cels3/rharry22.cel",
        "cels3/rharry23.cel",
        "cels3/rharry24.cel",
        "cels3/rharry25.cel",
        "cels3/rharry26.cel",
        "cels3/rharry27.cel",
        "cels3/rharry28.cel",
        "cels3/rharry29.cel",
        "cels3/rharry30.cel",
        "cels3/rharry31.cel",
        "cels3/rharry32.cel",
        "cels3/rharry33.cel",
        "cels3/rharry34.cel",
        "cels3/rharry35.cel",
        "cels3/rharry36.cel",
        "cels3/rharry37.cel",
        "cels3/rharry38.cel",
        "cels3/rharry39.cel",
        "cels3/rharry40.cel",
        "cels3/rharry41.cel",
        "cels3/rharry42.cel",
        "cels3/rharry43.cel",
        "cels3/rharry44.cel",
        "cels3/rharry45.cel",
        "cels3/rharry46.cel",
        "cels3/rharry47.cel",
    });

    static const auto nurse = load_cel3_texture({
        "cels3/rnurse01.cel",
        "cels3/rnurse02.cel",
        "cels3/rnurse03.cel",
        "cels3/rnurse04.cel",
        "cels3/rnurse05.cel",
        "cels3/rnurse06.cel",
        "cels3/rnurse07.cel",
        "cels3/rnurse08.cel",
        "cels3/rnurse09.cel",
        "cels3/rnurse10.cel",
        "cels3/rnurse11.cel",
        "cels3/rnurse12.cel",
        "cels3/rnurse13.cel",
        "cels3/rnurse14.cel",
        "cels3/rnurse15.cel",
        "cels3/rnurse16.cel",
        "cels3/rnurse17.cel",
        "cels3/rnurse18.cel",
        "cels3/rnurse19.cel",
        "cels3/rnurse20.cel",
        "cels3/rnurse21.cel",
        "cels3/rnurse22.cel",
        "cels3/rnurse23.cel",
        "cels3/rnurse24.cel",
        "cels3/rnurse25.cel",
        "cels3/rnurse26.cel",
        "cels3/rnurse27.cel",
        "cels3/rnurse28.cel",
        "cels3/rnurse29.cel",
        "cels3/rnurse30.cel",
        "cels3/rnurse31.cel",
        "cels3/rnurse32.cel",
        "cels3/rnurse33.cel",
    });

    static const auto roy = load_cel3_texture({
        "cels3/roy01.cel",
        "cels3/roy02.cel",
        "cels3/roy03.cel",
        "cels3/roy04.cel",
        "cels3/roy05.cel",
        "cels3/roy06.cel",
        "cels3/roy07.cel",
        "cels3/roy08.cel",
        "cels3/roy09.cel",
        "cels3/roy10.cel",
        "cels3/roy11.cel",
        "cels3/roy12.cel",
        "cels3/roy13.cel",
        "cels3/roy14.cel",
        "cels3/roy15.cel",
        "cels3/roy16.cel",
        "cels3/roy17.cel",
        "cels3/roy18.cel",
        "cels3/roy19.cel",
        "cels3/roy20.cel",
        "cels3/roy21.cel",
        "cels3/roy22.cel",
        "cels3/roy23.cel",
        "cels3/roy24.cel",
        "cels3/roy25.cel",
        "cels3/roy26.cel",
        "cels3/roy27.cel",
        "cels3/roy28.cel",
        "cels3/roy29.cel",
        "cels3/roy30.cel",
        "cels3/roy31.cel",
        "cels3/roy32.cel",
        "cels3/roy33.cel",
        "cels3/roy34.cel",
        "cels3/roy35.cel",
        "cels3/roy36.cel",
        "cels3/roy37.cel",
        "cels3/roy38.cel",
        "cels3/roy39.cel",
        "cels3/roy40.cel",
        "cels3/roy41.cel",
        "cels3/roy42.cel",
        "cels3/roy43.cel",
        "cels3/roy44.cel",
    });

    static const auto tor = load_cel3_texture({
        "cels3/rtor01.cel",
        "cels3/rtor02.cel",
        "cels3/rtor03.cel",
        "cels3/rtor04.cel",
        "cels3/rtor05.cel",
        "cels3/rtor06.cel",
        "cels3/rtor07.cel",
        "cels3/rtor08.cel",
        "cels3/rtor09.cel",
        "cels3/rtor10.cel",
        "cels3/rtor11.cel",
        "cels3/rtor12.cel",
        "cels3/rtor13.cel",
        "cels3/rtor14.cel",
        "cels3/rtor15.cel",
        "cels3/rtor16.cel",
        "cels3/rtor17.cel",
        "cels3/rtor18.cel",
        "cels3/rtor19.cel",
        "cels3/rtor20.cel",
        "cels3/rtor21.cel",
        "cels3/rtor22.cel",
        "cels3/rtor23.cel",
        "cels3/rtor24.cel",
        "cels3/rtor25.cel",
        "cels3/rtor26.cel",
        "cels3/rtor27.cel",
        "cels3/rtor28.cel",
        "cels3/rtor29.cel",
        "cels3/rtor30.cel",
        "cels3/rtor31.cel",
        "cels3/rtor32.cel",
        "cels3/rtor33.cel",
        "cels3/rtor34.cel",
        "cels3/rtor35.cel",
        "cels3/rtor36.cel",
        "cels3/rtor37.cel",
        "cels3/rtor38.cel",
        "cels3/rtor39.cel",
        "cels3/rtor40.cel",
        "cels3/rtor41.cel",
        "cels3/rtor42.cel",
        "cels3/rtor43.cel",
        "cels3/rtor44.cel",
        "cels3/rtor45.cel",
        "cels3/rtor46.cel",
        "cels3/rtor47.cel",
        "cels3/rtor48.cel",
        "cels3/rtor49.cel",
        "cels3/rtor50.cel",
        "cels3/rtor51.cel",
        "cels3/rtor52.cel",
        "cels3/rtor53.cel",
    });

    static const auto wolf = load_cel3_texture({
        "cels3/rwolf01.cel",
        "cels3/rwolf02.cel",
        "cels3/rwolf03.cel",
        "cels3/rwolf04.cel",
        "cels3/rwolf05.cel",
        "cels3/rwolf06.cel",
        "cels3/rwolf07.cel",
        "cels3/rwolf08.cel",
        "cels3/rwolf09.cel",
        "cels3/rwolf10.cel",
        "cels3/rwolf11.cel",
        "cels3/rwolf12.cel",
        "cels3/rwolf13.cel",
        "cels3/rwolf14.cel",
        "cels3/rwolf15.cel",
        "cels3/rwolf16.cel",
        "cels3/rwolf17.cel",
        "cels3/rwolf18.cel",
        "cels3/rwolf19.cel",
        "cels3/rwolf20.cel",
        "cels3/rwolf21.cel",
        "cels3/rwolf22.cel",
        "cels3/rwolf23.cel",
        "cels3/rwolf24.cel",
        "cels3/rwolf25.cel",
        "cels3/rwolf26.cel",
        "cels3/rwolf27.cel",
        "cels3/rwolf28.cel",
        "cels3/rwolf29.cel",
        "cels3/rwolf30.cel",
        "cels3/rwolf31.cel",
        "cels3/rwolf32.cel",
        "cels3/rwolf33.cel",
        "cels3/rwolf34.cel",
        "cels3/rwolf35.cel",
        "cels3/rwolf36.cel",
        "cels3/rwolf37.cel",
        "cels3/rwolf38.cel",
        "cels3/rwolf39.cel",
        "cels3/rwolf40.cel",
        "cels3/rwolf41.cel",
        "cels3/rwolf42.cel",
        "cels3/rwolf43.cel",
        "cels3/rwolf44.cel",
        "cels3/rwolf45.cel",
    });


    static const auto drummer = load_cel3_texture({
        "cels3/drumr01.cel",
        "cels3/drumr02.cel",
        "cels3/drumr03.cel",
        "cels3/drumr04.cel",
        "cels3/drumr05.cel",
        "cels3/drumr06.cel",
        "cels3/drumr07.cel",
    });

    switch (segment.texture) {
        case 0:
            entities.emplace(segment.id, std::make_unique<AnimatedWall>(&segment, beach, 12));
            break;
        case 1:
            entities.emplace(segment.id, std::make_unique<Wall>(&segment, rcave1));
            break;
        case 2:
            entities.emplace(segment.id, std::make_unique<Wall>(&segment, rcave2));
            break;
        case 3:
            entities.emplace(segment.id, std::make_unique<Wall>(&segment, rcave3));
            break;
        case 4:
            entities.emplace(segment.id, std::make_unique<Wall>(&segment, rcave4));
            break;
        case 11:
        case 13:
        case 25:
        case 26:
        case 27:
        case 28:
            std::cout << "\t JUNGLE " << std::hex << segment.id << " " << std::dec << segment.x1 << "," << segment.y1 << " " << segment.x2 << "," << segment.y2 <<  ": " << segment.texture << " " << segment.flags << " " << segment.count << "\n";
            spawnPassage(segment);
            break;
        case 15:
            entities.emplace(segment.id, std::make_unique<Wall>(&segment, husk1));
            break;
        case 16:
            entities.emplace(segment.id, std::make_unique<Wall>(&segment, husk2));
            break;
        case 17:
            entities.emplace(segment.id, std::make_unique<Wall>(&segment, husk3));
            break;
        case 21:
        case 22:
            std::cout << "\t CAVE " << std::hex << segment.id << " " << std::dec << segment.x1 << "," << segment.y1 << " " << segment.x2 << "," << segment.y2 <<  ": " << segment.texture << " " << segment.flags << " " << segment.count << "\n";
            spawnPassage(segment);
            break;
        case 23:
        case 24:
            std::cout << "\t CAVE VINES " << std::hex << segment.id << " " << std::dec << segment.x1 << "," << segment.y1 << " " << segment.x2 << "," << segment.y2 <<  ": " << segment.texture << " " << segment.flags << " " << segment.count << "\n";
            spawnPassage(segment);
            break;
        case 18:
            entities.emplace(segment.id, std::make_unique<AnimatedWall>(&segment, plane_fire_right, 12));
            break;
        case 19:
            entities.emplace(segment.id, std::make_unique<AnimatedRoomEntry>(&segment, plane_fire_mid, 12, State::CrashedPlaneEntry));
            break;
        case 20:
            entities.emplace(segment.id, std::make_unique<AnimatedWall>(&segment, plane_fire_left, 12));
            break;
        case 5:
        case 7:
        case 9:
            entities.emplace(segment.id, std::make_unique<Wall>(&segment, trees1));
            break;
        case 6:
        case 8:
        case 10:
        case 12:
        case 14:
        case 44:
        case 45:
        case 46:
            entities.emplace(segment.id, std::make_unique<Wall>(&segment, trees2));
            break;
        case 29:
        case 30:
        case 31:
        case 32:
            entities.emplace(segment.id, std::make_unique<Wall>(&segment, jungle_fence));
            break;
        case 34:
            entities.emplace(segment.id, std::make_unique<Wall>(&segment, boowall_west));
            break;
        case 35:
            entities.emplace(segment.id, std::make_unique<Wall>(&segment, boowall_north));
            break;
        case 36:
            entities.emplace(segment.id, std::make_unique<Wall>(&segment, boowall_east));
            break;
        case 37:
            entities.emplace(segment.id, std::make_unique<Wall>(&segment, boowall_south));
            break;
        case 38:
            std::cout << "\t GATE " << std::hex << segment.id << " " << std::dec << segment.x1 << "," << segment.y1 << " " << segment.x2 << "," << segment.y2 <<  ": " << segment.texture << " " << segment.flags << " " << segment.count << "\n";
            entities.emplace(segment.id, std::make_unique<Wall>(&segment, boogate));
            break;
        case 39:
            entities.emplace(segment.id, std::make_unique<Wall>(&segment, hut_wall_west));
            break;
        case 40:
            entities.emplace(segment.id, std::make_unique<Wall>(&segment, hut_wall_north));
            break;
        case 41:
            entities.emplace(segment.id, std::make_unique<Wall>(&segment, hut_wall_east));
            break;
        case 42:
            entities.emplace(segment.id, std::make_unique<Wall>(&segment, hut_wall_south));
            break;
        case 43:
            entities.emplace(segment.id, std::make_unique<BarricadedRoomEntry>(&segment, bunker_entry_closed, bunker_entry_opened, DamageType::Machete, State::BunkerEntry));
            break;
        case 52:
            entities.emplace(segment.id, std::make_unique<Wall>(&segment, compound_wall_dark));
            break;
        case 53:
            entities.emplace(segment.id, std::make_unique<Wall>(&segment, compound_wall_light));
            break;
        case 54:
            entities.emplace(segment.id, std::make_unique<Wall>(&segment, compound_window_light));
            break;
        case 55:
            entities.emplace(segment.id, std::make_unique<Wall>(&segment, compound_window_dark));
            break;
        case 59:
            std::cout << "\t TEMPLE DOOR " << std::hex << segment.id << " " << std::dec << segment.x1 << "," << segment.y1 << " " << segment.x2 << "," << segment.y2 <<  ": " << segment.texture << " " << segment.flags << " " << segment.count << "\n";
            //spawnPassage(segment);
            entities.emplace(segment.id, std::make_unique<AnimatedWall>(&segment, temple_door, 6));
            break;
        case 63:
            entities.emplace(segment.id, std::make_unique<Wall>(&segment, fence));
            break;
        case 69:
            entities.emplace(segment.id, std::make_unique<Wall>(&segment, shack2));
            break;
        case 70:
            entities.emplace(segment.id, std::make_unique<Wall>(&segment, shack3));
            break;
        case 71:
            entities.emplace(segment.id, std::make_unique<Wall>(&segment, shack2));
            break;
        case 72:
            entities.emplace(segment.id, std::make_unique<Wall>(&segment, shack1));
            break;
        case 81:
            entities.emplace(segment.id, std::make_unique<Wall>(&segment, mansion_window_light));
            break;
        case 82:
            entities.emplace(segment.id, std::make_unique<Wall>(&segment, mansion_window_dark));
            break;
        case 85:
            entities.emplace(segment.id, std::make_unique<Wall>(&segment, mansion_wall_light));
            break;
        case 86:
            entities.emplace(segment.id, std::make_unique<Wall>(&segment, mansion_wall_dark));
            break;
        case 91:
            entities.emplace(segment.id, std::make_unique<Wall>(&segment, temple_wall_dark));
            break;
        case 92:
            entities.emplace(segment.id, std::make_unique<Wall>(&segment, temple_wall_light));
            break;
        case 94:
            entities.emplace(segment.id, std::make_unique<Wall>(&segment, temple_face_left));
            break;
        case 95:
            entities.emplace(segment.id, std::make_unique<Wall>(&segment, temple_face_mid));
            break;
        case 96:
            entities.emplace(segment.id, std::make_unique<Wall>(&segment, temple_face_right));
            break;
        case 100:
            entities.emplace(segment.id, std::make_unique<ItemPickup>(&segment, jacket, Item::Jacket, -1));
            break;
        case 105:
        case 132:
            entities.emplace(segment.id, std::make_unique<ItemPickup>(&segment, banana, Item::Banana, 1));
            break;
        case 106:
            entities.emplace(segment.id, std::make_unique<Trap>(&segment, pit, DeathType::Acid));
            break;
        case 107:
            entities.emplace(segment.id, std::make_unique<Prop>(&segment, trees, Collision::Block));
            break;
        case 108:
            entities.emplace(segment.id, std::make_unique<Prop>(&segment, vine, Collision::Pass));
            break;
        case 109:
            entities.emplace(segment.id, std::make_unique<ItemPickup>(&segment, crystal, Item::Crystal, -1));
            break;
        case 110:
            entities.emplace(segment.id, std::make_unique<ItemPickup>(&segment, flower, Item::Flower, -1));
            break;
        case 115:
            entities.emplace(segment.id, std::make_unique<ItemPickup>(&segment, aid_kit, Item::FirstAid, 1));
            break;
        case 116:
            entities.emplace(segment.id, std::make_unique<ItemPickup>(&segment, ammo_shells, Item::Ammo1, 50));
            break;
        case 117:
            entities.emplace(segment.id, std::make_unique<Monster::Bat>(&segment, bat));
            break;
        case 118:
            entities.emplace(segment.id, std::make_unique<Monster::Wolf>(&segment, wolf));
            break;
        case 119:
            entities.emplace(segment.id, std::make_unique<Monster::Roy>(&segment, roy));
            break;
        case 120:
            entities.emplace(segment.id, std::make_unique<Monster::Dude>(&segment, dude));
            break;
        case 121:
            entities.emplace(segment.id, std::make_unique<Monster::CJ>(&segment, cj));
            break;
        case 122:
            entities.emplace(segment.id, std::make_unique<Monster::Nurse>(&segment, nurse));
            break;
        case 123:
            entities.emplace(segment.id, std::make_unique<Monster::Tor>(&segment, tor));
            break;
        case 124:
            entities.emplace(segment.id, std::make_unique<Monster::Doc>(&segment, doc));
            break;
        case 125:
        case 127:
            entities.emplace(segment.id, std::make_unique<Prop>(&segment, driftwood, Collision::Pass));
            break;
        case 126:
            entities.emplace(segment.id, std::make_unique<Monster::Kid>(&segment, kid));
            break;
        case 128:
            entities.emplace(segment.id, std::make_unique<Monster::Harry>(&segment, harry));
            break;
        case 129:
        case 130:
        case 131:
        case 133:
            entities.emplace(segment.id, std::make_unique<ItemPickup>(&segment, coconut, Item::Coconut, 1));
            break;
        case 134:
            entities.emplace(segment.id, std::make_unique<Prop>(&segment, dish, Collision::Block));
            break;
        case 135:
            entities.emplace(segment.id, std::make_unique<AnimatedProp>(&segment, tiki, 6, Collision::Pass));
            break;
        case 138:
            entities.emplace(segment.id, std::make_unique<Prop>(&segment, bed, Collision::Block));
            break;
        case 139:
            entities.emplace(segment.id, std::make_unique<Prop>(&segment, labtable, Collision::Block));
            break;
        case 140:
            entities.emplace(segment.id, std::make_unique<Prop>(&segment, labsink, Collision::Block));
            break;
        case 141:
            entities.emplace(segment.id, std::make_unique<Prop>(&segment, table, Collision::Block));
            break;
        case 144:
            entities.emplace(segment.id, std::make_unique<Prop>(&segment, barrel, Collision::Block));
            break;
        case 145:
            entities.emplace(segment.id, std::make_unique<Trap>(&segment, snare, DeathType::Snare));
            break;
        case 146:
            entities.emplace(segment.id, std::make_unique<Trap>(&segment, trapdoor, DeathType::Acid));
            break;
        case 150:
            entities.emplace(segment.id, std::make_unique<ItemPickup>(&segment, ammo_bullets, Item::Ammo2, 20));
            break;
        case 151:
            entities.emplace(segment.id, std::make_unique<ItemPickup>(&segment, ammo_clips, Item::Ammo3, 25));
            break;
        case 160:
            entities.emplace(segment.id, std::make_unique<Wall>(&segment, hanger_dark));
            break;
        case 161:
            entities.emplace(segment.id, std::make_unique<Wall>(&segment, hanger_light));
            break;
        case 166:
            entities.emplace(segment.id, std::make_unique<Wall>(&segment, plane_fence_left));
            break;
        case 167:
            entities.emplace(segment.id, std::make_unique<Wall>(&segment, plane_fence_right));
            break;
        case 168:
            entities.emplace(segment.id, std::make_unique<Wall>(&segment, runway));
            break;
        case 169:
            entities.emplace(segment.id, std::make_unique<Monster::Drummer>(&segment, drummer));
            break;
        case 171:
            entities.emplace(segment.id, std::make_unique<DamageableProp>(&segment, red_chair, red_chair_damaged, Collision::Block));
            break;
        case 173:
            entities.emplace(segment.id, std::make_unique<DamageableWall>(&segment, bookshelf, bookshelf_damaged));
            break;
        case 174:
            entities.emplace(segment.id, std::make_unique<DamageableWall>(&segment, tv, tv_damaged));
            break;
        case 176:
            entities.emplace(segment.id, std::make_unique<DamageableWall>(&segment, portrait, portrait_damaged));
            break;
        case 250:
            entities.emplace(segment.id, std::make_unique<AnimatedWall>(&segment, fireplace_left, 12));
            break;
        case 251:
            entities.emplace(segment.id, std::make_unique<AnimatedWall>(&segment, fireplace_right, 12));
            break;
        case 252:
            entities.emplace(segment.id, std::make_unique<ItemPickup>(&segment, shotgun, Item::Shotgun, -1));
            break;
        default:
            break;
    }
}

raylib::Vector2 World::findSpawn() const {
    const auto &map_entrance = entrances[1];

    return raylib::Vector2(map_entrance.X(), map_entrance.Y());

}
