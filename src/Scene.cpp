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

#include "Scene.h"
#include "Strings.h"
#include "SoundCache.h"
#include "TextureCache.h"

Scene::Scene(Panel *panel, const std::string &background_filename) : panel(panel) {
    background = TextureCache::LoadStillCel(background_filename);
}

std::tuple<bool, std::string, DeathType> Scene::useItemOnItem(Item source, Item destination) {
    const static std::string DIDNT_WORK = Strings::Lookup(326);

    return std::make_tuple(false, DIDNT_WORK, DeathType::None);
}

std::tuple<bool, std::string, DeathType> Scene::getItem(const Layout &layout) {
    return std::make_tuple(true, layout.pickup, DeathType::None);
}

std::string Scene::useItem(Player *player, Item item) {
    const static std::string HUH = Strings::Lookup(439);

    return HUH;
}

std::optional<Scene::Dialogue> Scene::talk(Player *player) {
    return std::nullopt;
}

void Scene::draw(Player *player, int scale) {
    const static std::string NOTHING_INTEREST = Strings::Lookup(240);
    const static std::string USE = Strings::Lookup(184);
    const static std::string USE_ON = Strings::Lookup(197);

    const static std::vector<Input> navigation_inputs {
        Input::StepForward,
        Input::StepBack,
        Input::StepLeft,
        Input::StepRight,
        Input::TurnLeft,
        Input::TurnRight,
        Input::LookUp,
        Input::LookDown,
    };

    BeginDrawing();
    {

        uint64_t player_input = player->getInput();

        background.Draw(Vector2(0, 0), 0.0f, scale);

        for (auto it = animations.begin(); it != animations.end();) {
            auto &[animation_id, animation] = *it;

            if (animation.draw(scale)) {
                animationCompleted(player, animation_id);
                animations.erase(it++);
            } else {
                ++it;
            }
        }

        for (const auto &layout : layouts) {
            layout.draw(scale);
        }

        if (player_input) {
            for (const auto navigation_input : navigation_inputs) {
                if (player_input & navigation_input) {
                    if (navigation.contains(navigation_input)) {
                        State state = navigation.at(navigation_input);

                        if (state == State::World) {
                            if (entrance) {
                                World *world = player->getWorld();
                                world->setCurrentLevel(entrance->getName());

                                player->setPosition(raylib::Vector2(entrance->X(), entrance->Y()));
                                player->setAngles(raylib::Vector2(entrance->getDirection(), 0));
                            } else {
                                throw std::runtime_error("World exit without an exit point");
                            }
                        }

                        player->setState(state);
                        player_input = 0;
                        break;
                    }
                }
            }
        }

        if (player_input & Input::PrimaryAction) {
            player_input &= ~Input::PrimaryAction;

            auto position = raylib::Mouse::GetPosition();

            std::optional<Layout> selected = std::nullopt;
            if (position.GetY() < 138) {
                for (const auto &layout : layouts) {
                    auto border = layout.Border();

                    if (border.CheckCollision(position)) {
                        selected = layout;
                    }
                }
            }

            auto action = player->getAction();

            if (action) {
                switch (*action) {
                    case Action::Look:
                        if (selected) {
                            player->setHighlight(selected->description);
                        } else {
                            player->setHighlight(NOTHING_INTEREST);
                        }
                        break;
                    case Action::Get:
                        if (selected) {
                            auto [result, highlight, death_type] = getItem(*selected);

                            if (death_type != DeathType::None) {
                                player->death(death_type);
                            }

                            if (result) {
                                switch (selected->item) {
                                    case Item::Crate:
                                        player->addItem(Item::Ammo2, 150);
                                        break;
                                    default:
                                        player->addItem(selected->item);
                                }

                                layouts.erase(std::remove_if(std::begin(layouts), std::end(layouts), [selected](const auto &layout) {
                                    return layout.item == selected->item;
                                }));
                            }
                            player->setHighlight(highlight);
                        } else {
                            player->setHighlight();
                        }
                        break;

                    case Action::Use:
                        if (auto player_selected = player->getSelectedItem()) {
                            auto [use_result, highlight, death_type] = useItemOnItem(*player_selected, selected->item, player);

                            if (death_type != DeathType::None) {
                                player->death(death_type);
                            }

                            if (use_result) {
                                player->useItem(*player_selected);
                                player->setSelectedItem(std::nullopt);
                            }

                            player->setHighlight(highlight);
                        } else {
                            if (selected) {
                                auto highlight = useItem(player, selected->item);

                                player->setHighlight(highlight);
                            } else {
                                const static std::string HUH = Strings::Lookup(439);
                                player->setHighlight(HUH);
                            }
                        }
                        break;
                    case Action::Talk:
                        if (auto dialogue = talk(player)) {
                            player->setHighlight(dialogue->text, dialogue->colour);
                            if (dialogue->sound) {
                                dialogue->sound->Play();
                            }
                        } else {
                            player->setHighlight();
                        } 
                        break;
                    default:
                        player->setHighlight();
                }
            }
        }

        player->setInput(player_input);

        panel->draw(player, player->getHighlight(), scale);
    }
    EndDrawing(); 
}

void Scene::animationCompleted(Player *player, uint16_t animation_id) {
}

Scene::~Scene() {

}

CrashedPlaneEntryScene::CrashedPlaneEntryScene(Panel *panel, const Entrance &new_entrance) : Scene(panel, "stillcel/room3.cel") {
    entrance = new_entrance;

    layouts.emplace_back(raylib::Vector2(203, 115), TextureCache::LoadStillCel("stillcel/clippers.cel"), Item::WireClipper, Strings::Lookup(315), Strings::Lookup(316), Strings::Lookup(317));

    navigation[Input::StepBack] = State::World;
    navigation[Input::LookDown] = State::World;
    navigation[Input::TurnLeft] = State::CrashedPlaneLeft;
    navigation[Input::TurnRight] = State::CrashedPlaneRight;
}

CrashedPlaneLeftScene::CrashedPlaneLeftScene(Panel *panel) : Scene(panel, "stillcel/room1.cel") {
    layouts.emplace_back(raylib::Vector2(36, 127), TextureCache::LoadStillCel("stillcel/phbook.cel"), Item::Book, Strings::Lookup(334), Strings::Lookup(335), Strings::Lookup(336));

    navigation[Input::StepForward] = State::CrashedPlaneCockpit;
    navigation[Input::LookUp] = State::CrashedPlaneCockpit;
    navigation[Input::TurnLeft] = State::CrashedPlaneExit;
    navigation[Input::TurnRight] = State::CrashedPlaneEntry;
}

CrashedPlaneCockpitScene::CrashedPlaneCockpitScene(Panel *panel) : Scene(panel, "stillcel/room2.cel") {
    layouts.emplace_back(raylib::Vector2(166, 48), TextureCache::LoadStillCel("stillcel/compass.cel"), Item::StuckCompass, Strings::Lookup(320), Strings::Lookup(321), Strings::Lookup(322));
    layouts.emplace_back(raylib::Vector2(98, 92), TextureCache::LoadStillCel("stillcel/flaregun.cel"), Item::FlareGun, Strings::Lookup(327), Strings::Lookup(328), Strings::Lookup(329));

    navigation[Input::StepBack] = State::CrashedPlaneLeft;
    navigation[Input::LookDown] = State::CrashedPlaneLeft;
}

