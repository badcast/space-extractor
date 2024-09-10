
#include <string>

#include "SpaceExtractor.hpp"
#include "Worlds/WMainMenu.hpp"
#include "Worlds/WParticleEdtitor.hpp"

#if WIN32
int WinMain(void *hInstance, void *hPrevInstance, char *lpCmdLine, int nShowCmd)
#else
int main()
#endif
{
    RoninSimulator::Init();
    // auto selectedScreen = RoninSimulator::ShowSplashScreen(false);
    // if(selectedScreen.first)
    {

        RoninSimulator::Show({1024, 768}, false);

        RoninSimulator::SetDebugMode(true);

        LoadAssets();

        WParticleEdtitor loadWorld;
        RoninSimulator::LoadWorld(&loadWorld);

        RoninSimulator::Simulate();
    }
    RoninSimulator::Finalize();

    return EXIT_SUCCESS;
}
