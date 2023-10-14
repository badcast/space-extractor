#include "Particle.hpp"

void Particle::make_particles(int n)
{
    ParticleDrain drain;
    maked = std::max(maxParticles, maked + n);
    std::string particle_name;
    for(; n-- > 0;)
    {
        SpriteRenderer *spriteRender = Primitive::create_empty_game_object()->AddComponent<SpriteRenderer>();
        particle_name = spriteRender->gameObject()->name();
        particle_name += " # ";
        particle_name += std::to_string(n);
        spriteRender->gameObject()->name(particle_name);

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
        drain = {spriteRender, BEGIN_STATE, TimeEngine::time(), randomDirection ? Random::RandomVector() : direction};

        // Auto destroy elapsed time
        if(duration > 0)
            Destroy(spriteRender->gameObject(), duration);

        m_particles.emplace_back(drain);
    }
}

void Particle::OnStart()
{
    // init
    transform()->layer(Layers::ParticleClass);

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
            if(drif_duration <= durationStartRange)
            {
                drif.state = BEGIN_STATE;
            }
            else if(drif_duration >= durationEndRange)
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
                drif.render->setColor(Color::Lerp(drif.render->getColor(), STATE_PROPERTY(startColor, centerColor, endColor), drif_speed));
            }

            // Scaling
            if(scalable)
            {
                drif.render->setSize(Vec2::Lerp(drif.render->getSize(), STATE_PROPERTY(startSize, centerSize, endSize), drif_speed));
            }

            // Move, rotate
            Transform *tr = drif.render->transform();
            tr->Translate(drif.direction * drif_speed);

            if(rotate)
            {
                tr->angle(tr->angle() + rotatePerSecond * drif_speed);
            }
        }
    }
    else if(!loop && destroyAfter && maked == maxParticles && m_particles.empty())
    {
        gameObject()->Destroy();
    }
}

bool Particle::setInterpolates(float duration)
{
    return setInterpolates(duration, 0.1f, 0.1f);
}

bool Particle::setInterpolates(float duration, float startRange, float endRange)
{
    if(duration < 0 || endRange < 0 || startRange < 0 || startRange > 1 || endRange > 1)
        return false;

    this->duration = duration;
    durationStartRange = duration * startRange;
    durationEndRange = duration * endRange;
    return true;
}

void Particle::setColor(Color color)
{
    colorable = false;
    startColor = color;
}

void Particle::setColors(Color startState, Color endState)
{
    setColors(startState, Color::Lerp(startState, endState, 0.5f), endState);
}

void Particle::setColors(Color startState, Color centerState, Color endState)
{
    colorable = true;
    startColor = startState;
    centerColor = centerState;
    endColor = endState;
}

void Particle::setSize(Vec2 size)
{
    scalable = false;
    startSize = size;
}

void Particle::setSizes(Vec2 startState, Vec2 endState)
{
    setSizes(startState, Vec2::Lerp(startState, endState, 0.5f), endState);
}

void Particle::setSizes(Vec2 startState, Vec2 centerState, Vec2 endState)
{
    scalable = true;
    startSize = startState;
    centerSize = centerState;
    endSize = endState;
}
