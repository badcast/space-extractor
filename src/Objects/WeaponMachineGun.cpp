#include "WeaponMachineGun.hpp"
#include "Worlds/WGame.hpp"

void WeaponMachineGun::setDefaultValues()
{
    this->rotateSpeed = 200;
    this->weight = 125;
    this->bulletSpeed = 25;
    this->bulletDelayShot = 0.1f;
    this->bulletThreshold = 10;
    this->damage = 6;

    if(bulletPrefab == nullptr)
    {
        SpriteRenderer *sprRender;
        // GameObject *rootPref = Primitive::CreateEmptyGameObject();
        bulletPrefab = Primitive::CreateEmptyGameObject();
        bulletPrefab->name("Gold Bullet");
        bulletPrefab->transform()->angle(180);
        bulletPrefab->SetActive(false);

        sprRender = bulletPrefab->AddComponent<SpriteRenderer>();
        sprRender->setSprite(Primitive::CreateSpriteFrom(assets.gameSprites->GetImage("bullet-machine-gun")));

        // bulletPrefab->transform()->setParent(rootPref->transform(), true);
        // bulletPrefab = rootPref;
    }
}
