#include "SpaceMainMenu.h"

using namespace RoninEngine;
using namespace RoninEngine::Runtime;

static std::list<World *> __inited_levels;
std::array<const char *, 6> strings {
    "Тест #1 - Турель 2D",
    "Тест #2 - Стратегия (авто)",
    "Тест #3 - Лабиринт I",
    "Тест #4 - Лабиринт II",
    "Тест #5 - Симуляция",
    "Тест #6 - Звук"};
std::array<const char *, 6> string_info {
    "WASD (стрелки) чтобы двигать камеру. Ничего не делайте, просто наблюдайте!",
    "WASD (стрелки) чтобы двигать камеру, эксперементируйте с настройками!",
    "WASD (стрелки) чтобы двигать камеру. Можно эксперементировать!",
    "WASD (стрелки) чтобы двигать камеру. Перемещение клик по точке, shift - ускорение!",
    "Круг за прямоугольником! Объект в движений!",
    "Клавиши от Q до Y - тоны, 2 - 9 полутоны"};
uid ui_groups;

uid but_run_planet_game;
uid but_run_sector;
uid but_run_overlay;
uid but_run_bunker_play;
uid but_run_piano_play;
uid but_run_population_play;
uid dropdown_set_resolution;
uid logo_img;

constexpr float delay = 1;
int logo_pos;
float last_time;
void main_menu_callback(uid but, void *)
{
    World *lev = nullptr;

    if(but == but_run_planet_game)
    {
        lev = RoninMemory::alloc<SpaceExtractorLevel>();
    }
    else if(but == but_run_sector)
    {
        lev = RoninMemory::alloc<SectorLine>();
    }
    else if(but == but_run_population_play)
    {
        lev = RoninMemory::alloc<PathMover>();
    }
    else if(but == but_run_overlay)
    {
        lev = RoninMemory::alloc<OverlayLevel>();
    }
    else if(but == but_run_bunker_play)
    {
        lev = RoninMemory::alloc<BunkerWorld>();
    }
    else if(but == but_run_piano_play)
    {
        lev = RoninMemory::alloc<WorldPiano>();
    }

    if(!lev)
        return;

    MusicPlayer::Stop(); // stop intro music

    RoninSimulator::LoadWorld(__inited_levels.emplace_back(lev), false);
}

void GameMainMenu::OnStart()
{
    for(World *_destr : __inited_levels)
    {
        RoninMemory::free(_destr);
    }

    __inited_levels.clear();

    if(main_menu)
    {
        MusicPlayer::Play(true);
        return;
    }

    logo_pos = 0;
    last_time = Time::time() + delay / 2;
    main_menu = this;

    GetGUI()->SetGeneralCallback(main_menu_callback, nullptr);

    Resolution res = RoninSimulator::GetCurrentResolution();
    int width = 200;
    int height = 30;
    Rect _but_pos = {res.width / 2 - width / 2, 170, width, height};
    ui_groups = GetGUI()->PushGroup(_but_pos);
    GetGUI()->PushLabel("Данный тест собран на движке RoninEngine!", _but_pos, 13, ui_groups);
    _but_pos.y += height;

    but_run_bunker_play = GetGUI()->PushButton(strings[0], _but_pos, nullptr, ui_groups);
    _but_pos.y += height;

    but_run_planet_game = GetGUI()->PushButton(strings[1], _but_pos, nullptr, ui_groups);
    _but_pos.y += height;
    but_run_sector = GetGUI()->PushButton(strings[2], _but_pos, nullptr, ui_groups);
    _but_pos.y += height;

    but_run_population_play = GetGUI()->PushButton(strings[3], _but_pos, nullptr, ui_groups);
    _but_pos.y += height;

    but_run_overlay = GetGUI()->PushButton(strings[4], _but_pos, nullptr, ui_groups);
    _but_pos.y += height;

    but_run_piano_play = GetGUI()->PushButton(strings[5], _but_pos, nullptr, ui_groups);
    _but_pos.y += height;

    GetGUI()->ElementSetVisible(ui_groups, false);

    std::list<std::string> __list_res;
    std::list<Resolution> resolutions = RoninSimulator::EnumerateResolutions();
    for(Resolution &res : resolutions)
    {
        __list_res.push_back(std::to_string(res.width) + "x" + std::to_string(res.height) + " " + std::to_string(res.hz) + "hz");
    }
    dropdown_set_resolution = GetGUI()->PushDropDown(
        __list_res, 0, _but_pos, [](uid id, int index) { RoninSimulator::ShowMessage(std::to_string(index)); }, ui_groups);

    Sprite *logo = Primitive::CreateSpriteFrom(Resources::GetImageSource(Resources::LoadImage("./data/logo.png", true)));

    _but_pos = logo->rect();
    _but_pos.x = res.width / 2 - _but_pos.w / 2;
    _but_pos.y = res.height / 2 - _but_pos.h / 2;
    logo_img = GetGUI()->PushPictureBox(logo, _but_pos);
    GetGUI()->ElementSetVisible(logo_img, false);

    MusicPlayer::setClip(Resources::GetMusicClipSource(Resources::LoadMusicClip("./data/intro.ogg", false)));
    MusicPlayer::Play(true);
    MusicPlayer::setVolume(0.6f);
}

void GameMainMenu::OnUpdate()
{
    float t = Time::time();

    switch(logo_pos)
    {
            // show logo
        case 0:
            if(last_time < t)
            {
                GetGUI()->ElementSetVisible(logo_img, true);
                last_time = delay + t;
                ++logo_pos;
            }
            break;
            // hide logo
        case 1:
            if(last_time < t)
            {
                GetGUI()->ElementSetVisible(logo_img, false);
                last_time = delay / 5 + t;
                ++logo_pos;
            }
            break;
            // show UI
        case 2:
            if(last_time < t)
            {
                GetGUI()->ElementSetVisible(ui_groups, true);
                ++logo_pos;
            }
            break;
            // every state
        default:
            break;
    }
}

void GameMainMenu::OnUnloading()
{
    main_menu = nullptr;
    for(World *_destr : __inited_levels)
    {
        RoninMemory::free(_destr);
    }
    __inited_levels.clear();
}

static uid button;
void __call_backmenu(uid but)
{
    if(!main_menu)
        return;

    if(but == button)
    {
        RoninSimulator::LoadWorld(main_menu);
    }
}

void __call_reload(uid but)
{
    RoninSimulator::ReloadWorld();
}

inline int indexof(World *world)
{
    if(dynamic_cast<BunkerWorld *>(world) != nullptr)
        return 0;
    else if(dynamic_cast<SpaceExtractorLevel *>(world) != nullptr)
        return 1;
    else if(dynamic_cast<SectorLine *>(world) != nullptr)
        return 2;
    else if(dynamic_cast<PathMover *>(world) != nullptr)
        return 3;
    else if(dynamic_cast<OverlayLevel *>(world) != nullptr)
        return 4;
    else if(dynamic_cast<WorldPiano *>(world) != nullptr)
        return 5;
    else
        return 0;
}

void switch_game_level(World *world)
{
    button = world->GetGUI()->PushButton("В главное меню", Vec2Int::zero, __call_backmenu);
    world->GetGUI()->PushButton("Reload World", Vec2Int::up * 90, __call_reload);
    world->GetGUI()->PushLabel(string_info[indexof(world)], Vec2Int {392, 16});
}
