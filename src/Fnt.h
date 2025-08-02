#ifndef FNT_H
#define FNT_H

#include <cstdint>

#include <raylib-cpp.hpp>

class Fnt {
public:
    static raylib::TextureUnmanaged Get(char c);
    static void ExtractFonts(const std::string &filename);
    static void Write(const std::string &text, int x, int y, int scale, float rotation=0.0f);
};

#endif //FNT_H
