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

Segment::Segment(size_t id, uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t texture, uint16_t flags, uint16_t count) : id(id), x1(x1), y1(y1), x2(x2), y2(y2), texture(texture), flags(flags), count(count) {
}

