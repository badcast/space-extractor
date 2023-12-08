#include "SpaceExtractor.hpp"

#if WIN32
typedef void *HINSTANCE;
int WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, char *lpCmdLine, int nShowCmd)
#else
int main()
#endif
{
    RoninSimulator::Init();

    //auto selectedScreen = RoninSimulator::ShowSplashScreen(false);
    //if(selectedScreen.first)
    {

        RoninSimulator::Show({1024, 768}, false);

        RoninSimulator::SetDebugMode(true);

        LoadAssets();

        WGame loadWorld;
        RoninSimulator::LoadWorld(&loadWorld);

        RoninSimulator::Simulate();
    }

    RoninSimulator::Finalize();

    return EXIT_SUCCESS;
}
