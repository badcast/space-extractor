#ifndef GAME_H
#define GAME_H

#include <ronin/framework.h>

class OverlayLevel : public World
{
public:
    void on_awake();
    void on_start();
    void on_update();
    void on_late_update();
    void on_gizmo();
    void on_unloading();
};

#endif // GAME_H
