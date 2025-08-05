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

        for (const auto &layout : layouts) {
            layout.draw(scale);
        }

        for (auto it = animations.begin(); it != animations.end();) {
            auto &[animation_id, animation] = *it;

            if (animation.draw(scale)) {
                animationCompleted(player, animation_id);
                animations.erase(it++);
            } else {
                ++it;
            }
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
                            auto [use_result, highlight, death_type] = useItemOnItem(*player_selected, selected->item); 

                            if (death_type != DeathType::None) {
                                player->death(death_type);
                            }

                            if (use_result) {
                                player->useItem(*player_selected);
                                player->setSelectedItem(std::nullopt);
                            }

                            player->setHighlight(highlight);
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
    animations.erase(animation_id);
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
    layouts.emplace_back(raylib::Vector2(36, 127), StillCel("stillcel/phbook.cel").getTexture(), Item::Machete, Strings::Lookup(334), Strings::Lookup(335), Strings::Lookup(336));

    navigation[Input::StepForward] = State::CrashedPlaneCockpit;
    navigation[Input::LookUp] = State::CrashedPlaneCockpit;
    navigation[Input::TurnLeft] = State::CrashedPlaneExit;
    navigation[Input::TurnRight] = State::CrashedPlaneEntry;
}

CrashedPlaneCockpitScene::CrashedPlaneCockpitScene(Panel *panel) : Scene(panel, "stillcel/room2.cel") {
    //layouts.emplace_back(raylib::Vector2(166, 48), StillCel("stillcel/compass.cel").getTexture(), Item::Compass, Strings::Lookup(320), Strings::Lookup(321), Strings::Lookup(322));
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

    navigation[Input::StepBack] = State::World;
    navigation[Input::LookDown] = State::World;
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

VillageGateShamanScene::VillageGateShamanScene(Panel *panel, const Entrance &new_entrance) : Scene(panel, "stillcel/gardsabg.cel") {
    static raylib::Sound guard_talk(Voc::Load("sound/umaguma.voc"));

    entrance = new_entrance;
    script = {
        Dialogue(Strings::Lookup(53), raylib::RAYWHITE, &guard_talk),
        Dialogue(Strings::Lookup(54)),
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

    if (pass)
        return Scene::talk(player);

    if (dialogueIndex < script.size()) {
        return script[dialogueIndex++];
    }

    background = alt_background;

    navigation[Input::StepForward] = State::World;
    navigation[Input::LookUp] = State::World;

    player->addGameFlag(PlayerGameFlag::VisitedVillage);

    pass = true;
    return Scene::talk(player);
}

VillageGateChiefScene::VillageGateChiefScene(Panel *panel, const Entrance &new_entrance) : Scene(panel, "stillcel/gardsbbg.cel") {
    static raylib::Sound guard_talk(Voc::Load("sound/umaguma.voc"));

    entrance = new_entrance;
    script = {
        Dialogue(Strings::Lookup(72), raylib::RAYWHITE, &guard_talk),
        Dialogue(Strings::Lookup(73)),
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

    if (pass)
        return Scene::talk(player);

    if (!player->testGameFlag(PlayerGameFlag::VisitedVillage)) {
        return Dialogue(Strings::Lookup(223), raylib::RAYWHITE);
    }

    if (dialogueIndex < script.size()) {
        return script[dialogueIndex++];
    }

    background = alt_background;

    navigation[Input::StepForward] = State::World;
    navigation[Input::LookUp] = State::World;

    pass = true;
    return Scene::talk(player);
}

TempleEntranceScene::TempleEntranceScene(Panel *panel, const Entrance &new_entrance) : Scene(panel, "stillcel/room17.cel") {
    entrance = new_entrance;

    layouts.emplace_back(raylib::Vector2(144, 97), StillCel("stillcel/hole1.cel").getTexture(), Item::MedalHole1, Strings::Lookup(334), Strings::Lookup(335), Strings::Lookup(336), true);
    layouts.emplace_back(raylib::Vector2(168, 97), StillCel("stillcel/hole1.cel").getTexture(), Item::MedalHole2, Strings::Lookup(334), Strings::Lookup(335), Strings::Lookup(336), true);

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
        static const std::vector<raylib::TextureUnmanaged> left_eye = {
             StillCel("stillcel/lefteye1.cel").getTexture(),
             StillCel("stillcel/lefteye2.cel").getTexture(),
             StillCel("stillcel/lefteye3.cel").getTexture(),
             StillCel("stillcel/lefteye4.cel").getTexture(),
        };

        static const std::vector<raylib::TextureUnmanaged> right_eye = {
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
    //Scene::animationCompleted(player, animation_id);

    if (animation_id == 1) {
        static const std::vector<raylib::TextureUnmanaged> doors = {
            StillCel("stillcel/tmpdoor1.cel").getTexture(),
            StillCel("stillcel/tmpdoor2.cel").getTexture(),
            StillCel("stillcel/tmpdoor3.cel").getTexture(),
            StillCel("stillcel/tmpdoor4.cel").getTexture(),
            StillCel("stillcel/tmpdoor5.cel").getTexture(),
            StillCel("stillcel/tmpdoor6.cel").getTexture(),
            StillCel("stillcel/tmpdoor7.cel").getTexture(),
            StillCel("stillcel/tmpdoor8.cel").getTexture(),
        };

        animations[3] = Animation(raylib::Vector2(101.0f, 86.0f), doors, false);
    } else if (animation_id == 3) {
        opened = true;

        navigation[Input::StepForward] = State::Oracle;
        navigation[Input::LookUp] = State::Oracle;

        animations[4] = Animation(raylib::Vector2(101.0f, 86.0f), {StillCel("stillcel/tmpdoor8.cel").getTexture()});
    }
}

OracleScene::OracleScene(Panel *panel, const Entrance &new_entrance) : Scene(panel, "stillcel/oraclebg.cel") {
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

    layouts.emplace_back(raylib::Vector2(122, 10), StillCel("stillcel/flame1.cel").getTexture(), Item::Oracle, Strings::Lookup(417), Strings::Lookup(418), Strings::Lookup(336), true);

    script = {
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

    navigation[Input::StepBack] = State::World;
    navigation[Input::LookDown] = State::World;
}

std::tuple<bool, std::string, DeathType> OracleScene::useItemOnItem(Item source, Item destination) {
    if (source == Item::DeadWolf && destination == Item::Oracle) {
        sacrifice = true;
        static const std::vector<raylib::TextureUnmanaged> oracle = {
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
        if (dialogueIndex < script.size()) {
            return script[dialogueIndex++];
        }
    }

    return Scene::talk(player);
}
