#ifndef GAMEMAINMENU_H
#define GAMEMAINMENU_H

#include <ronin/framework.h>
#include <initializer_list>
#include <array>

#include "AISectorLine.h"
#include "./Worlds/world_strategy/WorldStrategyAsteroids.h"
#include "./Worlds/world_piano/WorldPiano.h"
#include "./Worlds/world_population/PathMover.h"
#include "BunkerGameShooter.h"
#include "OverlayLevel.h"

using namespace RoninEngine::Runtime;

class GameMainMenu : public World
{

public:
    GameMainMenu() : World("Game Menu")
    {
    }

    void OnStart();
    void OnUpdate();
    void OnUnloading();
};

static GameMainMenu *main_menu = nullptr;

void switch_game_level(World *level);

#endif // GAMEMAINMENU_H
