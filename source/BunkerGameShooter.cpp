
#include "SpaceMainMenu.h"

using namespace RoninEngine::Runtime;

constexpr float bullet_destroy_after = 4; // seconds

float bullet_threshold = 3;
float bullet_delay_shot = 0.35;
float bullet_speed = 12;
float turret_rotate_speed = 1;

uid sldr_thrsh;
uid sldr_delay_shot;
uid sldr_speed;
uid sldr_turret_rot;

GameObject* target;
float target_health;
float target_health_max = 100;
Camera2D* camera;
AudioClip *gunClip, *hitClip;
bool auto_target = true;

void __sel_tar(uid id, void*)
{
    auto_target = !auto_target;
    std::string s = BunkerWorld::self()->get_gui()->get_text(id);
    s[1] = (auto_target ? '+' : '-');
    BunkerWorld::self()->get_gui()->set_text(id, s);
}

void BunkerWorld::on_start()
{
    switch_game_level(this);

    Vec2Int _u = { 0, 90 };
    Vec2Int of { 0, 18 };
    get_gui()->push_button("[+] Авто-атака", { _u, { 150, 16 } }, (ui_callback*)&__sel_tar);
    _u += of;
    sldr_thrsh = get_gui()->push_slider(bullet_threshold, 1, 100, _u);
    _u += of;
    get_gui()->push_label("Разброс пули", _u);
    _u += of;
    sldr_delay_shot = get_gui()->push_slider(0.35, 0, 1, _u);
    _u += of;
    get_gui()->push_label("Задержка выстрела", _u);
    _u += of;
    sldr_speed = get_gui()->push_slider(12, 1, 40, _u);
    _u += of;
    get_gui()->push_label("Скорость пуль", _u);
    _u += of;
    sldr_turret_rot = get_gui()->push_slider(1, 0, 10, _u);
    _u += of;
    get_gui()->push_label("Скорость поворота", _u);

    camera = Primitive::create_camera2D();
    camera->game_object()->add_component<MoveController2D>();
    //    camera->visibleGrids = true;
    AudioSource* aus = camera->game_object()->add_component<AudioSource>();

    gunClip = Resources::load_clip("./data/gun-thud.wav");
    hitClip = Resources::load_clip("./data/hit-impact.wav");

    aus->clip(gunClip);

    constexpr int max_tur = 4;
    Random::srand(43690);
    for (int x = 0; x < max_tur; ++x) {
        GameObject* turret = Primitive::create_empty_game_object();
        turret->add_component<SpriteRenderer>()->set_sprite(Primitive::create_sprite2D_triangle(Vec2::half / 2, 1, Color::red));
        turret->transform()->position(Camera::viewport_to_world(Vec2::one * Random::value())); // set position to Turret
        aus = turret->add_component<AudioSource>();
        aus->clip(gunClip);
        aus->volume(0.1f);
        turrets.push_back(turret->transform());
    }

    bulTempl = Primitive::create_empty_game_object();
    bulTempl->name("bullet");
    bulTempl->add_component<SpriteRenderer>()->set_sprite(Primitive::create_sprite2D_box({ 0.01f, 0.2f }, Color::aqua));
    bulTempl->set_active(false);

    target_health = target_health_max;
    target = Primitive::create_box2d(Vec2::one * 1.5f, 230, Color::white);
    target->get_sprite_renderer()->size /= 2;
    aus = target->add_component<AudioSource>();
    aus->clip(hitClip);
    aus->volume(0.3f);
}

void BunkerWorld::on_update()
{
    Vec2 ms { Camera::screen_to_world(Input::get_mouse_pointf()) };
    Vec2 __target = auto_target ? target->transform()->position() : ms;
    bullet_delay_shot = *(float*)get_gui()->get_resources(sldr_delay_shot);
    bullet_speed = *(float*)get_gui()->get_resources(sldr_speed);
    turret_rotate_speed = *(float*)get_gui()->get_resources(sldr_turret_rot);

    for (Transform* turret : turrets) {
        turret->look_at_lerp(__target, TimeEngine::deltaTime() * turret_rotate_speed);
    }
    // remove destroyed from list
    bullets.remove_if([](Transform* each) { return !instanced(each); });

    for (Transform* bul : bullets) {
        Vec2 bulPos = bul->position();
        Vec2 targetPos = bul->forward(bullet_speed);

        bulPos = Vec2::move_towards(bulPos, targetPos, TimeEngine::deltaTime() * bullet_speed);
        bul->position(bulPos);
    }

    if (last_time < TimeEngine::time()) /*Input::is_mouse_down()*/
    {
        last_time = bullet_delay_shot + TimeEngine::time();

        bullet_threshold = *(float*)get_gui()->get_resources(sldr_thrsh);
        for (Transform* turret : turrets) {

            if (!turret->look_of_angle(__target, 10))
                continue;
            turret->game_object()->get_component<AudioSource>()->play();
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
        p = Random::random_vector() * 2;

    target->transform()->look_at(p);
}

constexpr float hp_width = 0.2f;
void BunkerWorld::on_gizmo()
{
    Vec2 ms = Camera::screen_to_world(Input::get_mouse_pointf());
    if (!auto_target) {
        Gizmos::draw_position(ms, 0.1f);
        Gizmos::draw_circle(ms, 0.1f);
    }
    /*
        for (auto t : turrets)
            Gizmos::draw_arrow(t->position(), t->forward(), 0.3f);
    */
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
