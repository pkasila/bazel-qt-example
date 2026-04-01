#ifndef RAYCASTER_H
#define RAYCASTER_H

#include "Controller.h"
#include "FPSCounter.h"
#include "GraphicsView.h"
#include "Polygon.h"

#include <QGraphicsScene>
#include <QGraphicsView>
#include <QMainWindow>
#include <QRadioButton>
#include <QVBoxLayout>
#include <QWidget>

enum class InputModes { Light, Polygon, StaticLights };

class Raycaster final : public QMainWindow {
    Q_OBJECT
   public:
    Raycaster(QWidget* parent = nullptr);
   private slots:
    void LightModeToggled(bool checked);
    void PolygonModeToggled(bool checked);
    void StaticLightsModeToggled(bool checked);
    void MouseMoved(const QPointF&);
    void MousePressed(const QPointF&, Qt::MouseButton);

   private:
    QRadioButton* light_mode_{};
    QRadioButton* polygon_mode_{};
    QRadioButton* static_lights_mode_{};

    FPSCounter* fps_counter_{};

    GraphicsView* view_{};
    QGraphicsScene* scene_{};

    Controller controller_{};
    InputModes mode_ = InputModes::Light;

    QTimer* refresh_timer_{};

    mutable bool can_place_vertex_ = true;

    void Render() const;
    void DrawPolygon(const Polygon&, const QPen&, const QBrush&) const;
    void DrawPolygons() const;
    void DrawLight() const;
    void DrawLightArea() const;
    void MouseMovedLight(const QPointF&);
    void BuildingPolygon() const;
    void MousePressedPolygon(const QPointF&, Qt::MouseButton);
    void MousePressedStaticLights(const QPointF&, Qt::MouseButton);
};

#endif