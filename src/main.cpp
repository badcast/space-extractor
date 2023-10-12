#include "SpaceExtractor.hpp"

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

    WGame gameWorld;

    RoninSimulator::LoadWorld(&gameWorld);

    RoninSimulator::Simulate();

    RoninSimulator::Finalize();


    return EXIT_SUCCESS;
}
