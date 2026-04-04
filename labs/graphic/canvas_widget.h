#pragma once

#include <QWidget>

#include "controller.h"

class CanvasWidget : public QWidget {
public:
    enum class Mode {
        Light,
        Polygons,
    };

    explicit CanvasWidget(QWidget* parent = nullptr);

    Controller& GetController();
    void SetMode(Mode mode);
    Mode GetMode() const;

protected:
    void paintEvent(QPaintEvent* event) override;
    void resizeEvent(QResizeEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;

private:
    void DrawLightAreas(QPainter* painter);
    void DrawPolygons(QPainter* painter);
    void DrawLightSources(QPainter* painter);

    Controller controller_;
    Mode mode_;
    bool drawing_polygon_;
};
