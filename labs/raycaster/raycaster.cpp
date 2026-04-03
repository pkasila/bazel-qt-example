#include "raycaster.h"
#include <QPainter>
#include <QPen>
#include <QBrush>
#include <QColor>
#include <QComboBox>
#include <QMouseEvent>
#include <QCursor>
#include <cmath>

Raycaster::Raycaster(QWidget* parent)
    : QWidget(parent)
    , mode_(Mode::Polygons)
    , controller_(1400, 1000) {
    
    setFixedSize(1500, 1100);
    setMouseTracking(true);
    
    modeCombo_ = new QComboBox(this);
    modeCombo_->addItem("Polygons");
    modeCombo_->addItem("Light");
    modeCombo_->addItem("Soft Shadows");
    modeCombo_->addItem("Static Lights");
    modeCombo_->setGeometry(10, 10, 120, 25);
    
    connect(modeCombo_, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &Raycaster::onModeChanged);
}

void Raycaster::onModeChanged(int index) {
    switch (index) {
        case 0: mode_ = Mode::Polygons; break;
        case 1: mode_ = Mode::Light; break;
        case 2: mode_ = Mode::SoftShadows; break;
        case 3: mode_ = Mode::StaticLights; break;
    }
    update();
}

void Raycaster::paintEvent(QPaintEvent* event) {
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    
    painter.fillRect(rect(), QColor(50, 50, 50));
    
    painter.setPen(QPen(Qt::white, 2));
    painter.setBrush(QBrush(QColor(30, 30, 30)));
    painter.drawRect(0, 0, controller_.getWidth(), controller_.getHeight());
    
    if (mode_ == Mode::SoftShadows) {
        drawSoftShadows(painter);
    } else if (mode_ == Mode::Light) {
        drawLightArea(painter);
    } else if (mode_ == Mode::StaticLights) {
        drawStaticLights(painter);
    }
    
    drawPolygons(painter);
    drawCurrentPolygon(painter);
    
    if (mode_ == Mode::SoftShadows) {
        drawMultipleLightSources(painter);
    } else if (mode_ == Mode::Light) {
        drawLightSource(painter);
    } else if (mode_ == Mode::StaticLights) {
        for (const auto& light : controller_.getStaticLights()) {
            painter.setBrush(QBrush(Qt::yellow));
            painter.setPen(QPen(Qt::white, 2));
            painter.drawEllipse(light, STATIC_LIGHT_RADIUS, STATIC_LIGHT_RADIUS);
        }
    }
}

void Raycaster::drawLightArea(QPainter& painter) {
    if (controller_.getLightSource().x() < 0) return;
    
    Polygon lightArea = controller_.CreateLightArea(controller_.getLightSource());
    
    if (lightArea.getVertices().size() >= 3) {
        QPolygon polygon;
        for (const auto& vertex : lightArea.getVertices()) {
            polygon << vertex;
        }
        
        painter.setBrush(QBrush(QColor(255, 255, 0, 100)));
        painter.setPen(Qt::NoPen);
        painter.drawPolygon(polygon);
    }
}

void Raycaster::drawSoftShadows(QPainter& painter) {
    auto lightAreas = controller_.CreateMultipleLightAreas();
    
    for (const auto& lightArea : lightAreas) {
        if (lightArea.getVertices().size() >= 3) {
            QPolygon polygon;
            for (const auto& vertex : lightArea.getVertices()) {
                polygon << vertex;
            }
            
            painter.setBrush(QBrush(QColor(255, 255, 0, 20)));
            painter.setPen(Qt::NoPen);
            painter.drawPolygon(polygon);
        }
    }
}

void Raycaster::drawStaticLights(QPainter& painter) {
    auto lightAreas = controller_.CreateStaticLightAreas();
    
    // Рисуем области освещения статических источников
    for (const auto& lightArea : lightAreas) {
        if (lightArea.getVertices().size() >= 3) {
            QPolygon polygon;
            for (const auto& vertex : lightArea.getVertices()) {
                polygon << vertex;
            }
            painter.setBrush(QBrush(QColor(255, 255, 0, 20)));
            painter.setPen(Qt::NoPen);
            painter.drawPolygon(polygon);
        }
    }
}

void Raycaster::drawPolygons(QPainter& painter) {
    painter.setBrush(QBrush(QColor(100, 100, 100, 200)));
    painter.setPen(QPen(Qt::white, 2));
    
    for (const auto& polygon : controller_.GetPolygons()) {
        if (polygon.getVertices().size() < 3) continue;
        
        QPolygon qpoly;
        for (const auto& vertex : polygon.getVertices()) {
            qpoly << vertex;
        }
        painter.drawPolygon(qpoly);
        
        painter.setBrush(QBrush(Qt::red));
        painter.setPen(QPen(Qt::white, 1));
        for (const auto& vertex : polygon.getVertices()) {
            painter.drawEllipse(vertex, VERTEX_RADIUS, VERTEX_RADIUS);
        }
        painter.setBrush(QBrush(QColor(100, 100, 100, 200)));
        painter.setPen(QPen(Qt::white, 2));
    }
}

