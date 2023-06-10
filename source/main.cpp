#include <exception>
#include <iostream>

#include "SpaceMainMenu.h"

using namespace std;
using namespace RoninEngine;
#if WIN32
int WinMain(void*, void*, void*, int)
#else
int main()
#endif
{
    Application::init();

    Application::create_window(1024, 600);

    GameMainMenu level;

    Application::load_world(&level);

    Application::simulate();

    return 0;
}
