#ifndef GAMEMAINMENU_H
#define GAMEMAINMENU_H

#include <ronin/framework.h>

using namespace RoninEngine::Runtime;

class GameMainMenu : public Level
{
public:
    GameMainMenu() = default;

    void on_start();
    void on_update();
};

static GameMainMenu* main_menu = nullptr;

void switch_game_level(Level* level);

#endif // GAMEMAINMENU_H
