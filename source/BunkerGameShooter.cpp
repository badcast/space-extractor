
#include "SpaceMainMenu.h"

using namespace RoninEngine::Runtime;

constexpr float bullet_speed = 12.f;
constexpr float bullet_delay_shot = 0.1;
constexpr float bullet_destroy_after = 45; // seconds
constexpr float turret_rotate_speed = 3;

int bullet_threshold = 3;
GameObject* target;
uid sldr_thrsh;
float target_health;
float target_health_max = 100;
Camera2D* camera;
AudioClip *gunClip, *hitClip;

void BunkerWorld::on_start()
{
    switch_game_level(this);

    sldr_thrsh = get_gui()->push_slider(bullet_threshold, 1, 100, Vec2Int(0, 280));

    camera = Primitive::create_camera2D();
    camera->game_object()->add_component<MoveController2D>();
    //    camera->visibleGrids = true;
    AudioSource* aus = camera->game_object()->add_component<AudioSource>();

    gunClip = Resources::load_clip("./data/gun-thud.wav");
    hitClip = Resources::load_clip("./data/explosion.wav");

    aus->clip(gunClip);

    constexpr int max_tur = 1;
    Random::srand(43690);
    for (int x = 0; x < max_tur; ++x) {
        GameObject* turret = Primitive::create_empty_game_object();
        turret->add_component<SpriteRenderer>()->set_sprite(Primitive::create_sprite2D_triangle(Vec2::half / 2, 1, Color::red));
        turret->transform()->position(Camera::viewport_to_world(Vec2::one * Random::value())); // set position to Turret
        turret->add_component<AudioSource>()->clip(gunClip);
        turrets.push_back(turret->transform());
    }

    bulTempl = Primitive::create_empty_game_object();
    bulTempl->name("bullet");
    bulTempl->add_component<SpriteRenderer>()->set_sprite(Primitive::create_sprite2D_box({ 0.02f, 0.09f }, Color::yellow));
    bulTempl->set_active(false);

    target_health = target_health_max;
    target = Primitive::create_box2d(Vec2::one * 1.5f, 230, Color::white);
    target->get_sprite_renderer()->size /= 2;
    target->add_component<AudioSource>()->clip(hitClip);
}

void BunkerWorld::on_update()
{
    Vec2 ms { Input::get_mouse_point() };
    for (Transform* turret : turrets) {
        turret->look_at_lerp(Camera::screen_to_world(ms), TimeEngine::deltaTime() * turret_rotate_speed);
    }
    // remove destroyed from list
    bullets.remove_if([](Transform* each) { return !instanced(each); });

    for (Transform* bul : bullets) {
        Vec2 bulPos = bul->position();
        Vec2 targetPos = bul->forward();

        bulPos = Vec2::move_towards(bulPos, targetPos, TimeEngine::deltaTime() * bullet_speed);
        bul->position(bulPos);
    }

    if (last_time < TimeEngine::time() && Input::is_mouse_down()) {
        bullet_threshold = *(float*)get_gui()->get_resources(sldr_thrsh);
        last_time = bullet_delay_shot + TimeEngine::time();
        camera->game_object()->get_component<AudioSource>()->play();
        for (Transform* turret : turrets) {
            Vec2 origin = turret->transform()->position();
            Vec2 dir = turret->transform()->forward();
            dir += Random::random_vector() / 1000 * bullet_threshold;
            GameObject* bul = instantiate(bulTempl, origin);
            bul->transform()->look_at(dir);
            bullets.emplace_back(bul->transform());
            bul->destroy(bullet_destroy_after);
        }
    }

    // receive damage
    std::list<Transform*> accept = Physics2D::sphere_cast(target->transform()->position(), 0.3);
    bool accepted = false;
    for (Transform* t : accept) {
        if (t->game_object()->name().find_first_of("bullet")) {
            continue;
        }

        if (target_health-- == 0)
            target_health = target_health_max;
        t->destroy_cancel();
        t->game_object()->destroy();
        accepted = true;
    }
    if (accepted)
        target->get_component<AudioSource>()->play();

    static Vec2 p = Vec2::minusOne;
    Vec2 af = Vec2::move_towards(target->transform()->position(), p, TimeEngine::deltaTime());
    target->transform()->position(af);

    if (af == p)
        p = Camera::viewport_to_world(Vec2::one * Random::value());

    target->transform()->look_at(p);
}

constexpr float hp_width = 0.2f;
void BunkerWorld::on_gizmo()
{
    Vec2 ms = Camera::screen_to_world(Input::get_mouse_pointf());
    Gizmos::draw_position(ms, 0.1f);
    Gizmos::draw_circle(ms, 0.1f);

    //    for (auto t : turrets)
    //        Gizmos::draw_position(t->back(), 0.3f);

    Vec2 hp_offset = target->get_sprite_renderer()->get_size() + target->transform()->position() + Vec2 { -0.5f, -1 };

    Gizmos::set_color(Color::green);
    float progress = Math::map<float>(target_health, 0, target_health_max, -hp_width, hp_width);
    // draw green left to right side  left *****---- right
    Gizmos::draw_line(hp_offset + Vec2::left * hp_width, hp_offset + Vec2::right * progress);
    if (target_health < target_health_max) {
        Gizmos::set_color(Color::red);
        // draw red right to left side left **------ right
        Gizmos::draw_line(hp_offset + Vec2::right * progress, hp_offset + Vec2::right * hp_width);
    }

   // Gizmos::draw_circle(target->transform()->position(), 0.3);
}
