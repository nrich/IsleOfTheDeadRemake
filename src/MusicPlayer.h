#ifndef MUSICPLAYER_H
#define MUSICPLAYER_H

#include <cstdint>
#include <string>
#include <map>
#include <memory>

#include <raylib-cpp.hpp>

class MusicPlayer {
    raylib::AudioStream audioStream;
public:
    MusicPlayer();
    void play(const std::string &midi_filename);
    void stop();
    ~MusicPlayer();
};

#endif //MUSICPLAYER_H
