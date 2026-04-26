#ifndef DRAWINGWIDGET_H
#define DRAWINGWIDGET_H

#include "controller.h"

#include <QKeyEvent>
#include <QMouseEvent>
#include <QPaintEvent>
#include <QResizeEvent>
#include <QWidget>

enum class Mode : uint8_t { kLight, kPolygons };

class DrawingWidget : public QWidget {
   public:
    explicit DrawingWidget(QWidget* parent = nullptr);

    void SetMode(Mode mode);
    void SetLightSource(const QPoint& pos);
    [[nodiscard]] QPoint GetLightSource() const;

    using DrawingStateCallback = std::function<void(bool)>;
    void SetDrawingStateCallback(DrawingStateCallback cb) {
        drawing_state_callback_ = cb;
    }

   protected:
    void paintEvent(QPaintEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void resizeEvent(QResizeEvent* event) override;

   private:
    void UpdateBoundaryPolygon();
    Mode current_mode_ = Mode::kPolygons;
    Controller controller_;
    bool is_drawing_polygon_ = false;
    DrawingStateCallback drawing_state_callback_;
};

#endif  // DRAWINGWIDGET_H
