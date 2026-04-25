#include <QApplication>
#include <QPen>
#include <QColor>
#include <QComboBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QMouseEvent>
#include <QPainter>
#include <QPainterPath>
#include <QPaintEvent>
#include <QResizeEvent>
#include <QVBoxLayout>
#include <QWidget>

#include <vector>

#include "geometry.h"

namespace {
QPainterPath PathFromVertices(const std::vector<QPointF>& vertices) {
    QPainterPath path;
    if (vertices.empty()) {
        return path;
    }

    path.moveTo(vertices.front());
    for (std::size_t i = 1; i < vertices.size(); ++i) {
        path.lineTo(vertices[i]);
    }
    path.closeSubpath();
    return path;
}

QPainterPath OpenPathFromVertices(const std::vector<QPointF>& vertices) {
    QPainterPath path;
    if (vertices.empty()) {
        return path;
    }

    path.moveTo(vertices.front());
    for (std::size_t i = 1; i < vertices.size(); ++i) {
        path.lineTo(vertices[i]);
    }
    return path;
}

}  // namespace

class CanvasWidget : public QWidget {
public:
    enum class Mode {
        Light,
        Polygons
    };

    explicit CanvasWidget(QWidget* parent = nullptr)
        : QWidget(parent) {
        setMouseTracking(true);
        setFocusPolicy(Qt::StrongFocus);
        controller_.SetBounds(800.0, 600.0);
    }

    void SetMode(Mode mode) {
        mode_ = mode;
        update();
    }

protected:
    void resizeEvent(QResizeEvent* event) override {
        QWidget::resizeEvent(event);
        controller_.SetBounds(width(), height());
    }

    void mouseMoveEvent(QMouseEvent* event) override {
        const QPointF pos = event->pos();
        if (mode_ == Mode::Light) {
            controller_.SetLightSource(pos);
        } else if (drawing_polygon_) {
            controller_.UpdateLastPolygon(pos);
        }
        update();
    }

    void mousePressEvent(QMouseEvent* event) override {
        const QPointF pos = event->pos();

        if (mode_ == Mode::Light) {
            controller_.SetLightSource(pos);
            update();
            return;
        }

        if (event->button() == Qt::LeftButton) {
            if (!drawing_polygon_) {
                controller_.AddPolygon(Polygon(std::vector<QPointF>{pos, pos}));
                drawing_polygon_ = true;
            } else {
                controller_.AddVertexToLastPolygon(pos);
            }
            update();
            return;
        }

        if (event->button() == Qt::RightButton && drawing_polygon_) {
            controller_.FinalizeLastPolygon();
            drawing_polygon_ = false;
            update();
        }
    }

    void paintEvent(QPaintEvent* event) override {
        Q_UNUSED(event);

        QPainter painter(this);
        painter.setRenderHint(QPainter::Antialiasing, true);
        painter.fillRect(rect(), QColor(24, 26, 31));

        const auto& polygons = controller_.GetPolygons();

        for (std::size_t index = 1; index < polygons.size(); ++index) {
            const Polygon& polygon = polygons[index];
            const auto& vertices = polygon.GetVertices();

            if (vertices.size() >= 3) {
                painter.setPen(QPen(QColor(200, 200, 210, 220), 2.0));
                painter.setBrush(QColor(70, 75, 84, 200));
                painter.drawPath(PathFromVertices(vertices));
            } else if (vertices.size() == 2) {
                painter.setPen(QPen(QColor(200, 200, 210, 220), 2.0));
                painter.setBrush(Qt::NoBrush);
                painter.drawPath(OpenPathFromVertices(vertices));
            } else if (vertices.size() == 1) {
                painter.setPen(QPen(QColor(200, 200, 210, 220), 2.0));
                painter.setBrush(QColor(200, 200, 210, 220));
                painter.drawEllipse(vertices.front(), 2.5, 2.5);
            }
        }

        const auto light_areas = controller_.CreateLightAreas();
        for (const auto& area : light_areas) {
            const auto& area_vertices = area.GetVertices();
            if (area_vertices.size() < 3) {
                continue;
            }

            painter.setPen(Qt::NoPen);
            painter.setBrush(QColor(255, 244, 170, 18));
            painter.drawPath(PathFromVertices(area_vertices));
        }

        for (const auto& source : controller_.GetLightSources()) {
            painter.setPen(Qt::NoPen);
            painter.setBrush(QColor(255, 250, 235, 220));
            painter.drawEllipse(source, 4.5, 4.5);
        }

        if (drawing_polygon_ && polygons.size() > 1) {
            const auto& vertices = polygons.back().GetVertices();
            if (vertices.size() >= 2) {
                painter.setPen(QPen(QColor(255, 255, 255, 70), 1.0, Qt::DashLine));
                painter.setBrush(Qt::NoBrush);
                painter.drawPath(OpenPathFromVertices(vertices));
            }
        }
    }

private:
    Controller controller_;
    Mode mode_ = Mode::Light;
    bool drawing_polygon_ = false;
};

class MainWindow : public QWidget {
public:
    MainWindow() {
        auto* layout = new QVBoxLayout(this);
        layout->setContentsMargins(12, 12, 12, 12);
        layout->setSpacing(10);

        auto* controls = new QHBoxLayout();
        auto* mode_label = new QLabel("Mode:");
        auto* mode_box = new QComboBox();
        mode_box->addItem("light");
        mode_box->addItem("polygons");
        controls->addWidget(mode_label);
        controls->addWidget(mode_box);
        controls->addStretch(1);
        layout->addLayout(controls);

        canvas_ = new CanvasWidget();
        canvas_->setMinimumSize(800, 600);
        layout->addWidget(canvas_, 1);

        connect(mode_box, QOverload<int>::of(&QComboBox::currentIndexChanged), this, [this](int index) {
            canvas_->SetMode(index == 0 ? CanvasWidget::Mode::Light : CanvasWidget::Mode::Polygons);
        });
    }

private:
    CanvasWidget* canvas_ = nullptr;
};

int main(int argc, char* argv[]) {
    QApplication app(argc, argv);

    MainWindow window;
    window.setWindowTitle("Raycaster");
    window.resize(1100, 800);
    window.show();

    return app.exec();
}
