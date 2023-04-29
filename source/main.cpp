#include <exception>
#include <iostream>

#include "gamelevel.h"

using namespace std;

#if WIN32
int WinMain(void*, void*, void*, int)
#else
int main()
#endif
{
    using namespace RoninEngine;

    Application::init();

    Application::createWindow(1366, 768, 1);

    GameLevel level;
    Application::loadLevel(&level);

    Application::simulate();

    Application::Quit();

    return 0;
}
