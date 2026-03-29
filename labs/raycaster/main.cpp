// =========================================================
//                   Unified Raycaster (main.cpp)
// =========================================================

#include <QApplication>
#include <QComboBox>
#include <QHBoxLayout>
#include <QMouseEvent>
#include <QPainter>
#include <QPainterPath>
#include <QPushButton>
#include <QVBoxLayout>
#include <QWidget>
#include <algorithm>
#include <cmath>
#include <iterator>
#include <limits>
#include <numbers>
#include <optional>
#include <ranges>
#include <vector>

// =========================================================
//                     Global Constants & Colors
// =========================================================
namespace GlobalConfig {
constexpr int TOP_PANEL_HEIGHT = 40;
constexpr int LIGHT_DIAMETER = 6;
constexpr double EPSILON = 1e-9;
constexpr double ROTATION_DELTA = 1e-4;
constexpr double ENDPOINT_TOLERANCE = 1e-3;
}  // namespace GlobalConfig

namespace GlobalColors {
const QColor BG_COLOR(9, 9, 46);
const QColor FINISHED_FILL(Qt::black);
const QColor ACTIVE_FILL(QColor(89, 20, 89, 100));
const QColor STROKE_COLOR(Qt::white);
const QColor LIGHT_COLOR(Qt::red);
const QColor LIGHT_AREA_FILL(255, 255, 255, 200);
const QColor SHADOW_FILL(255, 255, 255, 30);
}  // namespace GlobalColors

// =========================================================
//                   UTILITY FUNCTIONS
// =========================================================

inline double calcDistance(const QPoint& a, const QPoint& b) {
    return std::hypot(a.x() - b.x(), a.y() - b.y());
}

inline double calcDistance(const QPointF& a, const QPointF& b) {
    return std::hypot(a.x() - b.x(), a.y() - b.y());
}

inline double normalizeAngle(double angle) {
    while (angle < 0) {
        angle += 2 * std::numbers::pi;
    }
    return std::fmod(angle, 2 * std::numbers::pi);
}

inline std::optional<std::pair<double, double>> computeIntersectionParams(
    const QPoint& segStart, const QPoint& segEnd, const QPoint& rayOrigin, double ray_dx,
    double ray_dy) {
    double seg_dx = segEnd.x() - segStart.x();
    double seg_dy = segEnd.y() - segStart.y();
    double denominator = ray_dx * seg_dy - ray_dy * seg_dx;
    if (std::abs(denominator) < GlobalConfig::EPSILON) {
        return std::nullopt;
    }

    double t = ((segStart.x() - rayOrigin.x()) * seg_dy - (segStart.y() - rayOrigin.y()) * seg_dx) /
               denominator;
    double u = ((segStart.x() - rayOrigin.x()) * ray_dy - (segStart.y() - rayOrigin.y()) * ray_dx) /
               denominator;
    return std::make_pair(t, u);
}

// =========================================================
//             MODULE: RAY SEGMENT
// =========================================================
namespace RaySegmentNS {

class RaySegment {
   public:
    RaySegment(const QPoint& origin, const QPoint& endpoint, double angle)
        : start(origin), end(endpoint), direction(normalizeAngle(angle)) {
    }

    RaySegment(const QPoint& origin, const QPoint& endpoint)
        : start(origin)
        , end(endpoint)
        , direction(
              normalizeAngle(std::atan2(endpoint.y() - origin.y(), endpoint.x() - origin.x()))) {
    }

    RaySegment(const QPoint& origin, double angle, double length)
        : start(origin), direction(normalizeAngle(angle)) {
        end = QPoint(
            static_cast<int>(origin.x() + std::cos(angle) * length),
            static_cast<int>(origin.y() + std::sin(angle) * length));
    }

    const QPoint& getStart() const {
        return start;
    }

    const QPoint& getEnd() const {
        return end;
    }

    double getDirection() const {
        return direction;
    }

    void setStart(const QPoint& pt) {
        start = pt;
    }

    void setEnd(const QPoint& pt) {
        end = pt;
    }

    void setDirection(double angle) {
        direction = normalizeAngle(angle);
    }

    RaySegment rotated(double delta_angle) const {
        double newAngle = normalizeAngle(direction + delta_angle);
        double len = getLength();
        return RaySegment(start, newAngle, len);
    }

    double getLength() const {
        return calcDistance(start, end);
    }

    static bool areParallel(const RaySegment& a, const RaySegment& b) {
        QPoint vecA = a.end - a.start;
        QPoint vecB = b.end - b.start;
        double cross = vecA.x() * vecB.y() - vecA.y() * vecB.x();
        return std::abs(cross) < GlobalConfig::EPSILON;
    }

   private:
    QPoint start;
    QPoint end;
    double direction;
};

}  // namespace RaySegmentNS

