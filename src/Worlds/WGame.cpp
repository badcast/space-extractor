#include "WGame.hpp"

using namespace RoninEngine::Runtime;

WGame *WGame::current = nullptr;

struct GameSession
{
    int waves;
    int wavesN;
    int enemyPer = 133;
    float delayed_nextWave = 0;
} session;

extern ParticleSystem *putEnemyParticleExplode(Vec2 position);

void make_simple_enemy();

void WGame::OnUnloading()
{
    current = nullptr;
    enemies.clear();
    RoninMemory::free(navMesh);
}

void WGame::OnAwake()
{
    current = this;
    RoninCursor::SetCursor(AssetManager::ConvertImageToCursor(assets.gameSprites->GetImage("cursor-target"), {16, 16}));
    RoninMemory::alloc_self(navMesh, 1000, 1000);
}

void WGame::OnStart()
{
    // Create Main Camera
    Primitive::CreateCamera2D();

    // Generate Custom Sprite Animator
    constexpr int nn = 90;
    for(int i = 0; i < nn; i++)
    {
        GameObject *spriteAnimatorObject = Primitive::CreateEmptyGameObject();
        spriteAnimatorObject->AddComponent<SpriteRenderer>();
        SpriteAnimator *spriteAnim = spriteAnimatorObject->AddComponent<SpriteAnimator>();
        spriteAnim->animationSpeed = 0.1f;
        spriteAnim->startSprite = i*1.f /nn;
        spriteAnim->SetSprites(assets.asteroids->GetAtlasObject()->GetSprites());
        spriteAnim->spriteRenderer()->setSize(Vec2::one / 2);

        constexpr float perf = .5f;
        spriteAnimatorObject->transform()->position(Vec2 {i * perf-nn/2*perf, 1});
    }
    // Set Cursor
    GameSetCursor(PlayerCursor::CusrorTargetAnime);

    // Create Player
    player = Primitive::CreateEmptyGameObject()->AddComponent<Player>();
    player->gameObject()->name("Player");
    player->transform()->position(Camera::ViewportToWorldPoint(Vec2 {0.5, 0.85}));

    // Background
    SpriteRenderer *spriteRender = Primitive::CreateEmptyGameObject()->AddComponent<SpriteRenderer>();
    spriteRender->setSprite(Primitive::CreateSpriteFrom(assets.gameSprites->GetImage("main-menu-background")));
    spriteRender->transform()->zOrder(RenderOrders::BackgroundOrder);

    ParticleSystem *smoke_particle = Primitive::CreateEmptyGameObject()->AddComponent<ParticleSystem>();
    smoke_particle->gameObject()->name("Particle Smoke");
    smoke_particle->rotate = false;
    smoke_particle->loop = true;
    smoke_particle->speed = 1;
    smoke_particle->direction = Vec2::left;
    smoke_particle->interval = 3;
    smoke_particle->setSource(Primitive::CreateSpriteFrom(assets.gameSprites->GetImage("smoke")));
    smoke_particle->setInterpolates(10);
    smoke_particle->setSize(Vec2::one * 4);
    smoke_particle->setColors({Color::white, 0}, {Color::white, 128}, {Color::white, 0});
    smoke_particle->transform()->position(Camera::ViewportToWorldPoint({1, 0.5f}));
    smoke_particle->transform()->zOrder(RenderOrders::ParticleMainSmokeOrder);

    // Background music
    AudioSource *aus = Camera::mainCamera()->AddComponent<AudioSource>();
    aus->setClip(Resources::GetAudioClipSource(Resources::LoadAudioClip(Paths::GetRuntimeDir() + "/data/music/ambient-2.ogg5", true)));
    aus->setVolume(0.3f);
    aus->Play();

    MusicPlayer::setClip(
        Resources::GetMusicClipSource(Resources::LoadMusicClip(Paths::GetRuntimeDir() + "/data/music/ambient-1.ogg", true)));
    MusicPlayer::Play();

    session.waves = 1;
    session.wavesN = 0;
}

