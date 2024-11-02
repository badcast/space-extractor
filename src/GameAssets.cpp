#include "GameAssets.hpp"

GlobalAssets globalAssets;

void LoadAssets()
{
    // Load Assets
    std::string p;
    std::string datadir = Paths::MakePathValid(Paths::GetRuntimeDir() + "data/");
    std::initializer_list<std::pair<const char *, Asset **>> _inits_ {
        {"resources.asset", &globalAssets.gameSprites},
        {"sounds.asset", &globalAssets.gameSounds},
        {"ui.asset", &globalAssets.uiAtlas},
        {"artefacts-v1.atlas.asset", &globalAssets.artefacts},
        {"asteroids-v1.atlas.asset", &globalAssets.asteroids}};

    Debug::Log("Load assets ...");

    Time::BeginWatch();
    for(const std::pair<const char *, Asset **> &i : _inits_)
    {
        p = datadir;
        p += i.first;
        Debug::Log(std::string(" Preparring ") + i.first);
        if(!AssetManager::LoadAsset(p, (i.second)))
        {
            RoninSimulator::ShowMessageFail("Failed load asset " + p);
        }
    }

    Debug::Log("Assets loaded at times: " + std::to_string(Time::EndWatch()) + "ms");
}