void Raycaster::drawCurrentPolygon(QPainter& painter) {
    const auto& vertices = controller_.GetCurrentPolygonVertices();
    if (vertices.empty()) return;
    
    painter.setPen(QPen(Qt::yellow, 2, Qt::DashLine));
    painter.setBrush(Qt::NoBrush);
    
    QPolygon qpoly;
    for (const auto& vertex : vertices) {
        qpoly << vertex;
    }
    painter.drawPolygon(qpoly);
    
    painter.setBrush(QBrush(Qt::green));
    painter.setPen(QPen(Qt::white, 1));
    for (const auto& vertex : vertices) {
        painter.drawEllipse(vertex, VERTEX_RADIUS, VERTEX_RADIUS);
    }
    
    if (!vertices.empty() && mode_ == Mode::Polygons) {
        painter.setPen(QPen(Qt::yellow, 1, Qt::DashLine));
        painter.drawLine(vertices.back(), mapFromGlobal(QCursor::pos()));
    }
}

void Raycaster::drawLightSource(QPainter& painter) {
    painter.setBrush(QBrush(Qt::yellow));
    painter.setPen(QPen(Qt::white, 2));
    painter.drawEllipse(controller_.getLightSource(), LIGHT_RADIUS, LIGHT_RADIUS);
}

void Raycaster::drawMultipleLightSources(QPainter& painter) {
    const auto& sources = controller_.getMultipleLightSources();
    
    for (size_t i = 0; i < sources.size(); i++) {
        if (i == 0) {
            painter.setBrush(QBrush(Qt::yellow));
            painter.setPen(QPen(Qt::white, 2));
            painter.drawEllipse(sources[i], LIGHT_RADIUS, LIGHT_RADIUS);
        } else {
            painter.setBrush(QBrush(QColor(255, 255, 0, 150)));
            painter.setPen(QPen(Qt::white, 1));
            painter.drawEllipse(sources[i], LIGHT_RADIUS - 2, LIGHT_RADIUS - 2);
        }
    }
}

void Raycaster::mousePressEvent(QMouseEvent* event) {
    if (event->pos().x() > controller_.getWidth() || 
        event->pos().y() > controller_.getHeight()) {
        return;
    }
    
    if (event->button() == Qt::LeftButton) {
        if (mode_ == Mode::Polygons) {
            controller_.AddVertexToLastPolygon(event->pos());
            update();
        } else if (mode_ == Mode::Light) {
            int newX = std::max(0, std::min(event->pos().x(), controller_.getWidth()));
            int newY = std::max(0, std::min(event->pos().y(), controller_.getHeight()));
            controller_.setLightSource(QPoint(newX, newY));
            update();
        } else if (mode_ == Mode::SoftShadows) {
            int newX = std::max(0, std::min(event->pos().x(), controller_.getWidth()));
            int newY = std::max(0, std::min(event->pos().y(), controller_.getHeight()));
            controller_.updateMultipleLightsCenter(QPoint(newX, newY));
            update();
        } else if (mode_ == Mode::StaticLights) {
            controller_.addStaticLight(event->pos());
            update();
        }
    } else if (event->button() == Qt::RightButton) {
        if (mode_ == Mode::Polygons) {
            controller_.FinishCurrentPolygon();
            update();
        } else if (mode_ == Mode::StaticLights) {
            controller_.removeLastStaticLight();
            update();
        }
    }
}

void Raycaster::mouseMoveEvent(QMouseEvent* event) {
    if (mode_ == Mode::Light) {
        int newX = std::max(0, std::min(event->pos().x(), controller_.getWidth()));
        int newY = std::max(0, std::min(event->pos().y(), controller_.getHeight()));
        controller_.setLightSource(QPoint(newX, newY));
        update();
    } else if (mode_ == Mode::SoftShadows) {
        int newX = std::max(0, std::min(event->pos().x(), controller_.getWidth()));
        int newY = std::max(0, std::min(event->pos().y(), controller_.getHeight()));
        controller_.updateMultipleLightsCenter(QPoint(newX, newY));
        update();
    } else if (mode_ == Mode::Polygons && controller_.HasCurrentPolygon()) {
        update();
    }
}

void Raycaster::keyPressEvent(QKeyEvent* event) {
    QPoint step(0, 0);
    int delta = 5;
    
    switch (event->key()) {
        case Qt::Key_Left: step = QPoint(-delta, 0); break;
        case Qt::Key_Right: step = QPoint(delta, 0); break;
        case Qt::Key_Up: step = QPoint(0, -delta); break;
        case Qt::Key_Down: step = QPoint(0, delta); break;
        default: QWidget::keyPressEvent(event); return;
    }
    
    if (mode_ == Mode::Light) {
        QPoint newPos = controller_.getLightSource() + step;
        newPos.setX(std::max(0, std::min(newPos.x(), controller_.getWidth())));
        newPos.setY(std::max(0, std::min(newPos.y(), controller_.getHeight())));
        controller_.setLightSource(newPos);
        update();
    } else if (mode_ == Mode::SoftShadows) {
        QPoint newCenter = controller_.getMultipleLightSources()[0] + step;
        newCenter.setX(std::max(0, std::min(newCenter.x(), controller_.getWidth())));
        newCenter.setY(std::max(0, std::min(newCenter.y(), controller_.getHeight())));
        controller_.updateMultipleLightsCenter(newCenter);
        update();
    }
}