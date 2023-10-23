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
    std::set<ParticleDrain, LowerParticleDrain> m_drains;
    float m_timing;
    int m_maked;

    // bool:colorable - if true simulate it
    bool colorable = true;
    Color startColor = Color::transparent;
    Color centerColor = Color::white;
    Color endColor = Color::transparent;

    // bool:scalable - if true simulate it
    bool scalable = true;
    Vec2 startSize = Vec2::zero;
    Vec2 centerSize = Vec2::one;
    Vec2 endSize = Vec2::zero;

    // It's percentages
    float m_duration = 10;
    float m_durationStartRange = 0.1f; // Range [0.0,1.0]
    float m_durationEndRange = 0.1f;   // Range [0.0,1.0]

    void make_particles(int n);

public:
    // Main Control
    bool emit = true;
    bool loop = true;
    bool rotate = true;
    bool worldSpace = true;
    bool destroyAfter = true;
    bool randomDirection = false;

    // Set the interval for the next particle
    float interval = 1.0f;
    float delay = 0;

    // State
    int startWith = 1;
    int maxParticles = 0;

    // Live
    float speed = 1;
    float rotatePerSecond = 10;

    Vec2 direction = Vec2::up;

    // Source for set
    Sprite *source = nullptr;
    /**
     * @brief Set the duration for interpolation, with default start and end ranges.
     *
     * This function sets the duration for interpolation with the default start and end ranges, which are both set to 0.1.
     *
     * @param duration The duration for interpolation.
     */
    void setInterpolates(float duration);

    /**
     * @brief Set the duration for interpolation with custom start and end ranges.
     *
     * This function sets the duration for interpolation and allows you to specify custom start and end ranges for the interpolation.
     *
     * @param duration    The duration for interpolation.
     * @param startRange  The start range for interpolation (0.0 to 1.0).
     * @param endRange    The end range for interpolation (0.0 to 1.0).
     */
    void setInterpolates(float duration, float startRange, float endRange);

    /**
     * @brief Set the color without interpolation.
     *
     * This function sets the color without any interpolation.
     *
     * @param color The color to be set.
     */
    void setColor(Color color);

    /**
     * @brief Set interpolation of colors from start to end.
     *
     * This function sets the interpolation of colors from a starting state to an ending state.
     *
     * @param startState The starting color.
     * @param endState   The ending color.
     */
    void setColors(Color startState, Color endState);

    /**
     * @brief Set interpolation of colors from specified ranges.
     *
     * This function sets the interpolation of colors from specified start, center, and end states.
     *
     * @param startState   The starting color.
     * @param centerState  The center color.
     * @param endState     The ending color.
     */
    void setColors(Color startState, Color centerState, Color endState);

    /**
     * @brief Set the size without interpolation.
     *
     * This function sets the size without any interpolation.
     *
     * @param size The size to be set.
     */
    void setSize(Vec2 size);

    /**
     * @brief Set interpolation of sizes from start to end.
     *
     * This function sets the interpolation of sizes from a starting state to an ending state.
     *
     * @param startState The starting size.
     * @param endState   The ending size.
     */
    void setSizes(Vec2 startState, Vec2 endState);

    /**
     * @brief Set interpolation of sizes from specified ranges.
     *
     * This function sets the interpolation of sizes from specified start, center, and end states.
     *
     * @param startState   The starting size.
     * @param centerState  The center size.
     * @param endState     The ending size.
     */
    void setSizes(Vec2 startState, Vec2 centerState, Vec2 endState);

    void OnAwake();
    void OnStart();
    void OnUpdate();
};

#endif
