#include "ComponentSpacePlanet.h"

SpacePlanet::SpacePlanet()
    : Behaviour("SpacePlanet")
{
}

void fabricate(SpacePlanet* planet, SpacePilot* reference, int count)
{
    for (; count--;) {
        SpacePilot* pilot = instantiate(reference->game_object(), planet->transform()->position())->add_component<SpacePilot>();
        pilot->owner_planet = planet;
        pilot->toWarn = Vec2::rotate_around(planet->transform()->position(), Vec2::one, Random::randomAngle() * Math::deg2rad);
    }
}

void SpacePlanet::OnStart()
{
    static bool f = false;
    SpriteRenderer* sprRenderer;
    Sprite* playerSprite;

    f = !f;
    playerColor = f == false ? Color::blue : Color::red;
    playerSprite = Primitive::create_sprite2D_triangle(Vec2::half, 1, playerColor);

    // create resource streams
    sprRenderer = Primitive::create_empty_game_object(transform()->position() + (f ? Vec2::left : Vec2::right) * 4)->add_component<SpriteRenderer>();
    sprRenderer->set_sprite(Primitive::create_sprite2D_triangle(Vec2::half, 1.f, Color::green));
    resource_plant = sprRenderer->game_object()->add_component<SpaceResourcePlant>();

    // create worker for get from resource stream
    SpaceWorker* worker = Primitive::create_empty_game_object(transform()->position())->add_component<SpaceWorker>();

    sprRenderer = worker->game_object()->add_component<SpriteRenderer>();
    sprRenderer->size = Vec2::half;
    sprRenderer->set_sprite(playerSprite);
    worker->owner_planet = this;

    // find target
    std::list<SpacePlanet*> planets = World::self()->find_objects_with_type<SpacePlanet>();
    planets.remove(this);

    if (planets.empty()) {
        return;
    }

    target = planets.front();

    referencePilot = Primitive::create_empty_game_object(this->transform()->position())->add_component<SpacePilot>();
    sprRenderer = referencePilot->game_object()->add_component<SpriteRenderer>();
    sprRenderer->set_sprite(playerSprite);
    sprRenderer->size /= 12;
    referencePilot->game_object()->set_active(false);
}

void SpacePlanet::OnUpdate()
{
    if (!target || lastTimeFab > TimeEngine::time()) {
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

    Gizmos::set_color(Color::gray);
    Gizmos::draw_text(pos + Vec2::down / 1.5, "Health " + std::to_string(health) + " Resources " + std::to_string(this->resources));

    Gizmos::draw_circle(pos, 0.5f);

    Gizmos::set_color(playerColor);
    for (float& f : damaged_lines) {
        Gizmos::draw_circle(pos, f);
        f += 0.2f;
    }

    if (!damaged_lines.empty()) {
        if (damaged_lines.back() > 3)
            damaged_lines.pop_back();
    }
}

void SpacePlanet::send_damage(SpacePilot* p)
{
    if (!--health) {
        this->game_object()->destroy();
        return;
    }
    p->game_object()->destroy();
    if (TimeEngine::frame() % 3 == 0)
        damaged_lines.emplace_back(0);
}

void SpacePilot::OnUpdate()
{

    if (!owner_planet->exists() || !owner_planet->target->exists())
        return;

    Vec2 p = transform()->position();
    if (p != toWarn) {
        transform()->look_at(toWarn);
        transform()->position(Vec2::move_towards(p, toWarn, TimeEngine::deltaTime() * speed));
    } else if (toWarn != (p = owner_planet->target->transform()->position())) {
        toWarn = p;
    } else /*if (Vec2::distance_sqr(p, toWarn) <= 10)*/ {
        // destroy and decrement 1 health
        owner_planet->target->send_damage(this);
    }
}

// worker
void SpaceWorker::OnUpdate()
{
    enum { GetResources = 0, SetResources };

    if (!owner_planet->exists())
        return;
    Vec2 target;
    Vec2 tp = transform()->position();
    switch (switch_state) {
    case GetResources:
        target = owner_planet->resource_plant->transform()->position();
        if (tp == target) {
            if (resources == maximumResources) {
                last_upload = 0;
                // resource to planet
                switch_state = SetResources;
            } else if (TimeEngine::time() > last_upload) {
                resources++; // get any more
                last_upload = TimeEngine::time() + delay_load / maximumResources;
            }
        }

        break;
    case SetResources:
        target = owner_planet->transform()->position();
        if (tp == target) {
            if (resources == 0) {
                switch_state = GetResources;
            } else if (TimeEngine::time() > last_upload) {
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
    transform()->look_at(target);
    // moving to
    transform()->position(Vec2::move_towards(tp, target, TimeEngine::deltaTime() * speed));
}

void SpaceWorker::OnGizmos()
{

    Vec2 pos = transform()->position() + Vec2::down / 4;
    Vec2 size { .5, .06f };

    Gizmos::set_color(Color::gray);
    Gizmos::draw_rectangle_rounded(pos, size.x, size.y, 5);
    if (resources)
        Gizmos::draw_fill_rect_rounded(pos, Math::map<float>(resources, 0, maximumResources, 0, size.x), size.y / 2, 5);
}
