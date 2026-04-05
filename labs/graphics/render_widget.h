#ifndef RENDER_WIDGET_H
#define RENDER_WIDGET_H

#include <QWidget>
#include <QMouseEvent>
#include <QPainter>
#include "controller.h"

namespace raycaster {

enum class Mode { Polygons, Light };

class RenderWidget : public QWidget {
    Q_OBJECT
public:
    explicit RenderWidget(QWidget* parent = nullptr);

    void SetMode(Mode mode) { mode_ = mode; update(); }
    void ClearAll();

protected:
    void paintEvent(QPaintEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void resizeEvent(QResizeEvent* event) override;

private:
    Controller controller_;
    Mode mode_ = Mode::Polygons;
    bool is_drawing_ = false;

    void DrawLightArea(QPainter& painter);
    void DrawObstacles(QPainter& painter);
};

} // namespace raycaster

#endif