inline void sortRaySegmentsByDirection(std::vector<RaySegmentNS::RaySegment>* rays) {
    std::ranges::sort(
        *rays, [](const RaySegmentNS::RaySegment& a, const RaySegmentNS::RaySegment& b) {
            return a.getDirection() < b.getDirection();
        });
}

// =========================================================
//        MODULE: POLYGON SHAPE (PolygonShapeNS)
// =========================================================
namespace PolygonShapeNS {

class PolygonShape {
   public:
    PolygonShape() = default;

    explicit PolygonShape(const std::vector<QPoint>& points) : vertices(points) {
    }

    void addVertex(const QPoint& pt) {
        vertices.push_back(pt);
    }

    void updateLastVertex(const QPoint& pt) {
        if (!vertices.empty()) {
            vertices.back() = pt;
        }
    }

    void clear() {
        vertices.clear();
    }

    const std::vector<QPoint>& getVertices() const {
        return vertices;
    }

    bool isValid() const {
        return vertices.size() >= 3;
    }

    std::vector<QPoint> closedVertices() const {
        std::vector<QPoint> pts = vertices;
        if (!pts.empty()) {
            pts.push_back(pts.front());
        }
        return pts;
    }

    std::optional<QPoint> findRayIntersection(const RaySegmentNS::RaySegment& ray) const {
        std::optional<QPoint> bestIntersection;
        double bestT = std::numeric_limits<double>::infinity();

        auto pts = closedVertices();
        for (size_t i = 1; i < pts.size(); ++i) {
            const QPoint& ptA = pts[i - 1];
            const QPoint& ptB = pts[i];

            double ray_dx = std::cos(ray.getDirection());
            double ray_dy = std::sin(ray.getDirection());

            auto optParams = computeIntersectionParams(ptA, ptB, ray.getStart(), ray_dx, ray_dy);
            if (!optParams.has_value()) {
                continue;
            }

            double denominator = ray_dx * (ptB.y() - ptA.y()) - ray_dy * (ptB.x() - ptA.x());
            double t = ((ptA.x() - ray.getStart().x()) * (ptB.y() - ptA.y()) -
                        (ptA.y() - ray.getStart().y()) * (ptB.x() - ptA.x())) /
                       denominator;
            double u = ((ptA.x() - ray.getStart().x()) * ray_dy -
                        (ptA.y() - ray.getStart().y()) * ray_dx) /
                       denominator;

            if (t >= 0 && u >= 0 && u <= 1) {
                if (t < bestT) {
                    bestT = t;
                    QPoint intersectPt(
                        static_cast<int>(ray.getStart().x() + ray_dx * t),
                        static_cast<int>(ray.getStart().y() + ray_dy * t));
                    bestIntersection = intersectPt;
                }
            }
        }
        return bestIntersection;
    }

   private:
    std::vector<QPoint> vertices;
};

}  // namespace PolygonShapeNS

// =========================================================
//      MODULE: RAYCASTER CONTROLLER (RaycasterController)
// =========================================================
enum class RenderMode { Light, Polygons };

class RaycasterController {
   public:
    RaycasterController() : lightPos(0, 0), currentMode(RenderMode::Light), constructing(false) {
        PolygonShapeNS::PolygonShape border(
            {QPoint(0, 0), QPoint(800, 0), QPoint(800, 600), QPoint(0, 600)});
        polygonList.push_back(border);
    }

    void beginPolygon(const QPoint& initPt) {
        currentPolygon = PolygonShapeNS::PolygonShape();
        currentPolygon.addVertex(initPt);
        currentPolygon.addVertex(initPt);
        polygonList.push_back(currentPolygon);
        constructing = true;
    }

    void appendVertex(const QPoint& pt) {
        if (!polygonList.empty()) {
            polygonList.back().addVertex(pt);
        }
    }

    void updateCurrentPolygon(const QPoint& pt) {
        if (!polygonList.empty()) {
            polygonList.back().updateLastVertex(pt);
        }
    }

    void completePolygon() {
        constructing = false;
    }

    const std::vector<PolygonShapeNS::PolygonShape>& getPolygons() const {
        return polygonList;
    }

    QPoint getLightPosition() const {
        return lightPos;
    }

    void setLightPosition(const QPoint& pt) {
        lightPos = pt;
    }

    std::vector<RaySegmentNS::RaySegment> generateLightRays(const QPoint& srcPos) const {
        std::vector<RaySegmentNS::RaySegment> rays;
        for (const auto& poly : polygonList) {
            for (const auto& vertex : poly.getVertices()) {
                RaySegmentNS::RaySegment baseRay(srcPos, vertex);
                rays.push_back(baseRay);
                rays.push_back(baseRay.rotated(-GlobalConfig::ROTATION_DELTA));
                rays.push_back(baseRay.rotated(GlobalConfig::ROTATION_DELTA));
            }
        }
        sortRaySegmentsByDirection(&rays);
        return rays;
    }

