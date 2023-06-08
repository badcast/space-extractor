/*
 * author: badcast <lmecomposer@gmai.com>
 * year 2023
 * status: already coding
 */

#ifndef BUNKER_GAME_H
#define BUNKER_GAME_H

#include "gamemainmenu.h"

namespace RoninEngine::Runtime
{

    class BunkerWorld : public World
    {
        GameObject* turret;

    public:
        void on_start();
        void on_update();
        void on_gizmo();
    };
}

#endif
