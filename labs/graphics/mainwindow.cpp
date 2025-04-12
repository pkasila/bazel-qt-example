#include "mainwindow.h"

#include <QActionGroup>
#include <QImage>
#include <QItemDelegate>
#include <QLabel>
#include <QOpenGLContext>
#include <QOpenGLFunctions>
#include <QOpenGLWidget>
#include <QPaintEngine>
#include <QPaintEvent>
#include <QPainter>
#include <QRandomGenerator>
#include <QStandardItem>
#include <QStandardItemModel>
#include <QToolBar>
#include <QToolButton>
#include <QWidgetAction>
#include <QtCore/QVariant>
#include <QtGui/QIcon>
#include <QtWidgets/QAbstractButton>
#include <QtWidgets/QApplication>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QColorDialog>
#include <QtWidgets/QDialog>
#include <QtWidgets/QDialogButtonBox>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QTextEdit>
#include <QtWidgets/QToolBar>
#include <QtWidgets/QVBoxLayout>

#include <algorithm>
#include <cassert>
#include <cmath>
#include <memory>
#include <optional>
#include <ranges>
#include <vector>

#include "math.h" // NOLINT

class CustomAction : public QWidgetAction
{
public:
    explicit CustomAction(QWidget *inner) : QWidgetAction(nullptr) { setDefaultWidget(inner); }
};

QIcon source_icon_theme(const QString &str, const char *theme)
{
    return QIcon(QString(":/resources/%1/%2.svg").arg(theme, str));
}

QIcon source_icon(const QString &str)
{
    const auto *theme = "light";
    const QPalette defaultPalette;
    const auto text = defaultPalette.color(QPalette::WindowText);
    const auto window = defaultPalette.color(QPalette::Window);
    if (text.lightness() > window.lightness()) {

        theme = "dark";
    }
    return source_icon_theme(str, theme);
}

QIcon bake_icon(const QColor &color)
{
    auto pixmap = QPixmap(24, 24);
    pixmap.fill(Qt::black);
    pixmap.fill(color);
    return { pixmap };
};

QIcon bake_icon_split(const QColor &left, const QColor &right)
{
    auto w = 24;
    auto pixmap = QPixmap(w, w);
    pixmap.fill(Qt::white);
    auto p = QPainter(&pixmap);
    p.setPen(Qt::transparent);
    std::array<QPoint, 3> points = {
        QPoint(0, 0),
        QPoint(w, 0),
        QPoint(0, w),
    };
    for (auto _ : std::views::iota(0, 9)) {
        p.setBrush(left);
        p.drawPolygon(points.begin(), points.size());
    }
    points[0] = QPoint(w, w);
    p.setBrush(right);
    p.drawPolygon(points.begin(), points.size());
    return { pixmap };
};

class Canvas : public QOpenGLWidget
{
public:
    struct Theme
    {
        int light_shards = 11;
        int light_radius = 8;
        int light_icon_size = 32;
        int vertex_radius = 3;
        int edge_width = 3;
        bool fading_enabled = false;
        bool fill_central = false;
        int fading_radius = 1000;
        QColor light_color = "#46ffffff";
        QColor background_color = "#7f7f7f";
        QColor vertex_color = "#000000";
        QIcon light_icon = source_icon_theme("brightness-high", "light");
    } theme;

    QPointF cursor = QPointF(theme.light_radius + 1, theme.light_radius + 1);
    QPoint cursor_raw = cursor.toPoint();
    QPointF offset = QPointF(0, 0);
    double scale = 1.0;

    std::vector<Polygon> poly{ Polygon{
            .verteces = std::vector<QPointF>(4, QPointF(0, 0)),
            .enclosed = true,
    } };
    std::vector<QPointF> intersections;
    bool selecting_poly = false;
    bool mouse_light = true;
    bool enclose_new_poly = true;

    Intersections intersections_buf;
    Light cursor_light;

    std::vector<QPointF> susp;

    struct LightCache
    {
        std::vector<Light> cache;
        Canvas *parent;

        explicit LightCache(Canvas *parent) : parent{ parent } { }
        [[nodiscard]] const auto &read() const { return cache; }
        auto &write_no_invalidate() { return cache; }
        auto &write_at(int i)
        {
            auto &ref = cache[i];
            ref.invalidate();
            return ref;
        }
    } lights{ this };

