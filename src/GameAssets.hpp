#ifndef _GAME_ASSETS_HPP_
#define _GAME_ASSETS_HPP_

#include "SpaceDepends.hpp"

extern struct _GameAssets
{
    Asset *gameSprites;
    Asset *gameSounds;
    Asset *uiAtlas;
    Asset *artefacts;
    Asset *asteroids;
} assets;

void LoadAssets();

#endif
