#include "gamelevel.h"
#include <fstream>

GameObject* referenceObject;

class Movement : public Behaviour
{
public:
    float angle = 0;
    Vec2 target;
    void OnStart() { target = transform()->position(); }
    void OnUpdate()
    {
        if (transform()->position() == target) {
            target = Random::randomVector() * 10;
        }

        transform()->position(Vec2::MoveTowards(transform()->position(), target, TimeEngine::deltaTime() * 0.09f));

        transform()->look_at(target);
    }
};

std::vector<GameObject*> vectorx(1);
void fabricate()
{
    float t = 0;
    for (int x = 0; x < vectorx.size(); ++x) {
        vectorx[x] = instantiate(referenceObject);
        Movement* m = new Movement;
        vectorx[x]->add_component(m);
        destroy(vectorx[x], 5);
    }
}
uid slider;
uid but;
uid textDestr;
uid textDestr2;
uid textGameTime;
uid butSave, butLoad;
const char* fsave = "./pointer.txt";
void callback(uid id, void* userData)
{
    if (id == but) {
        Camera::main_camera()->transform()->position(Vec2::zero);
    } else if (id == butSave) {
        Vec2 p = Camera::main_camera()->transform()->position();
        std::fstream f { fsave, std::ios_base::out};

        f.write((char*)&p.x, sizeof p.x);
        f.write((char*)&p.y, sizeof p.y);
        f.close();
    } else if (id == butLoad) {
        Vec2 p;
        std::fstream f { fsave, std::ios_base::in };
        if (!f) {
            Application::show_message("failed load file");
            return;
        }
        f.read((char*)&p.x, sizeof p.x);
        f.read((char*)&p.y, sizeof p.y);
        f.close();

        Camera::main_camera()->transform()->position(p);
    }
}

void GameLevel::start()
{
    auto camera = Primitive::create_camera2D(Vec2::zero);
    // camera->visibleNames = true;
    camera->visibleGrids = false;
    camera->visibleBorders = false;
    camera->visibleNames = false;
    camera->game_object()->add_component<Player>();

    referenceObject = Primitive::create_empty_game_object();

    auto spr = referenceObject->add_component<SpriteRenderer>();
    spr->set_sprite(Primitive::create_sprite2D({ 5, 5 }, Color::antiquewhite));

    slider = ui->push_slider(vectorx.size(), 1.f, 100.f, Vec2Int(0, 30));

    but = ui->push_button("center camera", { 700, 0, 90, 20 });
    butSave = ui->push_button("save", { 792, 0, 80, 20 });
    butLoad = ui->push_button("load", { 874, 0, 80, 20 });

    ui->register_callback(callback, nullptr);

    textDestr = ui->push_label("", Vec2Int::zero);
    textDestr2 = ui->push_label("", Vec2Int(0, 15));
    auto res = Application::get_resolution();
    textGameTime = ui->push_label("", Vec2Int(0, res.height - 17));
}

void GameLevel::update()
{
    //    static Vec2 t;
    //    Vec2 pos = obj->transform()->position();

    //    if(obj->transform()->position(Vec2::MoveTowards(pos,t,TimeEngine::deltaTime()*.05f)) == t){
    //        t = Camera::ViewportToWorldPoint(Random::randomVector()/2);
    //    }
    static float t = 0;
    static int v = 0;
    static int n = 0;
    float* sv = (float*)ui->get_resources(slider);
    if (*sv != static_cast<float>(vectorx.size())) {
        vectorx.resize(static_cast<int>(*sv));
    }
    if (TimeEngine::time() > t) {
        fabricate();
        t = TimeEngine::time() + 0.3;
    }

    if (get_destroyed_frames() != 0)
        n += (v = get_destroyed_frames());
    if (TimeEngine::frame() % 60 == 0) {
        n = v;
    }

    int time = TimeEngine::time();
    ui->set_text(textDestr, std::string("Destroyed queue: ") + std::to_string(object_destruction_count()));
    ui->set_text(textDestr2, std::string("Destroyed 1(frame): ") + std::to_string(v) + " destroyed 60(frames): " + std::to_string(n));
    ui->set_text(textGameTime, "Game time: " + std::string((time / 60 < 10) ? "0" : "") + std::to_string(time / 60) + std::string(":") + std::string((time % 60 < 10) ? "0" : "") + std::to_string(time % 60));
}

void GameLevel::onDrawGizmos() { }
