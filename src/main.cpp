#include "SpaceExtractor.hpp"

#include "Worlds/WParticleEdtitor.hpp"

#if WIN32
typedef void *HINSTANCE;
int WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, char *lpCmdLine, int nShowCmd)
#else
int main()
#endif
{
    RoninSimulator::Init();

    Resolution res {1366, 768};
    RoninSimulator::Show(res, false);

    RoninSimulator::SetDebugMode(true);

    WGame loadWorld;

    RoninSimulator::LoadWorld(&loadWorld);

    RoninSimulator::Simulate();

    RoninSimulator::Finalize();


    return EXIT_SUCCESS;
}
