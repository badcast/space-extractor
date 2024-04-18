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

    startSprite = Math::Clamp01(startSprite);

    if(renderer == nullptr)
        renderer = spriteRenderer();

    // Set default sprite
    setSprite(m_sprites.size() * startSprite);
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
