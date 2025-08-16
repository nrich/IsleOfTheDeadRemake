#include "Help.h"
#include "Fnt.h"

Help::Help(Panel *panel) : panel(panel) {

}

void Help::draw(Player *player, raylib::Window &window, int scale) {
    static const raylib::Color background_colour(0x0B, 0x3A, 0x0A, 0xFF);

    BeginDrawing();
    {
        window.ClearBackground(background_colour);

        Fnt::Write("STEP FORWARD:    W / UP", 10*scale, 9*scale, scale);
        Fnt::Write("STEP BACKWARD:   S / DOWN", 10*scale, 18*scale, scale);
        Fnt::Write("STEP LEFT:       A", 10*scale, 27*scale, scale);
        Fnt::Write("STEP RIGHT:      D", 10*scale, 36*scale, scale);
        Fnt::Write("TURN LEFT:       LEFT", 10*scale, 45*scale, scale);
        Fnt::Write("TURN RIGHT:      RIGHT", 10*scale, 54*scale, scale);
        Fnt::Write("USE / OPEN:      E", 10*scale, 63*scale, scale);

        Fnt::Write("ATTACK:          SPACE", 10*scale, 72*scale, scale);
        Fnt::Write("MACHETTE:        1", 10*scale, 81*scale, scale);
        Fnt::Write("RIFLE:           2", 10*scale, 90*scale, scale);
        Fnt::Write("SHOTGUN:         3", 10*scale, 99*scale, scale);
        Fnt::Write("UZI:             4", 10*scale, 108*scale, scale);
        Fnt::Write("INVENTORY:       TAB / I", 10*scale, 117*scale, scale);

        panel->draw(player, player->getHighlight(), scale);
    }
    EndDrawing();
 
}

Help::~Help() {

}
