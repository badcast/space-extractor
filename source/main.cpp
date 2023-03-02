#include <exception>
#include <iostream>

#include "gamelevel.h"

using namespace std;

int main() {
    using namespace RoninEngine;

    Application::init(1366, 768);

    GameLevel level;
    Application::loadLevel(&level);

    Application::simulate();

    Application::Quit();

    return 0;
}
