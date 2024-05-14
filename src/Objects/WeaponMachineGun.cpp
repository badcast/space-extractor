#include "WeaponMachineGun.hpp"
#include "Worlds/WGame.hpp"

void WeaponMachineGun::setDefaultValues()
{
    this->rotateSpeed = 200;
    this->weight = 125;
    this->bulletSpeed = 15;
    this->bulletDelayShot = 0.2f;
    this->bulletThreshold = 10;
    this->damage = 6;

    if(bulletPrefab == nullptr)
    {
        SpriteRenderer *sprRender;
        GameObject *rootPref;

        rootPref = Primitive::CreateEmptyGameObject();

        bulletPrefab = Primitive::CreateEmptyGameObject();
        bulletPrefab->name("Gold Bullet");
        bulletPrefab->transform()->angle(0);
        bulletPrefab->transform()->setParent(rootPref->transform(), false);

        sprRender = bulletPrefab->AddComponent<SpriteRenderer>();
        sprRender->setSprite(assets.gameSprites->GetSprite("bullet-machine-gun"));

        rootPref->AddComponent<Collision>()->setSizeFrom(sprRender);

        rootPref->SetActive(false);

        bulletPrefab = rootPref;
    }
}
