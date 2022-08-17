#include "PathMoverMenu.h"

using namespace RoninEngine::UI;

namespace RoninEngine::Runtime
{

    void controlEvent(uid, GUI *);
    // groups
    uint8_t group_mainmenu_screen, group_startgame_screen, group_setting_screen, group_author_screen;
    // controls
    uint8_t mmb_start_game, mmb_setting, mmb_aboutus, mmb_quit;
    uint8_t sgb_playgame, sgb_option, sgb_backmenu;
    uint8_t ssb_backmenu;
    uint8_t sab_backmenu;

    // Запуск после инициализаций
    void PathMoverMenu::OnAwake()
    {
        switch_game_level(this);

        constexpr int _OFSET = 0x21;
        int fontWidth = 10;

        // Инициализация меню
        auto gameDisplay = RoninSimulator::GetCurrentResolution();

        Vec2Int point = {gameDisplay.width / 2 - 127, 300};
        Vec2Int lastPoint;
        // Main menu screen

        group_mainmenu_screen = getGUI()->PushGroup();
        {
            mmb_start_game = getGUI()->PushButton("Start Game", point, nullptr, group_mainmenu_screen);
            point.y += _OFSET;
            mmb_setting = getGUI()->PushButton("Options", point, nullptr, group_mainmenu_screen);
            point.y += _OFSET;
            mmb_aboutus = getGUI()->PushButton("About US", point, nullptr, group_mainmenu_screen);
            point.y += _OFSET;
            mmb_quit = getGUI()->PushButton("Quit", point, nullptr, group_mainmenu_screen);
        }
        lastPoint = point;
        point = {gameDisplay.width / 2 - 127, 300};
        // Start game screen
        group_startgame_screen = getGUI()->PushGroup();
        {
            sgb_playgame = getGUI()->PushButton("Play", point, nullptr, group_startgame_screen);
            point.y += _OFSET;
            sgb_backmenu = getGUI()->PushButton("< Back", lastPoint, nullptr, group_startgame_screen);
        }
        // Setting screen
        group_setting_screen = getGUI()->PushGroup();
        {
            getGUI()->PushLabel("Graphics", Vec2Int {gameDisplay.width / 2 - 127, 300}, fontWidth, group_setting_screen);
            getGUI()->PushSlider(1, {gameDisplay.width / 2 - 127, 300});
            ssb_backmenu = getGUI()->PushButton("< Back", lastPoint, nullptr, group_setting_screen);
        }
        // Author & About US
        group_author_screen = getGUI()->PushGroup();
        {
            getGUI()->PushLabel("Night Fear", Vec2Int {gameDisplay.width / 2 - 127, 300}, fontWidth, group_author_screen);
            sab_backmenu = getGUI()->PushButton("< Back", lastPoint, nullptr, group_author_screen);
        }

        // Show as First
        getGUI()->GroupShowAsUnique(group_mainmenu_screen);
        // Register main event
        getGUI()->SetGeneralCallback((ui_callback)controlEvent, getGUI());
    }

    void controlEvent(uid id, GUI *guiInstance)
    {
        // Эти определения просто сравнивают ID :)
#define e(ID) ID == id
#define e2(ID1, ID2) ID1 == id || ID2 == id
#define e3(ID1, ID2, ID3) ID1 == id || ID2 == id || ID3 == id
#define e4(ID1, ID2, ID3, ID4) ID1 == id || ID2 == id || ID3 == id || ID4 == id
        // main menu events
        if(e(mmb_start_game))
        {
            guiInstance->GroupShowAsUnique(group_startgame_screen);
        }
        else if(e(mmb_setting))
        {
            guiInstance->GroupShowAsUnique(group_setting_screen);
        }
        else if(e(mmb_aboutus))
        {
            guiInstance->GroupShowAsUnique(group_author_screen);
        }
        else if(e(mmb_quit))
        {
            RoninSimulator::RequestQuit();
        }
        // start game menu events
        else if(e(sgb_playgame))
        {
            RoninSimulator::LoadWorld(RoninMemory::alloc<PathMover>());
        }
        // setting menu events
        else if(e3(sgb_backmenu, ssb_backmenu, sab_backmenu))
        {
            guiInstance->GroupShowAsUnique(group_mainmenu_screen);
        }
#undef e
#undef e2
#undef e3
#undef e4
    }

} // namespace RoninEngine::Runtime
