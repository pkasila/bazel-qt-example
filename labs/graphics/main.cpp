#include "controller.h"

#include <QApplication>
#include <QButtonGroup>
#include <QHBoxLayout>
#include <QMouseEvent>
#include <QPainter>
#include <QPainterPath>
#include <QRadioButton>
#include <QResizeEvent>
#include <QVBoxLayout>
#include <QWidget>

class RaycastWidget : public QWidget {
   public:
    enum Mode { LIGHT, POLYGONS, STATIC_LIGHTS };

    Mode currentMode = LIGHT;
    Controller controller;
    bool isDrawing = false;

    RaycastWidget(QWidget* parent = nullptr) : QWidget(parent) {
        setMouseTracking(true);
        controller.SetLightSource(QPointF(100, 100));
    }

    void abortDrawing() {
        if (isDrawing) {
            isDrawing = false;
            update();
        }
    }

   protected:
    void resizeEvent(QResizeEvent* event) override {
        float w = event->size().width();
        float h = event->size().height();
        controller.SetBoundary(Polygon({{0, 0}, {w, 0}, {w, h}, {0, h}}));
        QWidget::resizeEvent(event);
    }

    void drawSingleLightArea(QPainter& painter, const QPointF& pos, QColor color) {
        Polygon area = controller.CreateLightArea(pos);
        const auto& v = area.GetVertices();
        if (v.empty()) {
            return;
        }

        QPainterPath path;
        path.moveTo(v[0]);
        for (size_t i = 1; i < v.size(); ++i) {
            path.lineTo(v[i]);
        }
        path.closeSubpath();
        painter.fillPath(path, color);
    }

    void paintEvent(QPaintEvent*) override {
        QPainter painter(this);
        painter.setRenderHint(QPainter::Antialiasing);
        painter.fillRect(rect(), QColor("#0f0f1a"));

        const auto& lightCluster = controller.GetLightCluster();
        QColor mainLightColor(255, 255, 200, 40);
        for (const auto& point : lightCluster) {
            drawSingleLightArea(painter, point, mainLightColor);
        }

        for (const auto& sl : controller.GetStaticLights()) {
            QColor slColor = sl.color;
            slColor.setAlpha(45);
            drawSingleLightArea(painter, sl.pos, slColor);
        }

        const auto& polys = controller.GetPolygons();
        for (size_t i = 0; i < polys.size(); ++i) {
            const auto& v = polys[i].GetVertices();
            if (v.size() < 2) {
                continue;
            }

            if (i == 0) {
                painter.setPen(QPen(QColor("#222233"), 1));
            } else {
                painter.setPen(QPen(QColor("#7777aa"), 2));
            }

            for (size_t j = 0; j < v.size(); ++j) {
                painter.drawLine(v[j], v[(j + 1) % v.size()]);
            }
        }

        painter.setPen(Qt::NoPen);
        painter.setBrush(Qt::yellow);
        painter.drawEllipse(controller.GetBaseLightSource(), 4, 4);

        for (const auto& sl : controller.GetStaticLights()) {
            painter.setBrush(sl.color);
            painter.drawEllipse(sl.pos, 4, 4);
        }

        if (isDrawing && currentMode == POLYGONS) {
            painter.setPen(QPen(Qt::white, 1, Qt::DashLine));
            painter.drawText(10, height() - 10, "Режим рисования: ПКМ чтобы закончить");
        }
    }

    void mousePressEvent(QMouseEvent* event) override {
        QPointF pos = event->position();

        if (currentMode == POLYGONS) {
            if (event->button() == Qt::LeftButton) {
                if (!isDrawing) {
                    controller.AddPolygon(Polygon({pos, pos}));
                    isDrawing = true;
                } else {
                    controller.AddVertexToLastPolygon(pos);
                }
            } else if (event->button() == Qt::RightButton) {
                isDrawing = false;
            }
        } else if (currentMode == STATIC_LIGHTS && event->button() == Qt::LeftButton) {
            if (!controller.IsPointInAnyPolygon(pos)) {
                QColor colors[] = {Qt::cyan, Qt::magenta, QColor("#00ff00"), Qt::red};
                controller.AddStaticLight(pos, colors[rand() % 4]);
            }
        }
        update();
    }

    void mouseMoveEvent(QMouseEvent* event) override {
        QPointF pos = event->position();

        if (currentMode == LIGHT) {
            controller.SetLightSource(pos);
        } else if (currentMode == POLYGONS && isDrawing) {
            controller.UpdateLastPolygon(pos);
        }
        update();
    }
};

class MainWindow : public QWidget {
   public:
    MainWindow() {
        auto* mainLayout = new QVBoxLayout(this);
        auto* btnLayout = new QHBoxLayout();

        auto* rbLight = new QRadioButton("Лампа");
        auto* rbPoly = new QRadioButton("Стены");
        auto* rbStatic = new QRadioButton("Статик Свет");
        rbLight->setChecked(true);

        auto* group = new QButtonGroup(this);
        group->addButton(rbLight);
        group->addButton(rbPoly);
        group->addButton(rbStatic);

        btnLayout->addWidget(rbLight);
        btnLayout->addWidget(rbPoly);
        btnLayout->addWidget(rbStatic);
        mainLayout->addLayout(btnLayout);

        auto* view = new RaycastWidget();
        mainLayout->addWidget(view);

        auto handleModeChange = [view](RaycastWidget::Mode m) {
            view->abortDrawing();
            view->currentMode = m;
        };

        connect(rbLight, &QRadioButton::toggled, [=](bool c) {
            if (c) {
                handleModeChange(RaycastWidget::LIGHT);
            }
        });
        connect(rbPoly, &QRadioButton::toggled, [=](bool c) {
            if (c) {
                handleModeChange(RaycastWidget::POLYGONS);
            }
        });
        connect(rbStatic, &QRadioButton::toggled, [=](bool c) {
            if (c) {
                handleModeChange(RaycastWidget::STATIC_LIGHTS);
            }
        });

        setStyleSheet("background-color: #1e1e27; color: #eeeeee; font-weight: bold;");
        setWindowTitle("Raycaster Pro");
        resize(1000, 800);
    }
};

int main(int argc, char* argv[]) {
    QApplication app(argc, argv);
    MainWindow win;
    win.show();
    return app.exec();
}