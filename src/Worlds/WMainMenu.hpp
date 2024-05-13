#pragma once

#include "SpaceDepends.hpp"

class WMainMenu : public World
{
public:
    WMainMenu() : World("Main Menu"){}

    void OnStart();
    void OnUpdate();
};
