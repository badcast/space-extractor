#pragma once
#include <ronin/framework.h>

using namespace RoninEngine;
using namespace RoninEngine::Runtime;
using namespace RoninEngine::AIPathFinder;
using namespace RoninEngine::UI;

class GameLevel : public Level {
   public:
    GameLevel();
    void start() override;
    void update() override;
    void onDrawGizmos() override;
};
