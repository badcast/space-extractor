#include "ParticleSystem.hpp"

enum ParticleState
{
    BEGIN_STATE = 0,
    SIMULATE_STATE = 1,
    END_STATE = 2
};

bool operator<(const ParticleDrain &lhs, const ParticleDrain &rhs)
{
    return lhs.render < rhs.render;
}

// using LowerParticleDrain = std::integral_constant<decltype(&IsLowerParticleDrain), &IsLowerParticleDrain>;

inline Sprite *inspect_at(const std::vector<Sprite *> &sources, int &last_inspected, ParticleSourceInspect inspection)
{
    Sprite *inspected;

    if(!sources.empty())
    {
        int inspect = last_inspected;
        switch(inspection)
        {
            case ParticleSourceInspect::InspectNext:
                last_inspected = static_cast<int>((last_inspected + 1) % sources.size());
                break;
            case ParticleSourceInspect::InspectRandom:
                last_inspected = inspect = Random::Range(0, sources.size());
                break;
        }

        inspected = sources[inspect];
    }
    else
        inspected = nullptr;

    return inspected;
}

void ParticleSystem::link_particles(int n)
{
    std::string particle_name;

    // lambda method
    auto fabricate = [&](SpriteRenderer *spriteRender)
    {
        Transform *t = spriteRender->transform();
        t->position(transform()->position());
        if(worldSpace)
        {
            t->setParent(nullptr);
        }
        else
        {
            t->setParent(transform(), false);
        }
        if(rotate)
            t->angle(Random::AngleDegress());

        // Set Layer
        t->layer(Layers::ParticleClass);

        // Set Sprite Source
        spriteRender->setSprite(inspect_at(m_sources, m_lastInspected, m_sourceInspect));

        // Set Start Size
        spriteRender->setSize(startSize);

        // Set Start Color
        spriteRender->setColor(startColor);
        m_drains.insert({spriteRender, TimeEngine::time(), randomDirection ? Random::RandomVector() : direction});
    };

    // use existences
    if(reserving)
    {
        while(m_reserved_drains.empty() == false && n-- > 0)
        {
            SpriteRenderer *exist = m_reserved_drains.back();
            m_reserved_drains.pop_back();
            // showing
            exist->gameObject()->SetActive(true);
            fabricate(exist);
        }
    }
    else if(!m_reserved_drains.empty())
    {
        clearReserved();
    }

    // make new
    for(; n-- > 0;)
    {
        SpriteRenderer *spriteRender = Primitive::CreateEmptyGameObject()->AddComponent<SpriteRenderer>();
        particle_name = spriteRender->gameObject()->name();
        particle_name += " # ";
        particle_name += std::to_string(n);
        spriteRender->gameObject()->name(particle_name);
        spriteRender->AddOnDestroy(
            [this](Component *comp)
            {
                // Remove old's

                ParticleDrain drain {};
                drain.render = comp->GetComponent<SpriteRenderer>();
                auto iter = this->m_drains.find(drain);
                if(iter != std::end(this->m_drains))
                {
                    this->m_drains.erase(iter);
                }
            });
        fabricate(spriteRender);
    }
}

void ParticleSystem::unlink_particle(const ParticleDrain *drain)
{
    bool destroyToo = reserving == false;

    if(destroyToo)
    {
        drain->render->gameObject()->Destroy();
    }
    else
    {
        drain->render->gameObject()->SetActive(false);
        // State clear render cache
        drain->render->setSprite(nullptr);
        m_reserved_drains.emplace_back(drain->render);
    }
}

ParticleSystem::ParticleSystem() : Behaviour("Particle System"), m_drains()
{
}

void ParticleSystem::setSource(Sprite *source, ParticleSourceInspect inspectType)
{
    m_sources.clear();
    m_sourceInspect = inspectType;
    m_sources.push_back(source);
}

void ParticleSystem::setSources(Sprite *src1, Sprite *src2, ParticleSourceInspect inspectType)
{
    m_sources.clear();
    m_sourceInspect = inspectType;
    m_sources.push_back(src1);
    m_sources.push_back(src2);
}

void ParticleSystem::setSources(Sprite *src1, Sprite *src2, Sprite *src3, ParticleSourceInspect inspectType)
{
    m_sources.clear();
    m_sourceInspect = inspectType;
    m_sources.push_back(src1);
    m_sources.push_back(src2);
    m_sources.push_back(src3);
}

void ParticleSystem::OnDestroy()
{
    Reset();
}

void ParticleSystem::OnAwake()
{
    setInterpolates(m_duration, m_durationStartRange, m_durationEndRange);
    m_maked = 0;
    m_lastInspected = 0;
}

