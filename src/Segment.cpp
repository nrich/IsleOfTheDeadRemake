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

#include "Segment.h"
#include "Palette.h"
#include "CelThree.h"

#include <unordered_map>

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

void draw_wall(const Segment *segment, const raylib::TextureUnmanaged &texture) {
    const float height = 12.0f;
    
    rlBegin(RL_TRIANGLES);
    rlSetTexture(texture.id);

    if (segment->y1 == segment->y2) {
        rlTexCoord2f(0.0, 1.0);
        rlVertex3f(segment->x1, 0, segment->y1);
    
        rlTexCoord2f(1.0, 1.0);
        rlVertex3f(segment->x2, 0, segment->y1);

        rlTexCoord2f(1.0, 0.0);
        rlVertex3f(segment->x2, height, segment->y1);

        rlTexCoord2f(1.0, 0.0);
        rlVertex3f(segment->x2, height, segment->y1);

        rlTexCoord2f(0.0, 0.0);
        rlVertex3f(segment->x1, height, segment->y1);
    
        rlTexCoord2f(0.0, 1.0);
        rlVertex3f(segment->x1, 0, segment->y1);
    
    } else {
        rlTexCoord2f(0.0, 1.0);
        rlVertex3f(segment->x1, 0, segment->y1);

        rlTexCoord2f(1.0, 1.0); 
        rlVertex3f(segment->x1, 0, segment->y2);

        rlTexCoord2f(1.0, 0.0);
        rlVertex3f(segment->x1, height, segment->y2);

        rlTexCoord2f(1.0, 0.0);
        rlVertex3f(segment->x1, height, segment->y2);

        rlTexCoord2f(0.0, 0.0);
        rlVertex3f(segment->x1, height, segment->y1);

        rlTexCoord2f(0.0, 1.0);
        rlVertex3f(segment->x1, 0, segment->y1);
    }   

    rlEnd();
}

void draw_entity(const raylib::Camera3D &camera, const Segment *segment, const raylib::TextureUnmanaged &texture) {
    const float y_offset = 5.0f;
    const float scale = 10.0f;

    float mid_x = segment->x1;
    float mid_y = segment->y1;

    camera.DrawBillboard(texture, Vector3(mid_x, y_offset, mid_y), scale);
}

std::pair<raylib::Vector2, float> Segment::getBounds() const {
    if (y1 == y2) {
        float min_x = std::min(x1, x2);
        float max_x = std::max(x1, x2);

        float radius = (max_x - min_x) / 2.0f;
        float centre_x = min_x + radius;
        
        return std::make_pair(raylib::Vector2(centre_x, y1), radius);
    }

    float min_y = std::min(y1, y2);
    float max_y = std::max(y1, y2);

    float radius = (max_y - min_y) / 2.0f;
    float centre_y = min_y + radius;
    
    return std::make_pair(raylib::Vector2(x1, centre_y), radius);

}

