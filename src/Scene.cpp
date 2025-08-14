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

#include "Scene.h"
#include "StillCel.h"
#include "Strings.h"

Scene::Scene(Panel *panel, const std::string &background_filename) : panel(panel) {
    background = StillCel(background_filename).getTexture(); 
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

                std::cerr << position.ToString() << "\n";
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

    layouts.emplace_back(raylib::Vector2(203, 115), StillCel("stillcel/clippers.cel").getTexture(), Item::WireClipper, Strings::Lookup(315), Strings::Lookup(316), Strings::Lookup(317));

    navigation[Input::StepBack] = State::World;
    navigation[Input::LookDown] = State::World;
    navigation[Input::TurnLeft] = State::CrashedPlaneLeft;
    navigation[Input::TurnRight] = State::CrashedPlaneRight;
}

CrashedPlaneLeftScene::CrashedPlaneLeftScene(Panel *panel) : Scene(panel, "stillcel/room1.cel") {
    layouts.emplace_back(raylib::Vector2(36, 127), StillCel("stillcel/phbook.cel").getTexture(), Item::Book, Strings::Lookup(334), Strings::Lookup(335), Strings::Lookup(336));

    navigation[Input::StepForward] = State::CrashedPlaneCockpit;
    navigation[Input::LookUp] = State::CrashedPlaneCockpit;
    navigation[Input::TurnLeft] = State::CrashedPlaneExit;
    navigation[Input::TurnRight] = State::CrashedPlaneEntry;
}

CrashedPlaneCockpitScene::CrashedPlaneCockpitScene(Panel *panel) : Scene(panel, "stillcel/room2.cel") {
    layouts.emplace_back(raylib::Vector2(166, 48), StillCel("stillcel/compass.cel").getTexture(), Item::StuckCompass, Strings::Lookup(320), Strings::Lookup(321), Strings::Lookup(322));
    layouts.emplace_back(raylib::Vector2(98, 92), StillCel("stillcel/flaregun.cel").getTexture(), Item::FlareGun, Strings::Lookup(327), Strings::Lookup(328), Strings::Lookup(329));

    navigation[Input::StepBack] = State::CrashedPlaneLeft;
    navigation[Input::LookDown] = State::CrashedPlaneLeft;
}

