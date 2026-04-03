#include "canvas.h"
#include <QPainter>
#include <QMouseEvent>
#include <QMenu>
#include <QAction>
#include <QColorDialog>
#include <cmath>

Canvas::Canvas(QWidget *parent) 
    : QWidget(parent), m_mode(LIGHT_MODE), m_drawing_polygon(false), m_dragged_light_idx(-1) 
{
    setMouseTracking(true); 
    
    Polygon bounds;
    bounds.AddVertex(QPointF(0, 0));
    bounds.AddVertex(QPointF(3000, 0));
    bounds.AddVertex(QPointF(3000, 3000));
    bounds.AddVertex(QPointF(0, 3000));
    m_controller.AddPolygon(bounds);

    m_controller.AddLight(QPointF(200, 200), Qt::white);
}

void Canvas::setMode(Mode mode) {
    m_mode = mode;
    m_dragged_light_idx = -1;
    if (mode == LIGHT_MODE && m_drawing_polygon) {
        m_drawing_polygon = false;
        m_controller.FinalizeLastPolygon();
    }
    update();
}

void Canvas::mousePressEvent(QMouseEvent *event) {
    QPointF pos = event->pos();
    if (m_mode == LIGHT_MODE) {
        if (event->button() == Qt::LeftButton) {
            if (m_dragged_light_idx != -1) m_dragged_light_idx = -1;
            else {
                const auto& lights = m_controller.GetLights();
                for (size_t i = 0; i < lights.size(); ++i) {
                    if (Distance(lights[i].pos, pos) < 15.0) {
                        m_dragged_light_idx = static_cast<int>(i);
                        break;
                    }
                }
            }
        } else if (event->button() == Qt::RightButton) {
            int clicked_idx = -1;
            const auto& lights = m_controller.GetLights();
            for (size_t i = 0; i < lights.size(); ++i) {
                if (Distance(lights[i].pos, pos) < 15.0) {
                    clicked_idx = static_cast<int>(i);
                    break;
                }
            }

            QMenu menu(this);
            if (clicked_idx != -1) {
                QAction* changeCol = menu.addAction("Змяніць колер");
                QAction* deleteLt = menu.addAction("Выдаліць");
                
                QAction* selected = menu.exec(event->globalPosition().toPoint());
                
                if (selected == changeCol) {
                    QColor col = QColorDialog::getColor(lights[clicked_idx].color, this);
                    if (col.isValid()) m_controller.SetLightColor(clicked_idx, col);
                } else if (selected == deleteLt) {
                    m_controller.RemoveLight(clicked_idx);
                    m_dragged_light_idx = -1;
                }
            } else {
                if (!m_controller.IsPointInAnyPolygon(pos)) {
                    QAction* addLt = menu.addAction("Дадаць ліхтар");
                    if (menu.exec(event->globalPosition().toPoint()) == addLt) {
                        m_controller.AddLight(pos);
                    }
                }
            }
        }
    } else {
        if (event->button() == Qt::LeftButton) {
            if (!m_drawing_polygon) {
                Polygon p; p.AddVertex(pos); p.AddVertex(pos);
                m_controller.AddPolygon(p);
                m_drawing_polygon = true;
            } else {
                m_controller.UpdateLastPolygon(pos);
                m_controller.AddVertexToLastPolygon(pos);
            }
        } else if (event->button() == Qt::RightButton && m_drawing_polygon) {
            m_drawing_polygon = false;
            m_controller.FinalizeLastPolygon();
        }
    }
    update();
}

void Canvas::mouseMoveEvent(QMouseEvent *event) {
    if (m_mode == LIGHT_MODE && m_dragged_light_idx != -1) {
        if (!m_controller.IsPointInAnyPolygon(event->pos())) {
            m_controller.SetLightPos(m_dragged_light_idx, event->pos());
            update();
        }
    } else if (m_mode == POLYGON_MODE && m_drawing_polygon) {
        m_controller.UpdateLastPolygon(event->pos());
        update();
    }
}

QPolygonF Canvas::convertToQPolygonF(const Polygon& poly) const {
    QPolygonF qp;
    for (const auto& v : poly.GetVertices()) qp << v;
    return qp;
}

void Canvas::paintEvent(QPaintEvent *) {
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    QColor paperColor(245, 240, 225); 
    painter.fillRect(rect(), paperColor);

    QImage shadowMap(size(), QImage::Format_ARGB32_Premultiplied);
    shadowMap.fill(Qt::transparent);
    QPainter sPainter(&shadowMap);
    sPainter.setRenderHint(QPainter::Antialiasing);

    QColor darkness(20, 20, 25, 220); 
    sPainter.fillRect(rect(), darkness);

    sPainter.setCompositionMode(QPainter::CompositionMode_DestinationOut);

    const auto& lights = m_controller.GetLights();
    for (const auto& light : lights) {
        std::vector<QPointF> cluster;
        cluster.push_back(light.pos);
        double r_soft = 12.0; 
        for (int i = 0; i < 8; ++i) {
            double a = i * (M_PI / 4.0);
            cluster.push_back(light.pos + QPointF(cos(a)*r_soft, sin(a)*r_soft));
        }

        sPainter.setBrush(QColor(255, 255, 255, 40)); 
        sPainter.setPen(Qt::NoPen);

        for (const auto& lp : cluster) {
            if (m_controller.IsPointInAnyPolygon(lp)) continue;
            Polygon area = m_controller.CreateLightArea(lp);
            sPainter.drawPolygon(convertToQPolygonF(area));
        }
    }
    sPainter.end();

    painter.drawImage(0, 0, shadowMap);

    const auto& polys = m_controller.GetPolygons();
    for (size_t i = 1; i < polys.size(); ++i) {
        if (i == polys.size() - 1 && m_drawing_polygon) {
            painter.setPen(QPen(Qt::blue, 1, Qt::DashLine));
            painter.setBrush(QColor(100, 100, 100, 50));
        } else {
            painter.setPen(QPen(QColor(40, 35, 30), 2)); 
            painter.setBrush(QColor(70, 65, 60)); 
        }
        painter.drawPolygon(convertToQPolygonF(polys[i]));
    }

    for (size_t i = 0; i < lights.size(); ++i) {
        bool isDragged = (static_cast<int>(i) == m_dragged_light_idx);
        painter.setPen(QPen(isDragged ? Qt::red : Qt::black, 1));
        painter.setBrush(lights[i].color);
        painter.drawEllipse(lights[i].pos, 6, 6);
        painter.setBrush(Qt::white);
        painter.drawEllipse(lights[i].pos, 2, 2);
    }

    painter.setPen(Qt::black);
    painter.drawText(10, height() - 15, "Рэжым: " + QString(m_mode == LIGHT_MODE ? "Святло" : "Сцены"));
}