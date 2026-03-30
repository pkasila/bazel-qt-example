#ifndef PAINTWIDGET_H
#define PAINTWIDGET_H

#include <QWidget>
#include <QMouseEvent>

#include "Polygon.h"

class PaintWidget : public QWidget {
    Q_OBJECT

public:
    explicit PaintWidget(QWidget *parent = nullptr);

    void PaintPolygon(Polygon polygon);

    void ClearRaysBuffer();

    // void PaintRays(std::vector<Ray> rays);

    void PaintRays(Polygon& polygon, QColor color);

    void DrawRays(const QPointF& lightSource, const std::vector<Ray>& rays);



signals:
    void mouseMoved(QPointF pos); // Changed to QPointF
    void rightClicked(QPointF pos);
    void leftClicked(QPointF pos);

private:
    void paintEvent(QPaintEvent *event) override;

    void mouseMoveEvent(QMouseEvent *event) override;

    void mousePressEvent(QMouseEvent *event) override;

    // buffers
    QPixmap polygons_buffer_;  // Stores all drawings
    QPixmap rays_buffer_;  // Stores all drawings
    QPixmap rays_lines_buffer_; // New buffer for ray lines


    void initializeBuffer();

};

#endif // PAINTWIDGET_H
