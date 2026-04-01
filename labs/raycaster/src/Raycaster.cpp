#include "Raycaster.h"

#include "FPSCounter.h"
#include "GraphicsView.h"
#include "Polygon.h"
#include "Ray.h"
#include "utils.h"

#include <QBrush>
#include <QColorDialog>
#include <QGraphicsPathItem>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QHBoxLayout>
#include <QInputDialog>
#include <QKeySequence>
#include <QLabel>
#include <QMainWindow>
#include <QMessageBox>
#include <QPainterPath>
#include <QPen>
#include <QPoint>
#include <QRadialGradient>
#include <QRadioButton>
#include <QShortcut>
#include <QString>
#include <QTimer>
#include <QWidget>
#include <cmath>
#include <cstddef>
#include <tuple>

Raycaster::Raycaster(QWidget* parent) : QMainWindow(parent) {
    auto* central = new QWidget;

    setCentralWidget(central);

    auto* layout = new QVBoxLayout;

    auto* top_layout_ = new QHBoxLayout;

    light_mode_ = new QRadioButton("Light");
    polygon_mode_ = new QRadioButton("Polygon");
    static_lights_mode_ = new QRadioButton("Static Lights");

    light_mode_->setChecked(true);

    fps_counter_ = new FPSCounter;

    top_layout_->addWidget(new QLabel("Mode: "));
    top_layout_->addWidget(light_mode_);
    top_layout_->addWidget(polygon_mode_);
    top_layout_->addWidget(static_lights_mode_);
    top_layout_->addStretch(1);
    top_layout_->addWidget(fps_counter_);

    connect(light_mode_, &QRadioButton::toggled, this, &Raycaster::LightModeToggled);
    connect(polygon_mode_, &QRadioButton::toggled, this, &Raycaster::PolygonModeToggled);
    connect(static_lights_mode_, &QRadioButton::toggled, this, &Raycaster::StaticLightsModeToggled);

    layout->addLayout(top_layout_);

    view_ = new GraphicsView;
    view_->setMinimumSize(800, 600);

    scene_ = new QGraphicsScene;
    scene_->setSceneRect(0, 0, 800, 600);

    controller_.AddPolygon(
        Polygon({QPointF(0, 0), QPointF(0, 600), QPointF(800, 600), QPointF(800, 0)}));
    controller_.SetLightSource({400, 300});

    Render();

    view_->setScene(scene_);

    connect(view_, &GraphicsView::MouseMoved, this, &Raycaster::MouseMoved);
    connect(view_, &GraphicsView::MousePressed, this, &Raycaster::MousePressed);
    connect(view_, &GraphicsView::ViewScaled, this, [this](const QPointF& scale) {
        controller_.Scale(scale);
    });

    const auto* clear_view_shortcut = new QShortcut(QKeySequence("Ctrl+R"), this);
    connect(clear_view_shortcut, &QShortcut::activated, this, [this] {
        while (controller_.GetPolygons().size() > 1) {
            controller_.RemoveLastPolygon();
        }
        if (mode_ == InputModes::Polygon) {
            PolygonModeToggled(true);
        }
    });

    const auto* remove_last_polygon_shortcut = new QShortcut(QKeySequence("Ctrl+Z"), this);
    connect(remove_last_polygon_shortcut, &QShortcut::activated, this, [this] {
        switch (mode_) {
            case InputModes::Polygon:
                if (controller_.GetPolygons().back().GetVertices().empty()) {
                    controller_.RemoveLastPolygon();
                }
                if (controller_.GetPolygons().size() > 1) {
                    controller_.RemoveLastPolygon();
                }
                if (mode_ == InputModes::Polygon) {
                    PolygonModeToggled(true);
                }
                break;
            case InputModes::StaticLights:
                if (!controller_.GetStaticLights().empty()) {
                    controller_.RemoveLastStaticLight();
                }
                break;
            default:;
        }
    });

    const auto* end_polygon_shortcut = new QShortcut(QKeySequence("Ctrl+D"), this);
    connect(end_polygon_shortcut, &QShortcut::activated, this, [this] {
        MousePressed({}, Qt::RightButton);
    });

    const auto* clear_static_lights_shortcut = new QShortcut(QKeySequence("Alt+R"), this);
    connect(clear_static_lights_shortcut, &QShortcut::activated, this, [this] {
        while (!controller_.GetStaticLights().empty()) {
            controller_.RemoveLastStaticLight();
        }
    });

    const auto* swap_mode_shortcut = new QShortcut(QKeySequence("Tab"), this);
    connect(swap_mode_shortcut, &QShortcut::activated, this, [this] {
        switch (mode_) {
            case InputModes::Light:
                polygon_mode_->toggle();
                break;
            case InputModes::Polygon:
                static_lights_mode_->toggle();
                break;
            case InputModes::StaticLights:
                light_mode_->toggle();
                break;
            default:;
        }
    });

    refresh_timer_ = new QTimer;
    connect(refresh_timer_, &QTimer::timeout, this, [this] { Render(); });
    refresh_timer_->setTimerType(Qt::PreciseTimer);
    refresh_timer_->start(1);

    layout->addWidget(view_);

    central->setLayout(layout);
}

