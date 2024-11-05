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
     //auto selectedScreen = RoninSimulator::ShowSplashScreen(true);
     //if(selectedScreen.first)
    {

        RoninSimulator::Show({1024, 768}, false);

        RoninSimulator::SetDebugMode(true);

        RoninSettings settings = RoninSettings::GetCurrent();

        settings.verticalSync = false;

        settings.renderBackend = RenderDriverInfo::GPU;
        settings.Apply();

        LoadAssets();

        RoninSimulator::LoadWorld<WMainMenu>();

        RoninSimulator::Simulate();
    }
    RoninSimulator::Finalize();

    return EXIT_SUCCESS;
}
