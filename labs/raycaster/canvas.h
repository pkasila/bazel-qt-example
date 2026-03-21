#pragma once
#include "controller.h"

#include <QPixmap>
#include <QResizeEvent>
#include <QWidget>

enum class Mode { Light, Polygons, StaticLights };

class Canvas : public QWidget {
    Q_OBJECT
   public:
    explicit Canvas(QWidget* parent = nullptr);
    void SetMode(Mode mode);

    void SetLightEnabled(bool enabled) {
        lightEnabled = enabled;
        update();
    }

    void SetPhotoEnabled(bool enabled) {
        photoEnabled = enabled;
        update();
    }

    void SetShadowDetail(int detail) {
        shadowDetail = detail;
        update();
    }

   protected:
    void paintEvent(QPaintEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void resizeEvent(QResizeEvent* event) override;

   private:
    Controller controller;
    Mode currentMode = Mode::Light;
    bool isDrawing = false;
    bool lightEnabled = true;
    bool photoEnabled = false;
    int shadowDetail = 4;

    struct StaticLight {
        QPointF pos;
        QColor color;
    };

    std::vector<StaticLight> static_lights;

    QPixmap imgShadow;
    QPixmap imgLight;

    void drawLightTexture(QPainter& painter, const QPointF& pos, const QColor& color);
};