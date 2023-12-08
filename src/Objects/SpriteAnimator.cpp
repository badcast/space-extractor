#include "SpriteAnimator.hpp"

void SpriteAnimator::setSprite(unsigned spriteIndex)
{
    if(renderer == nullptr || spriteIndex > m_sprites.size() - 1)
        return;
    m_currentSprite = spriteIndex;

    renderer->setSprite(m_sprites[spriteIndex]);
}

void SpriteAnimator::OnStart()
{
    lastTime = Time::time() + animationSpeed;

    if(renderer == nullptr)
        renderer = spriteRenderer();

    // Set default sprite
    setSprite(0);
}

void SpriteAnimator::OnUpdate()
{
    if(lastTime > Time::time())
        return;

    unsigned newSpriteIndex = m_currentSprite;
    if(++newSpriteIndex == m_sprites.size())
        newSpriteIndex = 0; // revert to begin

    setSprite(newSpriteIndex);
    lastTime = Time::time() + animationSpeed;
}
