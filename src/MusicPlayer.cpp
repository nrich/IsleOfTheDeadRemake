#include <fstream>
#include <optional>
#include <map>
#include <array>
#include <cstdint>
#include <functional>
#include <stack>
#include <chrono>
#include <thread>
#include <unordered_map>

#include "Tone.h"
#include "MIDI.h"
#include "Song.h"
#include "MusicPlayer.h"

static std::unordered_map<std::string, Song> songs;
static std::string current_songname = "";

static void PlayCallback(void *buffer, uint32_t frames) {
    if (current_songname.size()) {
        Song *song = &songs.at(current_songname);

        song->generateBuffer(buffer, frames);
    }
}

MusicPlayer::MusicPlayer() : audioStream(44100, 32, 2) {
}

void MusicPlayer::play(const std::string &midi_filename) {
    if (current_songname == midi_filename)
        return;

    audioStream.Stop();

    if (songs.count(midi_filename)) {
        Song *song = &songs.at(midi_filename);
        song->generateTones();
    } else {
        MIDI midi = MIDI(midi_filename);
        songs.emplace(midi_filename, midi);
    }

    audioStream.SetCallback(PlayCallback);
    audioStream.Play();

    current_songname = midi_filename;
}

void MusicPlayer::stop() {
    if (!current_songname.size())
        return;

    audioStream.Stop();

    current_songname = "";
}

MusicPlayer::~MusicPlayer() {

}

