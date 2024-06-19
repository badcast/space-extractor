#include "WGame.hpp"

using namespace RoninEngine::Runtime;

WGame *WGame::current = nullptr;

extern ParticleSystem *putEnemyParticleExplode(Vec2 position);

void draw_ready_go();

void WGame::OnUnloading()
{
    current = nullptr;
    activeEnemies.clear();
    RoninMemory::free(navMesh);
}

void WGame::OnAwake()
{
    current = this;
    RoninCursor::SetCursor(AssetManager::ConvertImageToCursor(assets.gameSprites->GetSprite("cursor-target")->getImage(), {16, 16}));
    RoninMemory::alloc_self(navMesh, 1000, 1000);

    ivstars.set(Vec2::up_right, 1.0f, 100, true);
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
        spriteAnim->animationSpeed = 0.085f;
        spriteAnim->startSprite = i * 1.f / nn;
        spriteAnim->SetSprites(assets.asteroids->GetAtlasObject()->GetSprites());
        spriteAnim->spriteRenderer()->setSize(Vec2::one / 2);

        constexpr float perf = .5f;
        spriteAnimatorObject->transform()->position(Vec2 {i * perf - nn / 2 * perf, 1});
    }
    // Set Cursor
    GameSetCursor(PlayerCursor::CusrorTargetAnime);

    // Create Player
    player = Primitive::CreateEmptyGameObject()->AddComponent<Player>();
    player->gameObject()->name("Player");
    player->transform()->position(Camera::ViewportToWorldPoint(Vec2 {0.5, 0.15}));

    // Background
    SpriteRenderer *spriteRender = Primitive::CreateEmptyGameObject()->AddComponent<SpriteRenderer>();
    spriteRender->setSprite(assets.gameSprites->GetSprite("main-menu-background"));
    spriteRender->transform()->zOrder(RenderOrders::BackgroundOrder);

    ParticleSystem *smoke_particle = Primitive::CreateEmptyGameObject()->AddComponent<ParticleSystem>();
    smoke_particle->gameObject()->name("Particle Smoke");
    smoke_particle->rotate = false;
    smoke_particle->loop = true;
    smoke_particle->speed = 2;
    smoke_particle->direction = Vec2::left;
    smoke_particle->interval = 3;
    smoke_particle->setSource(assets.gameSprites->GetSprite("smoke"));
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

    MusicPlayer::setClip(Resources::GetMusicClipSource(Resources::LoadMusicClip(Paths::GetRuntimeDir() + "/data/music/ambient-1.ogg", true)));
    MusicPlayer::Play();

    enhancer.generateWave(5, 10);
}

void WGame::OnUpdate()
{
    if(Input::GetKeyDown(KeyboardCode::KB_ESCAPE))
        RoninSimulator::RequestQuit();

    ivstars.play();

    enhancer.doWave();

    if(Input::GetMouseDown(MouseMiddle))
        player->transform()->position(Camera::ScreenToWorldPoint(Input::GetMousePointf()));

    if(Input::GetKeyDown(KeyboardCode::KB_ESCAPE))
        RoninSimulator::RequestQuit();

    if(Input::GetMouseUp(MouseButton::MouseRight))
    {
        float step = 0;
        Vec2 lfs = Camera::ViewportToWorldPoint({0,1});
        Vec2 rfs = Camera::ViewportToWorldPoint({1,1});
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
}

void WGame::OnGizmos()
{
    // DRAW WAVE TIME
    if(enhancer.state() == EhState::Delay)
    {
        std::string str {"Left time is next wave: "};
        str += std::to_string(enhancer.after());
        str += "\nEstimated next wars: ";
        str += std::to_string(enhancer.activeWaveInfo().enemies);

        RenderUtility::DrawTextLegacy(Vec2::zero, str, true);
    }
}

WGame::WGame() : World("Space Extractor"), navMesh(nullptr), player(nullptr)
{
}

void draw_ready_go()
{

    // if(timeoutReadyGo > Time::time())
    //     return;

    // if(sprite == nullptr)
    // {
    //     sprite = assets.gameSprites->GetSprite("game-ready-go");
    // }

    // RenderUtility::DrawSpriteToScreen(sprite, {0, 0, 1000, 1000});
}
