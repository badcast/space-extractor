#include "Particle.hpp"

void Particle::make_particles(int n)
{
    ParticleDrain drain;
    maked = std::max(maxParticles, maked + n);
    for(; n-- > 0;)
    {
        SpriteRenderer *spriteRender = Primitive::create_empty_game_object()->AddComponent<SpriteRenderer>();
        spriteRender->gameObject()->name(this->gameObject()->name() + " # " + std::to_string(n));
        Transform *t = spriteRender->transform();
        t->RegisterOnDestroy(
            [this](Component *comp)
            {
                // Remove old's
                auto iter = std::find_if(
                    std::begin(this->m_particles),
                    std::end(this->m_particles),
                    [comp](ParticleDrain &__drain__) { return comp->transform() == __drain__.render->transform(); });

                if(iter != std::end(this->m_particles))
                {
                    this->m_particles.erase(iter);
                }
            });
        t->setParent(transform(), false);

        if(rotate)
            t->angle(Random::AngleDegress());

        t->layer(Layers::ParticleClass);
        spriteRender->setSprite(source);

        // Set Start Size
        spriteRender->setSize(startSize);

        // Set Start Color
        spriteRender->setColor(startColor);
        drain = {spriteRender, BEGIN_STATE, TimeEngine::time()};

        // Auto destroy elapsed time
        Destroy(spriteRender->gameObject(), duration);

        m_particles.emplace_back(drain);
    }
}

void Particle::OnStart()
{
    // init
    transform()->layer(Layers::ParticleClass);

    if(duration < 0)
        duration = 0;

    durationStartPercentage = Math::Clamp01(durationStartPercentage);
    durationEndPercentage = Math::Clamp01(durationEndPercentage);

    // TODO: Range durationStartPercentage and durationEndPercentage is not insides

    _timing = 0;
    maked = 0;
}

void Particle::OnUpdate()
{
    if(!emit)
        return;

    if(loop || maked < maxParticles || m_particles.size())
    {
        float t = TimeEngine::time();
        // Make new particle (interval)
        if(m_particles.size() < maxParticles && _timing < TimeEngine::time())
        {
            // update interval
            make_particles(Math::Min(startWith, static_cast<int>(maxParticles - m_particles.size())));
            _timing = t + interval;
        }

#define STATE_PROPERTY(BEGIN, ANY, END) (drif.state == SIMULATE_STATE ? (ANY) : drif.state == BEGIN_STATE ? (BEGIN) : (END))

        // Particle draw
        for(ParticleDrain &drif : m_particles)
        {
            // calc drif state
            float drif_duration = t - drif.initTime;
            float drif_speed = TimeEngine::deltaTime() * speed;
            if(drif_duration <= duration * durationStartPercentage)
            {
                drif.state = BEGIN_STATE;
            }
            else if(drif_duration >= duration - duration * durationEndPercentage)
            {
                drif.state = END_STATE;
            }
            else
            {
                drif.state = SIMULATE_STATE;
            }

            // Colorize
            if(colorable)
            {
                drif.render->setColor(Color::Lerp(drif.render->getColor(), STATE_PROPERTY(startColor, color, endColor), drif_speed));
            }

            // Scaling
            if(scalable)
            {
                drif.render->setSize(Vec2::Lerp(drif.render->getSize(), STATE_PROPERTY(startSize, size, endSize), drif_speed));
            }

            // Move, rotate
            Transform *tr = drif.render->transform();
            tr->Translate(direction * drif_speed);

            if(rotate)
            {
                tr->angle(tr->angle() + rotatePerSecond * drif_speed);
            }
        }
    }
    else if(maked == maxParticles && !m_particles.size() && destroyAfter)
    {
        gameObject()->Destroy();
    }
}

void Particle::OnDestroy()
{
}
