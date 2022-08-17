#include <exception>
#include <iostream>

#include "gamelevel.h"

using namespace std;

int main() {
    using namespace RoninEngine;

    Application::Init(1366, 768);

    jno::jno_object_parser jp;
    jp.deserialize("/home/badalloc/Projects/git/space-extractor/resources/data/locale/ru_RU/locale_levels.txt");

    auto cc = jp.find_node("level1/level_name1");
    auto v = cc->toString();

    GameLevel level;
    Application::LoadLevel(&level);

    Application::Simulate();

    Application::Quit();

    return 0;
}
