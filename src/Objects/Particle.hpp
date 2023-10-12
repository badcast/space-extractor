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
    int state;
    float initTime;
};

// It's base class for Particles
class Particle : public Behaviour
{
private:
    std::list<ParticleDrain> m_particles;
    float _timing;
    int maked;

    void OnStart();
    void OnUpdate();
    void OnDestroy();

protected:
    void make_particles(int n);

public:
    // Main Control
    bool emit = true;
    bool loop = true;
    bool rotate = true;
    bool colorable = true;
    bool scalable = true;
    bool worldSpace = true;
    bool destroyAfter = true;

    float interval = 1.0f;

    // It's percentages
    float duration = 10;
    float durationStartPercentage = 0.1f; // Range [0.0,1.0]
    float durationEndPercentage = 0.1f;   // Range [0.0,1.0]

    // State
    int startWith = 1;
    int maxParticles = 10;

    // Live
    float speed = 1;
    float rotatePerSecond = 10;

    // bool:colorable - if true simulate it
    Color color = Color::white;
    Color startColor = Color::white;
    Color endColor = Color::white;

    // bool:scalable - if true simulate it
    Vec2 size = Vec2::one;
    Vec2 startSize = Vec2::one;
    Vec2 endSize = Vec2::one;

    Vec2 direction = Vec2::up;

    // Source for set
    Sprite *source = nullptr;
};

#endif
