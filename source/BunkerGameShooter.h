/*
 * author: badcast <lmecomposer@gmai.com>
 * year 2023
 * status: already coding
 */

#ifndef BUNKER_GAME_H
#define BUNKER_GAME_H

#include "SpaceMainMenu.h"

namespace RoninEngine::Runtime
{
    class BunkerWorld : public World
    {
        float last_time = 0;
        GameObject* bulTempl;
        std::list<Transform*> turrets;
        std::list<Transform*> bullets;

    public:
        BunkerWorld()
            : World("2D Shooter")
        {
        }
        void OnStart();
        void OnUpdate();
        void OnGizmos();
        void OnUnloading();
    };
}

#endif