std::tuple<bool, std::string, DeathType> CrashedPlaneCockpitScene::useItemOnItem(Item source, Item destination) {
    const static std::string DIDNT_WORK = Strings::Lookup(326);

    if (source == Item::WireClipper && destination == Item::StuckCompass) {
        layouts.erase(std::remove_if(std::begin(layouts), std::end(layouts), [](const auto &layout) {
            return layout.item == Item::StuckCompass;
        }));

        layouts.emplace_back(raylib::Vector2(166, 48), StillCel("stillcel/compass.cel").getTexture(), Item::Compass, Strings::Lookup(320), Strings::Lookup(321), Strings::Lookup(322));

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
    layouts.emplace_back(raylib::Vector2(25, 62), StillCel("stillcel/machetis.cel").getTexture(), Item::Machete, Strings::Lookup(308), Strings::Lookup(309), Strings::Lookup(310));

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

    layouts.emplace_back(raylib::Vector2(244, 71), StillCel("stillcel/guninbun.cel").getTexture(), Item::Rifle, Strings::Lookup(376), Strings::Lookup(377), Strings::Lookup(378));
    layouts.emplace_back(raylib::Vector2(14, 83), StillCel("stillcel/crates.cel").getTexture(), Item::Crate, Strings::Lookup(364), Strings::Lookup(365), Strings::Lookup(366));
    layouts.emplace_back(raylib::Vector2(64, 133), StillCel("stillcel/wire.cel").getTexture(), Item::TripWire, Strings::Lookup(370), Strings::Lookup(370), Strings::Lookup(370));

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
    layouts.emplace_back(raylib::Vector2(0, 0), StillCel("stillcel/bunkleft.cel").getTexture(), Item::Soldier, Strings::Lookup(391), Strings::Lookup(392), Strings::Lookup(393));

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
    layouts.emplace_back(raylib::Vector2(7, 41), StillCel("stillcel/rag.cel").getTexture(), Item::Rags, Strings::Lookup(376), Strings::Lookup(377), Strings::Lookup(378));
    layouts.emplace_back(raylib::Vector2(86, 66), StillCel("stillcel/oil.cel").getTexture(), Item::OilCan, Strings::Lookup(364), Strings::Lookup(365), Strings::Lookup(366));

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
        layouts.emplace_back(raylib::Vector2(144, 97), StillCel("stillcel/hole1.cel").getTexture(), Item::MedalHoleFilled1, Strings::Lookup(334), Strings::Lookup(335), Strings::Lookup(336));
        return make_tuple(true, Strings::Lookup(425), DeathType::None);
    }

    if (source == Item::GoldMedal2 && destination == Item::MedalHole2) {
        static const std::vector<std::optional<raylib::TextureUnmanaged>> left_eye = {
             StillCel("stillcel/lefteye1.cel").getTexture(),
             StillCel("stillcel/lefteye2.cel").getTexture(),
             StillCel("stillcel/lefteye3.cel").getTexture(),
             StillCel("stillcel/lefteye4.cel").getTexture(),
        };

        static const std::vector<std::optional<raylib::TextureUnmanaged>> right_eye = {
             StillCel("stillcel/rteye1.cel").getTexture(),
             StillCel("stillcel/rteye2.cel").getTexture(),
             StillCel("stillcel/rteye3.cel").getTexture(),
             StillCel("stillcel/rteye4.cel").getTexture(),
        };

        layouts.emplace_back(raylib::Vector2(168, 97), StillCel("stillcel/hole1.cel").getTexture(), Item::MedalHoleFilled2, Strings::Lookup(334), Strings::Lookup(335), Strings::Lookup(336));

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
            StillCel("stillcel/tmpdoor1.cel").getTexture(),
            StillCel("stillcel/tmpdoor2.cel").getTexture(),
            StillCel("stillcel/tmpdoor3.cel").getTexture(),
            StillCel("stillcel/tmpdoor4.cel").getTexture(),
            StillCel("stillcel/tmpdoor5.cel").getTexture(),
            StillCel("stillcel/tmpdoor6.cel").getTexture(),
            StillCel("stillcel/tmpdoor7.cel").getTexture(),
            StillCel("stillcel/tmpdoor8.cel").getTexture(),
        };

        removeItemLayout(Item::MedalHoleFilled1);
        removeItemLayout(Item::MedalHoleFilled2);

        animations[3] = Animation(raylib::Vector2(101.0f, 86.0f), doors, false);
    } else if (animation_id == 3) {
        opened = true;

        navigation[Input::StepForward] = State::Oracle;
        navigation[Input::LookUp] = State::Oracle;

        animations[4] = Animation(raylib::Vector2(101.0f, 86.0f), {StillCel("stillcel/tmpdoor8.cel").getTexture()});
    }
}

