#include <exception>
#include <iostream>

#if WIN32
#include <windows.h>
#endif

#include "SpaceMainMenu.h"

using namespace std;
using namespace RoninEngine;
int main()
{
#if WIN32
    FreeConsole();
#endif

    RoninSimulator::Init(InitializeFlags::All);

    Resolution resolution {1024, 512};
    RoninSimulator::Show(resolution, false);

    auto drivers = RoninSimulator::EnumerateRenderDrivers();
    auto videoDrivers = RoninSimulator::EnumerateVideoDrivers();

    RoninSettings settings;
    RoninSimulator::GetSettings(&settings);
    bool last = RoninSimulator::SetSettings(&settings);

    GameMainMenu *world;
    RoninEngine::Runtime::RoninMemory::alloc_self(world);
    RoninSimulator::SetDebugMode(true);
    RoninSimulator::LoadWorld(world);
    RoninSimulator::Simulate();
    RoninEngine::Runtime::RoninMemory::free(world);

    RoninSimulator::Finalize();

    return 0;
}
