#include "mainwindow.h"

#include <QActionGroup>
#include <QDirIterator>
#include <QFile>
#include <QGraphicsRectItem>
#include <QGraphicsScene>
#include <QGraphicsSceneMouseEvent>
#include <QGraphicsView>
#include <QItemDelegate>
#include <QLabel>
#include <QMouseEvent>
#include <QPaintEngine>
#include <QPainter>
#include <QPoint>
#include <QRandomGenerator>
#include <QScrollBar>
#include <QSizePolicy>
#include <QStandardItem>
#include <QStandardItemModel>
#include <QStyleHints>
#include <QVector2D>
#include <QtCore/QVariant>
#include <QtGui/QIcon>
#include <QtWidgets/QAbstractButton>
#include <QtWidgets/QApplication>
#include <QtWidgets/QDialog>
#include <QtWidgets/QDialogButtonBox>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QTextEdit>
#include <QtWidgets/QVBoxLayout>

#include <algorithm>
#include <cmath>
#include <cstdlib>

QIcon source_icon(const QString &str)
{
    const auto *theme = "light";
    const QPalette defaultPalette;
    const auto text = defaultPalette.color(QPalette::WindowText);
    const auto window = defaultPalette.color(QPalette::Window);
    if (text.lightness() > window.lightness()) {

        theme = "dark";
    }
    return QIcon(QString(":/resources/%1/%2.svg").arg(theme, str));
}

QIcon bake_icon(const QColor &color)
{
    auto pixmap = QPixmap(64, 64);
    pixmap.fill(color);
    return { pixmap };
};

class Canvas : public QGraphicsView
{
public:
    explicit Canvas(QWidget *parent) : QGraphicsView(parent)
    {
        this->setBackgroundBrush(Qt::white);

        this->setScene(&scene);
        this->setAutoFillBackground(true);
        this->setResizeAnchor(QGraphicsView::AnchorViewCenter);
        this->setTransformationAnchor(QGraphicsView::AnchorViewCenter);

        this->setHorizontalScrollBarPolicy(Qt::ScrollBarPolicy::ScrollBarAlwaysOff);
        this->setVerticalScrollBarPolicy(Qt::ScrollBarPolicy::ScrollBarAlwaysOff);

        auto max = 100000.0;
        scene.setSceneRect(-max / 2, -max / 2, max, max);
        scene.setFocusOnTouch(false);
        view_point = QPoint(0, 0);
        this->centerOn(view_point);
        this->setRenderHint(QPainter::Antialiasing);
        scene.setBackgroundBrush(Qt::white);
    }

    void drawBackground(QPainter *painter, const QRectF &rect) override
    {
        painter->fillRect(rect, Qt::white);

        auto stroke_width = 2;
        auto scale = this->transform().m11();
        painter->setPen(QPen(Qt::gray, stroke_width / scale));

        auto step = 100.0;
        auto s2 = pow(2, ceil(log(1 / scale) / log(2))) / 2;
        auto grid_step = step * s2;

        auto ceil_step = [grid_step](auto val) { return grid_step * std::floor(val / grid_step); };
        // NOLINTNEXTLINE(cert-flp30-c)
        for (auto x = ceil_step(rect.x()); x <= rect.x() + rect.width(); x += grid_step) {
            // NOLINTNEXTLINE(cert-flp30-c)
            for (auto y = ceil_step(rect.y()); y <= rect.y() + rect.height(); y += grid_step) {
                painter->drawPoint(QPointF(x, y));
            }
        }
    }