OracleScene::OracleScene(Panel *panel, const Entrance &new_entrance) : Scene(panel, "stillcel/oraclebg.cel") {
    entrance = new_entrance;

    static raylib::Sound oracle_chat1 = raylib::Sound(Voc::Load("sound/or1.voc"));
    static raylib::Sound oracle_chat2 = raylib::Sound(Voc::Load("sound/or2.voc"));
    static raylib::Sound oracle_chat3 = raylib::Sound(Voc::Load("sound/or3.voc"));
    static raylib::Sound oracle_chat4 = raylib::Sound(Voc::Load("sound/or4.voc"));
    static raylib::Sound oracle_chat5 = raylib::Sound(Voc::Load("sound/or5.voc"));
    static raylib::Sound oracle_chat7 = raylib::Sound(Voc::Load("sound/or7.voc"));
    static raylib::Sound oracle_chat8 = raylib::Sound(Voc::Load("sound/or8.voc"));
    static raylib::Sound oracle_chat9 = raylib::Sound(Voc::Load("sound/or9.voc"));
    static raylib::Sound oracle_chat10 = raylib::Sound(Voc::Load("sound/or10.voc"));
    static raylib::Sound oracle_chat11 = raylib::Sound(Voc::Load("sound/or11.voc"));

    layouts.emplace_back(raylib::Vector2(122, 10), raylib::Vector2(73, 83), Item::Oracle, Strings::Lookup(417), Strings::Lookup(418), "");

    scripts[0] = {
        Dialogue(Strings::Lookup(89)),
        Dialogue(Strings::Lookup(90), raylib::RAYWHITE, &oracle_chat1),
        Dialogue(Strings::Lookup(92)),
        Dialogue(Strings::Lookup(93), raylib::RAYWHITE, &oracle_chat2),
        Dialogue(Strings::Lookup(95), raylib::RAYWHITE),
        Dialogue(Strings::Lookup(96)),
        Dialogue(Strings::Lookup(97), raylib::RAYWHITE, &oracle_chat3),
        Dialogue(Strings::Lookup(99)),
        Dialogue(Strings::Lookup(100), raylib::RAYWHITE, &oracle_chat4),
        Dialogue(Strings::Lookup(102)),
        Dialogue(Strings::Lookup(103), raylib::RAYWHITE, &oracle_chat5),
        Dialogue(Strings::Lookup(106)),
        Dialogue(Strings::Lookup(107), raylib::RAYWHITE, &oracle_chat7),
        Dialogue(Strings::Lookup(109)),
        Dialogue(Strings::Lookup(110), raylib::RAYWHITE, &oracle_chat8),
        Dialogue(Strings::Lookup(112)),
        Dialogue(Strings::Lookup(113), raylib::RAYWHITE, &oracle_chat9),
        Dialogue(Strings::Lookup(115)),
    };

    scripts[1] = {
        Dialogue(Strings::Lookup(116), raylib::RAYWHITE, &oracle_chat10),
    };

    scripts[2] = {
        Dialogue(Strings::Lookup(118), raylib::RAYWHITE, &oracle_chat11),
    };

    navigation[Input::StepBack] = State::World;
    navigation[Input::LookDown] = State::World;
}

