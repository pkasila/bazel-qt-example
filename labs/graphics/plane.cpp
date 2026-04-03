#include "plane.h"

#include <QMouseEvent>
#include <QDebug>
#include <QPainter>
#include <QPaintEvent>
#include <QMouseEvent>
#include <QEvent>
#include <QWidget>
#include <QKeyEvent>
#include <QRadialGradient>


Plane::Plane(QWidget *parent) : QWidget(parent) {
    controller_.AddLightSource(LightSource());
    controller_.MainLight().SetPosition(QPointF(1, 1));
    setMouseTracking(true);
    setFocusPolicy(Qt::StrongFocus);
}

Plane::~Plane() {
}

void Plane::mousePressEvent(QMouseEvent *event) {
    controller_.MainLight().SetPosition(event->pos());
    if (controller_.GetMode() == Controller::Mode::Polygons) {
        if (event->button() == Qt::LeftButton) {
            if (!controller_.IsDrawing()) {
                controller_.SetDrawing(true);
                controller_.AddPolygon(Polygon());
            }
            controller_.AddVertexToLastPolygon(event->pos());
        } else {
            controller_.SetDrawing(false);
        }
    } else if (controller_.GetMode() == Controller::Mode::Lamping) {
        if (event->button() == Qt::LeftButton) {
            controller_.AddLightSource(LightSource(event->pos()));
        }
    }
    update();
}

void Plane::mouseMoveEvent(QMouseEvent *event) {
    controller_.MainLight().SetPosition(event->pos());
    if (controller_.GetMode() == Controller::Mode::Light) {
        controller_.SetLightSourceVisibility(true);
        update();
    }
}

void Plane::leaveEvent(QEvent *event) {
    controller_.MainLight().SetPosition(QPointF(-1, -1));
    if (controller_.GetMode() == Controller::Mode::Light) {
        controller_.SetLightSourceVisibility(false);
        update();
    }
}

void Plane::paintEvent(QPaintEvent *event) {
    QPainter painter(this);
    painter.fillRect(rect(), QColor(0, 0, 10));

    if (controller_.IsLightSourceVisible()) {
        for (int it = 1 - static_cast<int>(controller_.IsMainLightSourceVisible()); it < controller_.LightsNumber(); it++) {
            auto& light = controller_.GetLight(it);
            QPointF old_pos_of_light_source = light.Position();

            for (int i = 1; i <= controller_.LampLightsNumber(); i++) {
                double alpha = 2*acos(-1)/controller_.LampLightsNumber()*i;
                light.SetPosition(
                    QPointF(old_pos_of_light_source.x() + controller_.LampRadius()*cos(alpha),
                            old_pos_of_light_source.y() + controller_.LampRadius()*sin(alpha)
                            )
                    );
                auto rays = controller_.CastRays(light.Position());
                controller_.IntersectRays(&rays);
                controller_.SortRaysByAngle(&rays);
                auto light_area = controller_.CreateLightArea(&rays);

                // painter.setRenderHint(QPainter::Antialiasing);
                painter.setPen(Qt::NoPen);
                /*qreal radius = light.LightAreaRadius()*light.Power()/100;
                QRadialGradient gradient(light.Position(), radius);
                gradient.setColorAt(0.0, light.LightColor());
                gradient.setColorAt(1.0, controller_.BackGroundColor())*/;
                painter.setBrush(light.LightColor());

                painter.drawPolygon(light_area.Data(), light_area.GetVerticesNumber());
            }

            light.SetPosition(old_pos_of_light_source);
        }
        for (int it = 1 - static_cast<int>(controller_.IsMainLightSourceVisible()); it < controller_.LightsNumber(); it++) {
            painter.setPen(QPen(Qt::white, 2));
            painter.setBrush(Qt::white);
            painter.drawEllipse(controller_.GetLight(it).Position(), 5, 5);
        }
    }

    painter.setPen(
        controller_.IsPolygonVisible()
            ? QPen(controller_.PolygonBorderColor(), 1)
            : QPen(Qt::NoPen)
        );
    painter.setBrush(QColor(0, 0, 10));

    QPointF prev_vertex;
    for (const auto& polygon : controller_.GetPolygons()) {
        const int sz = polygon.GetVerticesNumber();
        for (int i = 0; i <= sz; i++) {
            auto vertex = polygon.GetVertex(i % sz);
            //painter.drawEllipse(vertex, 5, 5);
            if (i) {
                painter.drawLine(prev_vertex, vertex);
            }
            prev_vertex = vertex;
        }
    }

}

void Plane::keyPressEvent(QKeyEvent *event) {

    switch(event->key()) {
        case Qt::Key_L:
            if (controller_.GetMode() != Controller::Mode::Light) {
                controller_.SetMode(Controller::Mode::Light);
                update();
            }
            break;
        case Qt::Key_P:
            if (controller_.GetMode() != Controller::Mode::Polygons) {
                controller_.SetMode(Controller::Mode::Polygons);
                update();
            }
            break;
        case Qt::Key_C:
            controller_.DeletePolygon();
            controller_.SetDrawing(false);
            update();
            break;
        case Qt::Key_B:
            if (controller_.GetMode() == Controller::Mode::Light) {
                controller_.SetPolygonVisibility(
                    1 - controller_.IsPolygonVisible());
                update();
            }
            break;
        case Qt::Key_T:
            if (controller_.GetMode() != Controller::Mode::Lamping) {
                controller_.SetMode(Controller::Mode::Lamping);
                update();
            }
            break;
        case Qt::Key_D:
            controller_.DeleteLightSource();
            update();
            break;
        default:
            QWidget::keyPressEvent(event);
    }
}
