#include "Animation.h"
#include "Voc.h"

Animation::Animation(const std::string &flic_filename, const std::string &sound_filename) : flic(flic_filename), sound(Voc::Load(sound_filename)) { 
}

Animation::Animation(const std::string &flic_filename) : flic(flic_filename), sound(std::nullopt) {

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

