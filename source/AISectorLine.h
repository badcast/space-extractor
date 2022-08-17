#pragma once
#include <ronin/framework.h>

using namespace RoninEngine;
using namespace RoninEngine::Runtime;
using namespace RoninEngine::AI;
using namespace RoninEngine::UI;

class SectorLine : public World
{
    NavMesh navMesh {100, 100};

public:
    SectorLine() : World("Sector Line")
    {
    }
    void OnStart();
    void OnGizmos();
};
