#include "gamemainmenu.h"

#include "gamelevel.h"
#include "sector_line_level.h"
#include "OverlayLevel.h"
#include "BunkerGame.h"

using namespace RoninEngine;
using namespace RoninEngine::Runtime;

static std::list<World*> __inited_levels;
std::array<const char*, 4> strings { "Воспроизвести игру", "Воспроизвести секторы", "Воспроизвести Overlay", "Воспроизвести игру Bunker" };

uid but_run_planet_game;
uid but_run_sector;
uid but_run_overlay;
uid but_run_bunker_play;

void main_menu_callback(uid but, void*)
{
    World* lev = nullptr;

    if (but == but_run_planet_game) {
        lev = RoninMemory::alloc<SpaceExtractorLevel>();
    } else if (but == but_run_sector) {
        lev = RoninMemory::alloc<SectorLine>();
    } else if (but == but_run_overlay) {
        lev = RoninMemory::alloc<OverlayLevel>();
    } else if (but == but_run_bunker_play) {
        lev = RoninMemory::alloc<BunkerWorld>();
    }

    if (!lev)
        return;

    Application::load_world(__inited_levels.emplace_back(lev));
}

void GameMainMenu::on_start()
{
    for (World* _destr : __inited_levels) {
        RoninMemory::free(_destr);
    }

    __inited_levels.clear();

    if (main_menu)
        return;
    main_menu = this;

    get_gui()->register_callback(main_menu_callback, nullptr);

    Resolution res = Application::get_resolution();
    float width = 200;
    float height = 30;
    Rect _but_pos = { res.width / 2 - width / 2, 200, width, height };
    but_run_planet_game = get_gui()->push_button(strings[0], _but_pos);
    _but_pos.y += height;

    but_run_sector = get_gui()->push_button(strings[1], _but_pos);
    _but_pos.y += height;

    but_run_overlay = get_gui()->push_button(strings[2], _but_pos);
    _but_pos.y += height;

    but_run_bunker_play = get_gui()->push_button(strings[3], _but_pos);
    _but_pos.y += height;
}

void GameMainMenu::on_update() { }

static uid button;
void __call_backmenu(uid but, void* userdata)
{
    if (but == button) {
        Application::load_world(main_menu);
    }
}

void switch_game_level(World* level)
{
    button = level->get_gui()->push_button("В главное меню", Vec2Int::zero, (ui_callback*)&__call_backmenu);
    level->get_gui()->set_resources(button, level);
}
