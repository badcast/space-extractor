#include "SpaceMainMenu.h"

using namespace RoninEngine::Runtime;
namespace ai = RoninEngine::AIPathFinder;
struct {
    Color defaultNeuron = Color(0xfff6f723);
    Color defaultBackground = Color(0xff1e1e1e);
    Color deafultFore = Color(0xff90791C);
    Color line = Color::red;
    Color pointStart = Color::red;

    Vec2 cellSize = Vec2(0.1, 0.1);
    float cellThickness = 0.05f;
} navMeshSchemes;

SectorLine::SectorLine()
    : World("Sector Line Level")
{
}
void rrandom_space(uid but, NavMesh* navMesh)
{
    // do
    navMesh->randomize(Random::range(0, 1201214ul));
}
void SectorLine::on_start()
{
    switch_game_level(this);
    Primitive::create_camera2D()->game_object()->add_component<MoveController2D>();
    navMesh.worldScale = Vec2::scale(navMesh.worldScale, navMeshSchemes.cellSize);

    get_gui()->set_resources(get_gui()->push_button("random", Vec2Int(0, 48), (ui_callback*)&rrandom_space), &navMesh);
    navMesh.randomize(Random::range(323, 45));
}

void SectorLine::on_update() { }

void SectorLine::on_gizmo()
{
    static Vec2 alpha = (Vec2::up + Vec2::right) * 2;
    static float angle = 0;
    static bool draw = true;

    if (Input::is_mouse_up(2)) {
        draw = !draw;
    }

    if (!draw)
        return;

    ai::NavResult result;
    Vec2 first = Camera::viewport_to_world(Vec2::half);
    Vec2 last = Vec2::rotate_around(first, alpha, angle * Math::deg2rad);

    if (TimeEngine::frame() % 10 == 0) {
        angle += TimeEngine::deltaTime() * 300;
        if (angle > 360.f)
            angle -= 360.f;

    } else
        navMesh.clear();
    navMesh.find(result, ai::NavMethodRule::NavigationIntelegency, first, last);

    auto neuron = navMesh.get_neuron(first);
    if (neuron)
        neuron->h = 1;

    // Draw nav mesh
    Gizmos::draw_nav_mesh(&navMesh);

    Gizmos::set_color(Color::red);
    Gizmos::draw_circle(first, 0.3f);

    Gizmos::set_color(Color::red);
    Gizmos::draw_circle(last, 0.2f);

    Gizmos::set_color(Color::red);
    if (result.status == ai::NavStatus::Opened) {
        first = navMesh.point_to_world_position(result.firstNeuron);
        Gizmos::set_color(navMeshSchemes.pointStart);
        Gizmos::draw_circle(first, navMesh.worldScale.x);
        Gizmos::set_color(navMeshSchemes.line);
        for (auto n = ++result.RelativePaths.begin(); n != result.RelativePaths.end(); ++n) {
            Gizmos::draw_line(first, navMesh.point_to_world_position(*n));
            first = navMesh.point_to_world_position(*n);
        }
    }

    Gizmos::draw_text(Vec2::zero, "RoninEngine Test");
}
