/*
 * author: badcast <lmecomposer@gmail.com>
 * year 2023
 * status: already coding
 */

#ifndef _WGAME_HPP_
#define _WGAME_HPP_

#include "SpaceExtractor.hpp"
#include "LevelEnhancer.hpp"
#include "Objects/IVStars.hpp"

class Enhancer;

class WGame : public World
{
public:
    void OnAwake();
    void OnStart();
    void OnUpdate();
    void OnUnloading();
    void OnGizmos();

public:
    NavMesh *navMesh;
    Player *player;
    Enhancer enhancer;
    std::set<Enemy *> activeEnemies;
    IVStars ivstars {};

    WGame() : World("Space Extractor"), navMesh(nullptr), player(nullptr)
    {
    }

    static WGame *current;
};
#endif
