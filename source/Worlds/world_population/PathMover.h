#pragma once
#include "PathMoverMenu.h"

namespace RoninEngine::Runtime
{
    class PathMover : public World
    {
    public:
        PathMover()
            : World("Game Scene levels[XXX]")
        {
        }
        void OnAwake() override;
        void OnStart() override;
        void OnUpdate() override;
        void OnGizmos() override;
        void OnUnloading() override;
    };
}