std::tuple<bool, std::string, DeathType> CrashedPlaneCockpitScene::useItemOnItem(Item source, Item destination) {
    const static std::string DIDNT_WORK = Strings::Lookup(326);

    if (source == Item::WireClipper && destination == Item::StuckCompass) {
        layouts.erase(std::remove_if(std::begin(layouts), std::end(layouts), [](const auto &layout) {
            return layout.item == Item::StuckCompass;
        }));

        layouts.emplace_back(raylib::Vector2(166, 48), TextureCache::LoadStillCel("stillcel/compass.cel"), Item::Compass, Strings::Lookup(320), Strings::Lookup(321), Strings::Lookup(322));

        return std::make_tuple(true, Strings::Lookup(324), DeathType::None);
    }

    return std::make_tuple(false, DIDNT_WORK, DeathType::None);
}

std::tuple<bool, std::string, DeathType> CrashedPlaneCockpitScene::getItem(const Layout &layout) {
    if (layout.item == Item::StuckCompass) {
        return std::make_tuple(false, Strings::Lookup(323), DeathType::None);
    }

    return Scene::getItem(layout);
}

CrashedPlaneRightScene::CrashedPlaneRightScene(Panel *panel) : Scene(panel, "stillcel/room4.cel") {
    layouts.emplace_back(raylib::Vector2(25, 62), TextureCache::LoadStillCel("stillcel/machetis.cel"), Item::Machete, Strings::Lookup(308), Strings::Lookup(309), Strings::Lookup(310));

    navigation[Input::TurnLeft] = State::CrashedPlaneEntry;
    navigation[Input::TurnRight] = State::CrashedPlaneExit;
}

CrashedPlaneExitScene::CrashedPlaneExitScene(Panel *panel, const Entrance &new_entrance) : Scene(panel, "stillcel/room5.cel") {
    entrance = new_entrance;

    navigation[Input::StepForward] = State::World;
    navigation[Input::LookUp] = State::World;
    navigation[Input::TurnLeft] = State::CrashedPlaneRight;
    navigation[Input::TurnRight] = State::CrashedPlaneLeft;
}

BunkerEntryScene::BunkerEntryScene(Panel *panel, const Entrance &new_entrance) : Scene(panel, "stillcel/bunkerbg.cel") {
    entrance = new_entrance;

    layouts.emplace_back(raylib::Vector2(244, 71), TextureCache::LoadStillCel("stillcel/guninbun.cel"), Item::Rifle, Strings::Lookup(376), Strings::Lookup(377), Strings::Lookup(378));
    layouts.emplace_back(raylib::Vector2(14, 83), TextureCache::LoadStillCel("stillcel/crates.cel"), Item::Crate, Strings::Lookup(364), Strings::Lookup(365), Strings::Lookup(366));
    layouts.emplace_back(raylib::Vector2(64, 133), TextureCache::LoadStillCel("stillcel/wire.cel"), Item::TripWire, Strings::Lookup(370), Strings::Lookup(370), Strings::Lookup(370));

    navigation[Input::TurnLeft] = State::BunkerLeft;
    navigation[Input::TurnRight] = State::BunkerRight;
}

std::tuple<bool, std::string, DeathType> BunkerEntryScene::getItem(const Layout &layout) {
    if (trapped)
        return std::make_tuple(false, "", DeathType::Wire);

    return Scene::getItem(layout);
}

std::tuple<bool, std::string, DeathType> BunkerEntryScene::useItemOnItem(Item source, Item destination) {
    const static std::string DIDNT_WORK = Strings::Lookup(326);

    if (source == Item::WireClipper && destination == Item::TripWire) {
        trapped = false;

        layouts.erase(std::remove_if(std::begin(layouts), std::end(layouts), [destination](const auto &layout) {
            return layout.item == destination;
        }));

        return std::make_tuple(true, Strings::Lookup(373), DeathType::None);
    }

    return std::make_tuple(false, DIDNT_WORK, DeathType::None);
}

BunkerExitScene::BunkerExitScene(Panel *panel, const Entrance &new_entrance) : Scene(panel, "stillcel/bunkin.cel") {
    entrance = new_entrance;

    navigation[Input::StepForward] = State::World;
    navigation[Input::LookUp] = State::World;
    navigation[Input::TurnLeft] = State::BunkerRight;
    navigation[Input::TurnRight] = State::BunkerLeft;
}

BunkerLeftScene::BunkerLeftScene(Panel *panel) : Scene(panel, "stillcel/bunkleft.cel") {
    layouts.emplace_back(raylib::Vector2(120, 38), raylib::Vector2(140, 100), Item::Soldier, Strings::Lookup(391), Strings::Lookup(392), Strings::Lookup(393));

    navigation[Input::TurnLeft] = State::BunkerExit;
    navigation[Input::TurnRight] = State::BunkerEntry;
}

std::tuple<bool, std::string, DeathType> BunkerLeftScene::getItem(const Layout &layout) {
    return std::make_tuple(false, Strings::Lookup(393), DeathType::None);
}

std::tuple<bool, std::string, DeathType> BunkerLeftScene::useItemOnItem(Item source, Item destination) {
    const static std::string WONT_HELP = Strings::Lookup(395);

    return std::make_tuple(false, WONT_HELP, DeathType::None);
}

BunkerRightScene::BunkerRightScene(Panel *panel) : Scene(panel, "stillcel/bunkrt.cel") {
    layouts.emplace_back(raylib::Vector2(7, 41), TextureCache::LoadStillCel("stillcel/rag.cel"), Item::Rags, Strings::Lookup(376), Strings::Lookup(377), Strings::Lookup(378));
    layouts.emplace_back(raylib::Vector2(86, 66), TextureCache::LoadStillCel("stillcel/oil.cel"), Item::OilCan, Strings::Lookup(364), Strings::Lookup(365), Strings::Lookup(366));

    navigation[Input::TurnRight] = State::BunkerExit;
    navigation[Input::TurnLeft] = State::BunkerEntry;
}

std::tuple<bool, std::string, DeathType> BunkerRightScene::getItem(const Layout &layout) {
    return Scene::getItem(layout);
}

std::tuple<bool, std::string, DeathType> BunkerRightScene::useItemOnItem(Item source, Item destination) {
    const static std::string DIDNT_WORK = Strings::Lookup(326);

    return std::make_tuple(false, DIDNT_WORK, DeathType::None);
}

TempleEntranceScene::TempleEntranceScene(Panel *panel, const Entrance &new_entrance) : Scene(panel, "stillcel/room17.cel") {
    entrance = new_entrance;

    layouts.emplace_back(raylib::Vector2(144, 97), raylib::Vector2(8, 8), Item::MedalHole1, Strings::Lookup(334), Strings::Lookup(335), Strings::Lookup(336));
    layouts.emplace_back(raylib::Vector2(168, 97), raylib::Vector2(8, 8), Item::MedalHole2, Strings::Lookup(334), Strings::Lookup(335), Strings::Lookup(336));

    navigation[Input::StepBack] = State::World;
    navigation[Input::LookDown] = State::World;
}

std::tuple<bool, std::string, DeathType> TempleEntranceScene::getItem(const Layout &layout) {
    if (layout.item == Item::MedalHole1 || layout.item == Item::MedalHole2 || layout.item == Item::MedalHoleFilled1 || layout.item == Item::MedalHoleFilled2) {
        return std::make_tuple(false, "", DeathType::None);
    }

    return Scene::getItem(layout);
}

