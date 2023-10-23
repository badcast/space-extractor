#include "Particle.hpp"

void Particle::make_particles(int n)
{
    ParticleDrain drain;
    std::string particle_name;

    for(; n-- > 0;)
    {
        SpriteRenderer *spriteRender = Primitive::CreateEmptyGameObject()->AddComponent<SpriteRenderer>();
        particle_name = spriteRender->gameObject()->name();
        particle_name += " # ";
        particle_name += std::to_string(n);
        spriteRender->gameObject()->name(particle_name);

        Transform *t = spriteRender->transform();
        t->RegisterOnDestroy(
            [this](Component *comp)
            {
                // Remove old's

                ParticleDrain drain {};
                drain.render = comp->GetComponent<SpriteRenderer>();
                auto iter = m_drains.find(drain);
                if(iter != std::end(this->m_drains))
                {
                    this->m_drains.erase(iter);
                }
            });

        if(!worldSpace)
            t->setParent(transform(), false);
        else
            t->position(transform()->position());

        if(rotate)
            t->angle(Random::AngleDegress());

        // Set Layer
        t->layer(Layers::ParticleClass);

        // Set Sprite Source
        spriteRender->setSprite(source);

        // Set Start Size
        spriteRender->setSize(startSize);

        // Set Start Color
        spriteRender->setColor(startColor);
        drain = {spriteRender, TimeEngine::time(), randomDirection ? Random::RandomVector() : direction};

        // Auto destroy elapsed time
        if(m_duration > 0)
            Destroy(spriteRender->gameObject(), m_duration);

        m_drains.insert(drain);
    }
}

void Particle::OnAwake()
{
    setInterpolates(m_duration, m_durationStartRange, m_durationEndRange);
    m_maked = 0;
}

void Particle::OnStart()
{
    // init
    transform()->layer(Layers::ParticleClass);

    if(delay > 0)
        m_timing = delay + TimeEngine::time();
    else
        m_timing = 0;
}

void Particle::OnUpdate()
{
    // Create new particles if the "timing" time has come, or its creation has reached the end (maxParticles)
    // Update existing particles and perform interpolation for them
    if(loop || !m_drains.empty())
    {
        float t = TimeEngine::time();
        // Make new particle (interval)
        if(emit && (m_timing <= t && (maxParticles == 0 || m_maked < maxParticles)))
        {
            int max = maxParticles == 0 ? std::numeric_limits<int>::max() : maxParticles;
            int make_n = Math::Min(startWith, static_cast<int>(max - m_drains.size()));
            make_particles(make_n);
            m_maked += make_n;

            // update interval
            m_timing = t + interval;
        }

        // Update existing particles (drains)
        for(const ParticleDrain &drain : m_drains)
        {
            float drain_duration = t - drain.initTime;
            float drain_speed = TimeEngine::deltaTime() * speed;
            float state_percentage;
            int interpolateBegin, interpolateEnd;

            if(drain_duration <= m_durationStartRange)
            {
                interpolateBegin = BEGIN_STATE;
                state_percentage = drain_duration / m_durationStartRange;
            }
            else if(drain_duration >= m_durationEndRange)
            {
                interpolateBegin = END_STATE;
                state_percentage = drain_duration / m_durationEndRange;
            }
            else
            {
                interpolateBegin = SIMULATE_STATE;
                state_percentage = drain_duration / m_duration;
            }

            interpolateEnd = Math::Min<int>(Math::Max<int>(interpolateBegin, interpolateBegin + 1), END_STATE);

#define INTERPOLATION_VALUE(STATE, BEGIN, ANY, END) (STATE == SIMULATE_STATE ? (ANY) : STATE == BEGIN_STATE ? (BEGIN) : (END))

            // Colorize
            if(colorable)
            {
                Color interpolate_from = INTERPOLATION_VALUE(interpolateBegin, startColor, centerColor, endColor);
                Color interpolate_to = INTERPOLATION_VALUE(interpolateEnd, startColor, centerColor, endColor);
                drain.render->setColor(Color::Lerp(interpolate_from, interpolate_to, state_percentage));
            }

            // Scaling
            if(scalable)
            {
                Vec2 interpolate_from = INTERPOLATION_VALUE(interpolateBegin, startSize, centerSize, endSize);
                Vec2 interpolate_to = INTERPOLATION_VALUE(interpolateEnd, startSize, centerSize, endSize);
                drain.render->setSize(Vec2::Lerp(interpolate_from, interpolate_to, state_percentage));
            }

#undef INTERPOLATION_VALUE

            // Move, rotate
            Transform *particleTransform = drain.render->transform();
            particleTransform->Translate(drain.direction * drain_speed);

            if(rotate)
            {
                particleTransform->angle(particleTransform->angle() + rotatePerSecond * drain_speed);
            }
        }
    }
    else if(!loop && destroyAfter && m_maked == maxParticles && m_drains.empty())
    {
        gameObject()->Destroy();
    }
}

void Particle::setInterpolates(float duration)
{
    setInterpolates(duration, 0.1f, 0.1f);
}

void Particle::setInterpolates(float duration, float startRange, float endRange)
{
    this->m_duration = Math::Max(duration, 0.f);
    this->m_durationStartRange = this->m_duration * Math::Clamp01(startRange);
    this->m_durationEndRange = this->m_duration - this->m_duration * Math::Clamp01(endRange);
}

void Particle::setColor(Color color)
{
    colorable = false;
    startColor = color;
}

void Particle::setColors(Color startState, Color endState)
{
    setColors(startState, Color::Lerp(startState, endState, 1.f / 2), endState);
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
    setSizes(startState, Vec2::Lerp(startState, endState, 1.f / 2), endState);
}

void Particle::setSizes(Vec2 startState, Vec2 centerState, Vec2 endState)
{
    scalable = true;
    startSize = startState;
    centerSize = centerState;
    endSize = endState;
}
