#include "gamelevel.h"

void callback(uid id, void* userData) { }

GameObject* referenceObject;

std::vector<GameObject*> v(100);
void fabricate()
{
    float t = 0;
    for (int x = 0; x < v.size(); ++x) {
        v[x] = instantiate(referenceObject);
        v[x]->transform()->position(Random::randomVector() * 2);
        destroy(v[x], 1);
    }
}
uid slider;
void GameLevel::start()
{
    auto camera = Primitive::CreateCamera2D(Vec2::zero);
    camera->visibleNames = true;
    camera->visibleGrids = true;
    camera->distanceEvcall = 1;
    camera->gameObject()->add_component<Player>();

    referenceObject = Primitive::CreateEmptyGameObject();

    auto spr = referenceObject->add_component<SpriteRenderer>();
    spr->setSprite(Primitive::CreateSprite2D({ 50, 50 }, Color::antiquewhite));

    slider = ui->Push_Slider(v.size(), 1.f, 100000.f, Vec2Int(0, 30));

    ui->Push_Button("cancel all", { 700, 0, 80, 15 });
}

void GameLevel::update()
{
    //    static Vec2 t;
    //    Vec2 pos = obj->transform()->position();

    //    if(obj->transform()->position(Vec2::MoveTowards(pos,t,TimeEngine::deltaTime()*.05f)) == t){
    //        t = Camera::ViewportToWorldPoint(Random::randomVector()/2);
    //    }
    static float t = 0;
    float* sv = (float*)ui->getResources(slider);
    if (*sv != static_cast<float>(v.size())) {
        v.resize(static_cast<int>(*sv));
    }
    if (TimeEngine::time() > t) {
        fabricate();
        t = TimeEngine::time() + 2;
    }
}

void GameLevel::onDrawGizmos()
{
    static int v = 0;
    static int n = 0;
    static int vc = 0;
    if (get_destroyed_frames() != 0)
        vc += (v = get_destroyed_frames());

    if (TimeEngine::frame() % 60 == 0) {
        vc = 0;
    } else
        n += vc / TimeEngine::frame();

    Gizmos::DrawTextOnPosition(Camera::ScreenToWorldPoint(Vec2::zero), std::string("Destroyed queue: ") + std::to_string(object_destruction_count()));
    Gizmos::DrawTextOnPosition(Camera::ScreenToWorldPoint(Vec2::down * -20), std::string("Destroyed 1(frame): ") + std::to_string(v) + " destroyed 60(frames): " + std::to_string(n));

    auto res = Application::get_resolution();
    int time = TimeEngine::time();
    Gizmos::DrawTextOnPosition(Camera::ScreenToWorldPoint(Vec2(10,res.height-20)), ( std::string((time/60 < 10) ? "0" : "") + std::to_string(time / 60) + std::string(":") + std::string((time%60 < 10) ? "0" : "") + std::to_string(time%60) ) );
}