std::tuple<bool, std::string, DeathType> TempleEntranceScene::useItemOnItem(Item source, Item destination) {
    if (source == Item::GoldMedal1 && destination == Item::MedalHole1) {
        layouts.emplace_back(raylib::Vector2(144, 97), TextureCache::LoadStillCel("stillcel/hole1.cel"), Item::MedalHoleFilled1, Strings::Lookup(334), Strings::Lookup(335), Strings::Lookup(336));
        return make_tuple(true, Strings::Lookup(425), DeathType::None);
    }

    if (source == Item::GoldMedal2 && destination == Item::MedalHole2) {
        static const std::vector<std::optional<raylib::TextureUnmanaged>> left_eye = {
             TextureCache::LoadStillCel("stillcel/lefteye1.cel"),
             TextureCache::LoadStillCel("stillcel/lefteye2.cel"),
             TextureCache::LoadStillCel("stillcel/lefteye3.cel"),
             TextureCache::LoadStillCel("stillcel/lefteye4.cel"),
        };

        static const std::vector<std::optional<raylib::TextureUnmanaged>> right_eye = {
             TextureCache::LoadStillCel("stillcel/rteye1.cel"),
             TextureCache::LoadStillCel("stillcel/rteye2.cel"),
             TextureCache::LoadStillCel("stillcel/rteye3.cel"),
             TextureCache::LoadStillCel("stillcel/rteye4.cel"),
        };

        layouts.emplace_back(raylib::Vector2(168, 97), TextureCache::LoadStillCel("stillcel/hole1.cel"), Item::MedalHoleFilled2, Strings::Lookup(334), Strings::Lookup(335), Strings::Lookup(336));

        animations[1] = Animation(raylib::Vector2(102.0f, 31.0f), left_eye, false);
        animations[2] = Animation(raylib::Vector2(181.0f, 32.0f), right_eye, false);

        return make_tuple(true, Strings::Lookup(425), DeathType::None);
    }

    return Scene::useItemOnItem(source, destination);
}

void TempleEntranceScene::draw(Player *player, int scale) {
    Scene::draw(player, scale);
}

void TempleEntranceScene::animationCompleted(Player *player, uint16_t animation_id) {
    if (animation_id == 1) {
        static const std::vector<std::optional<raylib::TextureUnmanaged>> doors = {
            TextureCache::LoadStillCel("stillcel/tmpdoor1.cel"),
            TextureCache::LoadStillCel("stillcel/tmpdoor2.cel"),
            TextureCache::LoadStillCel("stillcel/tmpdoor3.cel"),
            TextureCache::LoadStillCel("stillcel/tmpdoor4.cel"),
            TextureCache::LoadStillCel("stillcel/tmpdoor5.cel"),
            TextureCache::LoadStillCel("stillcel/tmpdoor6.cel"),
            TextureCache::LoadStillCel("stillcel/tmpdoor7.cel"),
            TextureCache::LoadStillCel("stillcel/tmpdoor8.cel"),
        };

        removeItemLayout(Item::MedalHoleFilled1);
        removeItemLayout(Item::MedalHoleFilled2);

        animations[3] = Animation(raylib::Vector2(101.0f, 86.0f), doors, false);
    } else if (animation_id == 3) {
        opened = true;

        navigation[Input::StepForward] = State::Oracle;
        navigation[Input::LookUp] = State::Oracle;

        animations[4] = Animation(raylib::Vector2(101.0f, 86.0f), {TextureCache::LoadStillCel("stillcel/tmpdoor8.cel")});
    }
}

OracleScene::OracleScene(Panel *panel, const Entrance &new_entrance) : Scene(panel, "stillcel/oraclebg.cel") {
    entrance = new_entrance;

    layouts.emplace_back(raylib::Vector2(122, 10), raylib::Vector2(73, 83), Item::Oracle, Strings::Lookup(417), Strings::Lookup(418), "");

    scripts[0] = {
        Dialogue(Strings::Lookup(89)),
        Dialogue(Strings::Lookup(90), raylib::RAYWHITE, SoundCache::Load("sound/or1.voc")),
        Dialogue(Strings::Lookup(92)),
        Dialogue(Strings::Lookup(93), raylib::RAYWHITE, SoundCache::Load("sound/or2.voc")),
        Dialogue(Strings::Lookup(95), raylib::RAYWHITE),
        Dialogue(Strings::Lookup(96)),
        Dialogue(Strings::Lookup(97), raylib::RAYWHITE, SoundCache::Load("sound/or3.voc")),
        Dialogue(Strings::Lookup(99)),
        Dialogue(Strings::Lookup(100), raylib::RAYWHITE, SoundCache::Load("sound/or4.voc")),
        Dialogue(Strings::Lookup(102)),
        Dialogue(Strings::Lookup(103), raylib::RAYWHITE, SoundCache::Load("sound/or5.voc")),
        Dialogue(Strings::Lookup(106)),
        Dialogue(Strings::Lookup(107), raylib::RAYWHITE, SoundCache::Load("sound/or7.voc")),
        Dialogue(Strings::Lookup(109)),
        Dialogue(Strings::Lookup(110), raylib::RAYWHITE, SoundCache::Load("sound/or8.voc")),
        Dialogue(Strings::Lookup(112)),
        Dialogue(Strings::Lookup(113), raylib::RAYWHITE, SoundCache::Load("sound/or9.voc")),
        Dialogue(Strings::Lookup(115)),
    };

    scripts[1] = {
        Dialogue(Strings::Lookup(116), raylib::RAYWHITE, SoundCache::Load("sound/or10.voc")),
    };

    scripts[2] = {
        Dialogue(Strings::Lookup(118), raylib::RAYWHITE, SoundCache::Load("sound/or11.voc")),
    };

    navigation[Input::StepBack] = State::World;
    navigation[Input::LookDown] = State::World;
}

std::tuple<bool, std::string, DeathType> OracleScene::useItemOnItem(Item source, Item destination) {
    if (source == Item::DeadWolf && destination == Item::Oracle) {
        sacrifice = true;
        static const std::vector<std::optional<raylib::TextureUnmanaged>> oracle = {
            TextureCache::LoadStillCel("stillcel/flame1.cel"),
            TextureCache::LoadStillCel("stillcel/flame2.cel"),
            TextureCache::LoadStillCel("stillcel/flame3.cel"),
            TextureCache::LoadStillCel("stillcel/flame4.cel"),
            TextureCache::LoadStillCel("stillcel/flame5.cel"),
            TextureCache::LoadStillCel("stillcel/flame6.cel"),
            TextureCache::LoadStillCel("stillcel/flame7.cel"),
            TextureCache::LoadStillCel("stillcel/flame8.cel"),
            TextureCache::LoadStillCel("stillcel/flame9.cel"),
            TextureCache::LoadStillCel("stillcel/flame10.cel"),
        };

        animations[1] = Animation(raylib::Vector2(122, 10), oracle);

        return std::make_tuple(false, Strings::Lookup(419), DeathType::None);
    }

    return Scene::useItemOnItem(source, destination);
}

