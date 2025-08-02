#ifndef VOC_H
#define VOC_H

#include <cstdint>

#include <raylib-cpp.hpp>

class Voc {
public:
    static raylib::Wave Load(const std::string &filename);
};

#endif //VOC_H
