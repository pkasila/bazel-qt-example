#pragma once

#include <QWidget>

#include "core/Controller.h"
#include "ui/Modes.h"

class DrawWidget : public QWidget {
    Q_OBJECT

public:
    explicit DrawWidget(QWidget* parent = nullptr);

    void SetMode(InteractionMode mode);
    InteractionMode GetMode() const;

protected:
    void paintEvent(QPaintEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void resizeEvent(QResizeEvent* event) override;

private:
    Controller controller_;
    InteractionMode mode_ = InteractionMode::Light;
    bool is_drawing_polygon_ = false;
    bool light_follows_cursor_ = true;
    bool is_light_following_cursor_ = true;

    Polygon MakeBoundaryPolygon() const;
    void UpdateBoundaryPolygon();

    QPointF ClampLightPosition(const QPointF& pos) const;
    QPointF ClampClusterCenter(const QPointF& pos) const;

    void DrawBackground(QPainter& painter) const;
    void DrawLightArea(QPainter& painter) const;
    void DrawPolygons(QPainter& painter) const;
    void DrawLightSource(QPainter& painter) const;

};