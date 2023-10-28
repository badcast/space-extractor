#include "WGame.hpp"

Asset *WGame::spriteAsset = nullptr;
Asset *WGame::soundAsset = nullptr;
WGame *WGame::current = nullptr;
constexpr int enemyPer = 15;
void make_simple_enemy();

void WGame::OnUnloading()
{
    current = nullptr;
    //    spriteAsset = nullptr;
    //    soundAsset = nullptr;

    enemies.clear();
    RoninMemory::free(navMesh);
}

void WGame::OnAwake()
{
    current = this;
    // Load resources

    std::string datadir = Path::app_dir() + "data" + Path::GetPathSeperatorOS();
    std::string p = datadir + "resources.json";
    if(spriteAsset == nullptr && !AssetManager::LoadAsset(p, &spriteAsset))
    {
        RoninSimulator::ShowMessageFail("Failed load asset " + p);
    }
    p = datadir + "sounds.json";
    if(soundAsset == nullptr && !AssetManager::LoadAsset(p, &soundAsset))
    {
        RoninSimulator::ShowMessageFail("Failed load asset " + p);
    }
    SetCursor(AssetManager::ConvertImageToCursor(spriteAsset->GetImage("cursor-target"), {16, 16}));

    RoninMemory::alloc_self(navMesh, 1000, 1000);
}

void WGame::OnStart()
{
    // Create Main Camera
    Primitive::CreateCamera2D();

    // Create Player
    player = Primitive::CreateEmptyGameObject()->AddComponent<Player>();
    player->gameObject()->name("Player");
    player->transform()->position(Camera::ViewportToWorldPoint(Vec2 {0.5, 0.85}));

    // Background
    SpriteRenderer *spriteRender = Primitive::CreateEmptyGameObject()->AddComponent<SpriteRenderer>();
    spriteRender->setSprite(Primitive::CreateSpriteFrom(spriteAsset->GetImage("main-menu-background")));
    spriteRender->transform()->layer(-100);

    ParticleSystem *smoke_particle = Primitive::CreateEmptyGameObject()->AddComponent<ParticleSystem>();
    smoke_particle->gameObject()->name("Particle Smoke");
    smoke_particle->rotate = false;
    smoke_particle->speed = 3;
    smoke_particle->direction = Vec2::left;
    smoke_particle->interval = 1.3f;
    smoke_particle->setSource(Primitive::CreateSpriteFrom(spriteAsset->GetImage("smoke")));
    smoke_particle->setLimit(15);
    smoke_particle->setInterpolates(15);
    smoke_particle->setSize(Vec2::one * 4);
    smoke_particle->setColors(Color::transparent, {Color::white, 50}, Color::transparent);
    smoke_particle->transform()->position(Camera::ViewportToWorldPoint({1, 0.5f}));
}

void WGame::OnUpdate()
{
    if(Input::GetKeyDown(KeyboardCode::KB_ESCAPE))
        RoninSimulator::RequestQuit();
    if(TimeEngine::frame() % 45 == 0)
    {
        make_simple_enemy();
    }

    if(Input::GetMouseDown(MouseMiddle))
        RoninSimulator::ReloadWorld();
}

void WGame::OnGizmos()
{
}

void make_simple_enemy()
{
    float offset_up = -0.05f;
    Vec2 s0 = Camera::ViewportToWorldPoint({0.03f, offset_up});
    Vec2 s1 = Camera::ViewportToWorldPoint(Vec2::right);
    s1.x -= s0.x;
    s1.y = s0.y;
    float off = (s1.x - s0.x) / enemyPer;
    for(int x = 0; x < enemyPer; ++x)
    {
        EKamikadze *kamikadze = Primitive::CreateEmptyGameObject()->AddComponent<EKamikadze>();
        kamikadze->gameObject()->name("EKamikadze");
        Collision *collision = kamikadze->AddComponent<Collision>();
        collision->targetLayer = static_cast<int>(Layers::PlayerOrBullet);

        kamikadze->AddOnDestroy(
            [](Component *self)
            {
                AudioClip *clip = WGame::current->soundAsset->GetAudioClip("space-explode");
                AudioSource::PlayClipAtPoint(clip, self->transform()->position(), 0.3f);
            });

        WGame::current->enemies.push_back(kamikadze);

        s0.y = Random::Range(s1.y, s1.y * 2);
        kamikadze->transform()->position(s0);
        kamikadze->gameObject()->SetLayer(static_cast<int>(Layers::EnemyClass));
        s0.x += off;
    }
}
