#pragma once
#include <ronin/framework.h>

using namespace RoninEngine;
using namespace RoninEngine::Runtime;
using namespace RoninEngine::AIPathFinder;
using namespace RoninEngine::UI;

class SectorLine : public World
{

    NavMesh navMesh { 128, 128 };

public:
    SectorLine()
        : World("Sector Line")
    {
    }
    void on_start();
    void on_update();
    void on_gizmo();
};
