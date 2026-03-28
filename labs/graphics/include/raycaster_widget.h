#ifndef RAYCASTER_WIDGET_H
#define RAYCASTER_WIDGET_H

#include "controller.h"

#include <QButtonGroup>
#include <QCheckBox>
#include <QMainWindow>
#include <QMouseEvent>
#include <QRadioButton>
#include <QWidget>
#include <cstddef>

class RaycasterWidget : public QMainWindow {
    Q_OBJECT

   public:
    explicit RaycasterWidget(QWidget* parent = nullptr);
    void OnModeChanged(int mode);
    void BuildWalls(bool go);

   protected:
    bool eventFilter(QObject* obj, QEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void paintEvent(QPaintEvent* event) override;
    void keyPressEvent(QKeyEvent* event) override;
    void resizeEvent(QResizeEvent* event) override;

   private:
    static bool CheckPolygonIntersections(
        const std::vector<Polygon>& polygons, const size_t modified_poly_index,
        const QPointF& segment_start, const QPointF& segment_end, bool is_closing_segment = false) {
        if (Controller::Distance(segment_start, segment_end) < 1.0) {
            return false;
        }
        if (modified_poly_index < polygons.size()) {
            const auto& poly = polygons[modified_poly_index];
            const auto& vertices = poly.GetVertices();
            const size_t n = vertices.size();

            if (n < 2) {
                return false;
            }
            for (size_t i = 0; i < n; ++i) {
                const QPointF& p1 = vertices[i];
                const QPointF& p2 = vertices[(i + 1) % n];

                /*if (p1 == segment_start || p2 == segment_end || p2 == segment_start ||
                    p1 == segment_end) {
                    continue;
                }
                if (i > 0 && vertices[i - 1] == segment_start && p1 == segment_end) {
                    continue;
                }
                if (i < n - 1 && p2 == segment_start && vertices[(i + 2) % n] == segment_end) {
                    continue;
                }
                if (is_closing_segment && i == n - 1) {
                    continue;
                }*/
                if ((is_closing_segment && i == n - 1) ||
                    (p1 == segment_start && p2 == segment_end) ||
                    (p2 == segment_start && p1 == segment_end)) {
                    continue;
                }

                if (Polygon::LineIntersection(p1, p2, segment_start, segment_end)) {
                    return true;
                }
            }
        }
        for (size_t i = 0; i < polygons.size(); ++i) {
            if (/*!is_closing_segment && */i == modified_poly_index) {
                continue;
            }

            const auto& poly = polygons[i];
            const auto& vertices = poly.GetVertices();
            const size_t n = vertices.size();

            for (size_t j = 0; j < n; ++j) {
                const QPointF& p1 = vertices[j];
                const QPointF& p2 = vertices[(j + 1) % n];
                if (Polygon::LineIntersection(p1, p2, segment_start, segment_end)) {
                    return true;
                }
            }
        }

        return false;
    }

    QWidget* drawing_area_;
    Controller controller_;
    QString mode_ = "light";
    bool wall_ = false;
    bool creating_polygon_ = false;

    QButtonGroup* mode_group_;
    QRadioButton* light_mode_radio_;
    QRadioButton* polygons_mode_radio_;
    QRadioButton* static_lights_radio_;
    std::vector<QPointF> static_lights_;
    QCheckBox* walls_;

    void DrawLightArea(QPainter& painter);
    [[nodiscard]] std::vector<QPointF> GetLights() const;
    void DrawPolygons(QPainter& painter) const;
    static bool IsPointInPolygon(const QPointF& point, const std::vector<QPointF>& polygon);
    static bool InLight(const std::vector<QPointF>& light_pos, const std::vector<Polygon>& polygon);
    void DrawLightSource(QPainter& painter) const;
    void CreateModeSelector(QWidget* parent);
    void UpdateBorderPolygon();
};

#endif  // RAYCASTER_WIDGET_H