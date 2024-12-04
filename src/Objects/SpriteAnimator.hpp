/*
 * author: badcast <lmecomposer@gmail.com>
 * year 2023
 * status: already coding
 */

#ifndef _SPRITE_ANIMATOR_HPP_
#define _SPRITE_ANIMATOR_HPP_

#include "SpaceDepends.hpp"

class SpriteAnimator : public Behaviour
{
private:
    float lastTime;
    SpriteRendererRef renderer;
    unsigned m_currentSprite;
    std::vector<SpriteRef> m_sprites;

    void setSprite(unsigned spriteIndex);

public:
    // Animation speed
    float animationSpeed = 1.0f;
    float startSprite = 0.0;

    template <typename Container>
    void SetSprites(const Container &sprites)
    {
        m_sprites.assign(sprites.begin(), sprites.end());
    }

    void OnStart() override;
    void OnUpdate() override;
};

#endif
