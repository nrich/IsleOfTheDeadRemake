#include "Inventory.h"
#include "StillCel.h"
#include "Strings.h"

#include <iostream>

Inventory::Inventory(Panel *panel) : panel(panel) {
    background = StillCel("stillcel/invbkg.cel").getTexture(); 

    itemLayouts.emplace(Item::Raft, Layout(raylib::Vector2(252, 3), StillCel("stillcel/rafti.cel").getTexture(), Strings::Lookup(432), Strings::Lookup(433)));

    itemLayouts.emplace(Item::Rifle, Layout(raylib::Vector2(10, 30), StillCel("stillcel/riflei.cel").getTexture(), Strings::Lookup(376), Strings::Lookup(377)));
    itemLayouts.emplace(Item::Uzi, Layout(raylib::Vector2(85, 30), StillCel("stillcel/oozyi.cel").getTexture(), Strings::Lookup(262), Strings::Lookup(263)));
    itemLayouts.emplace(Item::Shotgun, Layout(raylib::Vector2(140, 30), StillCel("stillcel/shotguni.cel").getTexture(), Strings::Lookup(264), Strings::Lookup(265)));
    itemLayouts.emplace(Item::Machete, Layout(raylib::Vector2(195, 30), StillCel("stillcel/massheti.cel").getTexture(), Strings::Lookup(266), Strings::Lookup(267)));
    itemLayouts.emplace(Item::Jacket, Layout(raylib::Vector2(255, 25), StillCel("stillcel/jacketi.cel").getTexture(), Strings::Lookup(268), Strings::Lookup(269)));

    itemLayouts.emplace(Item::Ammo1, Layout(raylib::Vector2(115, 60), StillCel("stillcel/ammo1i.cel").getTexture(), Strings::Lookup(270), Strings::Lookup(273)));
    itemLayouts.emplace(Item::Ammo2, Layout(raylib::Vector2(10, 60), StillCel("stillcel/ammo2i.cel").getTexture(), Strings::Lookup(270), Strings::Lookup(271)));
    itemLayouts.emplace(Item::Ammo3, Layout(raylib::Vector2(60, 60), StillCel("stillcel/ammo3i.cel").getTexture(), Strings::Lookup(270), Strings::Lookup(272)));
    itemLayouts.emplace(Item::Flower, Layout(raylib::Vector2(164, 65), StillCel("stillcel/floweri.cel").getTexture(), Strings::Lookup(276), Strings::Lookup(277)));
    itemLayouts.emplace(Item::Crystal, Layout(raylib::Vector2(210, 65), StillCel("stillcel/crystali.cel").getTexture(), Strings::Lookup(278), Strings::Lookup(279)));
    itemLayouts.emplace(Item::FlareGun, Layout(raylib::Vector2(240, 65), StillCel("stillcel/flarguni.cel").getTexture(), Strings::Lookup(327), Strings::Lookup(328)));
    itemLayouts.emplace(Item::Compass, Layout(raylib::Vector2(280, 65), StillCel("stillcel/compassi.cel").getTexture(), Strings::Lookup(320), Strings::Lookup(321)));

    itemLayouts.emplace(Item::GoldMedal1, Layout(raylib::Vector2(193, 44), StillCel("stillcel/medal1i.cel").getTexture(), Strings::Lookup(337), Strings::Lookup(338)));
    itemLayouts.emplace(Item::GoldMedal2, Layout(raylib::Vector2(227, 44), StillCel("stillcel/medal1i.cel").getTexture(), Strings::Lookup(427), Strings::Lookup(428)));

    itemLayouts.emplace(Item::DeadWolf, Layout(raylib::Vector2(4, 75), StillCel("stillcel/wolfi.cel").getTexture(), Strings::Lookup(280), Strings::Lookup(281)));
    itemLayouts.emplace(Item::BoltCutters, Layout(raylib::Vector2(60, 75), StillCel("stillcel/boltcuti.cel").getTexture(), Strings::Lookup(282), Strings::Lookup(283)));
    itemLayouts.emplace(Item::WireClipper, Layout(raylib::Vector2(105, 80), StillCel("stillcel/clipperi.cel").getTexture(), Strings::Lookup(315), Strings::Lookup(316)));
    itemLayouts.emplace(Item::Chemicals, Layout(raylib::Vector2(140, 85), StillCel("stillcel/ant3i.cel").getTexture(), Strings::Lookup(284), Strings::Lookup(285)));
    itemLayouts.emplace(Item::Beaker, Layout(raylib::Vector2(160, 95), StillCel("stillcel/beakeri.cel").getTexture(), Strings::Lookup(302), Strings::Lookup(302)));
    itemLayouts.emplace(Item::Rags, Layout(raylib::Vector2(180, 90), StillCel("stillcel/ragsi.cel").getTexture(), Strings::Lookup(388), Strings::Lookup(389)));
    itemLayouts.emplace(Item::OilCan, Layout(raylib::Vector2(220, 85), StillCel("stillcel/oilcani.cel").getTexture(), Strings::Lookup(382), Strings::Lookup(383)));
    itemLayouts.emplace(Item::Lighter, Layout(raylib::Vector2(255, 95), StillCel("stillcel/lighteri.cel").getTexture(), Strings::Lookup(288), Strings::Lookup(289)));
    itemLayouts.emplace(Item::Smokes, Layout(raylib::Vector2(285, 85), StillCel("stillcel/cigsi.cel").getTexture(), Strings::Lookup(290), Strings::Lookup(291)));

    itemLayouts.emplace(Item::Companion, Layout(raylib::Vector2(1, 98), StillCel("stillcel/babei.cel").getTexture(), Strings::Lookup(292), Strings::Lookup(293)));
    itemLayouts.emplace(Item::Book, Layout(raylib::Vector2(57, 115), StillCel("stillcel/phbooki.cel").getTexture(), Strings::Lookup(334), Strings::Lookup(335)));
    itemLayouts.emplace(Item::Syringe, Layout(raylib::Vector2(90, 115), StillCel("stillcel/syringei.cel").getTexture(), Strings::Lookup(306), Strings::Lookup(306)));
    itemLayouts.emplace(Item::Drug, Layout(raylib::Vector2(90, 125), StillCel("stillcel/drugi.cel").getTexture(), Strings::Lookup(420), Strings::Lookup(421)));
    itemLayouts.emplace(Item::FirstAid, Layout(raylib::Vector2(145, 120), StillCel("stillcel/aidkiti.cel").getTexture(), Strings::Lookup(294), Strings::Lookup(295)));
    itemLayouts.emplace(Item::Coconut, Layout(raylib::Vector2(187, 120), StillCel("stillcel/cocanuti.cel").getTexture(), Strings::Lookup(298), Strings::Lookup(299)));
    itemLayouts.emplace(Item::Mango, Layout(raylib::Vector2(235, 120), StillCel("stillcel/mangoi.cel").getTexture(), Strings::Lookup(296), Strings::Lookup(297)));
    itemLayouts.emplace(Item::Banana, Layout(raylib::Vector2(275, 120), StillCel("stillcel/bananai.cel").getTexture(), Strings::Lookup(300), Strings::Lookup(301)));

    itemLayouts.emplace(Item::OiledRifle, Layout(raylib::Vector2(10, 30), StillCel("stillcel/riflei.cel").getTexture(), Strings::Lookup(376), Strings::Lookup(377)));
    itemLayouts.emplace(Item::OilyRags, Layout(raylib::Vector2(180, 90), StillCel("stillcel/ragsi.cel").getTexture(), Strings::Lookup(287), Strings::Lookup(287)));

    itemLayouts.emplace(Item::BeakerFlower, Layout(raylib::Vector2(160, 95), StillCel("stillcel/beakeri.cel").getTexture(), Strings::Lookup(303), Strings::Lookup(303)));
    itemLayouts.emplace(Item::BeakerFlowerChemicals, Layout(raylib::Vector2(160, 95), StillCel("stillcel/beakeri.cel").getTexture(), Strings::Lookup(304), Strings::Lookup(304)));
    itemLayouts.emplace(Item::BeakerFlowerChemicalsCrystal, Layout(raylib::Vector2(160, 95), StillCel("stillcel/beakeri.cel").getTexture(), Strings::Lookup(305), Strings::Lookup(305)));

    itemLayouts.emplace(Item::Antidote, Layout(raylib::Vector2(90, 115), StillCel("stillcel/syringei.cel").getTexture(), Strings::Lookup(307), Strings::Lookup(307)));
}

