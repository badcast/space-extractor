#include "Player.hpp"

float lastShotTime;
char bulletFireStep;
float bullet_destroy_after = 4;
Sprite *defaultTurret, *fireTurret;

struct shaking_cam
{
    int force;
    float time;
} shaking;

void Player::OnStart()
{
    SpriteRenderer *sprRender;
    shaking = {0, 0};

    bullets = {};
    lastShotTime = 0;
    bulletFireStep = 0;

    // Load Sources
    Image *srcImagePlayerWeapon = WGame::spriteAsset->GetImage("player-weapon");
    Image *srcImagePlayerPlatform = WGame::spriteAsset->GetImage("player-platform");
    Image *srcImageMuzzleFlash = WGame::spriteAsset->GetImage("muzzle-flash");
    Image *srcImagePlayerShield = WGame::spriteAsset->GetImage("player-shield");
    AudioClip *srcAudioMachineGun = WGame::soundAsset->GetAudioClip("machinegun");

    // Make player weapon object
    weapon = RoninMemory::alloc<WeaponMachineGun>();

    // Audio Player for Machine gun
    playerAudio = gameObject()->AddComponent<AudioSource>();
    playerAudio->setClip(srcAudioMachineGun);

    turretRotatePivot = Primitive::create_empty_game_object()->transform();
    turretRotatePivot->setParent(transform(), false);

    turret = Primitive::create_empty_game_object()->transform();
    turret->layer(1);
    turret->setParent(turretRotatePivot);
    turret->position(Vec2::down * 2.7f);
    sprRender = turret->AddComponent<SpriteRenderer>();
    sprRender->setSprite(defaultTurret = Primitive::create_sprite2d_from(srcImagePlayerWeapon));
    sprRender->setSize(sprRender->getSize() / 4);

    fireTurret = Primitive::create_sprite2d_from(WGame::spriteAsset->GetImage("player-weapon-blob"));

    platform = Primitive::create_empty_game_object()->transform();
    platform->position(transform()->position());
    sprRender = platform->gameObject()->AddComponent<SpriteRenderer>();
    sprRender->setSprite(Primitive::create_sprite2d_from(srcImagePlayerPlatform));
    sprRender->setSize(sprRender->getSize() / 2);

    gunPosition = Primitive::create_empty_game_object()->transform();
    gunPosition->setParent(turret->transform());
    gunPosition->localPosition(Vec2::up * (5 / 10.f));

    gunPoint1 = Primitive::create_empty_game_object(gunPosition->position())->transform();
    gunPoint1->setParent(gunPosition);
    gunPoint1->localPosition(Vec2::left / 10);
    gunPoint2 = Primitive::create_empty_game_object(gunPosition->position())->transform();
    gunPoint2->setParent(gunPosition);
    gunPoint2->localPosition(-gunPoint1->localPosition());

    // Muzzle flash
    muzzleFlash = create_game_object()->transform();
    muzzleFlash->setParent(gunPosition);
    muzzleFlash->position(gunPosition->position());
    muzzleFlash->transform()->angle(0);
    sprRender = muzzleFlash->gameObject()->AddComponent<SpriteRenderer>();
    sprRender->setSprite(Primitive::create_sprite2d_from(srcImageMuzzleFlash));
    sprRender->offset += Vec2::up * 16;
    sprRender->enable(false); // of muzzle flash
    sprRender->setSize(sprRender->getSize() / 4);

    // Player shield
    Transform *playerShieldPivot = Primitive::create_empty_game_object()->transform();
    playerShieldPivot->setParent(transform(), false);

    playerShield = Primitive::create_empty_game_object()->transform();
    playerShield->setParent(playerShieldPivot, false);
    playerShield->localPosition(Vec2::up);
    sprRender = playerShield->AddComponent<SpriteRenderer>();
    sprRender->setSprite(Primitive::create_sprite2d_from(srcImagePlayerShield));
    sprRender->setSize(sprRender->getSize() / 4);
    sprRender->gameObject()->SetActive(false);
    playerShield = playerShieldPivot;
}

