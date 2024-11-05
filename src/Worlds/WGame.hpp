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
    PlayerRef player;
    Enhancer enhancer;
    std::set<EnemyRef> activeEnemies;
    IVStars ivstars {};
    Terrain2DRef terrain;

    WGame() : World("Space Extractor"), navMesh(nullptr), player(nullptr)
    {
    }

    static WGame *current;
};
#endif
