#include "SpaceExtractor.hpp"

#if WIN32
typedef void *HINSTANCE;
int WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, char *lpCmdLine, int nShowCmd)
#else
int main()
#endif
{
    RoninSimulator::Init();

    if(RoninSimulator::ShowSplashScreen().first)
    {

        // RoninSettings set;
        // RoninSimulator::GetSettings(&set);
        // set.selectRenderBackend = RenderBackend::CPU;
        // RoninSimulator::SetSettings(&set);

        RoninSimulator::SetDebugMode(true);

        WGame loadWorld;

        RoninSimulator::LoadWorld(&loadWorld);

        // Load Assets
        std::string datadir = Paths::GetRuntimeDir() + "data" + Paths::GetPathSeperatorOS();
        std::string p = datadir;
        p += "resources.asset";
        if(!AssetManager::LoadAsset(p, &spriteAsset))
        {
            RoninSimulator::ShowMessageFail("Failed load asset " + p);
        }

        p = datadir;
        p += "sounds.asset";
        if(!AssetManager::LoadAsset(p, &soundAsset))
        {
            RoninSimulator::ShowMessageFail("Failed load asset " + p);
        }

        p = datadir;
        p += "ui.asset";
        if(!AssetManager::LoadAsset(p, &uiAtlasAsset))
        {
            RoninSimulator::ShowMessageFail("Can not load " + p);
        }

        p = datadir;
        p += "artefacts-v1.atlas.asset";
        if(!AssetManager::LoadAsset(p, &asteroidAsset))
        {
            RoninSimulator::ShowMessageFail("Can not load " + p);
        }

        RoninSimulator::Simulate();
    }

    RoninSimulator::Finalize();

    return EXIT_SUCCESS;
}
