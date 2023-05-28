#ifndef GAMEMAINMENU_H
#define GAMEMAINMENU_H

#include <ronin/framework.h>

class GameMainMenu : public RoninEngine::Runtime::Level
{
public:
    GameMainMenu() = default;

    void on_start();
    void on_update();
};

#endif // GAMEMAINMENU_H