void Player::OnUpdate()
{
    // Mouse position to world position
    Vec2 target = Camera::ScreenToWorldPoint(Input::GetMousePointf());

    // Rotate player to Mouse Position
    turretRotatePivot->LookAtLerp(target, TimeEngine::deltaTime() * weapon->rotateSpeed);

    // Clamp angle
    turretRotatePivot->angle(Math::Clamp(turretRotatePivot->angle(), clampAngleLeft, clampAngleRight));

    playerShield->LookAt(target, Vec2::up);

    // Move exists bullets
    for(Transform *bullet : bullets)
    {
        bullet->Translate(bullet->back() * TimeEngine::deltaTime() * weapon->bulletSpeed);
    }

    // Append new bullet on fire
    if(lastShotTime < TimeEngine::time() && Input::GetMouseDown(MouseState::MouseLeft))
    {
        if(playerAudio->isPlaying() == false)
            playerAudio->Play(true);

        Transform *originObject = ((bulletFireStep % 2 == 0) ? gunPoint1 : gunPoint2);
        Vec2 origin = originObject->position();
        if(++bulletFireStep == 2)
            bulletFireStep = 0;

        GameObject *bulletInstance = Instantiate(weapon->bulletPrefab, origin);
        Vec2 direction = originObject->forward() + Random::RandomVector() / 1000 * weapon->bulletThreshold;
        bulletInstance->transform()->LookAt(origin + direction);
        bulletInstance->Destroy(bullet_destroy_after);
        bulletInstance->RegisterOnDestroy([&](GameObject *self) { bullets.erase(self->transform()); });
        Collision *bulletCollision = bulletInstance->AddComponent<Collision>();

        bulletCollision->targetLayer = static_cast<int>(Layers::EnemyOrBullet);

        bulletCollision->onCollision = [&](Collision *self_bullet, Collision *target)
        {
            Enemy *enemy = target->GetComponent<Enemy>();
            if(enemy == nullptr)
                return true;
            enemy->receiveDamage(WGame::current->player->weapon->damage);
            self_bullet->gameObject()->Destroy(); // destroy object
            return false;
        };

        bullets.insert(bulletInstance->transform());

        muzzleFlash->position(origin);
        muzzleFlash->spriteRenderer()->enable(true);

        turret->spriteRenderer()->setSprite(fireTurret);
        turret->spriteRenderer()->flip = bulletFireStep ? SpriteRenderFlip::FlipNone : SpriteRenderFlip::FlipHorizontal;

        // Shaking camera on fire
        shaking.force |= 2;
        shaking.time = TimeEngine::time() + 0.3f;

        // Update last shot time
        lastShotTime = weapon->bulletDelayShot + TimeEngine::time();
    }
    else if(Input::GetMouseUp(MouseState::MouseLeft) == true)
    {
        gameObject()->GetComponent<AudioSource>()->Stop();
        turret->spriteRenderer()->flip = SpriteRenderFlip::FlipNone;
        turret->spriteRenderer()->setSprite(defaultTurret);
        muzzleFlash->spriteRenderer()->enable(false);
        shaking.force = 0;
    }

    if(shaking.time > TimeEngine::time())
        Camera::mainCamera()->transform()->position(Random::RandomVector() * shaking.force / 100);
    else
        Camera::mainCamera()->transform()->position(Vec2::zero); // set default position
}

void Player::applyDamage(int damage)
{
    healthPoint = Math::Max(0, healthPoint - damage);

    shaking.time = TimeEngine::time() + 0.7f;
    shaking.force |= 5;
}

void Player::OnGizmos()
{
    constexpr float size = 0.3f;
    static Vec2 v[3] {{size, size}, {size * 2, size}, {size * 1.5, size * 2}}, cv[3];
    static float angle = 0;

    // rotate
    for(int x = 0; x < 3; ++x)
    {
        Vec2 p = Vec2::RotateAround(gunPosition->position() + v[x], Vec2::up, angle * Math::deg2rad);
        Vec2 n = Vec2::RotateAround(gunPosition->position() + (x == 2 ? v[0] : v[x + 1]), Vec2::up, angle * Math::deg2rad);
        Gizmos::DrawLine(Vec2::Rotate(p, angle * Math::deg2rad), Vec2::Rotate(n, angle * Math::deg2rad));
    }
    angle += 5;

    Gizmos::DrawTextLegacy(Camera::ViewportToWorldPoint(Vec2::zero), "HEALTH: " + std::to_string(healthPoint));
}

void Player::OnDestroy()
{
    // destroy
    RoninMemory::free(weapon);
}