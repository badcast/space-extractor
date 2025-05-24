#include "BulletDirection.hpp"

void BulletDirection::OnStart()
{

}

void BulletDirection::OnUpdate()
{
    transform()->Translate(dir * Time::deltaTime() * speed);
}
