#include "IVStars.hpp"

Vec2 calculateFinalPoint(Vec2 start, Vec2 direction, Vec2 rectMin, Vec2 rectMax) {
    direction.normalize();

    float tLeft = (rectMin.x - start.x) / -direction.x;
    float tRight = (rectMax.x - start.x) / -direction.x;
    float tBottom = (rectMin.y - start.y) / -direction.y;
    float tTop = (rectMax.y - start.y) / -direction.y;

    float tCollision = std::numeric_limits<float>::max();

    if (tLeft > 0) tCollision = Math::Min(tCollision, tLeft);
    if (tRight > 0) tCollision = Math::Min(tCollision, tRight);
    if (tBottom > 0) tCollision = Math::Min(tCollision, tBottom);
    if (tTop > 0) tCollision = Math::Min(tCollision, tTop);

    Vec2 initialPoint;
    initialPoint.x = start.x - tCollision * direction.x;
    initialPoint.y = start.y - tCollision * direction.y;

    return initialPoint;
}


IVStars::IVStars() : _dir(Vec2::up_right), _speed(2.0f), _stars {}
{
}

void IVStars::set(Vec2 direction, float speed, int count, bool startOfScreen)
{
    if(count <= 0)
    {
        clear();
        return;
    }

    if(speed <= 0)
        return;

    area.setXY(Camera::ViewportToWorldPoint(Vec2::zero));
    area.setWH(Camera::ViewportToWorldPoint(Vec2::one) - area.getXY());
    _dir = direction.normalized();
    _speed = speed;

    IVObj iv;

    for(; count--;)
    {
        Vec2 newPoint = Camera::ViewportToWorldPoint(Vec2( Random::Value(), Random::Value()));

        iv.obj = Primitive::CreateBox2D(newPoint)->transform();
        iv.obj->spriteRenderer()->setSize(Vec2::one / 60);
        iv.orig = calculateFinalPoint(iv.obj->position(), _dir, area.getXY(), area.getWH());
        _stars.emplace_back(iv);
    }
}

void IVStars::clear()
{
    std::for_each(_stars.begin(), _stars.end(), [](IVObj const &target) { target.obj->gameObject()->Destroy(); });
    _stars.clear();
}

void IVStars::play()
{
    Vec2 point;

    for(IVObj &self : _stars)
    {
        self.obj->Translate(_dir * Time::deltaTime() * _speed);

        point = self.obj->position();

        if(!Vec2::HasIntersection(point, area))
        {
            self.obj->position(self.orig);
        }

        RenderUtility::SetColor(Color::white);
        RenderUtility::DrawLine(self.orig, self.obj->position());

    }
}
