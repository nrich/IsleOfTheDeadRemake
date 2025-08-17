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

#ifndef LAUNCHOPTIONS_H
#define LAUNCHOPTIONS_H

#include <string>

#include <FL/Fl.H>
#include <FL/Fl_Window.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_Hor_Nice_Slider.H>
#include <FL/Fl_Check_Button.H>
#include <FL/Fl_Box.H>

class LaunchOptions : public Fl_Window {
    Fl_Button button;
    Fl_Hor_Nice_Slider slider;
    Fl_Check_Button checkbox;
    Fl_Box label;

    int *scale;
    bool *disable;
    bool launched = false;

    const std::string &getDisplayWindowSize() const;
public:
    LaunchOptions(const std::string &title, int *scale, bool *disable);

    bool run();
}; 

#endif //LAUNCHOPTIONS_H 
