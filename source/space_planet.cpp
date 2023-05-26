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
    static bool f = false;
    // find target
    std::list<SpacePlanet*> planets = Level::self()->find_objects_with_type<SpacePlanet>();
    planets.remove(this);

    if (planets.empty()) {
        return;
    }
    playerColor = f == false ? Color::blue : Color::red;
    f = !f;

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

void SpacePlanet::damage(SpacePilot* p)
{
    if (!--health)
        this->game_object()->destroy();
    p->game_object()->destroy();
    if (TimeEngine::frame() % 3 == 0)
        damaged_lines.emplace_back(0);
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