    std::vector<RaySegmentNS::RaySegment> generateLightRays() const {
        return generateLightRays(lightPos);
    }

    void processRayIntersections(std::vector<RaySegmentNS::RaySegment>* rays) const {
        for (auto& ray : *rays) {
            double minDist = std::numeric_limits<double>::infinity();
            std::optional<QPoint> bestIntersection;
            for (const auto& poly : polygonList) {
                if (auto intersect = poly.findRayIntersection(ray); intersect.has_value()) {
                    double dist = calcDistance(ray.getStart(), *intersect);
                    if (dist < minDist) {
                        minDist = dist;
                        bestIntersection = intersect;
                    }
                }
            }
            if (bestIntersection.has_value()) {
                ray.setEnd(*bestIntersection);
            }
        }
    }

    void filterDuplicateRays(std::vector<RaySegmentNS::RaySegment>* rays) const {
        if (rays->size() <= 1) {
            return;
        }
        auto newEnd = std::unique(
            rays->begin(), rays->end(),
            [](const RaySegmentNS::RaySegment& a, const RaySegmentNS::RaySegment& b) {
                return (calcDistance(a.getEnd(), b.getEnd()) < GlobalConfig::ENDPOINT_TOLERANCE);
            });
        rays->erase(newEnd, rays->end());
    }

    std::vector<QPoint> computeLightArea() const {
        auto rays = generateLightRays();
        processRayIntersections(&rays);
        filterDuplicateRays(&rays);
        std::vector<QPoint> area;
        for (const auto& ray : rays) {
            area.push_back(ray.getEnd());
        }
        return area;
    }

   private:
    std::vector<PolygonShapeNS::PolygonShape> polygonList;
    PolygonShapeNS::PolygonShape currentPolygon;
    QPoint lightPos;
    RenderMode currentMode;
    bool constructing;
};

// =========================================================
//          MODULE: CANVAS WIDGET (VIEW COMPONENT)
// =========================================================

class CanvasWidget : public QWidget {
   public:
    explicit CanvasWidget(QWidget* parent = nullptr)
        : QWidget(parent), activeMode(RenderMode::Light), isDrawing(false), previewPt(0, 0) {
        setMouseTracking(true);
    }

    void setRenderMode(RenderMode newMode) {
        if (activeMode != newMode) {
            if (activeMode == RenderMode::Polygons && isDrawing) {
                controller.completePolygon();
                isDrawing = false;
            }
            activeMode = newMode;
            update();
        }
    }

   protected:
    void paintEvent(QPaintEvent* /*event*/) override {
        QPainter painter(this);
        painter.fillRect(rect(), GlobalColors::BG_COLOR);
        painter.setRenderHint(QPainter::Antialiasing);

        double scaleX = static_cast<double>(width()) / 800.0;
        double scaleY = static_cast<double>(height()) / 600.0;
        painter.scale(scaleX, scaleY);

        painter.setPen(GlobalColors::STROKE_COLOR);
        for (const auto& poly : controller.getPolygons()) {
            auto closedVerts = poly.closedVertices();
            if (!closedVerts.empty()) {
                QPainterPath polyPath;
                polyPath.moveTo(poly.getVertices().front());
                for (size_t i = 1; i < poly.getVertices().size(); ++i) {
                    polyPath.lineTo(poly.getVertices()[i]);
                }
                polyPath.closeSubpath();
                painter.fillPath(polyPath, QBrush(GlobalColors::FINISHED_FILL));
                for (size_t i = 0; i < closedVerts.size() - 1; ++i) {
                    painter.drawLine(closedVerts[i], closedVerts[i + 1]);
                }
            }
        }

        if (activeMode == RenderMode::Light) {
            painter.setBrush(GlobalColors::LIGHT_COLOR);
            painter.setPen(Qt::NoPen);
            QPoint lightPos = controller.getLightPosition();
            painter.drawEllipse(
                lightPos, GlobalConfig::LIGHT_DIAMETER / 2, GlobalConfig::LIGHT_DIAMETER / 2);

            auto lightArea = controller.computeLightArea();
            if (!lightArea.empty()) {
                QPainterPath areaPath;
                areaPath.moveTo(lightArea.front());
                for (size_t i = 1; i < lightArea.size(); ++i) {
                    areaPath.lineTo(lightArea[i]);
                }
                areaPath.closeSubpath();
                painter.fillPath(areaPath, QBrush(GlobalColors::LIGHT_AREA_FILL));
            }
        }
        else if (activeMode == RenderMode::Polygons) {
            const auto& polys = controller.getPolygons();
            if (!polys.empty()) {
                const auto& verts = polys.back().getVertices();
                if (!verts.empty()) {
                    if (verts.size() >= 3) {
                        auto closedVerts = polys.back().closedVertices();
                        QPainterPath polyPath;
                        polyPath.moveTo(closedVerts.front());
                        for (size_t i = 1; i < closedVerts.size(); ++i) {
                            polyPath.lineTo(closedVerts[i]);
                        }
                        polyPath.closeSubpath();
                        painter.fillPath(polyPath, QBrush(GlobalColors::ACTIVE_FILL));
                        for (size_t i = 0; i < closedVerts.size() - 1; ++i) {
                            painter.drawLine(closedVerts[i], closedVerts[i + 1]);
                        }
                    } else {
                        for (size_t i = 0; i < verts.size() - 1; ++i) {
                            painter.drawLine(verts[i], verts[i + 1]);
                        }
                        painter.drawLine(verts.back(), previewPt);
                    }
                }
            }
        }
    }

