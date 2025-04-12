#include "canvas.h"

#include <QPainter>
#include <QMouseEvent>
//#include <algorithm>
#include <QGraphicsSceneMouseEvent>
#include <QPainterPath>

/*
void drawRaw() {
    Ray r(a, b);
    std::vector<std::optional<std::pair<QPointF, float>>> vector;
    auto wi = painter->window().width();
    auto he = painter->window().height();
    QPointF BottomRight(wi, he), BottomLeft(0, he), TopRight(wi, 0), TopLeft(0, 0);
    auto p1 = r.intersection(BottomRight, BottomLeft);
    auto p2 = r.intersection(BottomLeft, TopLeft);
    auto p3 = r.intersection(TopLeft, TopRight);
    auto p4 = r.intersection(TopRight, BottomRight);
}*/

Canvas::Canvas(QWidget *parent)
    : QWidget{parent}
{
    setMouseTracking(true);
    setAttribute(Qt::WA_OpaquePaintEvent);
    setAttribute(Qt::WA_NoSystemBackground);
    controller.getLight().setX(50);
    controller.getLight().setY(50);
    //controller.getL() = {};

    for (int i = 0; i < controller.getL().size(); ++i) {
        float angle = 2 * M_PI * i / controller.getL().size();
        float x = controller.getLight().x() + 20.0f * cos(angle);
        float y = controller.getLight().y() + 20.0f * sin(angle);
        controller.getL()[i] = QPointF(x, y);
    }
}

