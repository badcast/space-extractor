#include "WeaponMachineGun.hpp"

void WeaponMachineGun::setDefaultValues()
{
    this->rotateSpeed = 2;
    this->weight = 125;
    this->bulletSpeed = 10;
    this->bulletDelayShot = 0.07f;
    this->bulletThreshold = 10;
    this->damage = 6;

    if(bulletPrefab == nullptr)
    {
        bulletPrefab = Primitive::create_empty_game_object();
        bulletPrefab->name("Gold Bullet");
        bulletPrefab->transform()->layer(-1);
        bulletPrefab->SetActive(false);
        SpriteRenderer *sprRender = bulletPrefab->AddComponent<SpriteRenderer>();
        sprRender->setSprite(Primitive::create_sprite2D_box({0.02f, 0.19f}, Color::gold));
    }
}
