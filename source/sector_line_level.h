#pragma once
#include <ronin/framework.h>

using namespace RoninEngine;
using namespace RoninEngine::Runtime;
using namespace RoninEngine::AIPathFinder;
using namespace RoninEngine::UI;

class SectorLine : public Level {
   public:
    SectorLine();
    ~SectorLine() = default;
    void on_start() override;
    void on_update() override;
    void on_gizmo() override;
};