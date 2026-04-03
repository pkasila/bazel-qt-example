#pragma once

#include <QWidget>
#include <QComboBox>
#include <QMouseEvent>
#include <QKeyEvent>
#include <QPaintEvent>
#include "controller.h"

enum class Mode {
    Light,
    Polygons,
    SoftShadows,
    StaticLights  // Новый режим
};

class Raycaster : public QWidget {
    Q_OBJECT

public:
    explicit Raycaster(QWidget* parent = nullptr);

protected:
    void paintEvent(QPaintEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void keyPressEvent(QKeyEvent* event) override;

private slots:
    void onModeChanged(int index);

private:
    void drawLightArea(QPainter& painter);
    void drawSoftShadows(QPainter& painter);
    void drawStaticLights(QPainter& painter);
    void drawPolygons(QPainter& painter);
    void drawCurrentPolygon(QPainter& painter);
    void drawLightSource(QPainter& painter);
    void drawMultipleLightSources(QPainter& painter);
    
    Mode mode_;
    Controller controller_;
    QComboBox* modeCombo_;
    
    static constexpr int LIGHT_RADIUS = 5;
    static constexpr int VERTEX_RADIUS = 3;
    static constexpr int STATIC_LIGHT_RADIUS = 6;
};