    void wheelEvent(QWheelEvent *event) override
    {
        event->accept();

        double scale = 0;
        if (!event->pixelDelta().isNull()) {
            // Likely a touchpad
            scale = event->pixelDelta().y();
            scale /= 50.0;
        } else if (!event->angleDelta().isNull()) {
            // Likely a mouse
            scale = event->angleDelta().y();
            scale /= 240.0;
        } else {
            return;
        }
        if (scale == 0) {
            return;
        }

        scale = std::clamp(scale, -1.0, 1.0);
        if (scale >= 0.0) {
            scale += 1.0;
        } else {
            scale = 1 / (1 + std::abs(scale));
        }

        auto new_scale = this->transform().m11() * scale;
        if (new_scale > 10.0 || new_scale < 1.0 / 10.0) {
            return;
        }

        auto mouse_pos = event->position().toPoint();
        auto pre = this->mapToScene(mouse_pos);
        this->scale(scale, scale);
        auto post = this->mapToScene(mouse_pos);

        // Makes zoom appear centered around mouse pointer
        view_point += pre - post;
        this->centerOn(view_point);
    }

    void mouseMoveEvent(QMouseEvent *event) override
    {
        if (!start) {
            event->ignore();
            return;
        }
        auto new_pos = this->mapToScene(event->position().toPoint());
        auto start_pos = this->mapToScene(this->start_pos.toPoint());
        auto diff = new_pos - start_pos;

        switch (start_tool) {
        case ToolType::Move: {
            view_point += -diff;
            this->centerOn(view_point);
            this->start_pos = event->position();
            break;
        }
        case ToolType::Draw: {
            QLineF line;
            line.setP1(start_pos);
            line.setP2(new_pos);
            QPen pen;
            pen.setColor(color);
            pen.setWidth(stroke_size);
            scene.addLine(line, pen);
            this->start_pos = event->position();
            break;
        }
        case ToolType::Rectangle: {
            auto sx = start_pos.x();
            auto sy = start_pos.y();
            auto nx = new_pos.x();
            auto ny = new_pos.y();

            if (sx > nx) {
                std::swap(sx, nx);
            }
            if (sy > ny) {
                std::swap(sy, ny);
            }
            QRectF rect;
            rect.setTopLeft(QPointF(sx, sy));
            rect.setBottomRight(QPointF(nx, ny));
            start_rect->setRect(rect);
            break;
        }
        }
        event->accept();
    }

    void mousePressEvent(QMouseEvent *event) override
    {
        if (!start) {
            auto mods = QApplication::keyboardModifiers();
            auto right = event->button() == Qt::RightButton;
            auto left = event->button() == Qt::LeftButton;
            auto shift_or_ctrl =
                    mods.testFlag(Qt::ShiftModifier) || mods.testFlag(Qt::ControlModifier);

            if (right || (left && shift_or_ctrl) || (left && tool == ToolType::Move)) {
                start = true;
                start_button = event->button();
                start_pos = event->position();
                start_tool = ToolType::Move;
                setCursor(Qt::ClosedHandCursor);
                event->accept();
                return;
            }
            if (left) {
                start = true;
                start_button = event->button();
                start_pos = event->position();
                start_tool = tool;

                if (start_tool == ToolType::Rectangle) {
                    auto start_pos = this->mapToScene(this->start_pos.toPoint());
                    QRectF rect;
                    rect.setTopRight(start_pos);
                    rect.setBottomLeft(start_pos + QPointF(1, 1));
                    QBrush brush(color, Qt::SolidPattern);
                    QPen pen(brush, stroke_size);
                    if (!fill) {
                        brush = QBrush();
                    }
                    start_rect = scene.addRect(rect, pen, brush);
                }
                event->accept();
                return;
            }
        }
        event->ignore();
    }

    void mouseReleaseEvent(QMouseEvent *event) override
    {
        if (!start || start_button != event->button()) {
            event->ignore();
            return;
        }
        mouseMoveEvent(event);
        start = false;
        setCursor(Qt::ArrowCursor);
        event->accept();
    }

    QPointF view_point;

    ToolType tool{};
    int stroke_size{};
    QColor color;
    bool fill{};

    bool start{};
    ToolType start_tool{};
    QPointF start_pos;
    Qt::MouseButton start_button{};
    QGraphicsRectItem *start_rect{};