    enum Grabbing {
        None,
        Hover,
        Active,
    } grabbing{ Grabbing::None };
    int grabbing_poly{};
    int grabbing_vertex{};
    int grabbing_light{};

    enum Tool : uint8_t {
        Move,
        Poly,
        Light,
        Erase,
    } tool{ Tool::Poly };

    explicit Canvas(QWidget *parent) : QOpenGLWidget(parent)
    {
        auto f = this->format();
        f.setSamples(4);
        this->setFormat(f);

        this->setMouseTracking(true);
        this->setMinimumHeight(theme.light_radius * 4);
        this->setMinimumWidth(theme.light_radius * 4);
    }
    void recompute_intersections()
    {
        intersections.clear();
        for (const auto &poly : poly) {
            auto to_check = poly.verteces.size();
            if (!poly.enclosed) {
                to_check -= 1;
            }
            for (const auto &j : std::views::iota(0UL, to_check)) {
                auto from = poly.verteces[j];
                auto to = poly.verteces[(j + 1) % poly.verteces.size()];
                handle_intersections(Segment(from, to));
            }
        }
    }
    void handle_intersections(Segment segment)
    {
        for (const auto &poly_i_id : std::views::iota(0UL, poly.size())) {
            auto &poly_i = poly[poly_i_id];
            auto to_check = poly_i.verteces.size();
            if (!poly_i.enclosed) {
                to_check -= 1;
            }
            for (const auto &j : std::views::iota(0UL, to_check)) {
                auto from = poly_i.verteces[j];
                auto to = poly_i.verteces[(j + 1) % poly_i.verteces.size()];
                if (from == segment.from || to == segment.from) {
                    continue;
                }
                if (from == segment.to || to == segment.to) {
                    continue;
                }
                auto intersection = intersect_segment_segment(Segment(from, to), segment);

                if (intersection.has_value()) {
                    intersections.push_back(intersection.value());
                }
            }
        }
    };
    QRectF transformed_rect()
    {
        auto rect = this->rect().toRectF();
        return {
            offset + rect.topLeft(),
            offset + rect.bottomRight() * scale,
        };
    }
    void update_borders()
    {
        auto rect = transformed_rect();
        poly[0].verteces = {
            rect.topLeft(),
            rect.topRight(),
            rect.bottomRight(),
            rect.bottomLeft(),
        };
    }
    void update_susp()
    {
        susp.clear();
        for (const auto &poly : poly) {
            auto to_check = poly.verteces.size();
            for (const auto &vertex : poly.verteces | std::views::take(to_check)) {
                susp.push_back(vertex);
            }
        }
        for (const auto &vertex : intersections) {
            susp.push_back(vertex);
        }
    }
    void paintEvent(QPaintEvent * /*event*/) override
    {
        auto p = QPainter();
        p.begin(this);

        p.setPen(theme.background_color);
        p.setBrush(theme.background_color);
        p.drawRect(this->rect());

        p.setRenderHint(QPainter::Antialiasing);

        p.translate(-offset / scale);
        p.scale(1 / scale, 1 / scale);

        auto set_brush = [&](const struct Light &light, bool total_fill) {
            auto light_color = theme.light_color;
            if (total_fill) {
                light_color.setAlpha(255);
            }
            if (theme.fading_enabled) {
                auto g = QRadialGradient(light.position, theme.fading_radius);
                g.setColorAt(0.0, light_color);
                g.setColorAt(1.0, Qt::transparent);
                auto b = QBrush(g);
                p.setBrush(b);
            } else {
                p.setBrush(light_color);
            }
        };

        auto draw_visible = [&](const struct Light &light) {
            set_brush(light, theme.fill_central);
            p.drawPolygon(light.visible[0].verteces.data(),
                          static_cast<int>(light.visible[0].verteces.size()));

            set_brush(light, false);
            for (const auto &visible : light.visible | std::views::drop(1)) {
                p.drawPolygon(visible.verteces.data(), static_cast<int>(visible.verteces.size()));
            }
        };

        auto pen = [&](auto color) {
            auto p = QPen(color, theme.edge_width);
            p.setCosmetic(true);
            return p;
        };

        // Remove floating vertex before casting rays
        if (selecting_poly) {
            poly.back().enclosed = false;
            poly.back().verteces.pop_back();
        }

        intersections_buf.light_shards = theme.light_shards;
        intersections_buf.light_radius = static_cast<float>(theme.light_radius);

        p.setPen(QPen(Qt::transparent));
        p.setBrush(QBrush(theme.light_color));

        for (auto &light : lights.write_no_invalidate()) {
            intersections_buf.compute_visible_polygons(light, poly, susp);
            draw_visible(light);
        }

        if (mouse_light && (lights.read().empty() || this->underMouse())) {
            cursor_light.position = cursor;
            cursor_light.invalidate();
            intersections_buf.compute_visible_polygons(cursor_light, poly, susp);
            draw_visible(cursor_light);
        }

        // Don't forget to restore floating vertex
        if (selecting_poly) {
            poly.back().enclosed = enclose_new_poly;
            poly.back().verteces.push_back(cursor);
        }

        auto pos_rect = [&](QPointF pos, double off) {
            off *= scale;
            return QRectF(QPointF(pos.x() - off, pos.y() - off),
                          QPointF(pos.x() + off, pos.y() + off));
        };

        auto icon = theme.light_icon.pixmap(QSize(theme.light_icon_size, theme.light_icon_size));
        for (const auto &light : lights.read()) {
            p.drawPixmap(pos_rect(light.position, static_cast<double>(theme.light_icon_size) / 2),
                         icon, QRect(0, 0, theme.light_icon_size, theme.light_icon_size));
        }

        p.setPen(pen(theme.vertex_color));
        p.setBrush(theme.vertex_color);

        for (const auto &poly : poly | std::views::drop(1)) {
            const auto &verteces = poly.verteces;
            p.drawPolyline(verteces.data(), static_cast<int>(verteces.size()));
            if (poly.enclosed && verteces.size() > 2) {
                p.drawLine(verteces.back(), verteces[0]);
            }
            for (const auto &v : verteces) {
                p.drawEllipse(pos_rect(v, theme.vertex_radius));
            }
        }
    }
    [[nodiscard]] std::optional<int> grab_light(QPointF cursor) const
    {
        for (auto i : std::views::iota(0UL, lights.read().size()) | std::views::reverse) {
            auto diff = lights.read()[i].position - cursor;
            if (QPointF::dotProduct(diff, diff) < 200.0 * scale * scale) {
                return i;
            }
        }
        return std::nullopt;
    }
    std::optional<std::tuple<int, int>> grab_vertex(QPointF cursor)
    {
        for (auto i : std::views::iota(1UL, poly.size()) | std::views::reverse) {
            const auto &poly = this->poly[i];
            for (auto j : std::views::iota(0UL, poly.verteces.size()) | std::views::reverse) {
                const auto &vertex = poly.verteces[j];
                auto diff = vertex - cursor;
                if (QPointF::dotProduct(diff, diff) < 200.0 * scale * scale) {
                    if (selecting_poly && i == this->poly.size() - 1
                        && j == poly.verteces.size() - 1) {
                        continue;
                    }
                    return std::make_tuple(i, j);
                }
            }
        }
        return std::nullopt;
    }
    void disconnect_edge(Polygon poly, uint64_t i)
    {
        if (i >= 1) {
            this->poly.emplace_back();
            for (auto j : std::views::iota(0UL, i + 1)) {
                this->poly.back().verteces.push_back(poly.verteces[j]);
            }
        }
        i += 1;
        if (poly.verteces.size() - i >= 2) {
            this->poly.emplace_back();
            auto to_check = poly.verteces.size();
            if (poly.enclosed) {
                to_check += 1;
            }
            for (auto j : std::views::iota(i, to_check)) {
                j %= poly.verteces.size();
                this->poly.back().verteces.push_back(poly.verteces[j]);
            }
        }
    }
    [[nodiscard]] QRect erase_frame() const
    {
        auto width = theme.light_icon_size;
        return { QPoint(0, 0), QPoint(width, width) };
    }
    [[nodiscard]] QRectF erase_rect(QPointF cursor) const
    {
        auto rad = scale * theme.light_icon_size / 2;
        return { cursor - QPointF(rad, rad), cursor + QPointF(rad, rad) };
    }
    void grab_erase(QPointF cursor)
    {
        auto rect = erase_rect(cursor);
        Polygon area;
        area.enclosed = true;
        area.verteces = {
            rect.topLeft(),
            rect.topRight(),
            rect.bottomRight(),
            rect.bottomLeft(),
        };
        auto rem = std::remove_if(
                lights.write_no_invalidate().begin(), lights.write_no_invalidate().end(),
                [&](const auto &light) { return point_inside(light.position, rect, 0); });
        if (rem != lights.read().end()) {
            lights.write_no_invalidate().erase(rem, lights.read().end());
        }

        bool vertex_changed = false;
        for (int i = 1; i < poly.size(); ++i) {
            auto to_check = poly[i].verteces.size();
            if (!poly[i].enclosed) {
                to_check -= 1;
            }
            for (const auto j : std::views::iota(0UL, to_check)) {
                bool to_disconnect = false;
                auto from = poly[i].verteces[j];
                auto to = poly[i].verteces[(j + 1) % poly[i].verteces.size()];

                to_disconnect |= point_inside(to, rect, 0.0);
                to_disconnect |= point_inside(from, rect, 0.0);

                if (!to_disconnect) {
                    auto segment = Segment(from, to);
                    for (const auto k : std::views::iota(0UL, area.verteces.size())) {
                        auto from = area.verteces[k];
                        auto to = area.verteces[(k + 1) % area.verteces.size()];
                        if (intersect_segment_segment(Segment(to, from), segment).has_value()) {
                            to_disconnect = true;
                            break;
                        }
                    }
                }
                if (to_disconnect) {
                    disconnect_edge(poly[i], j);
                    vertex_changed = true;
                    auto at = poly.begin();
                    std::advance(at, i);
                    this->poly.erase(at);
                    i -= 1;
                    break;
                }
            }
        }
        if (vertex_changed) {
            recompute_scene();
        }
    }
    void recompute_scene()
    {
        for (auto &light : lights.write_no_invalidate()) {
            light.invalidate();
        }
        recompute_intersections();
        update_susp();
        this->update();
    }
    void resizeEvent(QResizeEvent *event) override
    {
        QOpenGLWidget::resizeEvent(event);
        cursor_moved(this->cursor_raw);
        update_borders();
        recompute_scene();
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

        scale = -std::clamp(scale, -1.0, 1.0);
        if (scale >= 0.0) {
            scale += 1.0;
        } else {
            scale = 1 / (1 + std::abs(scale));
        }

        auto new_scale = this->scale * scale;
        if (new_scale > 10.0 || new_scale < 1.0 / 10.0) {
            return;
        }

        auto old_offset = this->offset;
        auto old_scale = this->scale;
        auto abs_pos = old_offset + cursor_raw * old_scale;
        this->offset = abs_pos - cursor_raw * new_scale;
        this->scale = new_scale;

        update_borders();
        cursor_moved(cursor_raw);
        recompute_scene();
    }
    void leaveEvent(QEvent * /*event*/) override { this->update(); }
    void mouseReleaseEvent(QMouseEvent *event) override
    {
        cursor_moved(event->pos());
        if (grabbing != Grabbing::None && event->button() == Qt::LeftButton) {
            setCursor(Qt::ArrowCursor);
            grabbing = Grabbing::None;
        }
    }
    void mouseMoveEvent(QMouseEvent *event) override
    {
        auto prev_cursor_raw = cursor_raw;
        cursor_moved(event->pos());
        switch (tool) {
        case Tool::Move: {
            switch (grabbing) {
            case Grabbing::None: {
                grabbing = Grabbing::Hover;
                setCursor(Qt::ClosedHandCursor);
            } break;
            case Grabbing::Active: {
                offset -= (cursor_raw - prev_cursor_raw) * scale;
                update_borders();
                cursor_moved(event->pos());
                recompute_scene();
            } break;
            case Grabbing::Hover: {
            } break;
            }
        } break;
        case Tool::Light: {
            assert(!selecting_poly);
            if (grabbing == Grabbing::Hover) {
                setCursor(Qt::ArrowCursor);
                grabbing = Grabbing::None;
            }
            switch (grabbing) {
            case Grabbing::Active: {
                lights.write_at(grabbing_light).position = cursor;
            } break;
            default: {
                auto closest = grab_light(cursor);
                if (closest.has_value()) {
                    grabbing = Grabbing::Hover;
                    setCursor(Qt::ClosedHandCursor);
                }
            }
            }
        } break;
        case Tool::Poly: {
            if (grabbing == Grabbing::Hover) {
                setCursor(Qt::ArrowCursor);
                grabbing = Grabbing::None;
            }
            switch (grabbing) {
            case Grabbing::Active: {
                poly[grabbing_poly].verteces[grabbing_vertex] = cursor;
                recompute_scene();
            } break;
            default: {
                auto snap = cursor;
                if (grabbing == Grabbing::None) {
                    auto closest = grab_vertex(cursor);
                    if (closest.has_value()) {
                        grabbing = Grabbing::Hover;
                        setCursor(Qt::ClosedHandCursor);
                        auto [poly_i, vertex_j] = closest.value();
                        snap = poly[poly_i].verteces[vertex_j];
                    }
                }
                if (selecting_poly) {
                    poly.back().verteces.back() = snap;
                }
            }
            }
        } break;
        case Tool::Erase: {
            assert(!selecting_poly);
            if (grabbing == Grabbing::Active) {
                grab_erase(cursor);
            }
        } break;
        }
    }
    void mousePressEvent(QMouseEvent *event) override
    {
        cursor_moved(event->pos());
        switch (tool) {
        case Tool::Move: {
            switch (event->button()) {
            case Qt::LeftButton: {
                grabbing = Grabbing::Active;
                setCursor(Qt::ClosedHandCursor);
            } break;
            default:
                break;
            }
        } break;
        case Tool::Light: {
            switch (event->button()) {
            case Qt::LeftButton: {
                if (grabbing == Grabbing::Hover) {
                    auto light = grab_light(cursor);
                    if (!light.has_value()) {
                        assert(false);
                        return;
                    }
                    grabbing_light = light.value();
                    lights.write_at(grabbing_light).position = cursor;
                    grabbing = Grabbing::Active;
                    return;
                }
                lights.write_no_invalidate().emplace_back(cursor);
            } break;
            case Qt::RightButton: {
                if (!lights.read().empty() && grabbing != Grabbing::Active) {
                    lights.write_no_invalidate().pop_back();
                }
            } break;
            default:
                break;
            }
        } break;
        case Tool::Poly: {
            switch (event->button()) {
            case Qt::RightButton: {
                if (grabbing != Grabbing::Active) {
                    release_vertex();
                }
            } break;
            case Qt::LeftButton: {
                switch (grabbing) {
                case Grabbing::Hover: {
                    auto vertex = grab_vertex(cursor);
                    if (!vertex.has_value()) {
                        assert(false);
                        return;
                    }

                    if (selecting_poly) {
                        if (std::make_tuple(poly.size() - 1, poly.back().verteces.size() - 1)
                            == vertex.value()) {
                            poly.back().verteces.pop_back();
                            poly.back().enclosed = true;
                            if (poly.back().verteces.size() < 2) {
                                this->poly.pop_back();
                            }
                        } else {
                            auto [poly_i, vertex_j] = vertex.value();
                            poly.back().verteces.back() = poly[poly_i].verteces[vertex_j];
                        }
                        selecting_poly = false;
                        recompute_scene();
                    } else {
                        auto [poly_i, vertex_j] = vertex.value();
                        grabbing_poly = poly_i;
                        grabbing_vertex = vertex_j;
                        poly[grabbing_poly].verteces[grabbing_vertex] = cursor;
                        grabbing = Grabbing::Active;
                        recompute_scene();
                    }
                    break;
                } break;
                case Grabbing::None: {
                    if (!selecting_poly) {
                        poly.emplace_back();
                        poly.back().enclosed = enclose_new_poly;
                        poly.back().verteces.push_back(cursor);
                    }
                    poly.back().verteces.push_back(cursor);
                    selecting_poly = true;
                    recompute_scene();
                } break;
                case Grabbing::Active:
                    break;
                }
            } break;
            default:
                break;
            }
        } break;
        case Tool::Erase: {
            assert(!selecting_poly);
            switch (event->button()) {
            case Qt::LeftButton: {
                grabbing = Grabbing::Active;
                auto rect = erase_frame();
                auto pixmap = QPixmap(rect.width(), rect.height());
                auto p = QPainter(&pixmap);
                p.setPen(QPen(Qt::darkRed, 2));
                p.setBrush(QBrush(Qt::red));
                p.drawRect(rect);
                setCursor(QCursor(pixmap));
                grab_erase(cursor);
            } break;
            default:
                break;
            }
        } break;
        }
    }
    void cursor_moved(QPoint cursor)
    {
        this->cursor_raw = cursor;
        this->cursor = clamp_point(offset + cursor.toPointF() * scale, transformed_rect(),
                                   1.0 + theme.light_radius);
        this->update();
    }
    void release_vertex()
    {
        if (poly.size() > 1) {
            poly.back().verteces.pop_back();
        }
        if (poly.back().verteces.size() < 2) {
            poly.pop_back();
        }
        selecting_poly = false;
        recompute_scene();
    }
};

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent)
{
    this->setObjectName("Graphics");
    this->resize(800, 600);
    this->setMouseTracking(false);

    auto *central = new QWidget(this);

    auto *verticalLayout = new QVBoxLayout(central);

    auto *canvas = new Canvas(central);
    connect(this, &MainWindow::theme_updated, this, [canvas]() {
        canvas->cursor_moved(canvas->cursor_raw);
        canvas->recompute_scene(); // A bit of an overkill, but keeps code simple
    });
    verticalLayout->addWidget(canvas);

    auto *tool_bar = new QToolBar("Drawing tools", central);
    tool_bar->setMovable(false);
    tool_bar->setContextMenuPolicy(Qt::ContextMenuPolicy::PreventContextMenu);

    auto *group = new QActionGroup(tool_bar);
    group->setExclusionPolicy(QActionGroup::ExclusionPolicy::Exclusive);
    group->setObjectName("draw_action");

    auto theme = Canvas::Theme{};
    canvas->theme.light_color = theme.light_color;

    auto add_tool = [&](auto tool, auto name, auto icon) {
        auto *action = tool_bar->addAction(source_icon(icon), name);
        action->setCheckable(true);
        if (tool == Canvas::Tool::Poly) {
            canvas->tool = tool;
            action->setChecked(true);
        }
        group->addAction(action);
        connect(action, &QAction::triggered, this, [canvas, tool] {
            if (canvas->selecting_poly) {
                canvas->release_vertex();
            }
            canvas->tool = tool;
        });
    };
    add_tool(Canvas::Tool::Move, "Move", "hand");
    add_tool(Canvas::Tool::Poly, "Add Poly", "draw-polyline");
    add_tool(Canvas::Tool::Light, "Add Light", "brightness-high");
    add_tool(Canvas::Tool::Erase, "Eraser", "draw-eraser");

    auto *spacer = new QWidget();
    spacer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    tool_bar->addWidget(spacer);
    auto *action = tool_bar->addAction(source_icon("draw-polygon"), "Enclose New Polygons");
    action->setCheckable(true);
    action->setChecked(canvas->enclose_new_poly);
    action->setChecked(canvas->mouse_light);
    connect(action, &QAction::triggered, this,
            [action, canvas] { canvas->enclose_new_poly = action->isChecked(); });

    action = tool_bar->addAction(source_icon("lighttable"), "Cursor Light");
    action->setCheckable(true);
    action->setChecked(canvas->mouse_light);
    connect(action, &QAction::triggered, this, [action, canvas] {
        canvas->mouse_light = action->isChecked();
        canvas->update();
    });

    auto *menu_button = new QToolButton(this);
    menu_button->setIcon(source_icon("application-menu"));
    menu_button->setPopupMode(QToolButton::InstantPopup);
    auto *menu = new QMenu(menu_button);

    {
        menu->addSection("Presets");
        auto *frame = new QWidget();
        auto *frame_layout = new QHBoxLayout(frame);
        auto add_theme = [&](const Canvas::Theme &theme, auto tooltip) {
            auto *button = new QPushButton(this);
            button->setToolTip(tooltip);
            button->setFlat(true);
            button->setIcon(bake_icon_split(theme.light_color, theme.background_color));
            button->setSizePolicy(QSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum));
            connect(button, &QPushButton::clicked, this, [theme, canvas, this] {
                canvas->theme = theme;
                emit this->theme_updated();
            });
            frame_layout->addWidget(button);
        };
        add_theme(Canvas::Theme{}, "Default");
        add_theme(
                Canvas::Theme{
                        .light_shards = 8,
                        .light_color = QColor("#32ffffff"),
                        .background_color = QColor("#1b1a1e"),
                        .vertex_color = QColor("#eeeeee"),
                },
                "@mechakotik");
        add_theme(
                Canvas::Theme{
                        .light_shards = 8,
                        .edge_width = 4,
                        .light_color = QColor("#28b49a09"),
                        .background_color = QColor("#1a1d1f"),
                        .vertex_color = QColor("#05066a"),
                },
                "@Timkak");
        auto *spacer = new QWidget();
        spacer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
        frame_layout->addWidget(spacer);
        frame->setLayout(frame_layout);
        menu->addAction(new CustomAction(frame));
    }

    {
        menu->addSection("Pallete");
        std::shared_ptr<int> color_counter{ new int(0) };
        auto add_picker = [&](QColor *prop, auto desc) {
            auto *button = new QPushButton(this);
            button->setStyleSheet("text-align: left;");
            button->setText(desc);
            button->setFlat(true);
            button->setIcon(bake_icon(*prop));
            menu->addAction(new CustomAction(button));
            connect(this, &MainWindow::theme_updated, this,
                    [prop, button]() { button->setIcon(bake_icon(*prop)); });
            connect(button, &QPushButton::clicked, this, [prop, this, color_counter] {
                auto old_color = *prop;
                auto color = QColorDialog::getColor(old_color, this, QString(),
                                                    QColorDialog::ShowAlphaChannel);
                if (color != QColor()) {
                    QColorDialog::setCustomColor(*color_counter, old_color);
                    *color_counter = std::min(*color_counter + 1, 16);
                    *prop = color;
                    emit this->theme_updated();
                }
            });
        };
        add_picker(&canvas->theme.light_color, "Light Color");
        add_picker(&canvas->theme.background_color, "Background Color");
        add_picker(&canvas->theme.vertex_color, "Vertex Color");
    }

    {
        menu->addSection("Params");
        auto add_slider = [&](int *prop, auto desc, int min, int max, int tick) {
            auto *frame = new QWidget();
            auto *slider = new QSlider(Qt::Orientation::Horizontal, central);
            slider->setSizePolicy(QSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred));
            slider->setTickPosition(QSlider::TicksBothSides);
            slider->setTickInterval(tick);
            slider->setSingleStep(tick);
            slider->setFocusPolicy(Qt::StrongFocus);
            slider->setMinimum(min);
            slider->setMaximum(max);
            slider->setValue(*prop);
            connect(this, &MainWindow::theme_updated, this,
                    [prop, slider]() { slider->setValue(*prop); });
            connect(slider, &QSlider::valueChanged, this, [prop, this](int value) {
                *prop = value;
                emit this->theme_updated();
            });
            auto *frame_layout = new QHBoxLayout(frame);
            frame_layout->addWidget(new QLabel(desc));
            frame_layout->addWidget(slider);
            frame->setLayout(frame_layout);
            menu->addAction(new CustomAction(frame));
        };
        add_slider(&canvas->theme.light_shards, "Light Shards", 3, 18, 1);
        add_slider(&canvas->theme.light_radius, "Light Radius", 2, 24, 2);
        add_slider(&canvas->theme.light_icon_size, "Icon Size", 24, 64, 8);
        add_slider(&canvas->theme.vertex_radius, "Vertex Radius", 1, 9, 1);
        add_slider(&canvas->theme.edge_width, "Edge Width", 1, 9, 1);

        auto add_check_box = [&](bool *prop, auto desc) {
            auto *frame = new QWidget();
            auto *check_box = new QCheckBox();
            check_box->setChecked(*prop);
            connect(this, &MainWindow::theme_updated, this,
                    [prop, check_box]() { check_box->setChecked(*prop); });
            connect(check_box, &QCheckBox::checkStateChanged, this,
                    [prop, this](Qt::CheckState state) {
                        *prop = state == Qt::CheckState::Checked;
                        emit this->theme_updated();
                    });
            auto *frame_layout = new QHBoxLayout(frame);
            frame_layout->addWidget(new QLabel(desc));
            frame_layout->addWidget(check_box);
            frame->setLayout(frame_layout);
            menu->addAction(new CustomAction(frame));
        };
        add_check_box(&canvas->theme.fading_enabled, "Light Fading");
        add_slider(&canvas->theme.fading_radius, "Fading Radius", 100, 2000, 200);
        add_check_box(&canvas->theme.fill_central, "Fill Central");
    }

    menu_button->setMenu(menu);
    tool_bar->addWidget(menu_button);
    this->setLayout(verticalLayout);

    this->addToolBar(tool_bar);
    this->setCentralWidget(central);

    this->setWindowTitle("Graphics");

    QMetaObject::connectSlotsByName(this);
}
