#include "ComponentSpacePlanet.h"

SpacePlanet::SpacePlanet() : Behaviour("SpacePlanet")
{
}

void fabricate(SpacePlanet *planet, SpacePilot *reference, int count)
{
    for(; count--;)
    {
        SpacePilot *pilot = Instantiate(reference->gameObject(), planet->transform()->position())->AddComponent<SpacePilot>();
        pilot->owner_planet = planet;
        pilot->toWarn = Vec2::RotateAround(planet->transform()->position(), Vec2::one, Random::AngleDegress() * Math::deg2rad);
    }
}

void SpacePlanet::OnStart()
{
    static bool f = false;
    SpriteRenderer *sprRenderer;
    Sprite *playerSprite;

    f = !f;
    playerColor = f == false ? Color::blue : Color::red;
    playerSprite = Primitive::CreateSpriteTriangle(true, Vec2::half, 1, playerColor);

    // create resource streams
    sprRenderer =
        Primitive::CreateEmptyGameObject(transform()->position() + (f ? Vec2::left : Vec2::right) * 4)->AddComponent<SpriteRenderer>();
    sprRenderer->setSprite(Primitive::CreateSpriteTriangle(true, Vec2::half, 1.f, Color::green));
    resource_plant = sprRenderer->gameObject()->AddComponent<SpaceResourcePlant>();

    // create worker for get from resource stream
    SpaceWorker *worker = Primitive::CreateEmptyGameObject(transform()->position())->AddComponent<SpaceWorker>();

    sprRenderer = worker->gameObject()->AddComponent<SpriteRenderer>();
    sprRenderer->setSize(Vec2::half);
    sprRenderer->setSprite(playerSprite);
    worker->owner_planet = this;

    // find target
    std::list<SpacePlanet *> planets = World::self()->FindObjectsWithType<SpacePlanet>();
    planets.remove(this);

    if(planets.empty())
    {
        return;
    }

    target = planets.front();

    referencePilot = Primitive::CreateEmptyGameObject(this->transform()->position())->AddComponent<SpacePilot>();
    sprRenderer = referencePilot->gameObject()->AddComponent<SpriteRenderer>();
    sprRenderer->setSprite(playerSprite);
    sprRenderer->setSize(sprRenderer->getSize() / 12);
    referencePilot->gameObject()->SetActive(false);
}

void SpacePlanet::OnUpdate()
{
    if(!target || lastTimeFab > TimeEngine::time())
    {
        return;
    }

    lastTimeFab = TimeEngine::time() + 3;
    int count = std::min(startWith, resources);
    resources -= count;
    fabricate(this, referencePilot, count);
}

void SpacePlanet::OnGizmos()
{

    Vec2 pos = transform()->position();

    Gizmos::SetColor(Color::gray);
    Gizmos::DrawText(pos + Vec2::down / 1.5, "Health " + std::to_string(health) + " Resources " + std::to_string(this->resources));

    Gizmos::DrawCircle(pos, 0.5f);

    Gizmos::SetColor(playerColor);
    for(float &f : damaged_lines)
    {
        Gizmos::DrawCircle(pos, f);
        f += 0.2f;
    }

    if(!damaged_lines.empty())
    {
        if(damaged_lines.back() > 3)
            damaged_lines.pop_back();
    }
}

void SpacePlanet::send_damage(SpacePilot *p)
{
    if(!--health)
    {
        this->gameObject()->Destroy();
        return;
    }
    p->gameObject()->Destroy();
    damaged_lines.emplace_back(0);
}

void SpacePilot::OnUpdate()
{

    //    if(!owner_planet->exists() || !owner_planet->target->exists())
    //        return;

    Vec2 p = transform()->position();
    if(p != toWarn)
    {
        transform()->LookAt(toWarn);
        transform()->position(Vec2::MoveTowards(p, toWarn, TimeEngine::deltaTime() * speed));
    }
    else if(toWarn != (p = owner_planet->target->transform()->position()))
    {
        toWarn = p;
    }
    else /*if (Vec2::distance_sqr(p, toWarn) <= 10)*/
    {
        // destroy and decrement 1 health
        owner_planet->target->send_damage(this);
    }
}

// worker
void SpaceWorker::OnUpdate()
{
    enum
    {
        GetResources = 0,
        SetResources
    };

    // if(!owner_planet->exists())
    //    return;
    Vec2 target;
    Vec2 tp = transform()->position();
    switch(switch_state)
    {
        case GetResources:
            target = owner_planet->resource_plant->transform()->position();
            if(tp == target)
            {
                if(resources == maximumResources)
                {
                    last_upload = 0;
                    // resource to planet
                    switch_state = SetResources;
                }
                else if(TimeEngine::time() > last_upload)
                {
                    resources++; // get any more
                    last_upload = TimeEngine::time() + delay_load / maximumResources;
                }
            }

            break;
        case SetResources:
            target = owner_planet->transform()->position();
            if(tp == target)
            {
                if(resources == 0)
                {
                    switch_state = GetResources;
                }
                else if(TimeEngine::time() > last_upload)
                {
                    this->owner_planet->resources++;
                    --resources;
                    last_upload = TimeEngine::time() + delay_load / maximumResources;
                }
            }
            break;
        default:
            break;
    }
    // rotate to
    transform()->LookAt(target);
    // moving to
    transform()->position(Vec2::MoveTowards(tp, target, TimeEngine::deltaTime() * speed));
}

void SpaceWorker::OnGizmos()
{

    Vec2 pos = transform()->position() + Vec2::down / 4;
    Vec2 size {.5, .06f};

    Gizmos::SetColor(Color::gray);
    Gizmos::DrawRectangleRounded(pos, size.x, size.y, 5);
    if(resources)
        Gizmos::DrawFillRectRounded(pos, Math::Map<float>(resources, 0, maximumResources, 0, size.x), size.y / 2, 5);
}
