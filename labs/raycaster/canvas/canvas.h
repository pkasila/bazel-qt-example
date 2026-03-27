#ifndef CANVAS_H
#define CANVAS_H

#include "controller.h"
#include <QPainter>
#include <QWidget>
#include <memory>

class Canvas;

class CanvasMode {  // NOLINT
   public:
    virtual ~CanvasMode() = default;
    virtual void mousePressEvent(QMouseEvent* event, Canvas* canvas) = 0;    // NOLINT
    virtual void mouseMoveEvent(QMouseEvent* event, Canvas* canvas) = 0;     // NOLINT
    virtual void mouseReleaseEvent(QMouseEvent* event, Canvas* canvas) = 0;  // NOLINT
};

class Canvas : public QWidget {
    Q_OBJECT

   protected:
    void paintEvent(QPaintEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;
    void resizeEvent(QResizeEvent* event) override;

   public:
    explicit Canvas(QWidget* parent);
    void SetMode(std::unique_ptr<CanvasMode> mode);
    void SetController(std::unique_ptr<Controller> controller);
    void UpdateBorder(const QRect& rect);
    void ResetCanvas();
    int TakeFrameCount();
    [[nodiscard]] Controller* GetController() const;

   private:
    std::unique_ptr<Controller> controller_;
    std::unique_ptr<CanvasMode> current_mode_;
    int frame_count_ = 0;
};

class LightMode : public CanvasMode {
public:
    void mousePressEvent(QMouseEvent* event, Canvas* canvas) override;
    void mouseMoveEvent(QMouseEvent* event, Canvas* canvas) override;
    void mouseReleaseEvent(QMouseEvent* event, Canvas* canvas) override;
};

class PolygonMode : public CanvasMode {
public:
    void mousePressEvent(QMouseEvent* event, Canvas* canvas) override;
    void mouseMoveEvent(QMouseEvent* event, Canvas* canvas) override;
    void mouseReleaseEvent(QMouseEvent* event, Canvas* canvas) override;
};

class StaticLightsMode : public CanvasMode {
public:
    void mousePressEvent(QMouseEvent* event, Canvas* canvas) override;
    void mouseMoveEvent(QMouseEvent* event, Canvas* canvas) override;
    void mouseReleaseEvent(QMouseEvent* event, Canvas* canvas) override;
};

#endif
