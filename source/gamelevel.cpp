#include "gamelevel.h"
struct {
    uid quitButton;
    uid clickButton;
    uid restore = -1;
    uid text;
} mids;

NavMesh navigation(1000, 1000);

GameLevel::GameLevel() : Level("ZNake Game Level") {}

void callback(uid id, void* userData) {
    if (id == mids.quitButton)
        Application::RequestQuit();
    else if (id == mids.clickButton) {
        Rect r = guiInstance->getRect(id);
        r.w = 200;
        guiInstance->setRect(id, r);
        auto damaged = Level::matrixCheckDamaged();
        guiInstance->setText(id, "Damaged: " + std::to_string(damaged.size()));

        if (mids.restore == -1) {
            r.x += r.w;
            r.w = 100;
            mids.restore = guiInstance->Push_Button("Restore", r);
        }

        guiInstance->setVisible(mids.restore, true);

    } else if (id == mids.restore) {
        guiInstance->setVisible(mids.restore, false);
        Level::matrixRestore();
    }
}

std::vector<GameObject*> apples;
Player* player;
SpriteRenderer* radar;
void GameLevel::start() {
    // create a menu
    Texture* appleTexture = GC::GetTexture("apple");
    if (true) {
        Rect t(0, 0, 100, 32);
        mids.quitButton = guiInstance->Push_Button("Quit", t);
        t.x += t.x + t.w;
        mids.clickButton = guiInstance->Push_Button("Check", t);
        t.x += t.x + t.w;

        t.w = t.h = 32;

        guiInstance->Push_TextureStick(appleTexture, t);

        guiInstance->Register_Callback(&callback, nullptr);

        Resolution res = Application::getResolution();
        t.w = 200;
        t.x = res.width - t.w;
        mids.text = guiInstance->Push_Button("", t);
    }
    // instance games
    Texture* floorTexture = GC::GetTexture("concrete");

    GameObject* floor = CreateGameObject("Floor");
    SpriteRenderer* view = floor->addComponent<SpriteRenderer>();
    view->setSpriteFromTextureToGC(floorTexture);
    view->size = Vec2::one * 7;
    // view->renderType = SpriteRenderType::Tile;
    view->renderPresentMode = SpriteRenderPresentMode::Place;
    view->transform()->position(Vec2::infinity);

    GameObject* playerGameObject = CreateGameObject("Player");
    player = playerGameObject->addComponent<Player>();
    // player->playerCamera->visibleObjects = true;  // show objects in level
    player->spriteRenderer->transform()->layer = 100;

    GameObject * radarObject = CreateGameObject("Radar");
    radarObject->transform()->setParent(player->transform());
    radar = radarObject->addComponent<SpriteRenderer>();
    radar->setSpriteFromTextureToGC(GC::GetTexture("radar"));
    radar->size = Vec2::one *1;

    SpriteRenderer* tail = CreateGameObject("Tail")->addComponent<SpriteRenderer>();

//    Transform* t = Instantiate(playerGameObject)->transform();
//    t->position(Vec2::one);
//    t->angle(135);
//    t = Instantiate(playerGameObject)->transform();
//    t->position(Vec2::minusOne);
//    t->angle(360 - 135);
//    t = Instantiate(playerGameObject)->transform();
//    t->position(Vec2::up + Vec2::left);
//    t->angle(270);
//    t = Instantiate(playerGameObject)->transform();
//    t->position(Vec2::down + Vec2::right);
//    t->angle(90);

    // ?????????????? N ????????????
    int n = 10000;
    int x;
    float range = 15;
    GameObject* appleObject = CreateGameObject("apple");
    SpriteRenderer* view2 = appleObject->addComponent<SpriteRenderer>();
    view2->setSpriteFromTextureToGC(appleTexture);
    view2->renderPresentMode = SpriteRenderPresentMode::Fixed;
    view2->size = Vec2::one * 2;
    apples.reserve(n + 1);
    apples.emplace_back(appleObject);
    for (x = 0; x < n; ++x) {
        appleObject = Instantiate(appleObject);
        appleObject->transform()->position(Vec2(Random::range(-range, range), Random::range(-range, range)));
        apples.emplace_back(appleObject);
    }
}

int score = 0;

void GameLevel::update() {
    char mv[100];

    int culled, full;
    Level::render_info(&culled, &full);
    guiInstance->setText(mids.text, "Render: " + std::to_string(full - culled));

    if (Time::frame() % 30 == 0) {
        auto xx = matrixCheckDamaged().size();
        guiInstance->setText(mids.clickButton, "Damaged " + std::to_string(xx));
    }
    auto cmpnt = player->gameObject()->getComponents<SpriteRenderer>();
    // cmpnt.back()->offsetFromWorldPosition(Camera2D::ScreenToWorldPoint(input::getMousePointF()));

    std::string t;
    t = "Score: ";
    t += std::to_string(score);
    guiInstance->setText(mids.text, t);

    auto v = Camera::mainCamera()->transform()->position();

    Camera::mainCamera()->transform()->position(Vec2::Lerp(v, player->transform()->position(), 0.05f * Time::deltaTime()));

    return;
    apples[0]->transform()->position(Camera2D::ScreenToWorldPoint(input::getMousePointF()));

    if (apples.size() == 1) return;

    for (int i = 1; false && i < apples.size(); ++i) {
        apples[i]->transform()->position(Vec2(Random::range(-10, 10), Random::range(-10, 10)));
    }
}

void GameLevel::onDrawGizmos() {
    Gizmos::Draw2DWorldSpace(Vec2::zero);

    float distance = 3;
    Gizmos::setColor(0x88241dff);

    static std::list<Transform*> finded;
    int target_n = 0;

    if (finded.size() < 8) {
        finded.merge(Physics2D::sphereCast(player->transform()->position(), distance));
    }

    for (int x = 0; x < Math::min((int)finded.size(), 8); ++x) {
        Transform* t = nullptr;
        Transform* f = finded.front();
        finded.pop_front();
        if (f->gameObject()->name() != "apple (clone)") continue;

        f->transform()->position(
            Vec2::MoveTowards(f->transform()->position(), player->transform()->position(), 1 * Time::deltaTime()));
        if (!t && Vec2::Distance(player->transform()->position(), f->position()) < 0.3f) t = f;

        // Draw point destroyer
        // Gizmos::DrawLine(f->position(), player->transform()->position());

        if (t) {
            Vec2 newPoint;
            Vec2 j = Camera::ViewportToWorldPoint(Vec2::zero);
            Vec2 k = Camera::ViewportToWorldPoint(Vec2::one);
            newPoint.x = Math::outside(Random::range(-50.f, 50.f), j.x, k.x);
            newPoint.y = Math::outside(Random::range(-50.f, 50.f), j.y, k.y);
            t->position(newPoint);
            ++score;
        } else {
            ++target_n;
        }
    }

    // draw line

    static float angle = 360;

    if (angle < 1) angle += 360;

    Vec2 sidePoint = player->transform()->position();
    ;
    sidePoint.x += Math::cos(angle * Math::Deg2Rad) * distance;
    sidePoint.y += Math::sin(angle * Math::Deg2Rad) * distance;
    radar->transform()->angle(angle);
    angle -= 1;

    Gizmos::setColor(target_n ? Color::red : Color::green);
    Gizmos::DrawLine(player->transform()->position(), sidePoint);
    Gizmos::DrawCircle(player->transform()->position(), distance);

    Gizmos::DrawStorm(Vec2::zero, Math::number(Math::ceil(distance)));
}
