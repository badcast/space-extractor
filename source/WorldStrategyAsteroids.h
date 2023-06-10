#pragma once
#include <ronin/framework.h>

using namespace RoninEngine;
using namespace RoninEngine::Runtime;
using namespace RoninEngine::AIPathFinder;
using namespace RoninEngine::UI;

class SpaceExtractorLevel : public World
{

    std::vector<Transform*> stars { 100, nullptr };
    Runtime::Vec2 dmgReg { -6, 6 };

public:
    SpaceExtractorLevel()
        : World(std::string("Space Extractor Strategy game"))
    {
    }

    void on_start();
    void on_update();
    void on_gizmo();
    void on_unloading();
};
