#ifndef STRINGS_H
#define STRINGS_H

#include <cstdint>
#include <string>
#include <vector>

#include <raylib-cpp.hpp>

class Strings {
public:
    static void Extract(const std::string &filename);
    static std::string Lookup(const size_t index);
};

#endif //STRINGS_H
