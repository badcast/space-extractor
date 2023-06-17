#ifndef GAMEMAINMENU_H
#define GAMEMAINMENU_H

#include <ronin/framework.h>
#include <initializer_list>

#include "AISectorLine.h"
#include "WorldStrategyAsteroids.h"
#include "BunkerGameShooter.h"
#include "OverlayLevel.h"

using namespace RoninEngine::Runtime;

class GameMainMenu : public World
{

public:
    GameMainMenu()
        : World("Game Menu") {};

    void on_start() override;
    void on_update() override;
    void on_unloading() override;
};

static GameMainMenu* main_menu = nullptr;

void switch_game_level(World* level);

#endif // GAMEMAINMENU_H