void Canvas::paintEvent(QPaintEvent* event) {
    QPainter painter(this);
    //qDebug() << painter.window().height() << " "  << painter.window().width();
    painter.fillRect(rect(), QColor(Qt::black));
    painter.setPen(QPen(Qt::white, 3));
    for (int j = 0; j < controller.GetPolygons().size(); ++j) {
        auto pol = controller.GetPolygons()[j];
        /*if (isNew) {
            for (int i = 0; i < pol.getVertices().size(); ++i) {
                painter.drawLine(pol.getVertices()[i], pol.getVertices()[i + 1]);
            }
        } else {
            for (int i = 0; i < pol.getVertices().size(); ++i) {
                painter.drawLine(pol.getVertices()[i], pol.getVertices()[(i + 1) % pol.getVertices().size()]);
            }
        }*/
        if (pol.getVertices().size() <= 1) continue;
        for (int i = 0; i < pol.getVertices().size(); ++i) {
            painter.drawLine(pol.getVertices()[i], pol.getVertices()[(i + 1) % pol.getVertices().size()]);
        }
    }


    // отрисовка
    for (int i = 0; i < controller.getBoundedL().size(); ++i) {
        QColor c("#FFE340");
        c.setAlpha(100);
        QPointF light = controller.getBoundedL()[i].first;
        float len = controller.getBoundedL()[i].second;
        if (len <= 1e-6) {
            continue;
        }
        auto rays = controller.CastRaysBoundedLights()[i];
        controller.IntersectRaysB(&rays, len);
        Polygon p = controller.CreateLightArea(&rays);
        painter.setBrush(c);
        painter.setPen(Qt::NoPen);
        FillPolygon(p, painter);
        float mainRadius = 20.0f;
        //float dotRadius = 3.0f;
        c.setAlpha(50);
        for (int j = 0; j < 15; ++j) {
            float angle = 2 * M_PI * j / 15;
            float x = light.x() + mainRadius * cos(angle);
            float y = light.y() + mainRadius * sin(angle);
            if (x <= 0) {
                x = 1;
            }
            if (y <= 1) {
                y = 1;
            }
            if (y >= height() - 1) {
                y = height() - 1;
            }
            if (x >= width() - 1) {
                x = width() - 1;
            }
            auto lightl = QPointF(x, y);
            std::vector<Ray> ans;
            float he = 532;
            float wi = 790;
            QPointF BottomRight(wi, he), BottomLeft(0, he), TopRight(wi, 0), TopLeft(0, 0);
            Ray r(lightl, TopLeft);
            ans.push_back(r);
            ans.push_back(r.Rotate(0.0001));
            ans.push_back(r.Rotate(-0.0001));
            r.setEnd(TopRight);
            ans.push_back(r);
            ans.push_back(r.Rotate(0.0001));
            ans.push_back(r.Rotate(-0.0001));
            r.setEnd(BottomLeft);
            ans.push_back(r);
            ans.push_back(r.Rotate(0.0001));
            ans.push_back(r.Rotate(-0.0001));
            r.setEnd(BottomRight);
            ans.push_back(r);
            ans.push_back(r.Rotate(0.0001));
            ans.push_back(r.Rotate(-0.0001));
            for (auto p : controller.GetPolygons()) {
                for (int i = 0; i < p.getVertices().size(); ++i) {
                    Ray r(lightl, p.getVertices()[i]);
                    ans.push_back(r);
                    ans.push_back(r.Rotate(0.0001));
                    ans.push_back(r.Rotate(-0.0001));
                }
            }
            for (int i = 0; i < 180; ++i) {
                ans.push_back(r.Rotate(M_PI * i * 2 / 180));
            }
            controller.IntersectRaysB(&ans, len);
            //controller.RemoveAdjacentRays(&rays);
            Polygon p = controller.CreateLightArea(&ans);
            painter.setBrush(c);
            painter.setPen(Qt::NoPen);
            FillPolygon(p, painter);
        }
    }

    for (int i = 0; i < controller.getLights().size(); ++i) {
        QColor c("#FFE340");
        c.setAlpha(100);
        auto light = controller.getLights()[i];
        auto rays = controller.CastRaysLights()[i];
        controller.IntersectRays(&rays);
        //controller.RemoveAdjacentRays(&rays);
        Polygon p = controller.CreateLightArea(&rays);
        painter.setBrush(c);
        painter.setPen(Qt::NoPen);
        FillPolygon(p, painter);
        //painter.setPen(Qt::blue);
        //painter.drawEllipse(light, 5, 5);
        //std::vector<QPointF> help;
        //help.resize(10);

        float mainRadius = 20.0f;
        //float dotRadius = 3.0f;

        //painter.setPen(Qt::NoPen);
        //painter.setBrush(Qt::blue);
        c.setAlpha(50);
        for (int j = 0; j < 10; ++j) {
            float angle = 2 * M_PI * j / 10;
            float x = light.x() + mainRadius * cos(angle);
            float y = light.y() + mainRadius * sin(angle);
            if (x <= 0) {
                x = 1;
            }
            if (y <= 1) {
                y = 1;
            }
            if (y >= height() - 1) {
                y = height() - 1;
            }
            if (x >= width() - 1) {
                x = width() - 1;
            }
            //painter.setPen(QPen(Qt::blue));
            //painter.drawEllipse(QPointF(x, y), dotRadius, 5);
            //help[j] = QPointF(x, y);
            auto lightl = QPointF(x, y);
            std::vector<Ray> ans;
            float he = 532;
            float wi = 790;
            QPointF BottomRight(wi, he), BottomLeft(0, he), TopRight(wi, 0), TopLeft(0, 0);
            Ray r(lightl, TopLeft);
            ans.push_back(r);
            ans.push_back(r.Rotate(0.0001));
            ans.push_back(r.Rotate(-0.0001));
            r.setEnd(TopRight);
            ans.push_back(r);
            ans.push_back(r.Rotate(0.0001));
            ans.push_back(r.Rotate(-0.0001));
            r.setEnd(BottomLeft);
            ans.push_back(r);
            ans.push_back(r.Rotate(0.0001));
            ans.push_back(r.Rotate(-0.0001));
            r.setEnd(BottomRight);
            ans.push_back(r);
            ans.push_back(r.Rotate(0.0001));
            ans.push_back(r.Rotate(-0.0001));
            for (auto p : controller.GetPolygons()) {
                for (int i = 0; i < p.getVertices().size(); ++i) {
                    Ray r(lightl, p.getVertices()[i]);
                    ans.push_back(r);
                    ans.push_back(r.Rotate(0.0001));
                    ans.push_back(r.Rotate(-0.0001));
                }
            }
            controller.IntersectRays(&ans);
            //controller.RemoveAdjacentRays(&rays);
            Polygon p = controller.CreateLightArea(&ans);
            painter.setBrush(c);
            painter.setPen(Qt::NoPen);
            FillPolygon(p, painter);
        }

    }
    //if (controller.mode == 0) {

    //}

    // если скраю то нету света. Полигоны ставлю свет полигоны и границы рушатс - fixed

    // работа со светом
    if (controller.mode == 0) {
        QColor c("#FFE340");
        auto light = controller.getLight();
        auto rays = controller.CastRaysLight();
        controller.IntersectRays(&rays);
        //controller.RemoveAdjacentRays(&rays);
        Polygon p = controller.CreateLightArea(&rays);
        /*for (auto ray : rays) {
            painter.drawLine(ray.getBegin(), ray.getEnd());
        }*/
        //painter.setPen(Qt::red);
        /*for (auto& el : p.getVertices()) {
            painter.drawEllipse(el, 5, 5);
        }*/


        // отрисовка полигона
        auto vertices = p.getVertices();
        //painter.setBrush(QColor(0, 0, 0, 0));
        /*if (light.x() <= 10 || light.y() <= 10) {
            painter.setBrush(c);
            painter.setPen(Qt::NoPen);
            FillPolygon(p, painter);
            c.setAlpha(50);
        } else {
            c.setAlpha(50);
            painter.setBrush(c);
            painter.setPen(Qt::NoPen);
            FillPolygon(p, painter);
            c.setAlpha(30);
        }*/
        c.setAlpha(100);
        painter.setBrush(c);
        painter.setPen(Qt::NoPen);
        FillPolygon(p, painter);
        c.setAlpha(50);
        for (int i = 0; i < controller.getL().size(); ++i) {
            auto light = controller.getL()[i];
            auto rays = controller.CastRaysL()[i];
            controller.IntersectRays(&rays);
            //controller.RemoveAdjacentRays(&rays);
            Polygon p = controller.CreateLightArea(&rays);
            painter.setBrush(c);
            painter.setPen(Qt::NoPen);
            FillPolygon(p, painter);
        }

        painter.setPen(QPen(QColor("#FFF9D1"), 5));
        painter.drawEllipse(light, 5, 5);

        //float mainRadius = 20.0f;
        float dotRadius = 3.0f;

        painter.setPen(Qt::NoPen);
        painter.setBrush(QColor("#FFF9D1"));

        for (int i = 0; i < controller.getL().size(); ++i) {
            //float angle = 2 * M_PI * i / controller.getL().size();
            //float x = light.x() + mainRadius * cos(angle);
            //float y = light.y() + mainRadius * sin(angle);
            painter.drawEllipse(controller.getL()[i], dotRadius, dotRadius);
        }
    }
    painter.setPen(QPen(Qt::white, 5));
    for (auto pol : controller.GetPolygons()) {
        if (pol.getVertices().size() <= 1) continue;
        for (int i = 0; i < pol.getVertices().size(); ++i) {
            //painter.setPen(QPen(Qt::black, 5));
            painter.drawLine(pol.getVertices().at(i), pol.getVertices().at((i + 1) % pol.getVertices().size()));
        }
    }
    painter.setPen(Qt::NoPen);
    painter.setBrush(QColor("#FFF9D1"));
    for (int i = 0; i < controller.getLights().size(); ++i) {
        painter.drawEllipse(controller.getLights()[i], 5, 5);
        for (int j = 0; j < 10; ++j) {
            float angle = 2 * M_PI * j / 10;
            float x = controller.getLights()[i].x() + 20.0f * cos(angle);
            float y = controller.getLights()[i].y() + 20.0f * sin(angle);
            if (x <= 0) {
                x = 1;
            }
            if (y <= 1) {
                y = 1;
            }
            if (y >= height() - 1) {
                y = height() - 1;
            }
            if (x >= width() - 1) {
                x = width() - 1;
            }
            painter.drawEllipse(QPointF(x, y), 3, 3);
        }
    }
    painter.setPen(QPen(QColor("#FFF9D1"), 5));
    for (const auto& light :controller.getLights()) {
        painter.drawEllipse(light, 5, 5);
    }
    for (const auto& light :controller.getBoundedL()) {
        painter.drawEllipse(light.first, 5, 5);
    }
    if (controller.mode == 2) {
        if (!isNewB) {
            auto n = controller.getBoundedL().back().first;
            float len = std::sqrt((s.x() - n.x()) * (s.x() - n.x()) + (s.y() - n.y()) * (s.y() - n.y()));
            painter.setBrush(Qt::NoBrush);
            painter.drawEllipse(n, len + 10, len + 10);
        }
    }
}

