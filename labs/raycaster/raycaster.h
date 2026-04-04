#ifndef RAYCASTER_H
#define RAYCASTER_H

#include "controller.h"

#include <QComboBox>
#include <QKeyEvent>
#include <QMouseEvent>
#include <QPaintEvent>
#include <QResizeEvent>
#include <QWidget>

enum class Mode : std::uint8_t { Light, Polygons, SoftShadows, StaticLights };

class Raycaster : public QWidget {
    Q_OBJECT
   public:
    explicit Raycaster(QWidget* parent = nullptr);

   protected:
    void paintEvent(QPaintEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void keyPressEvent(QKeyEvent* event) override;
    void resizeEvent(QResizeEvent* event) override;

   private slots:
    void OnModeChanged(int index);

   private:
    void DrawLightArea(QPainter& painter);
    void DrawSoftShadows(QPainter& painter);
    void DrawStaticLights(QPainter& painter);
    void DrawPolygons(QPainter& painter);
    void DrawCurrentPolygon(QPainter& painter);
    void DrawLightSource(QPainter& painter);
    void DrawMultipleLightSources(QPainter& painter);

    Mode mode_ = Mode::Light;
    Controller controller_;
    QComboBox* mode_combo_ = nullptr;

    static constexpr int kLightRadius = 5;
    static constexpr int kVertexRadius = 3;
    static constexpr int kStaticLightRadius = 6;
};

#endif
