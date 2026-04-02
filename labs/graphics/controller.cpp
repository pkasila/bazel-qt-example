#include "controller.h"
#include <QDebug>
#include <exception>

Controller::Controller() {
}

/// polygons ------------------------------------------------------------------

const std::vector<Polygon>& Controller::GetPolygons() {
    return polygons_;
}

void Controller::AddPolygon(const Polygon& new_polygon) {
    polygons_.push_back(new_polygon);
}

void Controller::AddVertexToLastPolygon(const QPointF& new_vertex) {
    if (polygons_.empty()) {
        return;
    }
    polygons_.back().AddVertex(new_vertex);
}

void Controller::UpdateLastPolygon(const QPointF& new_vertex) {
    if (polygons_.empty()) {
        return;
    }
    polygons_.back().UpdateLastVertex(new_vertex);
}

void Controller::DeletePolygon() {
    if (polygons_.size() > 1) {
        polygons_.pop_back();
    }
}

Polygon Controller::CreateLightArea(std::vector<Ray>* rays) {
    Polygon polygon;
    for (const auto& ray : *rays) {
        polygon.AddVertex(ray.End());
    }
    return polygon;
}

void Controller::SetPolygonVisibility(bool new_status) {
    polygons_visibility_ = new_status;
}

bool Controller::IsPolygonVisible() const {
    return polygons_visibility_;
}

/// light source -----------------------------------------------------

// void Controller::SetLightSource(QPointF new_pos) {
//     light_source_ = new_pos;
// }

// [[nodiscard]]QPointF Controller::GetLightSource() const {
//     return light_source_;
// }

void Controller::AddLightSource(const LightSource& new_light_source) {
    lights_.push_back(new_light_source);
}

void Controller::DeleteLightSource() {
    if (lights_.size() > 1) {
        lights_.pop_back();
    }
}

LightSource& Controller::MainLight() {
    return lights_[0];
}

LightSource& Controller::GetLight(int index) {
    if (index < 0 || index >= lights_.size()) {
        throw std::runtime_error("invalid index lights");
    }
    return lights_[index];
}


void Controller::SetLightSourceVisibility(bool state) {
    light_source_visibility_ = state;
}


bool Controller::IsLightSourceVisible() const {
    return light_source_visibility_;
}

void Controller::SetMainLightSourceVisibility(bool new_state) {
    main_light_source_visibility_ = new_state;
}

bool Controller::IsMainLightSourceVisible() const {
    return main_light_source_visibility_;
}


void Controller::SetMode(Controller::Mode new_mode) {
    switch(new_mode) {
        case Mode::Light:
            SetDrawing(false);
            SetLightSourceVisibility(true);
            SetMainLightSourceVisibility(true);
            break;
        case Mode::Lamping:
            SetDrawing(false);
            SetLightSourceVisibility(true);
            SetMainLightSourceVisibility(false);
            SetPolygonVisibility(true);
            break;
        case Mode::Polygons:
            SetPolygonVisibility(true);
            SetLightSourceVisibility(false);
            break;
    }
    mode_ = new_mode;
}

[[nodiscard]]Controller::Mode Controller::GetMode() const {
    return mode_;
}

void Controller::SetDrawing(bool new_state) {
    is_drawing_polygons = new_state;
}

bool Controller::IsDrawing() const {
    return is_drawing_polygons;
}

int Controller::LightsNumber() const {
    return lights_.size();
}

int Controller::LampLightsNumber() const {
    return LightSource::LightsNumber();
}

double Controller::LampRadius() const {
    return LightSource::LampRadius();
}


QColor Controller::BackGroundColor() const {
    return bg_color_;
}
QColor Controller::LightSourceColor() const {
    return light_source_color_;
}
QColor Controller::PolygonBorderColor() const {
    return polygon_border_color_;
}


// QPointF Controller::Cursor() const {
//     return cursor_;
// }

/// rays ---------------------------------------------------------------


std::vector<Ray> Controller::CastRays(QPointF light_source_position) {
    std::vector<Ray> rays;
    for (const auto& polygon : polygons_) {
        for(int i = 0; i < polygon.GetVerticesNumber(); i++) {
            auto vertex = polygon.GetVertex(i);
            Ray ray(light_source_position, vertex);
            rays.push_back(ray.Rotate(0.0001));
            rays.push_back(ray);
            rays.push_back(ray.Rotate(-0.0001));
        }
    }
    return rays;
}

void Controller::SortRaysByAngle(std::vector<Ray>* rays) {
    std::sort(rays->begin(), rays->end());
}

void Controller::IntersectRays(std::vector<Ray>* rays) {
    for (auto& ray : *rays) {
        for (const auto& polygon : polygons_) {
            auto new_end = polygon.IntersectRay(ray);
            if (new_end != std::nullopt) {
                ray.SetEnd(new_end.value());
            }
        }
    }
}
