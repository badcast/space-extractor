#ifndef GAME_H
#define GAME_H

#include <ronin/framework.h>

class OverlayLevel : public World
{
protected:
    Vec2 bodySize;
    Vec2 bublePos;
    Vec2 targetPos;
    int trigger;
    Vec2 cursor, plane;
    float speed;

public:
    OverlayLevel()
        : World("Overlay target")
    {
    }
    void on_start();
    void on_gizmo();
};

#endif // GAME_H
