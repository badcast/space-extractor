/*
 * author: badcast <lmecomposer@gmail.com>
 * year 2023
 * status: already coding
 */

#ifndef _WGAME_HPP_
#define _WGAME_HPP_

#include "SpaceExtractor.hpp"

class WGame : public World
{
public:
    NavMesh *navMesh;
    Player *player;
    std::set<Enemy *> enemies;

    WGame() : World("Space Extractor Game World"), navMesh(nullptr), player(nullptr)
    {
    }

    void OnAwake();
    void OnStart();
    void OnUpdate();
    void OnUnloading();
    void OnGizmos();

    static WGame *current;
};

#endif
