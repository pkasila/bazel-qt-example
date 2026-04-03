#ifndef CANVAS_H
#define CANVAS_H

#include <QWidget>
#include "core.h"

class Canvas : public QWidget {
    Q_OBJECT
public:
    enum Mode { LIGHT_MODE, POLYGON_MODE };

    explicit Canvas(QWidget *parent = nullptr);
    void setMode(Mode mode);
    
private:
    void paintEvent(QPaintEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;

    Controller m_controller;
    Mode m_mode;
    bool m_drawing_polygon;
    int m_dragged_light_idx;
    
    QPolygonF convertToQPolygonF(const Polygon& poly) const;
};

#endif // CANVAS_H