#include "WeaponMachineGun.hpp"

void WeaponMachineGun::setDefaultValues()
{
    this->rotateSpeed = 20;
    this->weight = 125;
    this->bulletSpeed = 25;
    this->bulletDelayShot = 0.4f;
    this->bulletThreshold = 10;
    this->damage = 6;

    if(bulletPrefab == nullptr)
    {
        bulletPrefab = Primitive::CreateEmptyGameObject();
        bulletPrefab->name("Gold Bullet");
        bulletPrefab->transform()->layer(-1);
        bulletPrefab->SetActive(false);
        SpriteRenderer *sprRender = bulletPrefab->AddComponent<SpriteRenderer>();
        sprRender->setSprite(Primitive::CreateSpriteRectangle(true, {0.02f, 1}, Color::gold));
    }
}
