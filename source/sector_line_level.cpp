#include "sector_line_level.h"

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
    : Level("Level")
{
}

ai::NavMesh navMesh(128, 128);
void SectorLine::on_start()
{
    Primitive::create_camera2D()->game_object()->add_component<MoveController2D>();
    navMesh.worldScale = Vec2::scale(navMesh.worldScale, navMeshSchemes.cellSize);

    navMesh.randomize(Random::range(323, 45));
}

void SectorLine::on_update() { }

void Plot()
{
    Vec2 lastPoint, a, b;
    AIPathFinder::Neuron* p;
    Runtime::Vec2Int p1, p2;
    Resolution res;
    Color prev, next;
    int yDefault;

    res = Application::get_resolution();
    prev = Gizmos::get_color();
    Gizmos::set_color(next = navMeshSchemes.defaultNeuron);
    p1 = navMesh.WorldPointToPoint(Camera::screen_2_world(Vec2::minusOne));
    p2 = navMesh.WorldPointToPoint(Camera::viewport_2_world(Vec2::one));
    yDefault = p1.y;
    // select draw from viewport neurons
    while (p1.x <= p2.x) {
        while (p1.y <= p2.y) {
            p = navMesh.get_neuron(p1);
            lastPoint = navMesh.PointToWorldPosition(p1);
            if (!p || navMesh.neuronLocked(p1)) {
                next.r = 255;
                next.g = 0;
                next.b = 0;
            } else {
                next.r = 53;
                next.g = navMesh.neuronGetTotal(p1) ? 200 : 0;
                next.b = 246;
            }
            Gizmos::set_color(next);
            Gizmos::draw_fill_rect(lastPoint, navMesh.worldScale.x - navMeshSchemes.cellThickness, navMesh.worldScale.y - navMeshSchemes.cellThickness);
            ++p1.y;
        }
        p1.y = yDefault;
        ++p1.x;
    }
    Gizmos::set_color(prev);
}

void SectorLine::on_gizmo()
{
    static Vec2 alpha = (Vec2::up + Vec2::right) * 2;
    static float angle = 0;

    ai::NavResult result;
    Vec2 first = Camera::viewport_2_world(Vec2::half);
    Vec2 last = Vec2::rotate_around(first, alpha, angle * Math::deg2rad);

    if (Input::is_mouse_down()) {
        auto ner = navMesh.neuronGetPoint(navMesh.get_neuron(Camera::screen_2_world(Input::get_mouse_pointf())));
        navMesh.neuronLock(ner, !navMesh.neuronLocked(ner));
    }

    if (TimeEngine::frame() % 10 == 0) {
        angle += 13;
        if (angle > 360)
            angle -= 360;

    } else
        navMesh.clear();
    navMesh.find(result, ai::NavMethodRule::NavigationIntelegency, first, last);

    auto neuron = navMesh.get_neuron(first);
    if (neuron)
        neuron->h = 1;

    // Draw nav mesh
    Plot();
    Gizmos::set_color(Color::red);
    Gizmos::draw_circle(first, 0.3f);

    Gizmos::set_color(Color::red);
    Gizmos::draw_circle(last, 0.2f);

    Gizmos::set_color(Color::red);
    if (result.status == ai::NavStatus::Opened) {
        first = navMesh.PointToWorldPosition(result.firstNeuron);
        Gizmos::set_color(navMeshSchemes.pointStart);
        Gizmos::draw_circle(first, navMesh.worldScale.x);
        Gizmos::set_color(navMeshSchemes.line);
        for (auto n = ++result.RelativePaths.begin(); n != result.RelativePaths.end(); ++n) {
            Gizmos::draw_line(first, navMesh.PointToWorldPosition(*n));
            first = navMesh.PointToWorldPosition(*n);
        }
    }

    Gizmos::draw_text(Vec2::zero, "RoninEngine Test");
}