void Inventory::draw(Player *player, int scale) {
    static const raylib::Rectangle exit_button(7, 6, 47, 24);

    const static std::string USE = Strings::Lookup(184);
    const static std::string USE_ON = Strings::Lookup(197);

    BeginDrawing();
    {
        background.Draw(Vector2(0, 0), 0.0f, scale);

        for (const auto &[item, layout] : itemLayouts) {
            int item_count = player->getItemCount(item);

            if (item_count) {
                itemLayouts[item].draw(scale, item_count);
            }
        }

        uint64_t player_input = player->getInput();

        if (player_input & Input::PrimaryAction) {
            player_input &= ~Input::PrimaryAction;

            auto old_selected = player->getSelectedItem();
            auto position = raylib::Mouse::GetPosition();

            if (position.GetY() < 138) {
                player->setSelectedItem(std::nullopt);
                player->setHighlight();
            }

            for (const auto &[item, layout] : itemLayouts) {
                auto border = layout.Border();

                if (!player->getItemCount(item))
                    continue;

                if (border.CheckCollision(position)) {
                    player->setSelectedItem(item);
                    break;
                }
            }

            auto action = player->getAction();
            auto selected = player->getSelectedItem();
            if (action && selected) {
                auto layout = itemLayouts[*selected];
                auto old_layout = itemLayouts[*old_selected];

                switch (*action) {
                    case Action::Look:
                        if (*selected == Item::Jacket) {
                            player->addItem(Item::Lighter);
                            player->addItem(Item::Smokes);
                        }
                        player->setHighlight(layout.description);
                        break;
                    case Action::Use:
                        if (old_selected) {
                            auto use_result = player->useItemOnItem(*old_selected, *selected);
                            if (use_result.first) {
                                player->setSelectedItem(std::nullopt);
                                //player->setAction(std::nullopt);
                            } else {
                                player->setSelectedItem(std::nullopt);
                            }

                            player->setHighlight(use_result.second);
                        } else {
                            switch (*selected) {
                                case Item::Rifle:
                                case Item::Uzi:
                                case Item::Shotgun:
                                case Item::Machete:
                                case Item::OiledRifle:
                                    player->equipWeapon(selected);
                                    player->setState(State::World);
                                    player->setSelectedItem(std::nullopt);
                                    player->setHighlight();
                                    break; 
                                case Item::WireClipper:
                                    player->setState(player->getInventoryReturnState());
                                    player->setHighlight(USE + layout.name + USE_ON);
                                    break;
                                case Item::Coconut:
                                    player->useItem(Item::Coconut);
                                    break;
                                default:
                                    player->equipWeapon(std::nullopt);
                                    player->setHighlight(USE + layout.name + USE_ON);
                            }
                        }
                        break;
                    default:
                        break;
                }
            } else {
                //auto layout = itemLayouts[*selected];
                //highlight = layout.name;
            }

            if (exit_button.CheckCollision(position)) {
                player->setState(player->getInventoryReturnState());
                player->setSelectedItem(std::nullopt);
                player->setHighlight();
            }

            std::cerr << position.GetX() << " " << position.GetY() << "\n";
        }

        player->setInput(player_input);

        if (auto selected = player->getSelectedItem()) {
            auto layout = itemLayouts[*selected];
            auto border = layout.Border();

            auto scale_border = raylib::Rectangle(border.GetX() * scale, border.GetY() * scale, border.GetWidth() * scale, border.GetHeight() * scale);
            scale_border.DrawLines(raylib::PURPLE, scale);
        }

        panel->draw(player, player->getHighlight(), scale);
    }
    EndDrawing();
}

Inventory::~Inventory() {

}
