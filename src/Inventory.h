#ifndef INVENTORY_H
#define INVENTORY_H

#include <cstdint>
#include <string>
#include <vector>
#include <optional>

#include <raylib-cpp.hpp>

#include "Player.h"
#include "Fnt.h"
#include "Panel.h"

class Inventory {
    struct Layout {
        raylib::Vector2 position;
        raylib::TextureUnmanaged icon;

        std::string name;
        std::string description;

        raylib::Rectangle Border() const {
            return raylib::Rectangle(position.GetX(), position.GetY(), icon.GetWidth(), icon.GetHeight());
        }

        void draw(int scale, int count) const {
            icon.Draw(position * scale, 0.0f, scale);

            if (count > 0) {
                std::string text = std::to_string(count);

                Fnt::Write(text, (position.GetX()+icon.GetWidth())*scale, position.GetY()*scale, scale);
            }
        }
    };

    raylib::TextureUnmanaged background;

    std::unordered_map<Item, Layout> itemLayouts;
    Panel *panel;
public: 
    Inventory(Panel *panel);
    void draw(Player *player, int scale);
    ~Inventory();
};

#endif //INVENTORY_H
