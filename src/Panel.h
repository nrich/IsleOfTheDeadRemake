#ifndef PANEL_H
#define PANEL_H

#include <cstdint>
#include <string>
#include <vector>
#include <optional>
#include <array>
#include <unordered_map>

#include <raylib-cpp.hpp>

#include "Player.h"
#include "Fnt.h"

class Panel {
    struct Button {
        raylib::Vector2 position;
        raylib::TextureUnmanaged icon;

        raylib::Rectangle Border() const {
            return raylib::Rectangle(position.GetX(), position.GetY(), icon.GetWidth(), icon.GetHeight());
        }

        void draw(int scale) const {
            icon.Draw(position * scale, 0.0f, scale);
        }
    };

    std::unordered_map<Action, Button> buttons;
    std::unordered_map<Input, Button> movements;

    raylib::TextureUnmanaged background;

    State previous;
public: 
    Panel();
    void draw(Player *player, const std::string &message, int scale);
    ~Panel();
};

#endif //PANEL_H