std::optional<Scene::Dialogue> OracleScene::talk(Player *player) {
    if (sacrifice) {
        if (!player->getItemCount(Item::Uzi)) {
            script = 0;
        } else if (player->getItemCount(Item::Companion)) {
            if (player->testFlag(Flag::CompanionCured)) {
                dialogueIndex = 0;
                script = 2;
            } else {
                dialogueIndex = 0;
                script = 1;
            }
        }

        if (script == 0) {
            if (dialogueIndex < scripts[script].size()) {
                return scripts[script][dialogueIndex++];
            }

            if (!giveUzi) {
                static const auto uzi_item_texture = TextureCache::LoadStillCel("stillcel/rcluzi.cel");

                layouts.emplace_back(raylib::Vector2(134, 63), uzi_item_texture, Item::Uzi, Strings::Lookup(262), Strings::Lookup(263), "");
                giveUzi = true;
            }
        } else if (script == 1) {
            if (dialogueIndex < scripts[script].size()) {
                return scripts[script][dialogueIndex++];
            }
        } else if (script == 2) {
            if (dialogueIndex < scripts[script].size()) {
                return scripts[script][dialogueIndex++];
            }
        }
    }

    return Scene::talk(player);
}

ToiletScene::ToiletScene(Panel *panel, const Entrance &new_entrance) : Scene(panel, "stillcel/toilet1.cel") {
    entrance = new_entrance;

    static const std::vector<std::optional<raylib::TextureUnmanaged>> eyes = {
        std::nullopt,
        TextureCache::LoadStillCel("stillcel/closeye1.cel"),
        TextureCache::LoadStillCel("stillcel/closeye2.cel"),
    };

    animations[1] = Animation(raylib::Vector2(139, 18), eyes, true, 12);

    navigation[Input::StepBack] = State::World;
    navigation[Input::LookDown] = State::World;
}

ShowerScene::ShowerScene(Panel *panel, const Entrance &new_entrance) : Scene(panel, "stillcel/shower1.cel") {
    entrance = new_entrance;

    static const std::vector<std::optional<raylib::TextureUnmanaged>> eyes = {
        TextureCache::LoadStillCel("stillcel/shower2.cel"),
        TextureCache::LoadStillCel("stillcel/shower3.cel"),
    };

    animations[1] = Animation(raylib::Vector2(82, 0), eyes, true, 6);

    navigation[Input::StepBack] = State::World;
    navigation[Input::LookDown] = State::World;
}

DevelopersScene::DevelopersScene(Panel *panel, const Entrance &new_entrance) : Scene(panel, "stillcel/rainhut.cel") {
    entrance = new_entrance;

    navigation[Input::StepBack] = State::World;
    navigation[Input::LookDown] = State::World;
}

std::optional<Scene::Dialogue> DevelopersScene::talk(Player *player) {
    return Dialogue(Strings::Lookup(225), raylib::RAYWHITE);
}

VillageEyesScene::VillageEyesScene(Panel *panel, const Entrance &new_entrance) : Scene(panel, "stillcel/hut3.cel") {
    entrance = new_entrance;

    navigation[Input::StepBack] = State::World;
    navigation[Input::LookDown] = State::World;

    script = {
        Dialogue(Strings::Lookup(223), raylib::RAYWHITE),
        Dialogue(Strings::Lookup(224), raylib::RAYWHITE),
    };
}

std::optional<Scene::Dialogue> VillageEyesScene::talk(Player *player) {
    auto dialogue = script[dialogueIndex++];

    if (dialogueIndex >= script.size()) {
        dialogueIndex = 0;
    }

    return dialogue;
}

RocketLauncherScene::RocketLauncherScene(Panel *panel, const Entrance &new_entrance) : Scene(panel, "stillcel/room23.cel") {
    entrance = new_entrance;

    layouts.emplace_back(raylib::Vector2(29, 57), TextureCache::LoadStillCel("stillcel/panel.cel"), Item::Panel, Strings::Lookup(440), Strings::Lookup(441), Strings::Lookup(442));

    navigation[Input::StepBack] = State::World;
    navigation[Input::LookDown] = State::World;
}

std::tuple<bool, std::string, DeathType> RocketLauncherScene::getItem(const Layout &layout) {
    static const std::vector<std::optional<raylib::TextureUnmanaged>> wires1_anim = {
        TextureCache::LoadStillCel("stillcel/wires1.cel"),
    };

    if (layout.item == Item::Panel) {
        return std::make_tuple(false, Strings::Lookup(443), DeathType::None);
    }

    if (layout.item == Item::OpenPanel) {
        animations[1] = Animation(raylib::Vector2(83, 57), wires1_anim);

        layouts.emplace_back(raylib::Vector2(100, 81), raylib::Vector2(20, 6), Item::GreenWire, Strings::Lookup(450), Strings::Lookup(451), "");
        layouts.emplace_back(raylib::Vector2(80, 70), raylib::Vector2(20, 8), Item::BlueWire, Strings::Lookup(453), Strings::Lookup(453), "");
        layouts.emplace_back(raylib::Vector2(100, 70), raylib::Vector2(18, 5), Item::RedWire, Strings::Lookup(447), Strings::Lookup(448), "");

        removeItemLayout(Item::OpenPanel);
        return std::make_tuple(false, Strings::Lookup(444), DeathType::None);
    }

    return Scene::getItem(layout);
}

std::tuple<bool, std::string, DeathType> RocketLauncherScene::useItemOnItem(Item source, Item destination, Player *player) {
    static const std::vector<std::optional<raylib::TextureUnmanaged>> wires2_anim = {
        TextureCache::LoadStillCel("stillcel/wires2.cel"),
    };

    static const std::vector<std::optional<raylib::TextureUnmanaged>> wires3_anim = {
        TextureCache::LoadStillCel("stillcel/wires3.cel"),
    };


    if (source == Item::Machete && destination == Item::Panel) {
        layouts.emplace_back(raylib::Vector2(29, 57), TextureCache::LoadStillCel("stillcel/panel.cel"), Item::OpenPanel, Strings::Lookup(440), Strings::Lookup(441), Strings::Lookup(442));

        removeItemLayout(Item::Panel);

        return std::make_tuple(true, Strings::Lookup(444), DeathType::None);
    }

    if (source == Item::WireClipper && destination == Item::GreenWire) {
        if (cut != RocketLauncherScene::WireCut::Green) {
            return std::make_tuple(false, "", DeathType::Missile);
        }

        animations.erase(1);
        animations[2] = Animation(raylib::Vector2(83, 57), wires2_anim);

        cut = RocketLauncherScene::WireCut::Red;
        return std::make_tuple(true, Strings::Lookup(452), DeathType::None);
    }

    if (source == Item::WireClipper && destination == Item::RedWire) {
        if (cut != RocketLauncherScene::WireCut::Red) {
            return std::make_tuple(false, "", DeathType::Missile);
        }

        animations.erase(2);
        animations[3] = Animation(raylib::Vector2(83, 57), wires3_anim);

        cut = RocketLauncherScene::WireCut::Blue;
        return std::make_tuple(true, Strings::Lookup(449), DeathType::None);
    }

    if (source == Item::WireClipper && destination == Item::BlueWire) {
        if (cut != RocketLauncherScene::WireCut::Blue) {
            return std::make_tuple(false, "", DeathType::Missile);
        }

        animations.erase(3);

        player->setFlag(Flag::RocketsDisabled);

        return std::make_tuple(true, Strings::Lookup(455), DeathType::None);
    }

    return Scene::useItemOnItem(source, destination);
}

