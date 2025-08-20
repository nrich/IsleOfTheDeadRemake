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

#include "Animation.h"
#include "SoundCache.h"


Animation::Animation(const std::string &flic_filename, const std::string &sound_filename) : flic(flic_filename), sound(SoundCache::Load(sound_filename)) {
}

Animation::Animation(const std::string &flic_filename) : flic(flic_filename), sound(nullptr) {

}

bool Animation::play(const int scale) {
    if (frame == 0 && sound) {
        sound->Play();
    }

    size_t animation_frame = frame / 6; 

    if (animation_frame >= flic.getFrameCount()) {
        frame = 0;
        return true;
    }

    raylib::Camera2D camera(
        raylib::Vector2(scale, scale),
        raylib::Vector2(),
        0.0f
    );

    flic.getTexture(animation_frame).Draw(Vector2(0, 0), 0.0f, scale); 

    frame += 1;

    return false;
}

Animation::~Animation() {

}

