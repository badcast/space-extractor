#pragma once
#include <ronin/framework.h>

using namespace RoninEngine;
using namespace RoninEngine::Runtime;
using namespace RoninEngine::AI;
using namespace RoninEngine::UI;

class SpaceExtractorLevel : public World
{
    std::vector<Transform*> stars { 500, nullptr };
    Runtime::Vec2 dmgReg { -10, 10 };

public:
    SpaceExtractorLevel()
        : World(std::string("Space Extractor Strategy game"))
    {
    }

    void OnStart();
    void OnUpdate();
    void OnGizmos();
    void OnUnloading();
};
