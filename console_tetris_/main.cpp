#include <iostream>
#include <sstream>

#include "logic/menu.h"


using namespace std::chrono_literals;

int main()
{   
    //Window* w = new Window(800, 400);
    Menu* menu = new Menu(400, 800);
    menu->start();
    menu->run();
}
