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

GameObjectRef makeEnemyOnPoint(Vec2 placePoint)
{
    Ref<Collision> collision;
    Ref<EKamikadze> kamikadze;

    kamikadze = Primitive::CreateEmptyGameObject("EKamikadze")->AddComponent<EKamikadze>();
    collision = kamikadze->GetComponent<Collision>();
    if(collision)
        collision->targetLayer = static_cast<int>(GameLayers::PlayerOrBullet);
    // On Collision
    kamikadze->AddOnDestroy(
        [](ComponentRef self)
        {
            AudioClip *clip = globalAssets.gameSounds->GetAudioClip("destroy1");
            AudioSource::PlayClipAtPoint(clip, self->transform()->position(), 0.2f);
            WGame::current->activeEnemies.erase(self->GetComponent<Enemy>());
        });

    kamikadze->transform()->position(placePoint);
    kamikadze->gameObject()->SetLayer(static_cast<int>(GameLayers::EnemyClass));

    WGame::current->activeEnemies.insert(StaticCast<Enemy>(kamikadze));

    return kamikadze->gameObject();
}

void makeMap(Terrain2DRef terrain)
{
    SpriteRef ground1,ground2;
    ground1 = globalAssets.maps->GetSprite("Ground1");
    ground2 = globalAssets.maps->GetSprite("Ground2");
    terrain->setSize({5,5});

   // terrain->setMesh({0,0,1,1}, ground1);
    terrain->setMesh({0,0,1,1}, ground2);
}

void WGame::OnAwake()
{
    current = this;
    SpriteRef sprite;
    sprite = globalAssets.gameSprites->GetSprite("cursor-target");
    if(sprite)
        RoninCursor::SetCursor(AssetManager::ConvertImageToCursor(sprite->getImage(), {16, 16}));
    RoninMemory::alloc_self(navMesh, 1000, 1000);
    terrain = Primitive::CreateEmptyGameObject("Terrain2D")->AddComponent<Terrain2D>();
    terrain->transform()->zOrder(RenderOrders::BackgroundOrder);
    makeMap(terrain);
    ivstars.set(Vec2::up_right, .4f, 220, true);
    enhancer.setDelegate(makeEnemyOnPoint);
}

void WGame::OnStart()
{
    // Create Main Camera
    Primitive::CreateCamera2D()->SetZoomOut(150);

    // Set Cursor
    GameSetCursor(PlayerCursor::CusrorTargetAnime);

    // Create Player
    player = Primitive::CreateEmptyGameObject()->AddComponent<Player>();
    player->gameObject()->name("Player");
    // Set Player to Center position
    player->transform()->position(Vec2::zero);
    player->canClampAngle = false;

    ParticleSystemRef smoke_particle = Primitive::CreateEmptyGameObject()->AddComponent<ParticleSystem>();
    smoke_particle->gameObject()->name("Particle Smoke");
    smoke_particle->rotate = false;
    smoke_particle->loop = true;
    smoke_particle->speed = 2;
    smoke_particle->direction = Vec2::left;
    smoke_particle->interval = 3;
    smoke_particle->setSource(globalAssets.gameSprites->GetSprite("smoke"));
    smoke_particle->setInterpolates(10);
    smoke_particle->setSize(Vec2::one * 4);
    smoke_particle->setColors({Color::white, 0}, {Color::white, 128}, {Color::white, 0});
    smoke_particle->transform()->position(Camera::ViewportToWorldPoint({1, 0.5f}));
    smoke_particle->transform()->zOrder(RenderOrders::ParticleMainSmokeOrder);

    enhancer.generateWave(100, 100, 30);
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
        for(Transform* enemy : Physics2D::GetRectangleCast(lfs + rfs, lfs - rfs, GameLayers::EnemyClass))
        {
            if(enemy->GetComponent<Enemy>())
            {
                enemy->gameObject()->Destroy(step);
                step += 1.f / 100;
            }
        }
    }

    if(Input::GetMouseWheel())
    {
        float newZoom = Camera::mainCamera()->GetComponent<Camera2D>()->GetZoomOut();
        newZoom += Input::GetMouseWheel()*5;
        Camera::mainCamera()->GetComponent<Camera2D>()->SetZoomOut(newZoom);
    }

    Camera::mainCamera()->transform()->Translate(Input::      GetAxis() * Time::deltaTime() * 2);
}

void WGame::OnGizmos()
{
    float time;
    int nbit;

    // DRAW WAVE TIME
    if(enhancer.state() == WaveState::Delay)
    {
        std::string str {"Left time is next wave: "};
        time = enhancer.awaitNextWave();
        nbit = static_cast<int>(str.size());
        nbit |= snprintf(nullptr, 0, "%.1f", time) << 16;
        str.resize(str.size() + (nbit >> 16));
        snprintf(str.data() + (nbit & 0xFFFF), nbit << 16, "%.1f", time);
        str += "\nEstimated next wars: ";
        str += std::to_string(enhancer.getWaveInfo().maxEnemies);

        RenderUtility::DrawTextLegacy(Vec2::zero, str, true);
    }
    else if(enhancer.state() == WaveState::Finish)
    {
        RenderUtility::DrawTextLegacy(Vec2::zero, "FINISH!", true, true);
    }

    RenderUtility::DrawTextLegacy(Vec2::zero, std::to_string(Camera::mainCamera()->GetComponent<Camera2D>()->GetZoomOut()), 1, 1);
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
