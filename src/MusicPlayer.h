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

#ifndef MUSICPLAYER_H
#define MUSICPLAYER_H

#include <cstdint>
#include <string>
#include <map>
#include <memory>

#include <raylib-cpp.hpp>

class MusicPlayer {
    raylib::AudioStream audioStream;
    bool disable;
public:
    MusicPlayer(bool disable_playback);
    void play(const std::string &midi_filename);
    void stop();
    ~MusicPlayer();
};

#endif //MUSICPLAYER_H
