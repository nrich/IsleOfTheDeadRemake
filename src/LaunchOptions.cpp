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

#include <sstream>
#include <iostream>

#include "LaunchOptions.h"

LaunchOptions::LaunchOptions(const std::string &title, int *scale, bool *disable) : Fl_Window(400, 190, title.c_str()), button(310, 150, 70, 28, "Launch"), slider(20, 50, 200, 28), checkbox(20, 100, 110, 28, "Disable music"), label(20, 10, 200, 28), scale(scale), disable(disable) {
    slider.bounds(1, 10);

    *this->scale = 1;

    label.align(FL_ALIGN_LEFT | FL_ALIGN_INSIDE | FL_ALIGN_CLIP | FL_ALIGN_WRAP);
    label.label(getDisplayWindowSize().c_str());
}

bool LaunchOptions::run() {
    checkbox.value(*this->disable);
    slider.value(*this->scale);

    slider.callback([](Fl_Widget *slider_as_widget, void *window_ptr) {
        auto window = reinterpret_cast<LaunchOptions*>(window_ptr);
        auto slider = reinterpret_cast<Fl_Hor_Nice_Slider*>(slider_as_widget);

        int value = (int)slider->value();
        slider->value(value);

        *window->scale = value;

        window->label.label(window->getDisplayWindowSize().c_str());
    }, this);

    button.callback([](Fl_Widget *button_as_widget, void *window_ptr) {
        auto window = reinterpret_cast<LaunchOptions*>(window_ptr);
        window->launched = true;
        window->hide();
    }, this);

    checkbox.callback([](Fl_Widget* checkbox_as_widget, void *window_ptr) {
        auto checkbox = reinterpret_cast<Fl_Check_Button*>(checkbox_as_widget);
        auto window = reinterpret_cast<LaunchOptions*>(window_ptr);

        *window->disable = checkbox->value();
    }, this);

    this->show();

    Fl::run();

    return launched;
}

const std::string &LaunchOptions::getDisplayWindowSize() const {
    static std::string display_string = "";
    std::ostringstream s;

    int scale_size = *scale;

    s << "Window size: " << std::to_string(320 * scale_size) << "x" << std::to_string(200 * scale_size);

    display_string = s.str();

    return display_string;
}
