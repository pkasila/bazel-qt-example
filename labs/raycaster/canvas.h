// canvas.h
#include <QWidget>
#include <QMouseEvent>
#include <QPainter>
#include "controller.h"

class Canvas : public QWidget {
    Q_OBJECT
public:
    enum Mode { Light, Polygons };
    Canvas(QWidget* parent = nullptr) : QWidget(parent), mode_(Polygons) {
        setMouseTracking(true);
        // Граничный многоугольник (фиктивный)
        controller_.addPolygon(Polygon({{0,0}, {800,0}, {800,600}, {0,600}}));
    }
    
    void setMode(Mode m) { mode_ = m; update(); }

protected:
    void paintEvent(QPaintEvent*) override {
        QPainter painter(this);
        painter.setRenderHint(QPainter::Antialiasing);
        
        // 1. Рисуем многоугольники
        painter.setPen(Qt::white);
        for (const auto& poly : controller_.getPolygons()) {
            const auto& v = poly.getVertices();
            for(size_t i=0; i<v.size(); ++i) 
                painter.drawLine(v[i], v[(i+1)%v.size()]);
        }

        // 2. Рисуем свет (Задание 3: Полутени)
        if (mode_ == Light) {
            // Создаем эффект полутени через 5 источников по кругу
            QColor lightColor(255, 255, 150, 50); // Прозрачный желтый
            double offset = 3.0;
            QPointF center = controller_.getLightSource();
            QPointF sources[] = { 
                center, 
                center + QPointF(offset, 0), center + QPointF(-offset, 0),
                center + QPointF(0, offset), center + QPointF(0, -offset) 
            };

            for (const auto& src : sources) {
                Polygon area = controller_.createLightArea(src);
                QPolygonF qp;
                for (auto p : area.getVertices()) qp << p;
                painter.setBrush(lightColor);
                painter.setPen(Qt::NoPen);
                painter.drawPolygon(qp);
            }
        }
    }

    void mousePressEvent(QMouseEvent* event) override {
        if (mode_ == Polygons) {
            if (event->button() == Qt::LeftButton) {
                if (!isDrawing_) {
                    controller_.addPolygon(Polygon({event->pos()}));
                    isDrawing_ = true;
                } else {
                    controller_.addVertexToLastPolygon(event->pos());
                }
            } else if (event->button() == Qt::RightButton) {
                isDrawing_ = false;
            }
        }
        update();
    }

    void mouseMoveEvent(QMouseEvent* event) override {
        if (mode_ == Light) {
            controller_.setLightSource(event->pos());
        } else if (mode_ == Polygons && isDrawing_) {
            controller_.updateLastPolygon(event->pos());
        }
        update();
    }

private:
    Controller controller_;
    Mode mode_;
    bool isDrawing_ = false;
};