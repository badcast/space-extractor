#pragma once
#include "../../SpaceMainMenu.h"

#include "PathMover.h"

namespace RoninEngine::Runtime
{

    class PathMoverMenu : public World
    {
    public:
        PathMoverMenu() : World("Main Menu")
        {
        }

        void OnAwake();
    };
} // namespace RoninEngine::Runtime
