#include "GameAssets.hpp"

GameAssets assets;

void LoadAssets()
{
    // Load Assets
    std::string p;
    std::string datadir = Paths::GetRuntimeDir() + "data" + Paths::GetPathSeperatorOS();
    std::initializer_list<std::pair<const char *, Asset **>> _inits_ {
        {"resources.asset", &assets.gameSprites},
        {"sounds.asset", &assets.gameSounds},
        {"ui.asset", &assets.uiAtlas},
        {"artefacts-v1.atlas.asset", &assets.artefacts},
        {"asteroids-v1.atlas.asset", &assets.asteroids}};

    for(const std::pair<const char *, Asset **> &i : _inits_)
    {
        p = datadir;
        p += i.first;
        if(!AssetManager::LoadAsset(p, (i.second)))
        {
            RoninSimulator::ShowMessageFail("Failed load asset " + p);
        }
    }    
}
