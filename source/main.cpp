#include <exception>
#include <iostream>

#include "gamelevel.h"

using namespace std;
using namespace RoninEngine;
#if WIN32
int WinMain(void*, void*, void*, int)
#else
int main()
#endif
{
    Application::init();

    Application::create_window(1024, 600, 0);

    GameLevel level;
    Application::load_level(&level);

    Application::simulate();

    return 0;
}