PlaneCockpitScene::PlaneCockpitScene(Panel *panel, const Entrance &new_entrance) : Scene(panel, "stillcel/room22.cel") {
    entrance = new_entrance;

    layouts.emplace_back(raylib::Vector2(135, 68), raylib::Vector2(50, 38), Item::Throttle, Strings::Lookup(396), Strings::Lookup(396), "");

    navigation[Input::StepLeft] = State::World;
    navigation[Input::TurnLeft] = State::World;

    navigation[Input::StepBack] = State::PlaneGalley;
    navigation[Input::LookDown] = State::PlaneGalley;
}

std::string PlaneCockpitScene::useItem(Player *player, Item item) {
    if (item == Item::Throttle) {
        if (!player->getItemCount(Item::Companion) && !player->testFlag(BombCountdown)) {
            return Strings::Lookup(227);
        }

        if (player->getItemCount(Item::Companion) && !player->testFlag(Flag::CompanionCalmed)) {
            raylib::Sound *afraid = SoundCache::Load("sound/afraid.voc");
            afraid->Play();

            return Strings::Lookup(228);
        }

        if (player->getItemCount(Item::Companion) && !player->testFlag(Flag::CompanionCured)) {
            player->takeDamage(999, DeathType::Companion);
            return "";
        }

        if (!player->testFlag(Flag::RocketsDisabled)) {
            player->takeDamage(999, DeathType::Launch);
            return "";
        }
        player->setState(State::Ending2);
        return Strings::Lookup(230);
    }

    return Scene::useItem(player, item);
}

PlaneGalleyScene::PlaneGalleyScene(Panel *panel) : Scene(panel, "stillcel/room21.cel"){
    navigation[Input::StepForward] = State::PlaneCockpit;
    navigation[Input::LookUp] = State::PlaneCockpit;

    layouts.emplace_back(raylib::Vector2(247, 80), TextureCache::LoadStillCel("stillcel/raft.cel"), Item::Raft, Strings::Lookup(432), Strings::Lookup(433), Strings::Lookup(434));
}

LabZombieScene::LabZombieScene(Panel *panel, const Entrance &new_entrance) : Scene(panel, "stillcel/labbkg.cel") {
    entrance = new_entrance;

    static const std::vector<std::optional<raylib::TextureUnmanaged>> monitor = {
        TextureCache::LoadStillCel("stillcel/labd1.cel"),
        TextureCache::LoadStillCel("stillcel/labd2.cel"),
        TextureCache::LoadStillCel("stillcel/labd3.cel"),
        TextureCache::LoadStillCel("stillcel/labd4.cel"),
        TextureCache::LoadStillCel("stillcel/labd5.cel"),
    };

    animations[1] = Animation(raylib::Vector2(260, 6), monitor);

    static const std::vector<std::optional<raylib::TextureUnmanaged>> arc = {
        TextureCache::LoadStillCel("stillcel/labb1.cel"),
        TextureCache::LoadStillCel("stillcel/labb2.cel"),
        TextureCache::LoadStillCel("stillcel/labb3.cel"),
        TextureCache::LoadStillCel("stillcel/labb4.cel"),
        TextureCache::LoadStillCel("stillcel/labb5.cel"),
    };

    animations[2] = Animation(raylib::Vector2(174, 0), arc);

    static const std::vector<std::optional<raylib::TextureUnmanaged>> test_tubes = {
        TextureCache::LoadStillCel("stillcel/laba1.cel"),
        TextureCache::LoadStillCel("stillcel/laba2.cel"),
        TextureCache::LoadStillCel("stillcel/laba3.cel"),
        TextureCache::LoadStillCel("stillcel/laba4.cel"),
        TextureCache::LoadStillCel("stillcel/laba5.cel"),
    };

    animations[3] = Animation(raylib::Vector2(10, 33), test_tubes);

    static const std::vector<std::optional<raylib::TextureUnmanaged>> scanner = {
        TextureCache::LoadStillCel("stillcel/labc1.cel"),
        TextureCache::LoadStillCel("stillcel/labc2.cel"),
        TextureCache::LoadStillCel("stillcel/labc3.cel"),
        TextureCache::LoadStillCel("stillcel/labc4.cel"),
        TextureCache::LoadStillCel("stillcel/labc5.cel"),
    };

    animations[4] = Animation(raylib::Vector2(259, 89), scanner);

    layouts.emplace_back(raylib::Vector2(87, 46), TextureCache::LoadStillCel("stillcel/beaker.cel"), Item::Beaker, Strings::Lookup(403), Strings::Lookup(404), Strings::Lookup(405));
    layouts.emplace_back(raylib::Vector2(163, 128), TextureCache::LoadStillCel("stillcel/syringe.cel"), Item::Syringe, Strings::Lookup(409), Strings::Lookup(410), Strings::Lookup(411));

    navigation[Input::StepBack] = State::World;
    navigation[Input::LookDown] = State::World;
}

LabCompanionScene::LabCompanionScene(Panel *panel, const Entrance &new_entrance) : Scene(panel, "stillcel/room18.cel") {
    entrance = new_entrance;

    layouts.emplace_back(raylib::Vector2(171, 78), raylib::Vector2(11, 7), Item::LabButtonA, Strings::Lookup(438), Strings::Lookup(438), Strings::Lookup(438));
    layouts.emplace_back(raylib::Vector2(196, 73), raylib::Vector2(14, 5), Item::LabButtonB, Strings::Lookup(438), Strings::Lookup(438), Strings::Lookup(438));
    layouts.emplace_back(raylib::Vector2(221, 68), raylib::Vector2(13, 5), Item::LabButtonC, Strings::Lookup(438), Strings::Lookup(438), Strings::Lookup(438));
    layouts.emplace_back(raylib::Vector2(199, 88), raylib::Vector2(9, 5), Item::LabButtonD, Strings::Lookup(438), Strings::Lookup(438), Strings::Lookup(438));
    layouts.emplace_back(raylib::Vector2(222, 82), raylib::Vector2(12, 5), Item::LabButtonE, Strings::Lookup(438), Strings::Lookup(438), Strings::Lookup(438));
    layouts.emplace_back(raylib::Vector2(241, 77), raylib::Vector2(8, 4), Item::LabButtonF, Strings::Lookup(438), Strings::Lookup(438), Strings::Lookup(438));

    navigation[Input::StepBack] = State::World;
    navigation[Input::LookDown] = State::World;
}

std::string LabCompanionScene::useItem(Player *player, Item item) {
    const static raylib::TextureUnmanaged alt_background = TextureCache::LoadStillCel("stillcel/room24.cel");

    switch (item) {
        case Item::LabButtonA:
            player->setState(State::Lab2A);
            break;
        case Item::LabButtonB:
            player->setState(State::Lab2B);
            break;
        case Item::LabButtonC:
            player->setState(State::Lab2C);
            break;
        case Item::LabButtonD:
            player->setState(State::Lab2D);
            break;
        case Item::LabButtonE:
            player->setState(State::Lab2E);
            break;
        case Item::LabButtonF:
            background = alt_background;
            player->setState(State::Lab2F);

            removeItemLayout(Item::LabButtonA);
            removeItemLayout(Item::LabButtonB);
            removeItemLayout(Item::LabButtonC);
            removeItemLayout(Item::LabButtonD);
            removeItemLayout(Item::LabButtonE);
            removeItemLayout(Item::LabButtonF);

            player->addItem(Item::Companion);
            break;
        default:
            return Scene::useItem(player, item);
    }

    return "";
}

