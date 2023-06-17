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

    Resolution resolution{1024, 600};
    Application::show(resolution, false);

    GameMainMenu* level;
    RoninEngine::Runtime::RoninMemory::alloc_self(level);
    Application::load_world(level);
    Application::simulate();
    RoninEngine::Runtime::RoninMemory::free(level);

    Application::utilize();

    return 0;
}
