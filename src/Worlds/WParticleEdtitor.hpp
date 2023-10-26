/*
 * author: badcast <lmecomposer@gmail.com>
 * year 2023
 * status: already coding
 */

#ifndef _WPARTICLE_EDITOR_HPP_
#define _WPARTICLE_EDITOR_HPP_

#include "SpaceExtractor.hpp"

class WParticleEdtitor : public World
{
public:
    ParticleSystem* particle;
    WParticleEdtitor() : World("Particle Editor")
    {
    }

    void OnStart();
    void OnUpdate();
};

#endif