std::tuple<bool, std::string, DeathType> LabCompanionScene::getItem(const Layout &layout) {
    return std::make_tuple(false, "", DeathType::None);
}

void LabCompanionScene::draw(Player *player, int scale) {
    static bool sound_played = false;

    if (!sound_played) {
        raylib::Sound *help = SoundCache::Load("sound/helpbabe.voc");
        help->Play();
        sound_played = true;
    }

    Scene::draw(player, scale);
}

MirrorScene::MirrorScene(Panel *panel, const Entrance &new_entrance) : Scene(panel, "stillcel/room16.cel") {
    entrance = new_entrance;

    layouts.emplace_back(raylib::Vector2(226, 103), TextureCache::LoadStillCel("stillcel/drug.cel"), Item::Drug, Strings::Lookup(409), Strings::Lookup(410), Strings::Lookup(411));

    navigation[Input::StepBack] = State::World;
    navigation[Input::LookDown] = State::World;
}

ShamanScene::ShamanScene(Panel *panel, const Entrance &new_entrance) : Scene(panel, "stillcel/shaman1.cel") {
    entrance = new_entrance;

    static const std::vector<std::optional<raylib::TextureUnmanaged>> smoke = {
        TextureCache::LoadStillCel("stillcel/smoke1.cel"),
        TextureCache::LoadStillCel("stillcel/smoke2.cel"),
        TextureCache::LoadStillCel("stillcel/smoke3.cel"),
        TextureCache::LoadStillCel("stillcel/smoke4.cel"),
    };

    animations[1] = Animation(raylib::Vector2(230, 61), smoke, 12);

    scripts[0] = {
        Dialogue(Strings::Lookup(0), raylib::RAYWHITE, SoundCache::Load("sound/umaguma.voc")),
        Dialogue(Strings::Lookup(2)),
    };

    scripts[1] = {
        Dialogue(Strings::Lookup(3), raylib::RAYWHITE, SoundCache::Load("sound/sha1.voc")),
        Dialogue(Strings::Lookup(5)),
        Dialogue(Strings::Lookup(6), raylib::RAYWHITE, SoundCache::Load("sound/sha2.voc")),
        Dialogue(Strings::Lookup(8)),
        Dialogue(Strings::Lookup(9), raylib::RAYWHITE, SoundCache::Load("sound/sha3.voc")),
        Dialogue(Strings::Lookup(12), raylib::RAYWHITE, SoundCache::Load("sound/sha4.voc")),
        Dialogue(Strings::Lookup(14)),
        Dialogue(Strings::Lookup(15), raylib::RAYWHITE, SoundCache::Load("sound/sha5.voc")),
        Dialogue(Strings::Lookup(17), raylib::RAYWHITE, SoundCache::Load("sound/sha6.voc")),
        Dialogue(Strings::Lookup(19)),
        Dialogue(Strings::Lookup(20), raylib::RAYWHITE, SoundCache::Load("sound/sha7.voc")),
        Dialogue(Strings::Lookup(22), raylib::RAYWHITE, SoundCache::Load("sound/sha8.voc")),
        Dialogue(Strings::Lookup(24)),
        Dialogue(Strings::Lookup(25), raylib::RAYWHITE, SoundCache::Load("sound/sha9.voc")),
        Dialogue(Strings::Lookup(27)),
        Dialogue(Strings::Lookup(28)),
        Dialogue(Strings::Lookup(29), raylib::RAYWHITE, SoundCache::Load("sound/sha10.voc")),
        Dialogue(Strings::Lookup(31)),
        Dialogue(Strings::Lookup(32), raylib::RAYWHITE, SoundCache::Load("sound/sha11.voc")),
        Dialogue(Strings::Lookup(34)),
        Dialogue(Strings::Lookup(35), raylib::RAYWHITE, SoundCache::Load("sound/sha12.voc")),
        Dialogue(Strings::Lookup(38), raylib::RAYWHITE, SoundCache::Load("sound/sha13.voc")),
        Dialogue(Strings::Lookup(40), raylib::RAYWHITE, SoundCache::Load("sound/sha14.voc")),
        Dialogue(Strings::Lookup(42)),
        Dialogue(Strings::Lookup(43), raylib::RAYWHITE, SoundCache::Load("sound/sha15.voc")),
    };

    scripts[2] = {
        Dialogue(Strings::Lookup(45), raylib::RAYWHITE, SoundCache::Load("sound/sha16.voc")),
        Dialogue(Strings::Lookup(47), raylib::RAYWHITE, SoundCache::Load("sound/sha17.voc")),
        Dialogue(Strings::Lookup(49)),
        Dialogue(Strings::Lookup(50), raylib::RAYWHITE, SoundCache::Load("sound/sha18.voc")),
        Dialogue(Strings::Lookup(52)),
    };

    layouts.emplace_back(raylib::Vector2(145, 58), TextureCache::LoadStillCel("stillcel/medallio.cel"), Item::GoldMedal1, Strings::Lookup(337), Strings::Lookup(338), Strings::Lookup(339));
    layouts.emplace_back(raylib::Vector2(275, 22), TextureCache::LoadStillCel("stillcel/bolt.cel"), Item::BoltCutters, Strings::Lookup(361), Strings::Lookup(338), Strings::Lookup(339));

    layouts.emplace_back(raylib::Vector2(131, 7), raylib::Vector2(40, 36), Item::Shaman, Strings::Lookup(346), Strings::Lookup(347), "");
    layouts.emplace_back(raylib::Vector2(67, 93), raylib::Vector2(28, 40), Item::Bong, Strings::Lookup(341), Strings::Lookup(342), "");
    layouts.emplace_back(raylib::Vector2(230, 61), raylib::Vector2(19, 50), Item::Bong, Strings::Lookup(352), Strings::Lookup(353), "");
    layouts.emplace_back(raylib::Vector2(189, 0), raylib::Vector2(30, 30), Item::Mask, Strings::Lookup(357), Strings::Lookup(358), "");
    layouts.emplace_back(raylib::Vector2(83, 0), raylib::Vector2(30, 30), Item::Mask, Strings::Lookup(357), Strings::Lookup(358), "");

    navigation[Input::StepBack] = State::World;
    navigation[Input::LookDown] = State::World;
}

std::optional<Scene::Dialogue> ShamanScene::talk(Player *player) {
    if (!player->testFlag(Flag::TranslateShaman)) {
        script = 0;
    } else if (script == 0) {
        script = 1;
        dialogueIndex = 0;
    }

    if (script == 0) {
        if (dialogueIndex >= scripts[script].size()) {
            dialogueIndex = 0;
        }

        return scripts[script][dialogueIndex++];
    } else if (script == 1) {
        if (dialogueIndex < scripts[script].size()) {
            return scripts[script][dialogueIndex++];
        }

        if (!player->getItemCount(Item::GoldMedal2)) {
            return Dialogue(Strings::Lookup(223), raylib::RAYWHITE);
        }

        if (gaveSmokes) {
            script = 2;
        }
    } else if (script == 2) {
        if (!player->getItemCount(Item::GoldMedal2)) {
            return Dialogue(Strings::Lookup(223), raylib::RAYWHITE);
        }

        if (dialogueIndex < scripts[script].size()) {
            return scripts[script][dialogueIndex++];
        }
        script = 3;
    }

    return Scene::talk(player);
}

