#include <iostream>
#include <sstream>

#include "logic/menu.h"

#include "graphics/graphics.h"
#include <vector>

using namespace std::chrono_literals;

int main()
{   
    // This is a good test case scenario for collapse algorythm.
    //auto s = new geometry::Shape(std::vector<geometry::Point>{geometry::Point(1, 2), geometry::Point(3, 0), geometry::Point(1, 0), geometry::Point(0, 0), geometry::Point(2, 0), geometry::Point(0, 4), geometry::Point(1, 4), geometry::Point(2, 2)}, '#', colours::fg_def);
    //auto b = s->collapse();

    Menu* menu = new Menu(400, 800);
    menu->start();
    menu->run();
}
