/*
 * author: badcast <lmecomposer@gmail.com>
 * year 2023
 * status: already coding
 */

#ifndef _PARTICLE_HPP_
#define _PARTICLE_HPP_

#include "SpaceDepends.hpp"

enum ParticleState
{
    BEGIN_STATE = 0,
    SIMULATE_STATE = 1,
    END_STATE = 2
};

struct ParticleDrain
{
    SpriteRenderer *render;
    mutable int state;
    float initTime;
    Vec2 direction;
};

static bool IsLowerParticleDrain(const ParticleDrain &lhs, const ParticleDrain &rhs)
{
    return lhs.render < rhs.render;
}

using LowerParticleDrain = std::integral_constant<decltype(&IsLowerParticleDrain), &IsLowerParticleDrain>;

// It's base class for Particles
class Particle : public Behaviour
{
protected:
    std::set<ParticleDrain, LowerParticleDrain> m_particles;
    float _timing;
    int maked;

    // bool:colorable - if true simulate it
    bool colorable = true;
    Color startColor = Color::transparent;
    Color centerColor = Color::white;
    Color endColor = Color::transparent;

    // bool:scalable - if true simulate it
    bool scalable = true;
    Vec2 startSize = Vec2::one;
    Vec2 centerSize = Vec2::one;
    Vec2 endSize = Vec2::one;

    // It's percentages
    float duration = 10;
    float durationStartRange = 0.1f; // Range [0.0,1.0]
    float durationEndRange = 0.1f;   // Range [0.0,1.0]

    void make_particles(int n);

public:
    // Main Control
    bool emit = true;
    bool loop = true;
    bool rotate = true;
    bool worldSpace = true;
    bool destroyAfter = true;
    bool randomDirection = false;

    float interval = 1.0f;

    // State
    int startWith = 1;
    int maxParticles = 10;

    // Live
    float speed = 1;
    float rotatePerSecond = 10;

    Vec2 direction = Vec2::up;

    // Source for set
    Sprite *source = nullptr;

    // Set the interpolation ranges of the duration and startRange, endRange equaled 0.1
    bool setInterpolates(float duration);
    // Set the interpolation ranges of the duration
    // param startRange from 0.0 to 1.0
    // param endRange from 0.0 to 1.0
    bool setInterpolates(float duration, float startRange, float endRange);

    // Set no interpolate colors
    void setColor(Color color);
    // Set interpolation with start to end
    void setColors(Color startState, Color endState);
    // Set interpolation from ranges
    void setColors(Color startState, Color centerState, Color endState);

    void setSize(Vec2 size);
    // Set interpolation with start to end
    void setSizes(Vec2 startState, Vec2 endState);
    // Set interpolation from ranges
    void setSizes(Vec2 startState, Vec2 centerState, Vec2 endState);

    void OnStart();
    void OnUpdate();
};

#endif
