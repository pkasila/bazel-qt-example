#pragma once

#include <QWidget>
#include <QMouseEvent>
#include <QPainter>
#include <QVector>
#include <vector>
#include "Controller.h"

enum class Mode {
    Light,
    Polygons
};

class Canvas : public QWidget {
    Q_OBJECT
public:
    explicit Canvas(QWidget* parent = nullptr);
    void SetController(Controller* controller);
    void SetMode(Mode mode);
protected:
    void paintEvent(QPaintEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void enterEvent(QEvent* event);
    void resizeEvent(QResizeEvent* event) override;
private:
    bool drawing_polygon_ = false;

    Mode currentMode_;
    std::vector<Controller*> controllers_;

    QVector<QPointF> currentPolygon_;
    QVector<QPointF> points_;
    std::vector<QVector<QPointF>> polygons_;

    Polygon automaticPolygon_;
    void updateAutomaticPolygon() {
        automaticPolygon_ = Polygon();

        automaticPolygon_.AddVertex(QPointF(0, 0));
        automaticPolygon_.AddVertex(QPointF(width(), 0));
        automaticPolygon_.AddVertex(QPointF(width(), height()));
        automaticPolygon_.AddVertex(QPointF(0, height()));
    }
};
