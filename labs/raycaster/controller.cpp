#include "controller.h"
#include "polygon.h"

#include <vector>

#include <QPointF>
#include <vector>
#include <QPainter>
#include <QDebug>
#include <QPainterPath>



Controller::Controller() {
    // std::vector<QPointF> fictive = {QPointF(0, 0), QPointF(width(), 0), QPointF(width(), height()), QPointF(0, height()), QPointF(0, 0)};
    // polygons.push_back(Polygon(fictive));
}

const std::vector<Polygon>& Controller::GetPolygons() {
    return polygons;
}

void Controller::AddPolygon(const Polygon& polygon) {
    polygons.back().FinishPolygon();
    polygons.push_back(polygon);
    repaint();
}

void Controller::AddVertexToLastPolygon(const QPointF& new_vertex) {
    if (polygons.size() == 0) {
        polygons.push_back(Polygon());
    }
    polygons.back().AddVertex(new_vertex);
    repaint();
}

void Controller::UpdateLastPolygon(const QPointF& new_vertex) {
    polygons.back().UpdateLastVertex(new_vertex);
}

void Controller::paintEvent(QPaintEvent*) {
    QPainter painter;
    painter.begin(this);

    for (auto& polygon : polygons) {
        for (int i = 1; i < polygon.GetVertices().size(); ++i) {
            int x1 = polygon.GetVertices()[i - 1].x();
            int y1 = polygon.GetVertices()[i - 1].y();
            int x2 = polygon.GetVertices()[i].x();
            int y2 = polygon.GetVertices()[i].y();
            painter.drawLine(x1, y1, x2, y2);
        }
    }

    if (mode == 1) {
        painter.fillRect(rect(), QColor(255, 204, 51, 160));
        for (auto light : light_sources) {
            light_source = light;
            auto rays = CastRays();
            IntersectRays(rays);
    
            // for (auto &ray : rays) {
            //     QPointF end = ray.MakeEnd();
            //     painter.drawLine(ray.GetBegin(), end);
            // }
    
            Polygon polygon_for_brushing = CreateLightArea();
            std::vector<QPointF> vertices = polygon_for_brushing.GetVertices();
    
            QPolygonF polygon;
            for (auto point : vertices) {
                polygon << point;
            }
    
            painter.setBrush(QBrush(QColor(0, 0, 0, 40)));
            painter.setPen(Qt::NoPen);
    
            painter.drawPolygon(polygon);
        }
    }

    painter.end();
}




void Controller::mousePressEvent(QMouseEvent* event) {
    if (event->button() == Qt::LeftButton && mode == 0) {
        AddVertexToLastPolygon(event->pos());
    } else if (mode == 0) {
        AddPolygon(Polygon());
    }
    repaint();
}

void Controller::mouseMoveEvent(QMouseEvent* event) {
    if (mode == 1) {
        light_sources.clear();
        light_source = event->pos();
        light_sources.push_back(light_source);
        
        light_source.setX(light_source.x() + 5);
        light_source.setY(light_source.y() + 5);
        light_sources.push_back(light_source);

        light_source.setY(light_source.y() - 10);
        light_sources.push_back(light_source);

        light_source.setX(light_source.x() - 10);
        light_sources.push_back(light_source);

        light_source.setY(light_source.y() + 10);
        light_sources.push_back(light_source);
    }
    repaint();
}

void Controller::SetMode(int index) {
    mode = index;
}

std::vector<Ray> Controller::CastRays() {
    std::vector<Ray> rays;
    for (auto &polygon : polygons) {
        for (auto point : polygon.GetVertices()) {
            Ray ray(light_source, point, 0);
            rays.push_back(ray);
            rays.push_back(ray.Rotate(0.0001));
            rays.push_back(ray.Rotate(-0.0001));
        }
    }

    return rays;
}

void Controller::IntersectRays(std::vector<Ray>& rays) {
    for (auto& ray : rays) {
        for (auto polygon : polygons) {
            double intersection_t = polygon.IntersectRay(ray);
            if (ray.GetT() > intersection_t) {
                ray.SetT(intersection_t);
            }
        }
    }
}

Polygon Controller::CreateLightArea() {
    std::vector<QPointF> vertices;
    auto rays = CastRays();
    IntersectRays(rays);

    std::sort(rays.begin(), rays.end(), [this](const Ray& a, const Ray& b) {
        QPointF da = a.MakeEnd() - light_source;
        QPointF db = b.MakeEnd() - light_source;
        return std::atan2(da.y(), da.x()) < std::atan2(db.y(), db.x());
    });

    for (auto ray : rays) {
        vertices.push_back(ray.MakeEnd());
    }

    Polygon polygon(vertices);
    return polygon;
}



