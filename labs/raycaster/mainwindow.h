#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "controller.h"

#include <QComboBox>
#include <QMainWindow>
#include <QButtonGroup>
#include <QMouseEvent>
#include <QWidget>
#include <QSpinBox>
#include <cstddef>

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget* parent = nullptr);
    void OnModeChanged(int mode);

protected:
    bool eventFilter(QObject* obj, QEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void paintEvent(QPaintEvent* event) override;
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
                const QPointF& P1 = vertices[i];
                const QPointF& P2 = vertices[(i + 1) % n];
                if ((is_closing_segment && i == n - 1) ||
                    (P1 == segment_start && P2 == segment_end) ||
                    (P2 == segment_start && P1 == segment_end)) {
                    continue;
                }

                if (Polygon::LineIntersection(P1, P2, segment_start, segment_end)) {
                    return true;
                }
            }
        }

        for (size_t i = 0; i < polygons.size(); ++i) {
            if (i == modified_poly_index) {
                continue;
            }

            const auto& poly = polygons[i];
            const auto& vertices = poly.GetVertices();
            const size_t size = vertices.size();

            for (size_t j = 0; j < size; j++) {
                const QPointF& P1 = vertices[j];
                const QPointF& P2 = vertices[(j + 1) % size];
                if (Polygon::LineIntersection(P1, P2, segment_start, segment_end)) {
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

    QSpinBox* spinBox;
    QComboBox* mode_combo;
    std::vector<QPointF> static_lights_;
    QButtonGroup* mode_group_;

    void DrawLightArea(QPainter& painter);
    std::vector<QPointF> GetLights() const;
    void DrawPolygons(QPainter& painter) const;
    static bool IsPointInPolygon(const QPointF& point, const std::vector<QPointF>& polygon);
    static bool InLight(const std::vector<QPointF>& light_pos, const std::vector<Polygon>& polygon);
    void DrawLightSource(QPainter& painter) const;
    void CreateModeSelector(QWidget* parent);
    void UpdateBorderPolygon();
};

#endif
