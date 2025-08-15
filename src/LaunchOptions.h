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
