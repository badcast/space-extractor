#include "space_planet.h"

SpacePlanet::SpacePlanet()
    : Behaviour("SpacePlanet")
{
}

void fabricate(SpacePlanet* planet, SpacePilot* reference, int countCell)
{
    if (countCell < 1)
        return;
    for (int x = 0; x < countCell; ++x) {
        SpacePilot* pilot = instantiate(reference->game_object(), planet->transform()->position())->add_component<SpacePilot>();
        pilot->owner_planet = planet;
        pilot->toWarn = Vec2::rotate_around(planet->transform()->position(), Vec2::one, Random::randomAngle() * Math::deg2rad);
    }
}

void SpacePlanet::OnStart()
{

    // find target
    std::list<SpacePlanet*> planets = Level::self()->find_objects_with_type<SpacePlanet>();
    planets.remove(this);

    if (planets.empty()) {
        return;
    }
    Color playerColor = Random::value() >= 0.3 ? Color::blue : Color::red;

    target = planets.front();

    referencePilot = Primitive::create_empty_game_object(this->transform()->position())->add_component<SpacePilot>();
    SpriteRenderer* lastSpr = referencePilot->game_object()->add_component<SpriteRenderer>();
    lastSpr->set_sprite(Primitive::create_sprite2D_triangle(Vec2::one, 1.f, playerColor));
    lastSpr->size /= 12;
    referencePilot->game_object()->set_active(false);
}

void SpacePlanet::OnUpdate()
{
    if (lastTimeFab > TimeEngine::time()) {
        return;
    }
    lastTimeFab = TimeEngine::time() + 3;
    fabricate(this, referencePilot, startWith);
}

void SpacePlanet::OnGizmos()
{
    Vec2 pos = transform()->position();

    Gizmos::set_color(Color::gray);
    Gizmos::draw_text(pos + Vec2::down / 1.5, "Health " + std::to_string(health));

    Gizmos::draw_circle(pos, 0.5f);
}

void SpacePlanet::damage(SpacePilot* p)
{
    if (!--health)
        this->game_object()->destroy();
    p->game_object()->destroy();
}

void SpacePilot::OnUpdate()
{
    Vec2 p = transform()->position();
    if (p != toWarn) {
        transform()->position(Vec2::move_towards(p, toWarn, TimeEngine::deltaTime() * 1));
    } else if (toWarn != (p = owner_planet->target->transform()->position())) {
        toWarn = p;
    } else if (Vec2::distance_sqr(p, toWarn) <= 10) {
        // destroy and decrement 1 health
        owner_planet->target->damage(this);
    }
}