void WGame::OnUpdate()
{
    if(Input::GetKeyDown(KeyboardCode::KB_ESCAPE))
        RoninSimulator::RequestQuit();

    make_simple_enemy();

    if(Input::GetMouseDown(MouseMiddle))
        player->transform()->position(Camera::ScreenToWorldPoint(Input::GetMousePointf()));

    if(Input::GetKeyDown(KeyboardCode::KB_ESCAPE))
        RoninSimulator::RequestQuit();

    if(Input::GetMouseUp(MouseButton::MouseRight))
    {
        float step = 0;
        Vec2 lfs = Camera::ViewportToWorldPoint(Vec2::zero);
        Vec2 rfs = Camera::ViewportToWorldPoint(Vec2::one);
        for(Transform *enemy : Physics2D::GetRectangleCast(lfs + rfs, lfs - rfs, GameLayers::EnemyClass))
        {
            if(enemy->GetComponent<Enemy>())
            {
                enemy->gameObject()->Destroy(step);
                step += 1.f / 100;
            }
        }
    }

    Camera::mainCamera()->transform()->Translate(Input::GetAxis() * 0.1f);

    if(Time::frame() + 1 % 120 == 0)
    {
        RoninSettings rs;
        RoninSimulator::GetSettings(&rs);

        rs.selectTextureQuality = (int) !rs.selectTextureQuality;
        RoninSimulator::SetSettings(&rs);
    }
}

void WGame::OnGizmos()
{
    // DRAW WAVE TIME
    if(session.delayed_nextWave)
    {
        std::string str {"Left time is next wave: "};
        str += std::to_string(session.delayed_nextWave - Time::time());
        str += "\nEstimated next wars: ";
        str += std::to_string(session.waves * session.enemyPer);

        RenderUtility::DrawTextLegacy(Vec2::zero, str, true);
    }
}

float atime = 0;
void make_simple_enemy()
{
    float off, offset_up = -0.05f;
    Vec2 s0, s1;

    int maxCount = session.waves * session.enemyPer;
    int empty = WGame::current->enemies.size();
    if(maxCount == session.wavesN || session.delayed_nextWave > Time::time())
    {
        if(empty == 0)
        {
            if(session.wavesN == maxCount)
            {
                atime = 0;
                session.waves++; // Push next wave
                session.wavesN = 0;
                session.delayed_nextWave = Time::time() + 5; // wait 3 sec
            }
        }
    }
    else if(session.wavesN < maxCount && Time::time() > atime && empty < 2)
    {
        session.delayed_nextWave = 0;
        s0 = Camera::ViewportToWorldPoint({0.03f, offset_up});
        s1 = Camera::ViewportToWorldPoint(Vec2::right);
        s1.x -= s0.x;
        s1.y = s0.y;
        off = (s1.x - s0.x) / session.enemyPer;
        int count = Math::Min<int>(maxCount - session.wavesN, Math::Ceil(session.enemyPer * 0.1f));

        for(int x = 0; x < count; ++x)
        {
            EKamikadze *kamikadze = Primitive::CreateEmptyGameObject()->AddComponent<EKamikadze>();
            kamikadze->gameObject()->name("EKamikadze");
            Collision *collision = kamikadze->AddComponent<Collision>();
            collision->targetLayer = static_cast<int>(GameLayers::PlayerOrBullet);
            // Om Collision
            kamikadze->AddOnDestroy(
                [](Component *self)
                {
                    AudioClip *clip = assets.gameSounds->GetAudioClip("space-explode");
                    AudioSource::PlayClipAtPoint(clip, self->transform()->position(), 0.2f);
                    WGame::current->enemies.erase(self->GetComponent<Enemy>());
                    if(self->GetComponent<Enemy>()->healthPoint)
                        putEnemyParticleExplode(self->transform()->position());
                });

            WGame::current->enemies.insert(kamikadze);

            s0.y = Random::Range(s1.y, s1.y * 2);
            kamikadze->transform()->position(s0);
            kamikadze->gameObject()->SetLayer(static_cast<int>(GameLayers::EnemyClass));
            s0.x += off;
        }
        session.wavesN += count;
        atime = Time::time() + 2;
    }
}
