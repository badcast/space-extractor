#include "Player.hpp"

#include <ronin/UI.h>

using namespace RoninEngine::UI;

class UIPushBoxLayer : public UIOverlay
{
public:
    SpriteRef xDrawRing;
    float xDrawAngles;

    void OnInit() override;
    void OnDraw(const UIData *const uiData) override;
    void OnDestroy() override;
};

UIPushBoxLayer boxLayer {};
void Player::InitPlayerGUI()
{
    GUI *gui = WGame::current->GetGUI();
    Rect topCenter;
    Resolution res = RoninSimulator::GetCurrentResolution();

    topCenter.w = 300;
    topCenter.h = 90;
    topCenter.x = (res.width - topCenter.w) / 2;
    topCenter.y = 0;

    gui->PushCustomUI(&boxLayer, topCenter);
}

void Player::OnGizmos()
{
    RenderUtility::DrawTextLegacy(Camera::ScreenToWorldPoint(Vec2 {50, 50}), "SCORES: " + std::to_string(scores));
}

void UIPushBoxLayer::OnInit()
{
    xDrawRing = globalAssets.gameSprites->GetSprite("explode-v1");
}

void UIPushBoxLayer::OnDraw(const UIData *const uiData)
{
    Rect rect;
    AtlasRef uiAtlas = globalAssets.uiAtlas->GetAtlasObject();
    Resolution res = RoninSimulator::GetCurrentResolution();

    SpriteRef foreground = uiAtlas->GetSpriteFromName("game-progress-foreground");
    SpriteRef extentArmory = uiAtlas->GetSpriteFromName("game-progress-extent");
    SpriteRef corners = uiAtlas->GetSpriteFromName("game-top-corners");
    SpriteRef extentHealth = uiAtlas->GetSpriteFromName("game-progress-foreground-mask");
    SpriteRef rightRing = uiAtlas->GetSpriteFromName("game-right-ring");

    rect.w = res.width;
    rect.h = 25;

    RenderUtility::DrawSpriteToScreen(corners, rect);

    rect.y = res.height - rect.h;

    RenderUtility::DrawSpriteToScreen(corners, rect);

    // Draw Right Ring
    rect.x = res.width - rightRing->width();
    rect.y = 0;
    rect.w = rightRing->width();
    rect.h = rightRing->height();
    RenderUtility::DrawSpriteToScreen(rightRing, rect);

    rect.w = xDrawRing->width() / 3;
    rect.h = xDrawRing->height() / 3;
    rect.x = res.width - rect.w - 22;
    rect.y = -7;

    xDrawAngles += 200 * Time::deltaTime();
    if(xDrawAngles > 360)
        xDrawAngles -= xDrawAngles;

    RenderUtility::DrawSpriteToScreen(xDrawRing, rect, xDrawAngles * Math::deg2rad);

    // Draw Healt Point
    rect.x = uiData->rect.x;
    rect.y = uiData->rect.y;
    rect.w = Math::Map(WGame::current->player->healthPoint, 0, WGame::current->player->maxHealthPoint, 0, uiData->rect.w + 40);
    rect.h = extentHealth->height();
    RenderUtility::DrawSpriteExtent(extentHealth, rect);

    // Draw Armory Point
    rect.x = uiData->rect.x + 15;
    rect.y = uiData->rect.y + 2;
    rect.w = Math::Map(WGame::current->player->armoryPoint, 0, WGame::current->player->maxArmoryPoint, 0, uiData->rect.w + 20);
    rect.h = extentArmory->height();
    RenderUtility::DrawSpriteExtent(extentArmory, rect);

    // Draw Foreground Bar
    rect.x = uiData->rect.x;
    rect.y = uiData->rect.y;
    rect.w = foreground->width();
    rect.h = foreground->height();
    RenderUtility::DrawSpriteToScreen(foreground, rect);
}

void UIPushBoxLayer::OnDestroy()
{
}
