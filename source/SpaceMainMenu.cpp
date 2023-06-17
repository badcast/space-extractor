#include <array>
#include "SpaceMainMenu.h"
using namespace RoninEngine;
using namespace RoninEngine::Runtime;

static std::list<World*> __inited_levels;
std::array<const char*, 4> strings { "Воспроизвести игру", "Воспроизвести секторы", "Воспроизвести Overlay", "Воспроизвести игру Bunker" };
std::array<const char*, 4> string_info { "WASD (стрелки) чтобы двигать камеру.", "WASD (стрелки) чтобы двигать камеру.", "Типо кошка и мышь, бегает за квадратом", "WASD (стрелки) чтобы двигать камеру, левая мышь - стрелять." };
uid but_run_planet_game;
uid but_run_sector;
uid but_run_overlay;
uid but_run_bunker_play;
uid sldr_set_resolution;

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

    Application::load_world(__inited_levels.emplace_back(lev), false);
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

    get_gui()->register_general_callback(main_menu_callback, nullptr);

    Resolution res = Application::get_current_resolution();
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

    std::list<std::string> __list_res;
    std::list<Resolution> resolutions = Application::get_display_resolutions();
    for(Resolution & res : resolutions){
        __list_res.push_back(std::to_string(res.width) + "x" + std::to_string(res.height) + " " + std::to_string(res.hz) + "hz");
    }
    sldr_set_resolution = get_gui()->push_drop_down(__list_res,0, _but_pos, [](uid id, int index) {

        int xx = 0;

        Application::show_message(std::to_string(index));

    });

    MusicPlayer::clip(Resources::get_music_clip(Resources::load_music_clip("./data/main_trailer.mp3", false)));
    MusicPlayer::play();
    MusicPlayer::volume(0.6f);
}

void GameMainMenu::on_update() { }

void GameMainMenu::on_unloading()
{
    main_menu = nullptr;
    for (World* _destr : __inited_levels) {
        RoninMemory::free(_destr);
    }
    __inited_levels.clear();
}

static uid button;
void __call_backmenu(uid but, void* userdata)
{
    if (!main_menu)
        return;

    if (but == button) {
        Application::load_world(main_menu);
    }
}

inline int indexof(World* world)
{
    if (dynamic_cast<SpaceExtractorLevel*>(world) != nullptr)
        return 0;
    else if (dynamic_cast<SectorLine*>(world) != nullptr)
        return 1;
    else if (dynamic_cast<OverlayLevel*>(world) != nullptr)
        return 2;
    else if (dynamic_cast<BunkerWorld*>(world) != nullptr)
        return 3;
    else
        return 0;
}

void switch_game_level(World* level)
{
    button = level->get_gui()->push_button("В главное меню", Vec2Int::zero, __call_backmenu);
    level->get_gui()->push_label(string_info[indexof(level)], Vec2Int { 392, 16 });
    level->get_gui()->set_resources(button, level);
}