void Canvas::mouseMoveEvent(QMouseEvent* event) {
    if (controller.mode == 0) {
        //controller.getLight() = event->pos();
        /*if (controller.getLight().x() == 0) {
            controller.getLight().setX(1);
        }
        if (controller.getLight().y() == 0) {
            controller.getLight().setY(1);
        }*/
        QPointF pos = event->pos();
        auto pos1 = pos;

        if (pos.x() <= 1) pos.setX(1);
        if (pos.y() <= 1) pos.setY(1);
        if (pos.x() >= width() - 1) pos.setX(width() - 1);
        if (pos.y() >= height() - 1) pos.setY(height() - 1);
        controller.getLight() = pos;

        float mainRadius = 20.0f;
        //float dotRadius = 3.0f;

        for (int i = 0; i < controller.getL().size(); ++i) {
            float angle = 2 * M_PI * i / controller.getL().size();
            float x = controller.getLight().x() + mainRadius * cos(angle);
            float y = controller.getLight().y() + mainRadius * sin(angle);
            if (x <= 0) {
                x = 1;
            }
            if (y <= 1) {
                y = 1;
            }
            if (y >= height() - 1) {
                y = height() - 1;
            }
            if (x >= width() - 1) {
                x = width() - 1;
            }
            controller.getL()[i] = QPointF(x, y);
        }
        //controller.getLight() = pos1;
    }
    if (controller.mode == 2) {
        if (!isNewB) {
            s = event->pos();
        } else {
            s = event->pos() + QPointF(1, 1);
        }
    }
    update();
}

