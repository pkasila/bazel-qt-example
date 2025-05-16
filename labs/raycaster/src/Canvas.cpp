#include "Canvas.h"
#include "Controller.h"

Canvas::Canvas(QWidget* parent)
    : QWidget(parent), currentMode_(Mode::Polygons) {
    currentPolygon_.reserve(100);
    updateAutomaticPolygon();

    for (int i = 0; i < 10; ++i) {
        controllers_.push_back(new Controller(QColor(200, 200, 200, 100)));
    }

    for (const auto& controller_ : controllers_) {
        if (controller_) {
            controller_->AddPolygon(automaticPolygon_);
        }
    }
}


void Canvas::SetController(Controller* controller) {
    delete controllers_[0];
    controllers_[0] = controller;
    if (controllers_[0]) {
        controllers_[0]->ClearPolygons();
        controllers_[0]->AddPolygon(automaticPolygon_);
    }
}

void Canvas::SetMode(Mode mode) {
    currentMode_ = mode;
}
void Canvas::mouseMoveEvent(QMouseEvent* event) {
    if (currentMode_ == Mode::Light) {
        if (!controllers_.empty()) {
            const QPointF basePos = event->pos();
            const int radius = 13;
            const int count = controllers_.size();

            controllers_[0]->SetLightSource(basePos);

            for (int i = 1; i < count; ++i) {
                double angle = (2 * M_PI * (i - 1)) / (count - 1);
                QPointF offset(radius * std::cos(angle), radius * std::sin(angle));
                controllers_[i]->SetLightSource(basePos + offset);
            }
        }
    } else if (drawing_polygon_) {
        const QPointF basePos = event->pos();
        for (auto& controller_ : controllers_) {
            controller_->UpdateLastPolygon(basePos);
        }
    }
    update();
}

void Canvas::mousePressEvent(QMouseEvent* event) {
    if (currentMode_ == Mode::Light) {
        if (!controllers_.empty()) {
            const QPointF basePos = event->pos();
            const int radius = 13;
            const int count = controllers_.size();

            controllers_[0]->SetLightSource(basePos);

            for (int i = 1; i < count; ++i) {
                double angle = (2 * M_PI * (i - 1)) / (count - 1);
                QPointF offset(radius * std::cos(angle), radius * std::sin(angle));
                controllers_[i]->SetLightSource(basePos + offset);
            }
        }
    } else if (currentMode_ == Mode::Polygons) {
        QPoint clickPos = event->pos();
        if (event->button() == Qt::LeftButton) {
            if (!drawing_polygon_) {
                drawing_polygon_ = true;
                Polygon new_poly;
                new_poly.AddVertex(event->pos());
                new_poly.AddVertex(event->pos());
                for (auto& controller_ : controllers_) {
                    controller_->AddPolygon(new_poly);
                }
            } else {
                for (auto& controller_ : controllers_) {
                    controller_->AddVertexToLastPolygon(event->pos());
                }
            }
        } else if (event->button() == Qt::RightButton) {
            drawing_polygon_ = false;
        }
    }
    update();
}


void Canvas::paintEvent(QPaintEvent* event) {
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setPen(QPen(QColor(200, 200, 200, 100), 2));

    const auto& polygons = controllers_.front()->GetPolygons();
    for (const auto& polygon : polygons) {
        const auto& vertices = polygon.GetVertices();
        if (vertices.size() > 1) {
            for (size_t i = 0; i < vertices.size() - 1; ++i) {
                painter.drawLine(vertices[i], vertices[i + 1]);
            }
            painter.drawLine(vertices.back(), vertices.front());
        }
        painter.setBrush(QColor(200, 200, 200, 100));
        painter.setPen(QColor(200, 200, 200, 100));
        for (const QPointF& point : vertices) {
            painter.drawEllipse(point, 3, 3);
        }
    }

    if (!currentPolygon_.empty() && currentPolygon_.size() > 1) {
        for (size_t i = 0; i < currentPolygon_.size() - 1; ++i) {
            painter.drawLine(currentPolygon_[i], currentPolygon_[i + 1]);
        }
        painter.setBrush(QColor(200, 200, 200, 100));
        painter.setPen(Qt::NoPen);
        for (const QPointF& point : currentPolygon_) {
            painter.drawEllipse(point, 3, 3);
        }
    }

    if (currentMode_ == Mode::Light) {
        QPainter painter(this);
        painter.setBrush(controllers_[0]->GetColor());
        painter.setPen(Qt::NoPen);

        for (const auto& controller_ : controllers_) {
            Polygon light_area = controller_->CreateLightArea();
            const auto& vertices = light_area.GetVertices();

            if (vertices.size() >= 2) {
                for (size_t i = 0; i < vertices.size(); ++i) {
                    QPointF p1 = vertices[i];
                    QPointF p2 = vertices[(i + 1) % vertices.size()];
                    QPointF triangle[3] = {controller_->GetLightSource(), p1, p2};
                    painter.drawPolygon(triangle, 3);
                }
            }
        }


        painter.setCompositionMode(QPainter::CompositionMode_SourceOver);
        for(const auto& controller_ : controllers_ ) {
            painter.setRenderHint(QPainter::Antialiasing);
            painter.setBrush(QColor(200, 200, 200, 100));
            painter.setPen(Qt::black);
            painter.drawEllipse(controller_->GetLightSource(), 3.5, 3.5);
        }
    }
}

void Canvas::enterEvent(QEvent* event) {
    setMouseTracking(true);
}

void Canvas::resizeEvent(QResizeEvent* event) {
    QWidget::resizeEvent(event);
    updateAutomaticPolygon();

    for (auto& controller_ : controllers_) {
        if (controller_) {
            controller_->ClearPolygons();
            controller_->AddPolygon(automaticPolygon_);
        }
    }
    update();
}

void Controller::ClearPolygons() {
    polygons_.clear();
}
