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
    Ref<Player> player;
    Enhancer enhancer;
    std::set<Ref<Enemy>> activeEnemies;
    IVStars ivstars {};
    Terrain2DRef terrain;

    SpriteRef sprite_explode = nullptr;
    SpriteRef sprite_explode_flow = nullptr;
    SpriteRef sprite_drop_drains = nullptr;
    SpriteRef sprite_drop_drains2 = nullptr;

    WGame() : World("Space Extractor"), navMesh(nullptr), player(nullptr)
    {
    }

    static WGame *current;
};
#endif