void ParticleSystem::OnStart()
{
    // init
    transform()->layer(Layers::ParticleClass);

    if(delay > 0)
        m_timing = delay + TimeEngine::time();
    else
        m_timing = 0;

    if(maxParticles == 0)
        maxParticles = startWith;
}

void ParticleSystem::OnUpdate()
{
    // Create new particles if the "timing" time has come, or its creation has reached the end (maxParticles)
    // Update existing particles and perform interpolation for them
    if(loop || m_maked < maxParticles || !m_drains.empty())
    {
        float t = TimeEngine::time();
        // Make new particle (interval)
        if(emit && m_timing <= t && m_maked < maxParticles)
        {
            int max = maxParticles;
            int make_n = Math::Min(startWith, static_cast<int>(max - m_drains.size()));
            link_particles(make_n);
            m_maked += make_n;

            // update interval
            m_timing = t + interval;
        }

        // Update existing particles (drains)
        for(std::set<ParticleDrain>::iterator drain = std::begin(m_drains); drain != std::end(m_drains); ++drain)
        {
            float drain_duration = t - drain->initTime;
            if(drain_duration >= m_duration)
            {
                unlink_particle(&(*drain));
                continue;
            }

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
                drain->render->setColor(Color::Lerp(interpolate_from, interpolate_to, state_percentage));
            }

            // Scaling
            if(scalable)
            {
                Vec2 interpolate_from = INTERPOLATION_VALUE(interpolateBegin, startSize, centerSize, endSize);
                Vec2 interpolate_to = INTERPOLATION_VALUE(interpolateEnd, startSize, centerSize, endSize);
                drain->render->setSize(Vec2::Lerp(interpolate_from, interpolate_to, state_percentage));
            }

#undef INTERPOLATION_VALUE

            // Move, rotate
            Transform *particleTransform = drain->render->transform();
            particleTransform->Translate(drain->direction * drain_speed);

            if(rotate)
            {
                particleTransform->angle(particleTransform->angle() + rotatePerSecond * drain_speed);
            }
        }

        ParticleDrain drained;
        for(SpriteRenderer *reserved : m_reserved_drains)
        {
            drained.render = reserved;
            m_drains.erase(drained);
        }
    }
    else if(destroyAfter && !loop && m_maked == maxParticles && m_drains.empty())
    {
        gameObject()->Destroy();
    }
}

void ParticleSystem::setInterpolates(float duration)
{
    setInterpolates(duration, 0.1f, 0.1f);
}

void ParticleSystem::setInterpolates(float duration, float startRange, float endRange)
{
    this->m_duration = Math::Max(duration, 0.f);
    this->m_durationStartRange = this->m_duration * Math::Clamp01(startRange);
    this->m_durationEndRange = this->m_duration - this->m_duration * Math::Clamp01(endRange);
}

void ParticleSystem::setColor(Color color)
{
    colorable = false;
    startColor = color;
}

void ParticleSystem::setColors(Color startState, Color endState)
{
    setColors(startState, Color::Lerp(startState, endState, 1.f / 2), endState);
}

void ParticleSystem::setColors(Color startState, Color centerState, Color endState)
{
    colorable = true;
    startColor = startState;
    centerColor = centerState;
    endColor = endState;
}

void ParticleSystem::setSize(Vec2 size)
{
    scalable = false;
    startSize = size;
}

void ParticleSystem::setSizes(Vec2 startState, Vec2 endState)
{
    setSizes(startState, Vec2::Lerp(startState, endState, .5f), endState);
}

void ParticleSystem::setSizes(Vec2 startState, Vec2 centerState, Vec2 endState)
{
    scalable = true;
    startSize = startState;
    centerSize = centerState;
    endSize = endState;
}

float ParticleSystem::getDuration()
{
    return m_duration;
}

int ParticleSystem::getCount()
{
    return static_cast<int>(m_drains.size() + m_reserved_drains.size());
}

void ParticleSystem::Reset()
{
    clearReserved();

    for(const ParticleDrain &drain : m_drains)
    {
        drain.render->ClearOnDestroy();
        drain.render->gameObject()->Destroy(); // destroy now
    }

    m_drains.clear();
    m_maked = 0;
    m_timing = 0;
    m_lastInspected = 0;
}

void ParticleSystem::clearReserved()
{
    for(SpriteRenderer *drain : m_reserved_drains)
    {
        drain->ClearOnDestroy();
        drain->gameObject()->Destroy(); // destroy now
    }
    m_reserved_drains.clear();
    m_reserved_drains.shrink_to_fit();
}
