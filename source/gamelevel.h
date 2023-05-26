#pragma once
#include <ronin/framework.h>

using namespace RoninEngine;
using namespace RoninEngine::Runtime;
using namespace RoninEngine::AIPathFinder;
using namespace RoninEngine::UI;

class GameLevel : public Level
{
public:
    void on_start();
    void on_update();
    void on_gizmo();
    void on_unloading();
};
