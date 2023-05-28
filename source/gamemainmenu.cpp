#include "gamemainmenu.h"

#include "gamelevel.h"
#include "sector_line_level.h"
#include "OverlayLevel.h"

using namespace RoninEngine;
using namespace RoninEngine::Runtime;

static std::list<Level*> __inited_levels;
std::array<const char*, 3> strings { "Воспроизвести игру", "Воспроизвести секторы", "Воспроизвести Overlay" };

uid but_run_planet_game;
uid but_run_sector;
uid but_run_overlay;

void main_menu_callback(uid but, void*)
{
    Level* lev = nullptr;

    for (Level* _destr : __inited_levels) {
        RoninMemory::free(_destr);
    }
    __inited_levels.clear();

    if (but == but_run_planet_game) {
        lev = RoninMemory::alloc<SpaceExtractorLevel>();
    } else if (but == but_run_sector) {
        lev = RoninMemory::alloc<SectorLine>();
    } else if (but == but_run_overlay) {
        lev = RoninMemory::alloc<OverlayLevel>();
    }

    if (!lev)
        return;

    Application::load_level(__inited_levels.emplace_back(lev));
}

void GameMainMenu::on_start()
{
    main_menu = this;

    ui->register_callback(main_menu_callback, nullptr);

    Resolution res = Application::get_resolution();
    float width = 200;
    float height = 30;
    Rect _but_pos = { res.width / 2 - width / 2, 200, width, height };
    but_run_planet_game = ui->push_button(strings[0], _but_pos);
    _but_pos.y += height;

    but_run_sector = ui->push_button(strings[1], _but_pos);
    _but_pos.y += height;

    but_run_overlay = ui->push_button(strings[2], _but_pos);
    _but_pos.y += height;
}

void GameMainMenu::on_update() { }
static uid button;
void __call_backmenu(uid but, void* userdata)
{
    if (but == button) {
        Application::load_level(main_menu);
    }
}

void switch_game_level(Level* level)
{

    button = level->get_gui()->push_button("В главное меню", Vec2Int::zero, (ui_callback*)&__call_backmenu);
    level->get_gui()->set_resources(button,level);
}
