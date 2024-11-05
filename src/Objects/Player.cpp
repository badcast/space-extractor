#include "Player.hpp"

float lastShotTime;
char bulletFireStep;
float bullet_destroy_after = 4;

void DestructPut(GameObjectRef root)
{
    std::list<SpriteRendererRef> renders;
    if(root.isNull())
        return;
    // renders = root->GetComponentsAnChilds<SpriteRenderer>();
}

void Player::OnStart()
{
    SpriteRendererRef sprRender;
    shaking = {0, 0};

    bullets = {};
    lastShotTime = 0;
    bulletFireStep = 0;

    if(healthPoint == 0)
        healthPoint = maxHealthPoint;
    if(armoryPoint == 0)
        armoryPoint = maxArmoryPoint;

    // Load Sources
    SpriteRef spritePlayerWeapon = globalAssets.gameSprites->GetSprite("player-weapon");
    SpriteRef spritePlayerPlatform = globalAssets.gameSprites->GetSprite("player-platform");
    SpriteRef spriteMuzzleFlash = globalAssets.gameSprites->GetSprite("muzzle-flash");
    SpriteRef spritePlayerShield = globalAssets.gameSprites->GetSprite("player-shield");

    AudioClip *srcAudioMachineGun = globalAssets.gameSounds->GetAudioClip("machinegun-1");

    // Make player weapon object
    weapon = RoninMemory::alloc<WeaponMachineGun>();

    // Audio Player for Machine gun
    playerAudio = gameObject()->AddComponent<AudioSource>();
    playerAudio->setClip(srcAudioMachineGun);

    turretRotatePivot = Primitive::CreateEmptyGameObject()->transform();
    turretRotatePivot->setParent(transform(), false);

    turret = Primitive::CreateEmptyGameObject()->transform();
    if(turret)
    {
        turret->zOrder(RenderOrders::PlayerOrder);
        turret->setParent(turretRotatePivot);
        turret->localPosition(Vec2::up * 0.14f);
        sprRender = turret->AddComponent<SpriteRenderer>();
        sprRender->setSprite(defaultTurret = spritePlayerWeapon);
        sprRender->setSize(sprRender->getSize() / 4);
    }

    fireTurret = globalAssets.gameSprites->GetSprite("player-weapon-blob");
    /////////////////////////////////////////////////////////////////
    /// Gun Points
    /////////////////////////////////////////////////////////////////
    gunPosition = Primitive::CreateEmptyGameObject()->transform();
    if(gunPosition)
    {
        gunPosition->setParent(turret->transform());
        gunPosition->localPosition(Vec2::up * (5 / 10.f));
    }

    gunPoint1 = Primitive::CreateEmptyGameObject(gunPosition->position())->transform();
    if(gunPoint1)
    {
        gunPoint1->setParent(gunPosition);
        gunPoint1->localPosition(Vec2::left / 10);
    }

    gunPoint2 = Primitive::CreateEmptyGameObject(gunPosition->position())->transform();
    if(gunPoint2)
    {
        gunPoint2->setParent(gunPosition);
        gunPoint2->localPosition(-gunPoint1->localPosition());
    }
    /////////////////////////////////////////////////////////////////
    /// Muzzle flash
    /////////////////////////////////////////////////////////////////
    muzzleFlash = Primitive::CreateEmptyGameObject()->transform();
    if(muzzleFlash)
    {
        muzzleFlash->setParent(gunPosition);
        muzzleFlash->position(gunPosition->position());
        sprRender = muzzleFlash->gameObject()->AddComponent<SpriteRenderer>();
        sprRender->setSprite(spriteMuzzleFlash);
        sprRender->setSize(sprRender->getSize() / 4);
        sprRender->enable(false); // of muzzle flash
        sprRender->transform()->zOrder(RenderOrders::PlayerOrder);
    }
    /////////////////////////////////////////////////////////////////
    /// Player Platform
    /////////////////////////////////////////////////////////////////
    platform = Primitive::CreateEmptyGameObject()->transform();
    if(platform)
    {
        platform->zOrder(RenderOrders::OtherOrder);
        platform->position(transform()->position());
        platform->setParent(this->transform());
        sprRender = platform->AddComponent<SpriteRenderer>();
        sprRender->setSprite(spritePlayerPlatform);
        sprRender->setSize(sprRender->getSize());
    }
    /////////////////////////////////////////////////////////////////
    /// Player shield
    /////////////////////////////////////////////////////////////////
    TransformRef playerShieldPivot = Primitive::CreateEmptyGameObject()->transform();
    playerShieldPivot->setParent(transform(), false);

    playerShield = Primitive::CreateEmptyGameObject()->transform();
    if(playerShield)
    {
        playerShield->setParent(playerShieldPivot, false);
        playerShield->localPosition(-Vec2::up);
        sprRender = playerShield->AddComponent<SpriteRenderer>();
        sprRender->setSprite(spritePlayerShield);
        sprRender->setSize(sprRender->getSize() / 4);
        playerShield = playerShieldPivot;
        playerShield->gameObject()->SetActive(false);
    }

    InitPlayerGUI();

    DestructPut(this->gameObject());
}

void Player::OnDestroy()
{
    // destroy
    RoninMemory::free(weapon);
}