void Segment::render(const raylib::Camera3D &camera, uint64_t frame_count) const {
    const auto unknown = load_cel3_texture("cels3/unknown.cel");

    const auto beach = load_cel3_texture({
        "cels3/beach1.cel",
        "cels3/beach2.cel",
        "cels3/beach3.cel",
        "cels3/beach2.cel",
    });

    const auto trees1 = load_cel3_texture("cels3/rtrees1.cel");
    const auto trees2 = load_cel3_texture("cels3/rtrees2.cel");

    const auto trees = load_cel3_texture("cels3/rtrees.cel");

    const auto tree_entry = load_cel3_texture("cels3/rent1.cel");

    const auto jacket = load_cel3_texture("cels3/jacket.cel");
    const auto coconut = load_cel3_texture("cels3/food2c1.cel");
    const auto banana = load_cel3_texture("cels3/food3c1.cel");
    const auto aid_kit = load_cel3_texture("cels3/aidkitc1.cel");
    const auto ammo_shells = load_cel3_texture("cels3/ammo1c1.cel");
    const auto ammo_hmm = load_cel3_texture("cels3/ammo2c1.cel");
    const auto ammo_huh = load_cel3_texture("cels3/ammo3c1.cel");
    const auto shotgun = load_cel3_texture("cels3/sgunc1.cel");

    const auto husk1 = load_cel3_texture("cels3/husk1.cel");
    const auto husk2 = load_cel3_texture("cels3/husk2.cel");
    const auto husk3 = load_cel3_texture("cels3/husk3.cel");

    const auto bunker_entry1 = load_cel3_texture("cels3/bunkent1.cel");

    const auto rcave1 = load_cel3_texture("cels3/rcave1.cel");
    const auto rcave2 = load_cel3_texture("cels3/rcave2.cel");
    const auto rcave3 = load_cel3_texture("cels3/rcave3.cel");
    const auto rcave4 = load_cel3_texture("cels3/rcave4.cel");

    const auto cave_entry = load_cel3_texture("cels3/cav2cave.cel");
    const auto jungle_entry = load_cel3_texture("cels3/cavetoj.cel");

    const auto fence = load_cel3_texture("cels3/fence.cel");
    const auto jungle_fence = load_cel3_texture("cels3/jfence.cel");

    const auto shack1 = load_cel3_texture("cels3/shack1.cel");
    const auto shack2 = load_cel3_texture("cels3/shack2.cel");
    const auto shack3 = load_cel3_texture("cels3/shack3.cel");

    const auto compound_wall_light = load_cel3_texture("cels3/compnd51.cel");
    const auto compound_wall_dark = load_cel3_texture("cels3/compnd53.cel");
    const auto compound_window_light = load_cel3_texture("cels3/compnd54.cel");
    const auto compound_window_dark = load_cel3_texture("cels3/compnd55.cel");

    const auto big_door = load_cel3_texture({
        "cels3/bigdoor1.cel",
        "cels3/bigdoor2.cel",
        "cels3/bigdoor3.cel",
        "cels3/bigdoor4.cel",
        "cels3/bigdoor3.cel",
        "cels3/bigdoor2.cel",
    });

    const auto red_chair = load_cel3_texture("cels3/frnture2.cel");
    const auto bookshelf = load_cel3_texture("cels3/frnture4.cel");
    const auto tv = load_cel3_texture("cels3/frnture5.cel");
    const auto portrait = load_cel3_texture("cels3/frnture7.cel");

    const auto labsink = load_cel3_texture("cels3/labsink.cel");
    const auto labtable = load_cel3_texture("cels3/labtable.cel");

    const auto bed = load_cel3_texture("cels3/bed.cel");
    const auto table = load_cel3_texture("cels3/table.cel");

    const auto temple_wall_light = load_cel3_texture("cels3/tmpl2.cel");
    const auto temple_wall_dark = load_cel3_texture("cels3/tmpl4.cel");
    const auto temple_door = load_cel3_texture("cels3/tmpdoor1.cel");

    const auto temple_face_left = load_cel3_texture("cels3/tmpltex1.cel");
    const auto temple_face_mid = load_cel3_texture("cels3/tmpltex2.cel");
    const auto temple_face_right = load_cel3_texture("cels3/tmpltex3.cel");

    const auto barrel = load_cel3_texture("cels3/barrel.cel");

    const auto camp_gate = load_cel3_texture("cels3/cmpgate1.cel");
    const auto camp_gate_open = load_cel3_texture("cels3/cmpgate3.cel");

    const auto driftwood = load_cel3_texture("cels3/driftwd.cel");

    const auto hut_door = load_cel3_texture({
        "cels3/hutdoor1.cel",
        "cels3/hutdoor2.cel",
        "cels3/hutdoor3.cel",
        "cels3/hutdoor2.cel",
    });

    const auto hut_wall_north = load_cel3_texture("cels3/nhut.cel");
    const auto hut_wall_west = load_cel3_texture("cels3/whut.cel");
    const auto hut_wall_east = load_cel3_texture("cels3/ehut.cel");
    const auto hut_wall_south = load_cel3_texture("cels3/shut.cel");

    const auto boowall_north = load_cel3_texture("cels3/nboowall.cel");
    const auto boowall_west = load_cel3_texture("cels3/wboowall.cel");
    const auto boowall_east = load_cel3_texture("cels3/eboowall.cel");
    const auto boowall_south = load_cel3_texture("cels3/sboowall.cel");

    const auto boogate = load_cel3_texture("cels3/boogate.cel");

    const auto hanger_dark = load_cel3_texture("cels3/hngar01.cel");
    const auto hanger_light = load_cel3_texture("cels3/hngar02.cel");

    const auto runway = load_cel3_texture("cels3/runway.cel");
    const auto plane_fence_left = load_cel3_texture("cels3/fencepl1.cel");
    const auto plane_fence_right = load_cel3_texture("cels3/fencepl2.cel");

    const auto missile_left = load_cel3_texture("cels3/misslec1.cel");
    const auto missile_right = load_cel3_texture("cels3/misslec2.cel");

    const auto dish = load_cel3_texture("cels3/dish.cel");

    const auto mansion_window_light = load_cel3_texture("cels3/mansext1.cel");
    const auto mansion_window_dark = load_cel3_texture("cels3/mansext2.cel");
    const auto mansion_door = load_cel3_texture("cels3/mansext3.cel");
    //const auto mansion_door_broken = load_cel3_texture("cels3/mansext5.cel");
    const auto mansion_wall_light = load_cel3_texture("cels3/mansext7.cel");
    const auto mansion_wall_dark = load_cel3_texture("cels3/mansext8.cel");

    const auto pit = load_cel3_texture("cels3/pit.cel");
    const auto snare = load_cel3_texture("cels3/snare.cel");
    const auto trapdoor = load_cel3_texture("cels3/trapdoor.cel");

    const auto flower = load_cel3_texture("cels3/flowerc1.cel");
    const auto vine = load_cel3_texture("cels3/vine.cel");
    const auto crystal = load_cel3_texture("cels3/crystalc.cel");

    const auto hanger_door = load_cel3_texture({
        "cels3/hngar03.cel",
        "cels3/hngar04.cel",
        "cels3/hngar05.cel",
        "cels3/hngar04.cel",
    });

    const auto plane_fire_left = load_cel3_texture({
        "cels3/fire1a.cel",
        "cels3/fire2a.cel",
        "cels3/fire3a.cel",
    });

    const auto plane_fire_mid = load_cel3_texture({
        "cels3/fire1b.cel",
        "cels3/fire2b.cel",
        "cels3/fire3b.cel",
    });

    const auto plane_fire_right = load_cel3_texture({
        "cels3/fire1c.cel",
        "cels3/fire2c.cel",
        "cels3/fire3c.cel",
    });

    const auto fireplace_left = load_cel3_texture({
        "cels3/firepl1a.cel",
        "cels3/firepl1b.cel",
        "cels3/firepl1c.cel",
    });

    const auto fireplace_right = load_cel3_texture({
        "cels3/firepl2a.cel",
        "cels3/firepl2b.cel",
        "cels3/firepl2c.cel",
    });

    const auto tiki = load_cel3_texture({
        "cels3/tiki1.cel",
        "cels3/tiki2.cel",
        "cels3/tiki3.cel",
        "cels3/tiki4.cel",
        "cels3/tiki5.cel",
        "cels3/tiki6.cel",
    });

    const auto tank = load_cel3_texture({
        "cels3/tnk01.cel",
        "cels3/tnk02.cel",
        "cels3/tnk03.cel",
        "cels3/tnk04.cel",
        "cels3/tnk05.cel",
        "cels3/tnk06.cel",
        "cels3/tnk07.cel",
        "cels3/tnk08.cel",
        "cels3/tnk09.cel",
        "cels3/tnk10.cel",
    });

    // nurse 01-33
    const auto nurse = load_cel3_texture({
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

    // doc 01-41
    const auto doc = load_cel3_texture({
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

    const auto drummer = load_cel3_texture({
        "cels3/drumr01.cel",
        "cels3/drumr02.cel",
        "cels3/drumr03.cel",
        "cels3/drumr04.cel",
        "cels3/drumr05.cel",
        "cels3/drumr06.cel",
        "cels3/drumr07.cel",
    });

    const auto bat = load_cel3_texture({
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

    const auto wolf = load_cel3_texture({
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

    const auto roy = load_cel3_texture({
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

    const auto dude = load_cel3_texture({
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

    const auto cj = load_cel3_texture({
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

    const auto tor = load_cel3_texture({
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

    const auto kid = load_cel3_texture({
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

    const auto harry = load_cel3_texture({
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

    uint64_t beach_anim = frame_count / 10;
    uint64_t plane_file_anim = frame_count / 10;
    uint64_t tiki_anim = frame_count / 6;
    uint64_t door_anim = frame_count / 10;
    uint64_t fireplace_anim = frame_count / 10;
    uint64_t tank_anim = frame_count / 10;

    uint64_t anim = frame_count / 6;

    switch (texture) {
        case 0:
            draw_wall(this, beach[beach_anim % beach.size()]);
            break;
        case 1:
            draw_wall(this, rcave1);
            break;
        case 2:
            draw_wall(this, rcave2);
            break;
        case 3:
            draw_wall(this, rcave3);
            break;
        case 4:
            draw_wall(this, rcave4);
            break;
        case 5:
        case 7:
        case 9:
            draw_wall(this, trees1);
            break;
        case 6:
        case 8:
        case 10:
        case 12:
        case 14:
            draw_wall(this, trees2);
            break;
        case 11:
        case 13:
        case 25:
        case 26:
        case 27:
        case 28:
            draw_wall(this, tree_entry);
            break;
        case 15:
            draw_wall(this, husk1);
            break;
        case 16:
            draw_wall(this, husk2);
            break;
        case 17:
            draw_wall(this, husk3);
            break;
        case 18:
            draw_wall(this, plane_fire_right[plane_file_anim % plane_fire_right.size()]);
            break;
        case 19:
            draw_wall(this, plane_fire_mid[plane_file_anim % plane_fire_mid.size()]);
            break;
        case 20:
            draw_wall(this, plane_fire_left[plane_file_anim % plane_fire_left.size()]);
            break;
        case 21:
        case 22:
            draw_wall(this, cave_entry);
            break;
        case 23:
        case 24:
            draw_wall(this, jungle_entry);
            break;
        case 29:
        case 30:
        case 31:
        case 32:
            draw_wall(this, jungle_fence);
            break;
        case 33:
            draw_wall(this, camp_gate);
            break;
        case 34:
            draw_wall(this, boowall_west);
            break;
        case 35:
            draw_wall(this, boowall_north);
            break;
        case 36:
            draw_wall(this, boowall_east);
            break;
        case 37:
            draw_wall(this, boowall_south);
            break;
        case 38:
            draw_wall(this, boogate);
            break;
        case 39:
            draw_wall(this, hut_wall_west);
            break;
        case 40:
            draw_wall(this, hut_wall_north);
            break;
        case 41:
            draw_wall(this, hut_wall_east);
            break;
        case 42:
            draw_wall(this, hut_wall_south);
            break;
        case 43:
            draw_wall(this, bunker_entry1);
            break;
        case 44:
        case 45:
        case 46:
            draw_wall(this, trees2);
            break;
        case 47:
        case 48:
        case 49:
        case 50:
            draw_wall(this, hut_door[door_anim % hut_door.size()]);
            break;
        case 52:
            draw_wall(this, compound_wall_dark);
            break;
        case 53:
            draw_wall(this, compound_wall_light);
            break;
        case 54:
            draw_wall(this, compound_window_light);
            break;
        case 55:
            draw_wall(this, compound_window_dark);
            break;
        case 59:
            draw_wall(this, temple_door);
            break;
        case 60:
            draw_wall(this, big_door[door_anim % big_door.size()]);
            break;
        case 63:
            draw_wall(this, fence);
            break;
        case 69:
            draw_wall(this, shack2);
            break;
        case 70:
            draw_wall(this, shack3);
            break;
        case 71:
            draw_wall(this, shack2);
            break;
        case 72:
            draw_wall(this, shack1);
            break;
        case 81:
            draw_wall(this, mansion_window_light);
            break;
        case 82:
            draw_wall(this, mansion_window_dark);
            break;
        case 83:
            draw_wall(this, mansion_door);
            break;
        case 85:
            draw_wall(this, mansion_wall_light);
            break;
        case 86:
            draw_wall(this, mansion_wall_dark);
            break;
        case 91:
            draw_wall(this, temple_wall_dark);
            break;
        case 92:
            draw_wall(this, temple_wall_light);
            break;
        case 94:
            draw_wall(this, temple_face_left);
            break;
        case 95:
            draw_wall(this, temple_face_mid);
            break;
        case 96:
            draw_wall(this, temple_face_right);
            break;
        case 100:
            draw_entity(camera, this, jacket);
            break;
        case 105:
            draw_entity(camera, this, banana);
            break;
        case 106:
            draw_entity(camera, this, pit);
            break;
        case 107:
            draw_entity(camera, this, trees);
            break;
        case 108:
            draw_entity(camera, this, vine);
            break;
        case 109:
            draw_entity(camera, this, crystal);
            break;
        case 110:
            draw_entity(camera, this, flower);
            break;
        case 115:
            draw_entity(camera, this, aid_kit);
            break;
        case 116:
            draw_entity(camera, this, ammo_shells);
            break;
        case 117:
            draw_entity(camera, this, bat[anim % bat.size()]);
            break;
        case 118:
            draw_entity(camera, this, wolf[anim % wolf.size()]);
            break;
        case 119:
            draw_entity(camera, this, roy[anim % roy.size()]);
            break;
        case 120:
            draw_entity(camera, this, dude[anim % dude.size()]);
            break;
        case 121:
            draw_entity(camera, this, cj[anim % cj.size()]);
            break;
        case 122:
            draw_entity(camera, this, nurse[anim % nurse.size()]);
            break;
        case 123:
            draw_entity(camera, this, tor[anim % tor.size()]);
            break;
        case 124:
            draw_entity(camera, this, doc[anim % doc.size()]);
            break;
        case 125:
            draw_entity(camera, this, driftwood);
            break;
        case 126:
            draw_entity(camera, this, kid[anim % kid.size()]);
            break;
        case 127:
            draw_entity(camera, this, driftwood);
            break;
        case 128:
            draw_entity(camera, this, harry[anim % harry.size()]);
            break;
        case 129:
            draw_entity(camera, this, coconut);
            break;
        case 130:
            draw_entity(camera, this, coconut);
            break;
        case 131:
            draw_entity(camera, this, coconut);
            break;
        case 132:
            draw_entity(camera, this, banana);
            break;
        case 133:
            draw_entity(camera, this, coconut);
            break;
        case 134:
            draw_entity(camera, this, dish);
            break;
        case 135:
            draw_entity(camera, this, tiki[tiki_anim % tiki.size()]);
            break;
        case 136:
            draw_entity(camera, this, tank[tank_anim % tank.size()]);
            break;
        case 138:
            draw_entity(camera, this, bed);
            break;
        case 139:
            draw_entity(camera, this, labtable);
            break;
        case 140:
            draw_entity(camera, this, labsink);
            break;
        case 141:
            draw_entity(camera, this, table);
            break;
        case 144:
            draw_entity(camera, this, barrel);
            break;
        case 145:
            draw_entity(camera, this, snare);
            break;
        case 146:
            draw_entity(camera, this, trapdoor);
            break;
        case 147:
            draw_wall(this, missile_left);
            break;
        case 148:
            draw_wall(this, missile_right);
            break;
        case 150:
            draw_entity(camera, this, ammo_hmm);
            break;
        case 151:
            draw_entity(camera, this, ammo_huh);
            break;
        case 159:
            draw_wall(this, camp_gate_open);
            break;
        case 160:
            draw_wall(this, hanger_dark);
            break;
        case 161:
            draw_wall(this, hanger_light);
            break;
        case 162:
            draw_wall(this, hanger_door[door_anim % hanger_door.size()]);
            break;
        case 166:
            draw_wall(this, plane_fence_left);
            break;
        case 167:
            draw_wall(this, plane_fence_right);
            break;
        case 168:
            draw_wall(this, runway);
            break;
        case 169:
            draw_entity(camera, this, drummer[anim % drummer.size()]);
            break;
        case 171:
            draw_entity(camera, this, red_chair);
            break;
        case 173:
            draw_wall(this, bookshelf);
            break;
        case 174:
            draw_wall(this, tv);
            break;
        case 176:
            draw_wall(this, portrait);
            break;
        case 250:
            draw_wall(this, fireplace_left[fireplace_anim % fireplace_left.size()]);
            break;
        case 251:
            draw_wall(this, fireplace_right[fireplace_anim % fireplace_right.size()]);
            break;
        case 252:
            draw_entity(camera, this, shotgun);
            break;
        default:
            std::cerr << "Uknown texture " << this->texture << "\n";
            draw_wall(this, unknown);
    } 
}
