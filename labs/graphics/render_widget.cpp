#include "render_widget.h"

namespace raycaster {

RenderWidget::RenderWidget(QWidget* parent) : QWidget(parent) {
    setMinimumSize(800, 600);
    setMouseTracking(true);
}

void RenderWidget::ClearAll() {
    controller_.ClearPolygons();
    controller_.ClearLights();
    is_drawing_ = false;
    update();
}

void RenderWidget::resizeEvent(QResizeEvent* event) {
    controller_.SetBounds(width(), height());
    QWidget::resizeEvent(event);
}

void RenderWidget::paintEvent(QPaintEvent* event) {
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.fillRect(rect(), Qt::black);

    DrawLightArea(painter); // Рисуем все области света
    DrawObstacles(painter); // Рисуем стены

    // Рисуем стационарные лампочки (белые точки)
    painter.setBrush(Qt::white);
    painter.setPen(Qt::NoPen);
    for (const auto& lp : controller_.GetStationaryLights()) {
        painter.drawEllipse(lp, 3, 3);
    }

    // Рисуем активный кластер (красно-белая структура под мышкой)
    if (mode_ == Mode::Light) {
        auto cluster = controller_.GetActiveLightCluster();
        for (size_t i = 0; i < cluster.size(); ++i) {
            painter.setBrush(i == 0 ? Qt::white : Qt::red);
            painter.drawEllipse(cluster[i], 2, 2);
        }
    }
}

void RenderWidget::DrawLightArea(QPainter& painter) {
    if (mode_ != Mode::Light) return;

    // Очень прозрачный цвет, чтобы слои плавно накладывались
    QColor layerColor(255, 255, 180, 30); 
    painter.setBrush(layerColor);
    painter.setPen(Qt::NoPen);

    // --- 1. Отрисовываем стационарные источники (которые уже поставили кликом) ---
    // Для них ставим прозрачность чуть выше (например, 100), так как они одиночные
    painter.setBrush(QColor(255, 255, 180, 100)); 
    for (const auto& src : controller_.GetStationaryLights()) {
        Polygon light_poly = controller_.CreateLightArea(src);
        const auto& v = light_poly.GetVertices();
        if (v.size() >= 3) {
            painter.drawPolygon(v.data(), v.size(), Qt::WindingFill);
        }
    }

    // --- 2. Отрисовываем активный кластер под мышкой (9 слоев) ---
    painter.setBrush(QColor(255, 255, 180, 30)); // Снова очень прозрачный для наложения
    std::vector<QPointF> cluster = controller_.GetActiveLightCluster();

    for (const auto& src : cluster) {
        Polygon light_poly = controller_.CreateLightArea(src);
        const auto& v = light_poly.GetVertices();
        if (v.size() >= 3) {
            painter.drawPolygon(v.data(), v.size(), Qt::WindingFill);
        }
    }
}

void RenderWidget::DrawObstacles(QPainter& painter) {
    painter.setPen(QPen(Qt::cyan, 1)); // Тонкая линия
    painter.setBrush(Qt::NoBrush);

    for (const auto& poly : controller_.GetPolygons()) {
        const auto& v = poly.GetVertices();
        if (v.size() >= 2) {
            painter.drawPolygon(v.data(), v.size());
        }
    }
}

void RenderWidget::mousePressEvent(QMouseEvent* event) {
    QPointF pos = event->position();

    if (mode_ == Mode::Light) {
        if (event->button() == Qt::LeftButton) {
            controller_.AddStationaryLight(pos);
        }
    } else if (mode_ == Mode::Polygons) {
        if (event->button() == Qt::LeftButton) {
            if (!is_drawing_) {
                controller_.StartNewPolygon(pos);
                is_drawing_ = true;
                controller_.AddVertexToLastPolygon(pos); 
            } else {
                controller_.AddVertexToLastPolygon(pos);
            }
        } else if (event->button() == Qt::RightButton) {
            is_drawing_ = false;
        }
    }
    update();
}

void RenderWidget::mouseMoveEvent(QMouseEvent* event) {
    QPointF pos = event->position();
    if (mode_ == Mode::Light) {
        controller_.SetActiveLight(pos);
    } else if (mode_ == Mode::Polygons && is_drawing_) {
        controller_.UpdateLastPolygon(pos);
    }
    update();
}

} // namespace raycaster