void Player::OnUpdate()
{
    // Mouse position to world position
    Vec2 target = Camera::ScreenToWorldPoint(Input::GetMousePointf());
    //    Enemy *e = *(WGame::current->enemies.cbegin());
    //    target = e->transform()->position();

    // Rotate player to Mouse Position
    turretRotatePivot->LookAtLerp(target, Time::deltaTime() * weapon->rotateSpeed);
    // turretRotatePivot->LookAt(target, Vec2::up);

    // Clamp angle
    if(canClampAngle)
        turretRotatePivot->angle(Math::Clamp(turretRotatePivot->angle(), clampAngleLeft, clampAngleRight));

    if(Time::frame() % 60 == 0)
        playerShield->LookAt(target);

    // Move exists bullets
    for(const TransformRef& bullet : bullets)
    {
        bullet->Translate(bullet->back() * Time::deltaTime() * weapon->bulletSpeed);
    }

    // Append new bullet on fire
    if(lastShotTime < Time::time() && Input::GetMouseDown(MouseButton::MouseLeft))
    {
        TransformRef originObject = ((bulletFireStep % 2 == 0) ? gunPoint1 : gunPoint2);
        Vec2 origin = originObject->position();
        if(++bulletFireStep == 2)
            bulletFireStep = 0;

        GameObjectRef bulletInstance = Instantiate(weapon->bulletPrefab, origin);
        CollisionRef bulletCollision = bulletInstance->GetComponent<Collision>();
        Vec2 direction = originObject->forward() + Random::RandomVector() / 1000 * weapon->bulletThreshold;
        bulletInstance->SetZOrderAll(RenderOrders::BulletOrder, ZOrderBy::Inherit);
        bulletCollision->targetLayer = static_cast<int>(GameLayers::EnemyOrBullet);
        bulletCollision->onCollision = [&](CollisionRef self_bullet, CollisionRef target) -> bool
        {
            Ref<Enemy> enemy = std::move(target->GetComponent<Enemy>());
            if(enemy == nullptr)
                return true;
            enemy->receiveDamage(WGame::current->player->weapon->damage);
            if(self_bullet)
                self_bullet->gameObject()->Destroy(); // destroy object
            this->scores++;
            return false;
        };
        bulletInstance->transform()->zOrder(RenderOrders::PlayerOrder);
        bulletInstance->transform()->LookAt(origin + direction);
        bulletInstance->AddOnDestroy([&](GameObjectRef self) { bullets.erase(self->transform()); });
        bulletInstance->Destroy(bullet_destroy_after);

        bullets.insert(bulletInstance->transform());

        muzzleFlash->position(origin);
        muzzleFlash->spriteRenderer()->enable(true);

        turret->spriteRenderer()->setSprite(fireTurret);
        turret->spriteRenderer()->flip = bulletFireStep ? SpriteRenderFlip::FlipNone : SpriteRenderFlip::FlipHorizontal;

        gunPosition->localPosition(bulletFireStep ? Vec2::down * 0.1f : Vec2::zero);

        // Shaking camera on fire
        shaking.force |= 2;
        shaking.time = Time::time() + 0.3f;

        // Audio Shot
        AudioSource::PlayClipAtPoint(playerAudio->getClip(), transform()->position(), 0.2f);

        // Update last shot time
        lastShotTime = weapon->bulletDelayShot + Time::time();
    }
    else if(Input::GetMouseUp(MouseButton::MouseLeft))
    {
        turret->spriteRenderer()->flip = SpriteRenderFlip::FlipNone;
        turret->spriteRenderer()->setSprite(defaultTurret);
        muzzleFlash->spriteRenderer()->enable(false);
        shaking.force = 0;
    }

    if(false)
    {
        if(shaking.time > Time::time())
            Camera::mainCamera()->transform()->position(Random::RandomVector() * shaking.force / 100.f);
        else
            Camera::mainCamera()->transform()->position(Vec2::zero); // set default position
    }

    if(canRepairs && m_timeoutRepairs < Time::time())
        onRepairing();
}

void Player::onRepairing()
{
    // First damage armory, and after health point!
    int *pHP, *pMaxHP;

    m_timeoutRepairs = Time::time() + repairsAfterTime;

    if(healthPoint < maxHealthPoint)
    {
        pHP = &healthPoint;
        pMaxHP = &maxHealthPoint;
    }
    else
    {
        pHP = &armoryPoint;
        pMaxHP = &maxArmoryPoint;
    }

    if(*pHP < *pMaxHP)
    {
        *pHP += repairScore;
    }

    *pHP = Math::Min(*pHP, *pMaxHP);
}

void Player::showShield(Vec2 lookPosition)
{
    return;
    playerShield->LookAtLerp(lookPosition, 1);
    playerShield->gameObject()->SetActive(true);
}

void Player::applyDamage(int damageValue, Vec2 closestPosition)
{
    int *pHP;
    if(armoryPoint == 0)
        pHP = &healthPoint;
    else
    {
        pHP = &armoryPoint;
        showShield(closestPosition);
    }

    *pHP = Math::Max(0, *pHP - damageValue);

    shaking.time = Time::time() + 0.7f;
    shaking.force |= 5;

    // delay repairs after apply damages
    m_timeoutRepairs = Time::time() + 5;
}
