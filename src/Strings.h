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