void Raycaster::LightModeToggled(bool checked) {
    if (!checked) {
        return;
    }
    mode_ = InputModes::Light;
}

void Raycaster::PolygonModeToggled(bool checked) {
    if (!checked) {
        MousePressedPolygon({}, Qt::RightButton);
        controller_.RemoveLastPolygon();
        return;
    }
    mode_ = InputModes::Polygon;
    controller_.AddPolygon({}, PolygonType::Creating);
}

void Raycaster::StaticLightsModeToggled(bool checked) {
    if (!checked) {
        return;
    }
    mode_ = InputModes::StaticLights;
}

void Raycaster::MouseMoved(const QPointF& scene_pos) {
    switch (mode_) {
        case InputModes::Light:
            MouseMovedLight(scene_pos);
            break;
        default:;
    }
}

void Raycaster::MousePressed(const QPointF& scene_pos, Qt::MouseButton button) {
    switch (mode_) {
        case InputModes::Polygon:
            MousePressedPolygon(scene_pos, button);
            break;
        case InputModes::StaticLights:
            MousePressedStaticLights(scene_pos, button);
            break;
        default:;
    }
}

void Raycaster::Render() const {
    scene_->clear();
    DrawPolygons();
    if (mode_ == InputModes::Polygon) {
        BuildingPolygon();
    }
    DrawLightArea();
    DrawLight();
}

void Raycaster::DrawPolygon(const Polygon& polygon, const QPen& pen, const QBrush& brush) const {
    const auto& vertices = polygon.GetVertices();
    if (vertices.empty()) {
        return;
    }
    QPainterPath path;
    path.moveTo(vertices[0]);
    for (size_t i = 1; i < vertices.size(); ++i) {
        path.lineTo(vertices[i]);
    }
    switch (polygon.GetType()) {
        case PolygonType::Creating:
            path.addEllipse(vertices.back(), 2, 2);
            break;
        case PolygonType::Finished:
            path.closeSubpath();
            break;
        default:;
    }
    scene_->addPath(path, pen, brush);
}

void Raycaster::DrawPolygons() const {
    scene_->clear();

    const auto& polygons = controller_.GetPolygons();

    DrawPolygon(polygons[0], QPen(Qt::transparent), QBrush(Qt::black));
    for (size_t i = 1; i < polygons.size() - 1; i++) {
        DrawPolygon(polygons[i], QPen(QColor(255, 77, 1)), QBrush(Qt::transparent));
    }
    switch (polygons.back().GetType()) {
        case PolygonType::Creating:
            DrawPolygon(polygons.back(), QPen(QColor(255, 77, 1)), QBrush(Qt::lightGray));
            break;
        case PolygonType::Finished:
            DrawPolygon(polygons.back(), QPen(QColor(255, 77, 1)), QBrush(Qt::transparent));
            break;
        default: ;
    }
}

void Raycaster::DrawLight() const {
    QPainterPath path;
    for (const auto& delta_light : controller_.GetDeltaLights()) {
        path.clear();
        const auto light = delta_light + controller_.GetLightSource();
        path.addEllipse(light, controller_.GetLightRadius(), controller_.GetLightRadius());
        scene_->addPath(path, QPen(Qt::darkRed), QBrush(Qt::darkRed));
    }
    for (const auto& [light, color, radius] : controller_.GetStaticLights()) {
        path.clear();
        path.addEllipse(light, controller_.GetLightRadius(), controller_.GetLightRadius());
        scene_->addPath(
            path, QPen(Qt::black), QBrush(QColor(color.red(), color.green(), color.blue())));
    }
}

void Raycaster::DrawLightArea() const {
    if (mode_ == InputModes::Polygon) {
        return;
    }
    for (const auto& delta_light : controller_.GetDeltaLights()) {
        const auto light = delta_light + controller_.GetLightSource();
        const auto light_area = controller_.CreateLightArea(light);
        DrawPolygon(light_area, QPen(Qt::transparent), QBrush(QColor(255, 255, 255, 52)));
    }
    for (const auto& [light, color, radius] : controller_.GetStaticLights()) {
        const auto& light_area = controller_.CreateLightArea(light);

        QRadialGradient gradient(light, radius);
        gradient.setColorAt(0, color);
        QColor transparent_color = color;
        transparent_color.setAlpha(0);
        gradient.setColorAt(1, transparent_color);

        DrawPolygon(light_area, QPen(Qt::transparent), QBrush(gradient));
    }
}

