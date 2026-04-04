#pragma once

#include <QtCore/QPointF>
#include <QtGui/QColor>
#include <QtGui/QMouseEvent>
#include <QtGui/QPaintEvent>
#include <QtGui/QPainter>
#include <QtGui/QResizeEvent>
#include <QtWidgets/QWidget>

#include "labs/raycaster/controller.h"

namespace raycaster {

class DrawingWidget : public QWidget {
public:
    enum class Mode {
        kLight,
        kPolygons,
    };

    explicit DrawingWidget(QWidget* parent = nullptr);

    Controller* GetController();
    const Controller* GetController() const;

    void SetMode(Mode mode);

protected:
    void paintEvent(QPaintEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;
    void resizeEvent(QResizeEvent* event) override;

private:
    void DrawPolygon(
        QPainter* painter,
        const Polygon& polygon,
        const QColor& fill_color,
        const QColor& outline_color) const;
    void DrawLightSource(QPainter* painter, const QPointF& point, const QColor& color) const;

    Controller controller_;
    Mode mode_ = Mode::kLight;
};

}  // namespace raycaster