std::tuple<bool, std::string, DeathType> ShamanScene::useItemOnItem(Item source, Item destination) {
    if (source == Item::Book && destination == Item::Shaman) {
        if (script == 0) {
            script = 1;
            dialogueIndex = 0;
        }

        return std::make_tuple(true, "", DeathType::None);
    }

    if (source == Item::Smokes && destination == Item::Shaman) {
        if (script == 1) {
            script = 2;
            dialogueIndex = 0;
        }

        return std::make_tuple(true, Strings::Lookup(349), DeathType::None);
    }

    return Scene::useItemOnItem(source, destination);
}

std::string ShamanScene::useItem(Player *player, Item item) {
    if (item == Item::Bong) {
        return Strings::Lookup(344);
    }

    if (item == Item::Shaman) {
        return Strings::Lookup(351);
    }

    return Scene::useItem(player, item);
}

std::tuple<bool, std::string, DeathType> ShamanScene::getItem(const Layout &layout) {
    if (layout.item == Item::Bong || layout.item == Item::Mask) {
        return std::make_tuple(false, Strings::Lookup(360), DeathType::None);
    }

    if (layout.item == Item::Cig) {
        return std::make_tuple(false, Strings::Lookup(354), DeathType::None);
    }

    if (layout.item == Item::GoldMedal1 || layout.item == Item::BoltCutters) {
        if (script != 3) {
            return std::make_tuple(false, Strings::Lookup(360), DeathType::None);
        }
    }

    return Scene::getItem(layout);
}

ChiefScene::ChiefScene(Panel *panel, const Entrance &new_entrance) : Scene(panel, "stillcel/chiefbkg.cel") {
    entrance = new_entrance;

    layouts.emplace_back(raylib::Vector2(137, 73), TextureCache::LoadStillCel("stillcel/medal2.cel"), Item::GoldMedal2, Strings::Lookup(337), Strings::Lookup(338), Strings::Lookup(339));

    static const std::vector<std::optional<raylib::TextureUnmanaged>> eyes_left = {
        TextureCache::LoadStillCel("stillcel/eyesl1.cel"),
        TextureCache::LoadStillCel("stillcel/eyesl2.cel"),
        TextureCache::LoadStillCel("stillcel/eyesl3.cel"),
        TextureCache::LoadStillCel("stillcel/eyesl4.cel"),
        TextureCache::LoadStillCel("stillcel/eyesl5.cel"),
        TextureCache::LoadStillCel("stillcel/eyesl6.cel"),
        TextureCache::LoadStillCel("stillcel/eyesl7.cel"),
        TextureCache::LoadStillCel("stillcel/eyesl8.cel"),
        TextureCache::LoadStillCel("stillcel/eyesl9.cel"),
        TextureCache::LoadStillCel("stillcel/eyesl10.cel"),
        TextureCache::LoadStillCel("stillcel/eyesl11.cel"),
        TextureCache::LoadStillCel("stillcel/eyesl12.cel"),
    };

    animations[1] = Animation(raylib::Vector2(16, 55), eyes_left, 6);

    static const std::vector<std::optional<raylib::TextureUnmanaged>> eyes_middle = {
        TextureCache::LoadStillCel("stillcel/eyesm1.cel"),
        TextureCache::LoadStillCel("stillcel/eyesm2.cel"),
        TextureCache::LoadStillCel("stillcel/eyesm3.cel"),
        TextureCache::LoadStillCel("stillcel/eyesm4.cel"),
        TextureCache::LoadStillCel("stillcel/eyesm5.cel"),
        TextureCache::LoadStillCel("stillcel/eyesm6.cel"),
        TextureCache::LoadStillCel("stillcel/eyesm7.cel"),
        TextureCache::LoadStillCel("stillcel/eyesm8.cel"),
        TextureCache::LoadStillCel("stillcel/eyesm9.cel"),
        TextureCache::LoadStillCel("stillcel/eyesm10.cel"),
        TextureCache::LoadStillCel("stillcel/eyesm11.cel"),
        TextureCache::LoadStillCel("stillcel/eyesm12.cel"),
    };

    animations[2] = Animation(raylib::Vector2(149, 23), eyes_middle, 6);

    static const std::vector<std::optional<raylib::TextureUnmanaged>> fan = {
        TextureCache::LoadStillCel("stillcel/fan1.cel"),
        TextureCache::LoadStillCel("stillcel/fan2.cel"),
        TextureCache::LoadStillCel("stillcel/fan3.cel"),
        TextureCache::LoadStillCel("stillcel/fan4.cel"),
        TextureCache::LoadStillCel("stillcel/fan5.cel"),
        TextureCache::LoadStillCel("stillcel/fan6.cel"),
        TextureCache::LoadStillCel("stillcel/fan7.cel"),
        TextureCache::LoadStillCel("stillcel/fan8.cel"),
        TextureCache::LoadStillCel("stillcel/fan9.cel"),
        TextureCache::LoadStillCel("stillcel/fan10.cel"),
        TextureCache::LoadStillCel("stillcel/fan11.cel"),
        TextureCache::LoadStillCel("stillcel/fan12.cel"),
    };

    animations[3] = Animation(raylib::Vector2(243, 3), fan, 6);

    scripts[0] = {
        Dialogue(Strings::Lookup(120), raylib::RAYWHITE, SoundCache::Load("sound/umaguma.voc")),
        Dialogue(Strings::Lookup(122)),
    };

    scripts[1] = {
        Dialogue(Strings::Lookup(124), raylib::RAYWHITE, SoundCache::Load("sound/chf1.voc")),
        Dialogue(Strings::Lookup(126)),
        Dialogue(Strings::Lookup(127)),
        Dialogue(Strings::Lookup(128), raylib::RAYWHITE, SoundCache::Load("sound/chf2a.voc")),
        Dialogue(Strings::Lookup(130)),
        Dialogue(Strings::Lookup(131), raylib::RAYWHITE, SoundCache::Load("sound/chf3a.voc")),
        Dialogue(Strings::Lookup(133), raylib::RAYWHITE, SoundCache::Load("sound/chf3b.voc")),
        Dialogue(Strings::Lookup(135), raylib::RAYWHITE, SoundCache::Load("sound/chf3c.voc")),
        Dialogue(Strings::Lookup(137)),
        Dialogue(Strings::Lookup(138)),
        Dialogue(Strings::Lookup(139)),
        Dialogue(Strings::Lookup(141), raylib::RAYWHITE, SoundCache::Load("sound/chf4a.voc")),
        Dialogue(Strings::Lookup(143), raylib::RAYWHITE, SoundCache::Load("sound/chf4b.voc")),
        Dialogue(Strings::Lookup(145), raylib::RAYWHITE, SoundCache::Load("sound/chf4c.voc")),
        Dialogue(Strings::Lookup(147), raylib::RAYWHITE, SoundCache::Load("sound/chf4d.voc")),
        Dialogue(Strings::Lookup(149), raylib::RAYWHITE, SoundCache::Load("sound/chf4e.voc")),
        Dialogue(Strings::Lookup(151), raylib::RAYWHITE, SoundCache::Load("sound/chf4f.voc")),
        Dialogue(Strings::Lookup(153)),
        Dialogue(Strings::Lookup(154), raylib::RAYWHITE, SoundCache::Load("sound/chf5.voc")),
        Dialogue(Strings::Lookup(156)),
    };

    scripts[2] = {
        Dialogue(Strings::Lookup(157), raylib::RAYWHITE, SoundCache::Load("sound/chf6a.voc")),
        Dialogue(Strings::Lookup(160), raylib::RAYWHITE, SoundCache::Load("sound/chf6b.voc")),
        Dialogue(Strings::Lookup(162)),
        Dialogue(Strings::Lookup(163), raylib::RAYWHITE, SoundCache::Load("sound/chf7b.voc")),
        Dialogue(Strings::Lookup(165), raylib::RAYWHITE, SoundCache::Load("sound/chftake.voc")),
        Dialogue(Strings::Lookup(167)),
        Dialogue(Strings::Lookup(168), raylib::RAYWHITE, SoundCache::Load("sound/chf8.voc")),
        Dialogue(Strings::Lookup(170)),
        Dialogue(Strings::Lookup(171), raylib::RAYWHITE, SoundCache::Load("sound/chf9.voc")),
        Dialogue(Strings::Lookup(173)),
    };

    navigation[Input::StepBack] = State::World;
    navigation[Input::LookDown] = State::World;
}