std::tuple<bool, std::string, DeathType> OracleScene::useItemOnItem(Item source, Item destination) {
    if (source == Item::DeadWolf && destination == Item::Oracle) {
        sacrifice = true;
        static const std::vector<std::optional<raylib::TextureUnmanaged>> oracle = {
            StillCel("stillcel/flame1.cel").getTexture(),
            StillCel("stillcel/flame2.cel").getTexture(),
            StillCel("stillcel/flame3.cel").getTexture(),
            StillCel("stillcel/flame4.cel").getTexture(),
            StillCel("stillcel/flame5.cel").getTexture(),
            StillCel("stillcel/flame6.cel").getTexture(),
            StillCel("stillcel/flame7.cel").getTexture(),
            StillCel("stillcel/flame8.cel").getTexture(),
            StillCel("stillcel/flame9.cel").getTexture(),
            StillCel("stillcel/flame10.cel").getTexture(),
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
                static const auto uzi_item_texture = StillCel("stillcel/rcluzi.cel").getTexture();

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
        StillCel("stillcel/closeye1.cel").getTexture(),
        StillCel("stillcel/closeye2.cel").getTexture(),
    };

    animations[1] = Animation(raylib::Vector2(139, 18), eyes, true, 12);

    navigation[Input::StepBack] = State::World;
    navigation[Input::LookDown] = State::World;
}

ShowerScene::ShowerScene(Panel *panel, const Entrance &new_entrance) : Scene(panel, "stillcel/shower1.cel") {
    entrance = new_entrance;

    static const std::vector<std::optional<raylib::TextureUnmanaged>> eyes = {
        StillCel("stillcel/shower2.cel").getTexture(),
        StillCel("stillcel/shower3.cel").getTexture(),
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

    layouts.emplace_back(raylib::Vector2(29, 57), StillCel("stillcel/panel.cel").getTexture(), Item::Panel, Strings::Lookup(440), Strings::Lookup(441), Strings::Lookup(442));

    navigation[Input::StepBack] = State::World;
    navigation[Input::LookDown] = State::World;
}

std::tuple<bool, std::string, DeathType> RocketLauncherScene::getItem(const Layout &layout) {
    static const std::vector<std::optional<raylib::TextureUnmanaged>> wires1_anim = {
        StillCel("stillcel/wires1.cel").getTexture(),
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
        StillCel("stillcel/wires2.cel").getTexture(),
    };

    static const std::vector<std::optional<raylib::TextureUnmanaged>> wires3_anim = {
        StillCel("stillcel/wires3.cel").getTexture(),
    };


    if (source == Item::Machete && destination == Item::Panel) {
        layouts.emplace_back(raylib::Vector2(29, 57), StillCel("stillcel/panel.cel").getTexture(), Item::OpenPanel, Strings::Lookup(440), Strings::Lookup(441), Strings::Lookup(442));

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
            static raylib::Sound afraid(Voc::Load("sound/afraid.voc"));
            afraid.Play();

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

    layouts.emplace_back(raylib::Vector2(247, 80), StillCel("stillcel/raft.cel").getTexture(), Item::Raft, Strings::Lookup(432), Strings::Lookup(433), Strings::Lookup(434));
}

LabZombieScene::LabZombieScene(Panel *panel, const Entrance &new_entrance) : Scene(panel, "stillcel/labbkg.cel") {
    entrance = new_entrance;

    static const std::vector<std::optional<raylib::TextureUnmanaged>> monitor = {
        StillCel("stillcel/labd1.cel").getTexture(),
        StillCel("stillcel/labd2.cel").getTexture(),
        StillCel("stillcel/labd3.cel").getTexture(),
        StillCel("stillcel/labd4.cel").getTexture(),
        StillCel("stillcel/labd5.cel").getTexture(),
    };

    animations[1] = Animation(raylib::Vector2(260, 6), monitor);

    static const std::vector<std::optional<raylib::TextureUnmanaged>> arc = {
        StillCel("stillcel/labb1.cel").getTexture(),
        StillCel("stillcel/labb2.cel").getTexture(),
        StillCel("stillcel/labb3.cel").getTexture(),
        StillCel("stillcel/labb4.cel").getTexture(),
        StillCel("stillcel/labb5.cel").getTexture(),
    };

    animations[2] = Animation(raylib::Vector2(174, 0), arc);

    static const std::vector<std::optional<raylib::TextureUnmanaged>> test_tubes = {
        StillCel("stillcel/laba1.cel").getTexture(),
        StillCel("stillcel/laba2.cel").getTexture(),
        StillCel("stillcel/laba3.cel").getTexture(),
        StillCel("stillcel/laba4.cel").getTexture(),
        StillCel("stillcel/laba5.cel").getTexture(),
    };

    animations[3] = Animation(raylib::Vector2(10, 33), test_tubes);

    static const std::vector<std::optional<raylib::TextureUnmanaged>> scanner = {
        StillCel("stillcel/labc1.cel").getTexture(),
        StillCel("stillcel/labc2.cel").getTexture(),
        StillCel("stillcel/labc3.cel").getTexture(),
        StillCel("stillcel/labc4.cel").getTexture(),
        StillCel("stillcel/labc5.cel").getTexture(),
    };

    animations[4] = Animation(raylib::Vector2(259, 89), scanner);

    layouts.emplace_back(raylib::Vector2(87, 46), StillCel("stillcel/beaker.cel").getTexture(), Item::Beaker, Strings::Lookup(403), Strings::Lookup(404), Strings::Lookup(405));
    layouts.emplace_back(raylib::Vector2(163, 128), StillCel("stillcel/syringe.cel").getTexture(), Item::Syringe, Strings::Lookup(409), Strings::Lookup(410), Strings::Lookup(411));

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
    const static raylib::TextureUnmanaged alt_background = StillCel("stillcel/room24.cel").getTexture();

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

MirrorScene::MirrorScene(Panel *panel, const Entrance &new_entrance) : Scene(panel, "stillcel/room16.cel") {
    entrance = new_entrance;

    layouts.emplace_back(raylib::Vector2(226, 103), StillCel("stillcel/drug.cel").getTexture(), Item::Drug, Strings::Lookup(409), Strings::Lookup(410), Strings::Lookup(411));

    navigation[Input::StepBack] = State::World;
    navigation[Input::LookDown] = State::World;
}

ShamanScene::ShamanScene(Panel *panel, const Entrance &new_entrance) : Scene(panel, "stillcel/shaman1.cel") {
    entrance = new_entrance;

    static const std::vector<std::optional<raylib::TextureUnmanaged>> smoke = {
        StillCel("stillcel/smoke1.cel").getTexture(),
        StillCel("stillcel/smoke2.cel").getTexture(),
        StillCel("stillcel/smoke3.cel").getTexture(),
        StillCel("stillcel/smoke4.cel").getTexture(),
    };

    animations[1] = Animation(raylib::Vector2(230, 61), smoke, 12);

    static raylib::Sound shaman_talk(Voc::Load("sound/umaguma.voc"));
    static raylib::Sound shaman_chat1 = raylib::Sound(Voc::Load("sound/sha1.voc"));
    static raylib::Sound shaman_chat2 = raylib::Sound(Voc::Load("sound/sha2.voc"));
    static raylib::Sound shaman_chat3 = raylib::Sound(Voc::Load("sound/sha3.voc"));
    static raylib::Sound shaman_chat4 = raylib::Sound(Voc::Load("sound/sha4.voc"));
    static raylib::Sound shaman_chat5 = raylib::Sound(Voc::Load("sound/sha5.voc"));
    static raylib::Sound shaman_chat6 = raylib::Sound(Voc::Load("sound/sha6.voc"));
    static raylib::Sound shaman_chat7 = raylib::Sound(Voc::Load("sound/sha7.voc"));
    static raylib::Sound shaman_chat8 = raylib::Sound(Voc::Load("sound/sha8.voc"));
    static raylib::Sound shaman_chat9 = raylib::Sound(Voc::Load("sound/sha9.voc"));
    static raylib::Sound shaman_chat10 = raylib::Sound(Voc::Load("sound/sha10.voc"));
    static raylib::Sound shaman_chat11 = raylib::Sound(Voc::Load("sound/sha11.voc"));
    static raylib::Sound shaman_chat12 = raylib::Sound(Voc::Load("sound/sha12.voc"));
    static raylib::Sound shaman_chat13 = raylib::Sound(Voc::Load("sound/sha13.voc"));
    static raylib::Sound shaman_chat14 = raylib::Sound(Voc::Load("sound/sha14.voc"));
    static raylib::Sound shaman_chat15 = raylib::Sound(Voc::Load("sound/sha15.voc"));
    static raylib::Sound shaman_chat16 = raylib::Sound(Voc::Load("sound/sha16.voc"));
    static raylib::Sound shaman_chat17 = raylib::Sound(Voc::Load("sound/sha17.voc"));
    static raylib::Sound shaman_chat18 = raylib::Sound(Voc::Load("sound/sha18.voc"));

    scripts[0] = {
        Dialogue(Strings::Lookup(0), raylib::RAYWHITE, &shaman_talk),
        Dialogue(Strings::Lookup(2)),
    };

    scripts[1] = {
        Dialogue(Strings::Lookup(3), raylib::RAYWHITE, &shaman_chat1),
        Dialogue(Strings::Lookup(5)),
        Dialogue(Strings::Lookup(6), raylib::RAYWHITE, &shaman_chat2),
        Dialogue(Strings::Lookup(8)),
        Dialogue(Strings::Lookup(9), raylib::RAYWHITE, &shaman_chat3),
        Dialogue(Strings::Lookup(12), raylib::RAYWHITE, &shaman_chat4),
        Dialogue(Strings::Lookup(14)),
        Dialogue(Strings::Lookup(15), raylib::RAYWHITE, &shaman_chat5),
        Dialogue(Strings::Lookup(17), raylib::RAYWHITE, &shaman_chat6),
        Dialogue(Strings::Lookup(19)),
        Dialogue(Strings::Lookup(20), raylib::RAYWHITE, &shaman_chat7),
        Dialogue(Strings::Lookup(22), raylib::RAYWHITE, &shaman_chat8),
        Dialogue(Strings::Lookup(24)),
        Dialogue(Strings::Lookup(25), raylib::RAYWHITE, &shaman_chat9),
        Dialogue(Strings::Lookup(27)),
        Dialogue(Strings::Lookup(28)),
        Dialogue(Strings::Lookup(29), raylib::RAYWHITE, &shaman_chat10),
        Dialogue(Strings::Lookup(31)),
        Dialogue(Strings::Lookup(32), raylib::RAYWHITE, &shaman_chat11),
        Dialogue(Strings::Lookup(34)),
        Dialogue(Strings::Lookup(35), raylib::RAYWHITE, &shaman_chat12),
        Dialogue(Strings::Lookup(38), raylib::RAYWHITE, &shaman_chat13),
        Dialogue(Strings::Lookup(40), raylib::RAYWHITE, &shaman_chat14),
        Dialogue(Strings::Lookup(42)),
        Dialogue(Strings::Lookup(43), raylib::RAYWHITE, &shaman_chat15),
    };

    scripts[2] = {
        Dialogue(Strings::Lookup(45), raylib::RAYWHITE, &shaman_chat16),
        Dialogue(Strings::Lookup(47), raylib::RAYWHITE, &shaman_chat17),
        Dialogue(Strings::Lookup(49)),
        Dialogue(Strings::Lookup(50), raylib::RAYWHITE, &shaman_chat18),
        Dialogue(Strings::Lookup(52)),
    };

    layouts.emplace_back(raylib::Vector2(145, 58), StillCel("stillcel/medallio.cel").getTexture(), Item::GoldMedal1, Strings::Lookup(337), Strings::Lookup(338), Strings::Lookup(339));
    layouts.emplace_back(raylib::Vector2(275, 22), StillCel("stillcel/bolt.cel").getTexture(), Item::BoltCutters, Strings::Lookup(361), Strings::Lookup(338), Strings::Lookup(339));

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

    layouts.emplace_back(raylib::Vector2(137, 73), StillCel("stillcel/medal2.cel").getTexture(), Item::GoldMedal2, Strings::Lookup(337), Strings::Lookup(338), Strings::Lookup(339));

    static const std::vector<std::optional<raylib::TextureUnmanaged>> eyes_left = {
        StillCel("stillcel/eyesl1.cel").getTexture(),
        StillCel("stillcel/eyesl2.cel").getTexture(),
        StillCel("stillcel/eyesl3.cel").getTexture(),
        StillCel("stillcel/eyesl4.cel").getTexture(),
        StillCel("stillcel/eyesl5.cel").getTexture(),
        StillCel("stillcel/eyesl6.cel").getTexture(),
        StillCel("stillcel/eyesl7.cel").getTexture(),
        StillCel("stillcel/eyesl8.cel").getTexture(),
        StillCel("stillcel/eyesl9.cel").getTexture(),
        StillCel("stillcel/eyesl10.cel").getTexture(),
        StillCel("stillcel/eyesl11.cel").getTexture(),
        StillCel("stillcel/eyesl11.cel").getTexture(),
    };

    animations[1] = Animation(raylib::Vector2(16, 55), eyes_left, 6);

    static const std::vector<std::optional<raylib::TextureUnmanaged>> eyes_middle = {
        StillCel("stillcel/eyesm1.cel").getTexture(),
        StillCel("stillcel/eyesm2.cel").getTexture(),
        StillCel("stillcel/eyesm3.cel").getTexture(),
        StillCel("stillcel/eyesm4.cel").getTexture(),
        StillCel("stillcel/eyesm5.cel").getTexture(),
        StillCel("stillcel/eyesm6.cel").getTexture(),
        StillCel("stillcel/eyesm7.cel").getTexture(),
        StillCel("stillcel/eyesm8.cel").getTexture(),
        StillCel("stillcel/eyesm9.cel").getTexture(),
        StillCel("stillcel/eyesm10.cel").getTexture(),
        StillCel("stillcel/eyesm11.cel").getTexture(),
        StillCel("stillcel/eyesm11.cel").getTexture(),
    };

    animations[2] = Animation(raylib::Vector2(149, 23), eyes_middle, 6);

    static const std::vector<std::optional<raylib::TextureUnmanaged>> fan = {
        StillCel("stillcel/fan1.cel").getTexture(),
        StillCel("stillcel/fan2.cel").getTexture(),
        StillCel("stillcel/fan3.cel").getTexture(),
        StillCel("stillcel/fan4.cel").getTexture(),
        StillCel("stillcel/fan5.cel").getTexture(),
        StillCel("stillcel/fan6.cel").getTexture(),
        StillCel("stillcel/fan7.cel").getTexture(),
        StillCel("stillcel/fan8.cel").getTexture(),
        StillCel("stillcel/fan9.cel").getTexture(),
        StillCel("stillcel/fan10.cel").getTexture(),
        StillCel("stillcel/fan11.cel").getTexture(),
        StillCel("stillcel/fan12.cel").getTexture(),
    };

    animations[3] = Animation(raylib::Vector2(243, 3), fan, 6);

    static raylib::Sound chief_talk(Voc::Load("sound/umaguma.voc"));
    static raylib::Sound chief_chat1 = raylib::Sound(Voc::Load("sound/chf1.voc"));
    static raylib::Sound chief_chat2a = raylib::Sound(Voc::Load("sound/chf2a.voc"));
    static raylib::Sound chief_chat3a = raylib::Sound(Voc::Load("sound/chf3a.voc"));
    static raylib::Sound chief_chat3b = raylib::Sound(Voc::Load("sound/chf3b.voc"));
    static raylib::Sound chief_chat3c = raylib::Sound(Voc::Load("sound/chf3c.voc"));
    static raylib::Sound chief_chat4a = raylib::Sound(Voc::Load("sound/chf4a.voc"));
    static raylib::Sound chief_chat4b = raylib::Sound(Voc::Load("sound/chf4b.voc"));
    static raylib::Sound chief_chat4c = raylib::Sound(Voc::Load("sound/chf4c.voc"));
    static raylib::Sound chief_chat4d = raylib::Sound(Voc::Load("sound/chf4d.voc"));
    static raylib::Sound chief_chat4e = raylib::Sound(Voc::Load("sound/chf4e.voc"));
    static raylib::Sound chief_chat4f = raylib::Sound(Voc::Load("sound/chf4f.voc"));
    static raylib::Sound chief_chat5 = raylib::Sound(Voc::Load("sound/chf5.voc"));
    static raylib::Sound chief_chat6a = raylib::Sound(Voc::Load("sound/chf6a.voc"));
    static raylib::Sound chief_chat6b = raylib::Sound(Voc::Load("sound/chf6b.voc"));
    static raylib::Sound chief_chat7b = raylib::Sound(Voc::Load("sound/chf7b.voc"));
    static raylib::Sound chief_chattake = raylib::Sound(Voc::Load("sound/chftake.voc"));
    static raylib::Sound chief_chat8 = raylib::Sound(Voc::Load("sound/chf8.voc"));
    static raylib::Sound chief_chat9 = raylib::Sound(Voc::Load("sound/chf9.voc"));

    scripts[0] = {
        Dialogue(Strings::Lookup(120), raylib::RAYWHITE, &chief_talk),
        Dialogue(Strings::Lookup(122)),
    };

    scripts[1] = {
        Dialogue(Strings::Lookup(124), raylib::RAYWHITE, &chief_chat1),
        Dialogue(Strings::Lookup(126)),
        Dialogue(Strings::Lookup(127)),
        Dialogue(Strings::Lookup(128), raylib::RAYWHITE, &chief_chat2a),
        Dialogue(Strings::Lookup(130)),
        Dialogue(Strings::Lookup(131), raylib::RAYWHITE, &chief_chat3a),
        Dialogue(Strings::Lookup(133), raylib::RAYWHITE, &chief_chat3b),
        Dialogue(Strings::Lookup(135), raylib::RAYWHITE, &chief_chat3c),
        Dialogue(Strings::Lookup(137)),
        Dialogue(Strings::Lookup(138)),
        Dialogue(Strings::Lookup(139)),
        Dialogue(Strings::Lookup(141), raylib::RAYWHITE, &chief_chat4a),
        Dialogue(Strings::Lookup(143), raylib::RAYWHITE, &chief_chat4b),
        Dialogue(Strings::Lookup(145), raylib::RAYWHITE, &chief_chat4c),
        Dialogue(Strings::Lookup(147), raylib::RAYWHITE, &chief_chat4d),
        Dialogue(Strings::Lookup(149), raylib::RAYWHITE, &chief_chat4e),
        Dialogue(Strings::Lookup(151), raylib::RAYWHITE, &chief_chat4f),
        Dialogue(Strings::Lookup(153)),
        Dialogue(Strings::Lookup(154), raylib::RAYWHITE, &chief_chat5),
        Dialogue(Strings::Lookup(156)),
    };

    scripts[2] = {
        Dialogue(Strings::Lookup(157), raylib::RAYWHITE, &chief_chat6a),
        Dialogue(Strings::Lookup(160), raylib::RAYWHITE, &chief_chat6b),
        Dialogue(Strings::Lookup(162)),
        Dialogue(Strings::Lookup(163), raylib::RAYWHITE, &chief_chat7b),
        Dialogue(Strings::Lookup(165), raylib::RAYWHITE, &chief_chattake),
        Dialogue(Strings::Lookup(167)),
        Dialogue(Strings::Lookup(168), raylib::RAYWHITE, &chief_chat8),
        Dialogue(Strings::Lookup(170)),
        Dialogue(Strings::Lookup(171), raylib::RAYWHITE, &chief_chat9),
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
    static raylib::Sound guard_talk(Voc::Load("sound/umaguma.voc"));

    entrance = new_entrance;
    scripts[0] = {
        Dialogue(Strings::Lookup(53), raylib::RAYWHITE, &guard_talk),
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
    const static raylib::TextureUnmanaged alt_background = StillCel("stillcel/gardsa2.cel").getTexture();

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
    static raylib::Sound guard_talk(Voc::Load("sound/umaguma.voc"));

    entrance = new_entrance;
    scripts[0] = {
        Dialogue(Strings::Lookup(72), raylib::RAYWHITE, &guard_talk),
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
    static const raylib::TextureUnmanaged alt_background = StillCel("stillcel/gardsb2.cel").getTexture();

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