    void resizeEvent(QResizeEvent* event) override {
        QWidget::resizeEvent(event);
        update();
    }

    QPoint convertToScene(const QPoint& widgetPos) const {
        double scaleX = static_cast<double>(width()) / 800.0;
        double scaleY = static_cast<double>(height()) / 600.0;
        return QPoint(
            static_cast<int>(widgetPos.x() / scaleX), static_cast<int>(widgetPos.y() / scaleY));
    }

    void mousePressEvent(QMouseEvent* event) override {
        QPoint scenePos = convertToScene(event->pos());
        if (activeMode == RenderMode::Light) {
            controller.setLightPosition(scenePos);
        } else if (activeMode == RenderMode::Polygons) {
            if (event->button() == Qt::LeftButton) {
                if (!isDrawing) {
                    isDrawing = true;
                    controller.beginPolygon(scenePos);
                    previewPt = scenePos;
                } else {
                    controller.appendVertex(scenePos);
                }
            } else if (event->button() == Qt::RightButton) {
                isDrawing = false;
                controller.completePolygon();
            }
        }
        update();
    }

    void mouseMoveEvent(QMouseEvent* event) override {
        QPoint scenePos = convertToScene(event->pos());
        if (activeMode == RenderMode::Light) {
            controller.setLightPosition(scenePos);
        } else if (activeMode == RenderMode::Polygons && isDrawing) {
            controller.updateCurrentPolygon(scenePos);
            previewPt = scenePos;
        }
        update();
    }

   private:
    RenderMode activeMode;
    RaycasterController controller;
    bool isDrawing;
    QPoint previewPt;
};

// =========================================================
//             MODULE: MAIN WINDOW SETUP
// =========================================================

QWidget* createMainWindow() {
    QWidget* mainWin = new QWidget;
    QVBoxLayout* mainLayout = new QVBoxLayout(mainWin);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);

    QWidget* topPanel = new QWidget(mainWin);
    topPanel->setFixedHeight(GlobalConfig::TOP_PANEL_HEIGHT);
    QHBoxLayout* topLayout = new QHBoxLayout(topPanel);
    topLayout->setContentsMargins(10, 10, 10, 5);
    topLayout->setSpacing(10);

    QComboBox* modeSwitcher = new QComboBox(topPanel);
    modeSwitcher->addItem("Light");
    modeSwitcher->addItem("Polygons");
    topLayout->addWidget(modeSwitcher, 0, Qt::AlignLeft);

    topLayout->addStretch();

    QPushButton* fpsIndicator = new QPushButton("FPS: N/A", topPanel);
    fpsIndicator->setEnabled(false);
    topLayout->addWidget(fpsIndicator, 0, Qt::AlignRight);

    mainLayout->addWidget(topPanel);

    QWidget* drawArea = new QWidget(mainWin);
    QVBoxLayout* drawLayout = new QVBoxLayout(drawArea);
    drawLayout->setContentsMargins(10, 5, 10, 10);
    drawLayout->setSpacing(0);

    CanvasWidget* canvas = new CanvasWidget(drawArea);
    drawLayout->addWidget(canvas);
    mainLayout->addWidget(drawArea);

    QObject::connect(
        modeSwitcher, QOverload<int>::of(&QComboBox::currentIndexChanged), [canvas](int index) {
            if (index == 0) {
                canvas->setRenderMode(RenderMode::Light);
            } else {
                canvas->setRenderMode(RenderMode::Polygons);
            }
        });

    return mainWin;
}

// =========================================================
//                            MAIN
// =========================================================

int main(int argc, char* argv[]) {
    QApplication app(argc, argv);
    QWidget* mainWindow = createMainWindow();
    mainWindow->resize(800, 600);
    mainWindow->show();
    return app.exec();
}
