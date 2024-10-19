#ifndef _OTHER_OBJECTS_HPP_
#define _OTHER_OBJECTS_HPP_

#include "SpaceDepends.hpp"

struct IVObj
{
    Vec2 orig;
    Transform *obj;
};

class IVStars
{
private:
    float _speed;
    Vec2 _dir;
    Rectf area;
    std::list<IVObj> _stars;

public:
    IVStars();

    void set(Vec2 direction, float speed, int count, bool startOfScreen);
    void clear();
    void play();
};

#endif
