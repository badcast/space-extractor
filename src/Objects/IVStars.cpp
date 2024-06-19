#include "IVStars.hpp"

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

    _dir = direction.normalized();
    _speed = speed;

    IVObj iv;
    iv.flag = 0;

    for(; count--;)
    {
        Vec2 newPoint = Camera::ViewportToWorldPoint(Random::RandomVector());

        iv.obj = Primitive::CreateBox2D(newPoint)->transform();
        iv.obj->spriteRenderer()->setSize(Vec2::one / 60);
        _stars.emplace_back(iv);
    }
}

void IVStars::clear()
{
    std::for_each(_stars.begin(), _stars.end(), [](IVObj const &target) { target.obj->gameObject()->Destroy(); });
    _stars.clear();
}

static Color cc = Color::white;
Rect s = Rect::one;

void IVStars::play()
{
    Rectf area;
    Vec2 point;

    area.setXY(Camera::ViewportToWorldPoint(Vec2::zero));
    area.setWH(Camera::ViewportToWorldPoint(Vec2::one) - area.getXY());

    for(IVObj &self : _stars)
    {
        self.obj->Translate(_dir * Time::deltaTime() * _speed);

        point = self.obj->position();

        /*
          // Координаты вершин A и B
            double Ax = 0, Ay = 0;
            double Bx = 0, By = 13;

            // Вектор (1,1) для угла C
            double vector_x = 1, vector_y = 1;

            // Находим разность координат между вершинами A и B
            double diff_x = Bx - Ax;
            double diff_y = By - Ay;

            // Находим длину вектора AC
            double length_AC = sqrt(diff_x * diff_x + diff_y * diff_y);

            // Нормализуем вектор AC
            double norm_x = diff_x / length_AC;
            double norm_y = diff_y / length_AC;

            // Находим угол между векторами AC и BC
            double dot_product = norm_x * vector_x + norm_y * vector_y;
            double angle_rad = acos(dot_product);

            // Находим координаты вершины C
            double Cx = Bx + cos(angle_rad) * length_AC;
            double Cy = By + sin(angle_rad) * length_AC;


        */

        if(self.flag == 0 || !Vec2::HasIntersection(point, area))
        {

            Vec2 _A;
            Vec2 _C;
            _A = {area.x, point.y};

            _C = Vec2::Perpendicular(point - _A);

            // Replace to Vec2::Max ()
            if(_C.x < _A.x)
                _C.x = _A.x;
            if(_C.y < _A.y)
                _C.y = _A.y;

            self.obj->position(_C);

            if(self.flag == 0 && Vec2::HasIntersection(self.obj->position(), area))
                self.flag = 1;
        }
    }
}
