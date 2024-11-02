#ifndef _GAME_ASSETS_HPP_
#define _GAME_ASSETS_HPP_

#include "SpaceDepends.hpp"

struct GlobalAssets
{
    Asset *gameSprites;
    Asset *gameSounds;
    Asset *uiAtlas;
    Asset *artefacts;
    Asset *asteroids;
};

extern GlobalAssets globalAssets;

void LoadAssets();

#endif
