#pragma once

#include <QPointF>
#include <QWidget>

#include "labs/raycaster/controller.h"

class QMouseEvent;
class QPaintEvent;
class QPainter;
class QResizeEvent;

enum class InteractionMode {
    kLight,
    kPolygons,
};

class CanvasWidget : public QWidget {
public:
    explicit CanvasWidget(QWidget* parent = nullptr);

    void SetMode(InteractionMode mode);

protected:
    void mouseMoveEvent(QMouseEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;
    void paintEvent(QPaintEvent* event) override;
    void resizeEvent(QResizeEvent* event) override;

private:
    void DrawDraftPolygon(QPainter& painter) const;
    void DrawFinishedPolygonFills(QPainter& painter) const;
    void DrawFinishedPolygonOutlines(QPainter& painter) const;
    void DrawLightAreas(QPainter& painter) const;
    void DrawLightSources(QPainter& painter) const;
    void SyncSceneRect();

    Controller controller_;
    InteractionMode mode_;
    QPointF last_mouse_position_;
};
