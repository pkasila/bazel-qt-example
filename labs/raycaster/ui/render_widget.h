#pragma once

#include "labs/raycaster/core/controller.h"

#include <QImage>
#include <QWidget>
#include <cstdint>
#include <functional>

namespace raycaster {

enum class InteractionMode : std::uint8_t {
    kLight = 0,
    kPolygons = 1,
    kStaticLights = 2,
};

class RenderWidget : public QWidget {
   public:
    explicit RenderWidget(QWidget* parent = nullptr);

    void SetMode(InteractionMode mode);
    InteractionMode GetMode() const;
    void SetSoftShadowsEnabled(bool enabled);
    bool AreSoftShadowsEnabled() const;
    void SetCatOverlayEnabled(bool enabled);
    bool IsCatOverlayEnabled() const;

    Controller& GetController();
    const Controller& GetController() const;

    void SetStatusCallback(std::function<void(const QString&)> callback);

   protected:
    void paintEvent(QPaintEvent* event) override;
    void resizeEvent(QResizeEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;
    void leaveEvent(QEvent* event) override;

   private:
    static QPolygonF ToQPolygonF(const Polygon& polygon);

    void DrawBackground(QPainter* painter);
    void UpdateCursor();
    void DrawLightArea(
        QPainter* painter, const QPointF& source, const QColor& center_color,
        const QColor& outer_color, double radius);
    void DrawCatOverlay(QPainter* painter);
    void DrawLights(QPainter* painter);
    void DrawObstacles(QPainter* painter);
    void DrawLightMarkers(QPainter* painter);
    void UpdateLightPosition(const QPointF& position);
    void ShowStatus(const QString& text) const;

    Controller controller_;
    InteractionMode mode_;
    bool cat_overlay_enabled_;
    QImage cat_background_image_;
    std::function<void(const QString&)> status_callback_;
};

}  // namespace raycaster
