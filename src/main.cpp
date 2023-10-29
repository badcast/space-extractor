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

    Resolution res {1024, 500};
    RoninSimulator::Show(res, false);

    RoninSimulator::SetDebugMode(true);

    WParticleEdtitor loadWorld;

    RoninSimulator::LoadWorld(&loadWorld);

    // Load Assets
    std::string datadir = Path::app_dir() + "data" + Path::GetPathSeperatorOS();
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

    RoninSimulator::Finalize();

    return EXIT_SUCCESS;
}
