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

#include "Panel.h"
#include "Fnt.h"
#include "StillCel.h"

Panel::Panel() {
    background = StillCel("stillcel/border.cel").getTexture();

    buttons.emplace(Action::Look, Button(raylib::Vector2(39, 147), StillCel("stillcel/b_aicon0.cel").getTexture()));
    buttons.emplace(Action::Get, Button(raylib::Vector2(68, 147), StillCel("stillcel/b_aicon1.cel").getTexture()));
    buttons.emplace(Action::Talk, Button(raylib::Vector2(97, 147), StillCel("stillcel/b_aicon2.cel").getTexture()));
    buttons.emplace(Action::Use, Button(raylib::Vector2(39, 162), StillCel("stillcel/b_aicon3.cel").getTexture()));
    buttons.emplace(Action::Inventory, Button(raylib::Vector2(68, 162), StillCel("stillcel/b_aicon4.cel").getTexture()));
    buttons.emplace(Action::Help, Button(raylib::Vector2(97, 162), StillCel("stillcel/helpbtnb.cel").getTexture()));

    movements.emplace(Input::TurnLeft, Button(raylib::Vector2(196, 147), StillCel("stillcel/b_arrow0.cel").getTexture()));
    movements.emplace(Input::StepForward, Button(raylib::Vector2(225, 147), StillCel("stillcel/b_arrow1.cel").getTexture()));
    movements.emplace(Input::TurnRight, Button(raylib::Vector2(255, 147), StillCel("stillcel/b_arrow2.cel").getTexture()));
    movements.emplace(Input::StepLeft, Button(raylib::Vector2(196, 162), StillCel("stillcel/b_arrow3.cel").getTexture()));
    movements.emplace(Input::StepBack, Button(raylib::Vector2(225, 162), StillCel("stillcel/b_arrow4.cel").getTexture()));
    movements.emplace(Input::StepRight, Button(raylib::Vector2(255, 162), StillCel("stillcel/b_arrow5.cel").getTexture()));
}

void Panel::draw(Player *player, const std::pair<std::string, std::optional<raylib::Color>> &higlight, int scale) {
    background.Draw(Vector2(0, 138*scale), 0.0f, scale);

    auto [message, colour] = higlight;

    Fnt::Write(message, 12*scale, 183*scale, scale, colour);

    auto health_text = std::to_string(player->getHealth());
    auto ammo1_text = std::to_string(player->getItemCount(Item::Ammo1));
    auto ammo2_text = std::to_string(player->getItemCount(Item::Ammo2));

    Fnt::Write(health_text, 170*scale, 145*scale, scale);
    Fnt::Write(ammo1_text, 170*scale, 157*scale, scale);
    Fnt::Write(ammo2_text, 170*scale, 169*scale, scale);

    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        //player->setAction(std::nullopt);
        auto position = raylib::Mouse::GetPosition();

        for (const auto &[action, button] : buttons) {
            auto border = button.Border();

            if (border.CheckCollision(position)) {
                player->setAction(action);
                if (action == Action::Use) {
                    player->setSelectedItem(std::nullopt);
                } else if (action == Action::Inventory) {
                    player->setSelectedItem(std::nullopt);
                    player->setState(State::Inventory);
                } else if (action == Action::Talk) {
                    player->setSelectedItem(std::nullopt);
                }
                break;
            }
        }

        uint64_t player_input = player->getInput();
        for (const auto &[input, button] : movements) {
            auto border = button.Border();

            if (border.CheckCollision(position)) {
                player_input |= input;
                button.draw(scale);
                break;
            }
        }
        player->setInput(player_input);
    }

    auto action = player->getAction();
    if (action) {
        switch (*action) {
            case Action::Look:
                buttons[Action::Look].draw(scale);
                break;
            case Action::Get:
                buttons[Action::Get].draw(scale);
                break;
            case Action::Talk:
                buttons[Action::Talk].draw(scale);
                break;
            case Action::Use:
                buttons[Action::Use].draw(scale);
                break;
            case Action::Inventory:
                buttons[Action::Inventory].draw(scale);
                break;
            case Action::Help:
                buttons[Action::Help].draw(scale);
                break;
        }
    }
}

Panel::~Panel() {

}
