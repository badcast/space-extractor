#include "SpaceMainMenu.h"

using namespace RoninEngine::Runtime;

constexpr float bullet_destroy_after = 10; // seconds

float bullet_threshold = 3;
float bullet_delay_shot = 0.35;
float bullet_speed = 12;
float turret_rotate_speed = 5;
bool auto_target = true;

uid sldr_thrsh;
uid sldr_delay_shot;
uid sldr_speed;
uid sldr_turret_rot;

GameObject *target;
float target_health;
float target_health_max = 1000;
Camera2D *camera;
AudioClip *gunClip, *hitClip, *destroyedClip;
AudioSource *destroyed_audio;

Image *muzzle_surface;

bool turret_attacking[4];

void BunkerWorld::OnStart()
{

    switch_game_level(this);

    Vec2Int _u = {0, 90};
    Vec2Int of {0, 18};
    GetGUI()->PushButton(
        "[+] Авто-атака",
        {_u, {150, 16}},
        [](uid id)
        {
            auto_target = !auto_target;
            std::string s = BunkerWorld::self()->GetGUI()->ElementGetText(id);
            s[1] = (auto_target ? '+' : '-');
            BunkerWorld::self()->GetGUI()->ElementSetText(id, s);
        });
    _u += of;
    sldr_thrsh = GetGUI()->PushSlider(bullet_threshold, 1, 100, _u);
    _u += of;
    GetGUI()->PushLabel("Разброс пули", _u);
    _u += of;
    sldr_delay_shot = GetGUI()->PushSlider(0.35, 0, 1, _u);
    _u += of;
    GetGUI()->PushLabel("Задержка выстрела", _u);
    _u += of;
    sldr_speed = GetGUI()->PushSlider(12, 1, 40, _u);
    _u += of;
    GetGUI()->PushLabel("Скорость пуль", _u);
    _u += of;
    sldr_turret_rot = GetGUI()->PushSlider(turret_rotate_speed, 0, 10, _u);
    _u += of;
    GetGUI()->PushLabel("Скорость поворота", _u);

    camera = Primitive::CreateCamera2D();
    camera->gameObject()->AddComponent<MoveController2D>();
    //    camera->visibleGrids = true;
    AudioSource *aus = camera->gameObject()->AddComponent<AudioSource>();

    gunClip = Resources::GetAudioClipSource(Resources::LoadAudioClip("./data/gun-thud.wav", true));
    hitClip = Resources::GetAudioClipSource(Resources::LoadAudioClip("./data/hit-impact.wav", true));
    destroyedClip = Resources::GetAudioClipSource(Resources::LoadAudioClip("./data/destroyed.mp3", true));
    muzzle_surface = Resources::GetImageSource(Resources::LoadImage("./data/muzle.png", true));
    aus->setClip(gunClip);

    constexpr int max_tur = 4;
    std::vector<Vec2> pos {{-1.f, 1.f}, {-1.0, -1.0}, {1.0, 1.0}, {1.0, -1.0}};
    for(int x = 0; x < max_tur; ++x)
    {
        GameObject *turret = Primitive::CreateEmptyGameObject();
        turret->AddComponent<SpriteRenderer>()->setSprite(Primitive::CreateSpriteTriangle(true, Vec2::half / 2, 1, Color::red));
        turret->transform()->position(pos[x] * 2); // set position to Turret
        aus = turret->AddComponent<AudioSource>();
        aus->setClip(gunClip);
        aus->setVolume(0.01f);
        turrets.push_back(turret->transform());
    }

    bulTempl = Primitive::CreateEmptyGameObject();
    bulTempl->name("bullet");
    bulTempl->AddComponent<SpriteRenderer>()->setSprite(Primitive::CreateSpriteRectangle(true, Vec2{0.01f, 0.2f}, Color::yellow));
    bulTempl->SetActive(false);

    target_health = target_health_max;
    target = Primitive::CreateBox2D(Vec2::one * 1.5f, 230, Color::white);
    target->spriteRenderer()->setSize( target->spriteRenderer()->getSize() / 2);
    aus = target->AddComponent<AudioSource>();
    aus->setClip(hitClip);
    aus->setVolume(0.05f);

    destroyed_audio = target->AddComponent<AudioSource>();
    destroyed_audio->setClip(destroyedClip);
    destroyed_audio->setVolume(0.5f);

    last_time = 0;

    RoninSettings asd;
    RoninSimulator::GetSettings(&asd);

    RoninSimulator::SetSettings(&asd);
}

