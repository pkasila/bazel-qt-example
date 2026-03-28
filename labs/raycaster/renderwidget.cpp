#include "renderwidget.h"
#include <QPainter>
#include <QPainterPath>
#include <QBrush>
#include <QPen>
#include <QColor>

RenderWidget::RenderWidget(Controller* controller, QWidget *parent)
    : QWidget(parent)
    , controller_(controller)
    , mode_(InteractionMode::Light)
    , is_drawing_polygon_(false) {
    
    setMouseTracking(true);
    setFocusPolicy(Qt::StrongFocus);
}

void RenderWidget::paintEvent(QPaintEvent *event) {
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    painter.fillRect(rect(), Qt::black);
    
    drawScene(painter);
}

void RenderWidget::mousePressEvent(QMouseEvent *event) {
    QPoint pos = event->pos();
    
    if (mode_ == InteractionMode::Polygons) {
        if (event->button() == Qt::LeftButton) {
            if (!is_drawing_polygon_) {
                controller_->AddPolygon(Polygon({pos}));
                is_drawing_polygon_ = true;
            } else {
                controller_->AddVertexToLastPolygon(pos);
            }
        } else if (event->button() == Qt::RightButton) {
            is_drawing_polygon_ = false;
        }
    } else if (mode_ == InteractionMode::StaticLights) {
        if (event->button() == Qt::LeftButton) {
            auto light_sources = controller_->GetLightSources();
            light_sources.push_back(pos);
            controller_->SetLightSources(light_sources);
        }
    }
    
    update();
}

void RenderWidget::mouseMoveEvent(QMouseEvent *event) {
    mouse_position_ = event->pos();
    
    if (mode_ == InteractionMode::Light) {
        controller_->SetLightSource(mouse_position_);
    } else if (mode_ == InteractionMode::Polygons && is_drawing_polygon_) {
        controller_->UpdateLastPolygon(mouse_position_);
    }
    
    update();
}

void RenderWidget::keyPressEvent(QKeyEvent *event) {
    if (mode_ == InteractionMode::Light) {
        QPoint current_light = controller_->GetLightSource();
        int step = 5;
        
        switch (event->key()) {
            case Qt::Key_Up:
                current_light.setY(current_light.y() - step);
                break;
            case Qt::Key_Down:
                current_light.setY(current_light.y() + step);
                break;
            case Qt::Key_Left:
                current_light.setX(current_light.x() - step);
                break;
            case Qt::Key_Right:
                current_light.setX(current_light.x() + step);
                break;
            default:
                QWidget::keyPressEvent(event);
                return;
        }
        
        controller_->SetLightSource(current_light);
        update();
    } else {
        QWidget::keyPressEvent(event);
    }
}

void RenderWidget::clearAll() {
    controller_ = new Controller();
    is_drawing_polygon_ = false;
    update();
}

void RenderWidget::drawScene(QPainter& painter) {
    drawLightAreas(painter);
    drawPolygons(painter);
    drawLightSources(painter);
    drawCurrentPolygon(painter);
}

void RenderWidget::drawPolygons(QPainter& painter) {
    QPen pen(Qt::white, 2);
    painter.setPen(pen);
    painter.setBrush(Qt::NoBrush);
    
    const auto& polygons = controller_->GetPolygons();

    for (size_t i = 1; i < polygons.size(); ++i) {
        const auto& vertices = polygons[i].getVertices();
        if (vertices.size() >= 2) {
            for (size_t j = 0; j < vertices.size(); ++j) {
                size_t next_j = (j + 1) % vertices.size();
                painter.drawLine(vertices[j], vertices[next_j]);
            }
        }
    }
}

void RenderWidget::drawLightSources(QPainter& painter) {
    QPoint light_pos = controller_->GetLightSource();
    painter.setPen(QPen(Qt::yellow, 3));
    painter.setBrush(QBrush(Qt::yellow));
    painter.drawEllipse(light_pos, 8, 8);

    const auto& static_lights = controller_->GetLightSources();
    for (const auto& light : static_lights) {
        painter.setPen(QPen(Qt::cyan, 2));
        painter.setBrush(QBrush(Qt::cyan));
        painter.drawEllipse(light, 6, 6);
    }
}

void RenderWidget::drawLightAreas(QPainter& painter) {
    if (mode_ == InteractionMode::Light || mode_ == InteractionMode::StaticLights) {
        if (mode_ == InteractionMode::StaticLights && !controller_->GetLightSources().empty()) {
            auto light_areas = controller_->CreateLightAreas();
            painter.setPen(QPen(Qt::NoPen));
            
            for (const auto& area : light_areas) {
                const auto& vertices = area.getVertices();
                if (vertices.size() >= 3) {
                    QPainterPath path;
                    path.moveTo(vertices[0]);
                    for (size_t i = 1; i < vertices.size(); ++i) {
                        path.lineTo(vertices[i]);
                    }
                    path.closeSubpath();
                    
                    painter.setBrush(QBrush(QColor(255, 255, 0, 50)));
                    painter.drawPath(path);
                }
            }
        } else {
            Polygon light_area = controller_->CreateLightArea();
            const auto& vertices = light_area.getVertices();
            
            if (vertices.size() >= 3) {
                QPainterPath path;
                path.moveTo(vertices[0]);
                for (size_t i = 1; i < vertices.size(); ++i) {
                    path.lineTo(vertices[i]);
                }
                path.closeSubpath();
                
                painter.setPen(QPen(Qt::NoPen));
                painter.setBrush(QBrush(QColor(255, 255, 0, 100)));
                painter.drawPath(path);
            }
        }
    }
}

void RenderWidget::drawCurrentPolygon(QPainter& painter) {
    if (mode_ == InteractionMode::Polygons && is_drawing_polygon_) {
        const auto& polygons = controller_->GetPolygons();
        if (!polygons.empty()) {
            const auto& vertices = polygons.back().getVertices();
            if (!vertices.empty()) {
                QPen pen(Qt::green, 2, Qt::DashLine);
                painter.setPen(pen);
                painter.setBrush(Qt::NoBrush);
                
                for (size_t i = 0; i < vertices.size(); ++i) {
                    size_t next_i = (i + 1) % vertices.size();
                    painter.drawLine(vertices[i], vertices[next_i]);
                }

                painter.setPen(QPen(Qt::green, 1));
                painter.setBrush(QBrush(Qt::green));
                for (const auto& vertex : vertices) {
                    painter.drawEllipse(vertex, 3, 3);
                }
            }
        }
    }
}
