#include "SpaceExtractor.hpp"

#if WIN32
typedef void *HINSTANCE;
int WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, char *lpCmdLine, int nShowCmd)
#else
int main()
#endif
{
    RoninSimulator::Init();
    auto a = RoninSimulator::EnumerateResolutions();

    if(RoninSimulator::ShowSplashScreen().first)
    {
        RoninSimulator::SetDebugMode(true);

        WGame loadWorld;

        RoninSimulator::LoadWorld(&loadWorld);

        // Load Assets
        std::string datadir = Paths::GetRuntimeDir() + "data" + Paths::GetPathSeperatorOS();
        std::string p = datadir;
        p += "resources.json";
        if(!AssetManager::LoadAsset(p, &spriteAsset))
        {
            RoninSimulator::ShowMessageFail("Failed load asset " + p);
        }

        p = datadir;
        p += "sounds.json";
        if(!AssetManager::LoadAsset(p, &soundAsset))
        {
            RoninSimulator::ShowMessageFail("Failed load asset " + p);
        }

        RoninSimulator::Simulate();
    }

    RoninSimulator::Finalize();

    return EXIT_SUCCESS;
}
