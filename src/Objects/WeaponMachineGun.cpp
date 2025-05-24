#include "WeaponMachineGun.hpp"
#include "Worlds/WGame.hpp"

void WeaponMachineGun::setDefaultValues()
{
    this->rotateSpeed = 200;
    this->weight = 125;
    this->bulletSpeed = 15;
    this->bulletDelayShot = 0.1f;
    this->bulletThreshold = 10;
    this->damage = 6;
}

GameObjectRef WeaponMachineGun::createBullet()
{
    SpriteRendererRef sprRender;
    GameObjectRef rootPref, bulletPrefab;
    rootPref = Primitive::CreateEmptyGameObject();
    rootPref->name("Gold Bullet");
    rootPref->setTag("BULLET");
    bulletPrefab = Primitive::CreateEmptyGameObject();
    bulletPrefab->name("Gold Bullet");
    bulletPrefab->transform()->angle(0);
    bulletPrefab->transform()->setParent(rootPref->transform(), false);
    sprRender = bulletPrefab->AddComponent<SpriteRenderer>();
    sprRender->setSprite(globalAssets.gameSprites->GetSprite("bullet-machine-gun"));
    clips = globalAssets.gameSounds->GetAudioClips("machinegun-1");
    rootPref->AddComponent<Collision>()->setSizeFrom(sprRender);
    rootPref->SetActive(true);
    return rootPref;
}
