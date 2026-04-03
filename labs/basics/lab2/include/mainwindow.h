#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "controller.h"
#include <QMainWindow>
#include <QPointF>
#include <QPainter>
#include <QMouseEvent>
#include <QString>
#include <QRadioButton>
#include <QVector>
#include <QRect>

class DrawingArea : public QWidget
{
    Q_OBJECT
public:
    enum Mode { Light, Polygons };

    DrawingArea(QWidget *parent = nullptr) : QWidget(parent), m_mode(Light) {
//        this->setStyleSheet("background-color: white;");
        setMouseTracking(true);
        Polygon p;
        p.AddVertex(QPointF(1,1));
        p.AddVertex(QPointF(1,(drawing_area_.height()) - 1));
        p.AddVertex(QPointF((drawing_area_.width()) - 1,(drawing_area_.height()) - 1));
        p.AddVertex(QPointF((drawing_area_.width()) - 1,1));
        polygons.AddPolygon(p);
    }

    void setMode(Mode mode) {
        m_mode = mode;
        updateBackground(); // Обновляем фон при смене режима
        update();
    }

    void addPolygonPoint(const QPointF& point) {
        if (m_mode == Polygons) {
            polygons.AddVertexToLastPolygon(point);
            update();
        }
    }

    void updateBackground() {
        // Устанавливаем цвет фона в зависимости от режима
        QPalette pal = palette();
        pal.setColor(QPalette::Window, m_mode == Light ? Qt::black : Qt::white);
        setPalette(pal);
        setAutoFillBackground(true);
    }

protected:
    void paintEvent(QPaintEvent *) override {
        QPainter painter(this);
        painter.setRenderHint(QPainter::Antialiasing);

        const QRect drawing_rect = drawing_area_.geometry();
        drawing_rect.width();
                painter.fillRect(drawing_rect, Qt::white);
                painter.save();
                painter.translate(drawing_rect.topLeft());

        if (m_mode == Light) {
            painter.setPen(Qt::white);
            painter.fillRect(drawing_rect, Qt::black);
            painter.setPen(Qt::white);
            painter.setBrush(Qt::white);
            for (int i = 0; i < 9; i++) {
                painter.drawEllipse(polygons.getvls(i), 2, 2);
            }
            QColor c(0,0,0,0);
            painter.setPen(QPen(c, 1, Qt::DashLine));
            std::vector<std::vector<Ray>> V;
            for (int i = 0; i < 9; i++) {
                V.push_back(polygons.CastRays(polygons.getvls(i)));
                polygons.IntersectRays(&V[V.size() - 1]);
                polygons.RemoveAdjacentRays(&V[V.size() - 1]);
            }
            for (int i = 0; i < 9; i++) {
                for (size_t j = 0; j < V[i].size(); j++) {
                    painter.drawLine(polygons.getvls(i), V[i][j].getEnd());
                }
            }
            for (int i = 0; i < 9; i++) {
                polygons.DrawLightArea(painter, i);
            }
 //           polygons.DrawLightArea(painter);
    //        polygons.DrawLightArea2(painter);
            painter.setPen(Qt::white);
            painter.setBrush(Qt::NoBrush);
            for (size_t i = 0; i < polygons.size(); i++) {
                painter.drawPolygon(polygons.getPolygon(i));
            }
        } else {
            painter.setPen(Qt::black);
            painter.fillRect(drawing_rect, Qt::white);


            // Рисуем все завершённые полигоны
            painter.setBrush(Qt::white);
            for (size_t i = 0; i < polygons.size(); i++) {
                painter.drawPolygon(polygons.getPolygon(i));
            }

            // Рисуем текущий незавершённый полигон (если есть точки)
            if (!currentPolygon.isEmpty()) {
                for (size_t i = 1; i < currentPolygon.getSize(); ++i) {
                    painter.drawLine(currentPolygon.getPoint(i-1), currentPolygon.getPoint(i));
                }

                // Рисуем точки текущего полигона
                painter.setBrush(Qt::red);
                for (size_t i = 0; i < currentPolygon.getSize(); i++) {
                    painter.drawEllipse(currentPolygon.getPoint(i), 3, 3);
                }
            }
        }
    }

    void mousePressEvent(QMouseEvent *event) override {
        if (m_mode == Polygons) {
            // ЛКМ - добавляем точку в текущий полигон
            if (event->button() == Qt::LeftButton) {
                currentPolygon.AddVertex(event->pos());
                update();
            }
            // ПКМ - завершаем текущий полигон (если есть хотя бы 2 точки)
            else if (event->button() == Qt::RightButton) {
                if (event->type() == QEvent::MouseButtonDblClick) {
                    // Двойной клик ПКМ - очищаем все полигоны
                    polygons.clear();
                    Polygon p;
                    p.AddVertex(QPointF(1,1));
                    p.AddVertex(QPointF(1,(drawing_area_.height()) - 1));
                    p.AddVertex(QPointF((drawing_area_.width()) - 1,(drawing_area_.height()) - 1));
                    p.AddVertex(QPointF((drawing_area_.width()) - 1,1));
                    polygons.AddPolygon(p);
                    currentPolygon.clear();
                } else if (currentPolygon.getSize() >= 2) {
                    // Одинарный клик ПКМ - завершаем текущий полигон
                    polygons.AddPolygon(currentPolygon);
                    currentPolygon.clear();
                }
                update();
            }
        }
    }

    void mouseMoveEvent(QMouseEvent *event) override {
        if (m_mode == Light) {
            QPointF mousePos = event->pos();
            polygons.setvls(mousePos, 0);
            polygons.setvls(mousePos + QPointF(10, 0), 1);
            polygons.setvls(mousePos + QPointF(7, 7), 2);
            polygons.setvls(mousePos + QPointF(0, 10), 3);
            polygons.setvls(mousePos + QPointF(-7, 7), 4);
            polygons.setvls(mousePos + QPointF(-10, 0), 5);
            polygons.setvls(mousePos + QPointF(-7, -7), 6);
            polygons.setvls(mousePos + QPointF(0, -10), 7);
            polygons.setvls(mousePos + QPointF(7, -7), 8);
            update();
        }
        QWidget::mouseMoveEvent(event);
    }

private:
    QWidget drawing_area_;
    Mode m_mode;
    QPointF m_lightPos;
    Controller polygons;
    Polygon currentPolygon;
    bool polygonFinished = false;
};

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow() = default;

protected:
    void resizeEvent(QResizeEvent *event) override {
        QWidget::resizeEvent(event);
        emit sizeChanged(event->size());
    }

signals:
    void sizeChanged(const QSize& newSize);

private:
    QRadioButton *lightMode;
    QRadioButton *polygonsMode;
    DrawingArea *m_drawingArea;
};
#endif // MAINWINDOW_H