void Canvas::mousePressEvent(QMouseEvent *event) {
    if (controller.mode == 1) {
        if (event->button() == Qt::LeftButton) {
            if (isNew) {
                Polygon p;
                controller.AddPolygon(p);
                controller.AddVertexToLastPolygon(event->pos());
                isNew = false;
            } else {
                controller.AddVertexToLastPolygon(event->pos());
            }
        } else {
            isNew = true;
        }
    }

    if (controller.mode == 0) {
        if (event->button() == Qt::LeftButton) {
            QPointF pos = event->pos();

            if (pos.x() <= 1) pos.setX(1);
            if (pos.y() <= 1) pos.setY(1);
            if (pos.x() >= width() - 1) pos.setX(width() - 1);
            if (pos.y() >= height() - 1) pos.setY(height() - 1);
            controller.getLights().push_back(pos);
        }
    }

    if (controller.mode == 2) {
        if (event->button() == Qt::LeftButton) {
            if (isNewB) {
                controller.getBoundedL().push_back(std::make_pair(event->pos(), 0));
                isNewB = false;
            } else {
                QPointF p = event->pos();
                QPointF O = controller.getBoundedL().back().first;
                float len = std::sqrt((p.x() - O.x()) * (p.x() - O.x()) + (p.y() - O.y()) * (p.y() - O.y()));
                controller.getBoundedL().back().second = len;
                isNewB = true;
            }
        }
    }
    qDebug() << event->pos();
    update();
}
/*
void Canvas::mouseDoubleClickEvent(QMouseEvent* event) {
    qDebug() << "TWO";
    if (controller.mode == 2) {
        if (!isDoubleClick) {
            qDebug() << "DoubleClick";
            isDoubleClick = true;
            controller.getBoundedL().push_back(std::make_pair(event->pos(), 0));
            QWidget::mouseDoubleClickEvent(event);
            update();
        } else {
            controller.getBoundedL().pop_back();
            qDebug() << "NEWDoubleClick!!!";
            isDoubleClick = true;
            controller.getBoundedL().push_back(std::make_pair(event->pos(), 0));
            QWidget::mouseDoubleClickEvent(event);
            update();
        }
    }
}*/

void Canvas::FillPolygon(Polygon& polygon, QPainter& painter) {
    QPainterPath path;
    std::vector<QPointF> verticies = polygon.getVertices();

    if (verticies.empty()) {
        return;
    }
    path.moveTo(verticies[0]);
    for (int i = 1; i < verticies.size(); i++) {
        path.lineTo(verticies[i]);
    }
    path.closeSubpath();
    painter.drawPath(path);
}