    QGraphicsScene scene;
};

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent)
{
    this->setObjectName("Yet Another Paint");
    this->resize(800, 600);
    this->setMouseTracking(false);

    auto *centralwidget = new QWidget(this);

    auto *verticalLayout = new QVBoxLayout(centralwidget);

    canvas = new Canvas(this);
    verticalLayout->addWidget(canvas);

    this->setLayout(verticalLayout);

    tool_bar = new QToolBar("Drawing tools", centralwidget);
    tool_bar->setMovable(false);
    tool_bar->setContextMenuPolicy(Qt::ContextMenuPolicy::PreventContextMenu);

    auto color = Qt::black;
    canvas->color = color;
    color_dialog.setCurrentColor(color);
    color_button = tool_bar->addAction(bake_icon(color), "Color");

    tool_bar->addSeparator();

    connect(&color_dialog, &QColorDialog::colorSelected, this, [this](const QColor &color) {
        canvas->color = color;
        this->color_button->setIcon(bake_icon(color));
    });

    connect(color_button, &QAction::triggered, this,
            [this] { this->color_dialog.setVisible(true); });

    auto *group = new QActionGroup(tool_bar);
    group->setExclusionPolicy(QActionGroup::ExclusionPolicy::Exclusive);
    group->setObjectName("draw_action");

    auto add_tool = [&](auto tool, auto name, auto icon) {
        auto *action = tool_bar->addAction(source_icon(icon), name);
        action->setCheckable(true);
        if (tool == ToolType::Draw) {
            canvas->tool = tool;
            action->setChecked(true);
        }
        group->addAction(action);
        connect(action, &QAction::triggered, this, [this, tool] {
            this->canvas->tool = tool; //
        });
    };
    add_tool(ToolType::Move, "Move", "hand");
    add_tool(ToolType::Draw, "Draw", "draw-freehand");
    add_tool(ToolType::Rectangle, "Rectangle", "draw-rectangle");

    tool_bar->addSeparator();

    auto *fill = tool_bar->addAction(source_icon("color-fill"), "Fill");
    fill->setCheckable(true);
    connect(fill, &QAction::toggled, this, [this](bool fill) {
        this->canvas->fill = fill; //
    });
    this->canvas->fill = false;

    auto *spacer = new QWidget();
    spacer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    tool_bar->addWidget(spacer);

    auto *stroke_width_icon = new QLabel("Stroke width");
    stroke_width_icon->setMargin(4);
    stroke_width_icon->setPixmap(source_icon("edit-line-width").pixmap(24));
    tool_bar->addWidget(stroke_width_icon);

    auto *stroke_size = new QSlider(Qt::Orientation::Horizontal, centralwidget);
    connect(stroke_size, &QSlider::valueChanged, this, [this](int stroke_size) {
        this->canvas->stroke_size = stroke_size; //
    });
    canvas->stroke_size = 4;
    stroke_size->setValue(canvas->stroke_size);
    stroke_size->setMinimum(1);
    stroke_size->setMaximum(10);
    stroke_size->setSizePolicy(QSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred));
    stroke_size->setTickPosition(QSlider::TicksBothSides);
    stroke_size->setTickInterval(1);
    stroke_size->setSingleStep(1);
    stroke_size->setFocusPolicy(Qt::StrongFocus);
    tool_bar->addWidget(stroke_size);

    tool_bar->addSeparator();

    auto *clear_action = tool_bar->addAction(source_icon("edit-clear-all"), "Clear");
    connect(clear_action, &QAction::triggered, this, [this] { this->canvas->scene.clear(); });

    this->setMinimumWidth(tool_bar->sizeHint().width() * 11 / 10);
    this->setMinimumHeight(tool_bar->sizeHint().width() / 2);

    this->addToolBar(tool_bar);
    this->setCentralWidget(centralwidget);

    this->setWindowTitle("Yet Another Paint");

    QMetaObject::connectSlotsByName(this);
}