std::optional<Scene::Dialogue> ChiefScene::talk(Player *player) {
    if (!player->testFlag(Flag::TranslateChief)) {
        script = 0;
    } else if (script == 0) {
        script = 1;
        dialogueIndex = 0;
    }

    if (script == 0) {
        if (dialogueIndex >= scripts[script].size()) {
            dialogueIndex = 0;
        }

        return scripts[script][dialogueIndex++];
    } else if (script == 1) {
        if (dialogueIndex < scripts[script].size()) {
            return scripts[script][dialogueIndex++];
        }

        if (player->testFlag(Flag::NoiseStopped)) {
            script = 2;
            dialogueIndex = 0;
        }
    } else if (script == 2) {
        if (dialogueIndex < scripts[script].size()) {
            return scripts[script][dialogueIndex++];
        }
        script = 3;
    }

    return Scene::talk(player);
}

std::tuple<bool, std::string, DeathType> ChiefScene::getItem(const Layout &layout) {
    if (layout.item == Item::GoldMedal2) {
        if (script != 3) {
            return std::make_tuple(false, Strings::Lookup(360), DeathType::None);
        }
    }

    return Scene::getItem(layout);
}

VillageGateShamanScene::VillageGateShamanScene(Panel *panel, const Entrance &new_entrance) : Scene(panel, "stillcel/gardsabg.cel") {
    entrance = new_entrance;
    scripts[0] = {
        Dialogue(Strings::Lookup(53), raylib::RAYWHITE, SoundCache::Load("sound/umaguma.voc")),
        Dialogue(Strings::Lookup(54)),
    };

    scripts[1] = {
        Dialogue(Strings::Lookup(55), raylib::RAYWHITE),
        Dialogue(Strings::Lookup(56)),
        Dialogue(Strings::Lookup(57), raylib::RAYWHITE),
        Dialogue(Strings::Lookup(58)),
        Dialogue(Strings::Lookup(59), raylib::RAYWHITE),
        Dialogue(Strings::Lookup(60)),
        Dialogue(Strings::Lookup(61), raylib::RAYWHITE),
        Dialogue(Strings::Lookup(62)),
        Dialogue(Strings::Lookup(63), raylib::RAYWHITE),
        Dialogue(Strings::Lookup(64)),
        Dialogue(Strings::Lookup(65), raylib::RAYWHITE),
        Dialogue(Strings::Lookup(66)),
        Dialogue(Strings::Lookup(68)),
        Dialogue(Strings::Lookup(69), raylib::RAYWHITE),
        Dialogue(Strings::Lookup(70)),
        Dialogue(Strings::Lookup(71), raylib::RAYWHITE),
    };
}

std::optional<Scene::Dialogue> VillageGateShamanScene::talk(Player *player) {
    const static raylib::TextureUnmanaged alt_background = TextureCache::LoadStillCel("stillcel/gardsa2.cel");

    if (!player->testFlag(Flag::TranslateVillage1)) {
        script = 0;
    } else if (script == 0) {
        script = 1;
        dialogueIndex = 0;
    }

    if (script == 2)
        return Scene::talk(player);

    if (script == 0) {
        if (dialogueIndex >= scripts[script].size()) {
            dialogueIndex = 0;
        }

        return scripts[script][dialogueIndex++];
    } else {
        if (dialogueIndex < scripts[script].size()) {
            return scripts[script][dialogueIndex++];
        }

        script = 2;
    }

    background = alt_background;

    navigation[Input::StepForward] = State::World;
    navigation[Input::LookUp] = State::World;

    player->setFlag(Flag::VisitedVillage);

    return Scene::talk(player);
}

VillageGateChiefScene::VillageGateChiefScene(Panel *panel, const Entrance &new_entrance) : Scene(panel, "stillcel/gardsbbg.cel") {
    entrance = new_entrance;
    scripts[0] = {
        Dialogue(Strings::Lookup(72), raylib::RAYWHITE, SoundCache::Load("sound/umaguma.voc")),
        Dialogue(Strings::Lookup(73)),

    };

    scripts[1] = {
        Dialogue(Strings::Lookup(223), raylib::RAYWHITE),
        Dialogue(Strings::Lookup(73)),
    };

    scripts[2] = {
        Dialogue(Strings::Lookup(74), raylib::RAYWHITE),
        Dialogue(Strings::Lookup(75)),
        Dialogue(Strings::Lookup(76), raylib::RAYWHITE),
        Dialogue(Strings::Lookup(77), raylib::RAYWHITE),
        Dialogue(Strings::Lookup(78)),
        Dialogue(Strings::Lookup(79), raylib::RAYWHITE),
        Dialogue(Strings::Lookup(80)),
        Dialogue(Strings::Lookup(81), raylib::RAYWHITE),
        Dialogue(Strings::Lookup(82)),
        Dialogue(Strings::Lookup(83), raylib::RAYWHITE),
        Dialogue(Strings::Lookup(84)),
        Dialogue(Strings::Lookup(86)),
        Dialogue(Strings::Lookup(87), raylib::RAYWHITE),
        Dialogue(Strings::Lookup(88)),
    };
}

std::optional<Scene::Dialogue> VillageGateChiefScene::talk(Player *player) {
    static const raylib::TextureUnmanaged alt_background = TextureCache::LoadStillCel("stillcel/gardsb2.cel");

    if (!player->testFlag(Flag::TranslateVillage2)) {
        script = 0;
    } else if (script == 0) {
        if (player->testFlag(Flag::VisitedVillage)) {
            script = 2;
        } else {
            script = 1;
        }

        dialogueIndex = 0;
    } else if (script == 1) {
        if (player->testFlag(Flag::VisitedVillage)) {
            script = 2;
            dialogueIndex = 0;
        }
    }

    if (script == 3)
        return Scene::talk(player);

    if (script == 0 || script == 1) {
        if (dialogueIndex >= scripts[script].size()) {
            dialogueIndex = 0;
        }

        return scripts[script][dialogueIndex++];
    } else {
        if (dialogueIndex < scripts[script].size()) {
            return scripts[script][dialogueIndex++];
        }

        script = 3;
    }

    background = alt_background;

    navigation[Input::StepForward] = State::World;
    navigation[Input::LookUp] = State::World;

    return Scene::talk(player);
}