void BunkerWorld::OnUpdate()
{
    Vec2 ms {Camera::ScreenToWorldPoint(Input::GetMousePointf())};
    Vec2 __target = auto_target ? target->transform()->position() : ms;
    bullet_delay_shot = GetGUI()->SliderGetValue(sldr_delay_shot);
    bullet_speed = GetGUI()->SliderGetValue(sldr_speed);
    turret_rotate_speed = GetGUI()->SliderGetValue(sldr_turret_rot);

    for(Transform *turret : turrets)
    {
        turret->LookAtLerp(__target, TimeEngine::deltaTime() * turret_rotate_speed);
    }
    // remove destroyed from list
   // bullets.remove_if([](Transform *each) { return !Instanced(each); });

    for(Transform *bul : bullets)
    {
        Vec2 bulPos = bul->position();
        Vec2 targetPos = bul->forward() * bullet_speed;

        bulPos = Vec2::MoveTowards(bulPos, targetPos, TimeEngine::deltaTime() * bullet_speed);
        bul->position(bulPos);
    }

    if(last_time < TimeEngine::time())
    {
        last_time = bullet_delay_shot + TimeEngine::time();

        bullet_threshold = GetGUI()->SliderGetValue(sldr_thrsh);
        int x = 0;
        for(Transform *turret : turrets)
        {
            if(!(turret_attacking[x++] =
                     turret->LookOfAngle(__target, 10) && Vec2::Distance(turret->position(), target->transform()->position()) < 6))
                continue;
            turret->gameObject()->GetComponent<AudioSource>()->Play();
            Vec2 origin = turret->transform()->position();
            Vec2 dir = turret->transform()->forward();
            dir += Random::RandomVector() / 1000 * bullet_threshold;
            GameObject *bul = Instantiate(bulTempl, origin);
            bul->transform()->LookAt(dir);
            bullets.emplace_back(bul->transform());
            bul->Destroy(bullet_destroy_after);
        }
    }

    // receive damage
    std::list<Transform *> accept = Physics2D::GetCircleCast(target->transform()->position(), 0.3);
    bool accepted = false;
    for(Transform *t : accept)
    {
        if(t->gameObject()->name().find_first_of("bullet"))
        {
            continue;
        }

        t->gameObject()->Destroy();
        accepted = true;
        if(target_health-- == 0)
        {
            target_health = target_health_max;
            destroyed_audio->Play();

            // BUG: muzzle auto moved to any pos.
            GameObject *muzzle = Primitive::CreateEmptyGameObject(target->transform()->position());
            SpriteRenderer *spr = muzzle->AddComponent<SpriteRenderer>();
            spr->setSprite(Primitive::CreateSpriteFrom(muzzle_surface));
            spr->setSize(spr->getSize() / 2);
            Destroy(muzzle, 1);
            target->transform()->position(Vec2::right * 10);
            break;
        }
    }
    if(accepted && !target->GetComponent<AudioSource>()->isPlaying())
        target->GetComponent<AudioSource>()->Play();

    static Vec2 p = Vec2::minusOne;
    Vec2 af = Vec2::MoveTowards(target->transform()->position(), p, TimeEngine::deltaTime());
    target->transform()->position(af);

    if(af == p)
        p = Random::RandomVector() * 2;

    target->transform()->LookAt(p);
}

constexpr float hp_width = 0.2f;
void BunkerWorld::OnGizmos()
{
    Vec2 ms = Camera::ScreenToWorldPoint(Input::GetMousePointf());
    if(!auto_target)
    {
        Gizmos::DrawPosition(ms, 0.1f);
        Gizmos::DrawCircle(ms, 0.1f);
    }
    /*
        for (auto t : turrets)
            Gizmos::draw_arrow(t->position(), t->forward(), 0.3f);
    */
    Vec2 hp_offset = target->spriteRenderer()->getSize() + target->transform()->position() + Vec2 {-0.5f, -1};

    Gizmos::SetColor(Color::green);
    float progress = Math::Map<float>(target_health, 0, target_health_max, -hp_width, hp_width);
    // draw green left to right side  left *****---- right
    Gizmos::DrawLine(hp_offset + Vec2::left * hp_width, hp_offset + Vec2::right * progress);
    if(target_health < target_health_max)
    {
        Gizmos::SetColor(Color::red);
        // draw red right to left side left **------ right
        Gizmos::DrawLine(hp_offset + Vec2::right * progress, hp_offset + Vec2::right * hp_width);
    }

    int x = 0;
    bool any = false;
    for(Transform *turret : turrets)
    {
        if(turret_attacking[x++])
        {
            Gizmos::DrawTextLegacy(turret->position() + Vec2::down / 4, "!");
            any = true;
        }
    }

    if(any)
    {
        Gizmos::DrawTextLegacy(target->transform()->position() + Vec2::down / 3 + Vec2::left / 4, "HELP!");
    }
}

void BunkerWorld::OnUnloading()
{
    bullets = {};
    turrets = {};
    target = nullptr;
}
