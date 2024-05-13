#include "WMainMenu.hpp"
#include "WParticleEdtitor.hpp"

using namespace RoninEngine::UI;

void onClickAny(uid id)
{
    RoninEngine::RoninSimulator::LoadWorld(new WParticleEdtitor );
}

void WMainMenu::OnStart()
{

    uid lastId;

    /////////////////////////////////////////////////////////////////
    /// Load Game
    /////////////////////////////////////////////////////////////////
    GetGUI()->LayoutNew(RoninEngine::UI::Horizontal, {0, 0, 1, 1});
    lastId = GetGUI()->LayoutButton("Continue");
    GetGUI()->AddEventListener_Click(lastId, onClickAny);

    /////////////////////////////////////////////////////////////////
    /// Play Game
    /////////////////////////////////////////////////////////////////
    lastId = GetGUI()->LayoutButton("New Game");
    GetGUI()->AddEventListener_Click(lastId, onClickAny);

    /////////////////////////////////////////////////////////////////
    /// Options
    /////////////////////////////////////////////////////////////////
    lastId = GetGUI()->LayoutButton("Options");
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
