#include "WMainMenu.hpp"
#include "WParticleEdtitor.hpp"

using namespace RoninEngine;
using namespace RoninEngine::Runtime;
using namespace RoninEngine::UI;

uid butContinue;
uid butNewGame;
uid butOptions;

void onClickAny(uid id)
{
    if(id == butNewGame)
    {
        RoninSimulator::LoadWorld<WGame>();
        return;
    }

    RoninEngine::RoninSimulator::LoadWorld<WParticleEdtitor>();
}

void WMainMenu::OnStart()
{
    uid lastId;

    // Background
    GetGUI()->PushPictureBox(globalAssets.gameSprites->GetSprite("main-menu-background"), {{0,0},RoninSimulator::GetCurrentResolution().GetSize()});

    GetGUI()->LayoutNew(RoninEngine::UI::Vertical, {.5, .5, 1, 1});

    /////////////////////////////////////////////////////////////////
    /// Load Game
    /////////////////////////////////////////////////////////////////
    butContinue = lastId = GetGUI()->LayoutButton("Continue");
    GetGUI()->AddEventListener_Click(lastId, onClickAny);

    /////////////////////////////////////////////////////////////////
    /// Play Game
    /////////////////////////////////////////////////////////////////
    butNewGame = lastId = GetGUI()->LayoutButton("New Game");
    GetGUI()->AddEventListener_Click(lastId, onClickAny);

    /////////////////////////////////////////////////////////////////
    /// Options
    /////////////////////////////////////////////////////////////////
    butOptions = lastId = GetGUI()->LayoutButton("Options");
    GetGUI()->AddEventListener_Click(lastId, onClickAny);

    /////////////////////////////////////////////////////////////////
    /// About
    /////////////////////////////////////////////////////////////////
    lastId = GetGUI()->LayoutButton("About");
    GetGUI()->AddEventListener_Click(lastId, onClickAny);

    /////////////////////////////////////////////////////////////////
    /// Exit
    /////////////////////////////////////////////////////////////////
    lastId = GetGUI()->LayoutButton("Exit");
    GetGUI()->AddEventListener_Click(lastId, onClickAny);
}

void WMainMenu::OnUpdate()
{
}
