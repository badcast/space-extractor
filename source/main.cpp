#include <exception>
#include <iostream>

#include "gamelevel.h"

using namespace std;

int main() {
    using namespace RoninEngine;

    Application::Init(1366, 768);

    GameLevel level;
    Application::LoadLevel(&level);

    Application::Simulate();

    Application::Quit();

    return 0;
}