void Raycaster::MouseMovedLight(const QPointF& scene_pos) {
    if (controller_.CanPlaceLight(scene_pos)) {
        controller_.SetLightSource(scene_pos);
    }
}

void Raycaster::BuildingPolygon() const {
    const QPoint& view_pos = view_->mapFromGlobal(QCursor::pos());
    const QPointF& scene_pos = view_->mapToScene(view_pos);
    const int radius = 3;

    QPainterPath path;
    path.addEllipse(scene_pos, radius, radius);
    auto last_vertex = scene_pos;
    if (!controller_.GetPolygons().back().GetVertices().empty()) {
        last_vertex = controller_.GetPolygons().back().GetVertices().back();
        path.moveTo(last_vertex);
        path.lineTo(scene_pos);
    }
    scene_->addPath(path, QPen(QColor(211, 211, 211, 128)), QBrush(QColor(0, 0, 0, 128)));
    can_place_vertex_ = controller_.CanAddLastPolygonVertex(scene_pos);
    if (!can_place_vertex_) {
        path.clear();
        const double diagonal = 2 * radius;
        path.moveTo(scene_pos.x() - diagonal, scene_pos.y() - diagonal);
        path.lineTo(scene_pos.x() + diagonal, scene_pos.y() + diagonal);
        path.moveTo(scene_pos.x() - diagonal, scene_pos.y() + diagonal);
        path.lineTo(scene_pos.x() + diagonal, scene_pos.y() - diagonal);
        scene_->addPath(path, QPen(Qt::red));
    }
}

void Raycaster::MousePressedPolygon(const QPointF& scene_pos, Qt::MouseButton button) {
    if (button == Qt::RightButton) {
        can_place_vertex_ = true;
        if (controller_.GetPolygons().back().GetVertices().size() > 2) {
            const auto& last_edge_begin = controller_.GetPolygons().back().GetVertices().back();
            const auto& last_edge_end = controller_.GetPolygons().back().GetVertex(0);
            Ray ray(
                last_edge_end, last_edge_begin, utils::GetAngle(last_edge_end, last_edge_begin));
            const double k_little_dist = 1e-8;
            ray = ray.PushBegin(k_little_dist);
            ray.SetEnd(ray.GetBegin());
            ray.SetBegin(last_edge_begin);
            ray.SetAngle(utils::GetAngle(ray.GetBegin(), ray.GetEnd()));
            ray = ray.PushBegin(k_little_dist);
            for (const auto& polygon : controller_.GetPolygons()) {
                if (polygon.IntersectRay(ray).has_value()) {
                    can_place_vertex_ = false;
                    break;
                }
            }
        }
        if (!can_place_vertex_) {
            QMessageBox::warning(
                this, ",jkmit yt ,eltn uhecnyj",
                "After ending polygon will be self-crossing / cross another polygon");
            return;
        }
        controller_.SetLastPolygonType(PolygonType::Finished);
        if (controller_.GetPolygons().back().GetVertices().empty()) {
            controller_.RemoveLastPolygon();
        }
        controller_.AddPolygon({}, PolygonType::Creating);
        return;
    }
    if (!can_place_vertex_) {
        QMessageBox::warning(
            this, ",jkmit yt ,eltn uhecnyj", "Placed inside of another polygon / self-crossed");
        return;
    }
    controller_.AddVertexToLastPolygon(scene_pos);
}

void Raycaster::MousePressedStaticLights(const QPointF& scene_pos, Qt::MouseButton button) {
    if (button == Qt::RightButton) {
        return;
    }
    if (controller_.StaticLightsOverflow()) {
        QMessageBox::warning(this, ",jkmit yt ,eltn uhecnyj", "Too much static lights.");
        return;
    }
    if (!controller_.CanPlaceLight(scene_pos)) {
        QMessageBox::warning(
            this, ",jkmit yt ,eltn uhecnyj",
            "Can't be placed here (inside of polygon / too close to polygon)");
        return;
    }
    refresh_timer_->stop();
    QColor color = QColorDialog::getColor(Qt::white, this, "Choose light color");
    if (!color.isValid()) {
        refresh_timer_->start();
        return;
    }
    color.setAlpha(100);
    bool ok{};
    const int radius =
        QInputDialog::getInt(this, "Enter radius", "Radius: ", 222, 0, INT_MAX, 1, &ok);
    if (!ok) {
        refresh_timer_->start();
        return;
    }
    controller_.AddStaticLight(scene_pos, color, radius);
    refresh_timer_->start();
}
