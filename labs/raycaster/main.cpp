#include <QApplication>
#include <QComboBox>
#include <QCursor>
#include <QDoubleSpinBox>
#include <QElapsedTimer>
#include <QEnterEvent>
#include <QFocusEvent>
#include <QFormLayout>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QKeyEvent>
#include <QKeySequence>
#include <QLabel>
#include <QMainWindow>
#include <QMouseEvent>
#include <QPaintEvent>
#include <QPainter>
#include <QPainterPath>
#include <QPixmap>
#include <QPolygonF>
#include <QPushButton>
#include <QSizePolicy>
#include <QSlider>
#include <QSpinBox>
#include <QStackedWidget>
#include <QTimer>
#include <QVBoxLayout>
#include <QWidget>
#include <cmath>
#include <exception>
#include <functional>
#include <limits>
#include <map>
#include <memory>
#include <mutex>
#include <optional>
#include <thread>
#include <variant>
#include <vector>

// --- Raycaster Constants ---
const double RC_EPSILON_ANGLE = 0.0001;
const double RC_VERY_SMALL_DISTANCE_SQUARED = 0.5 * 0.5;
const double RC_POINT_UNIQUENESS_THRESHOLD_SQUARED = 1e-6 * 1e-6;
const double RC_FAR_POINT_MULTIPLIER = 20000.0;
const double RC_SEGMENT_INTERSECTION_EPSILON = 1e-7;
const double RC_COLLISION_OFFSET = 0.1;

const int RC_MAX_MASTER_CORRECTION_ITERATIONS = 5;
const int RC_MAX_GROUP_CORRECTION_ATTEMPTS = 10;

// --- GUI Constants ---
const int MAX_EXTRA_LIGHT_SOURCES = 30;
const double MAX_BRIGHTNESS = 2.0;
const int SLIDER_PRECISION_FACTOR = 100;
const int PANEL_ANIMATION_DURATION = 700;
const int PANEL_COLLAPSED_WIDTH = 0;
const int PANEL_TOGGLE_BUTTON_WIDTH = 20;
const int PANEL_EXPANDED_WIDTH = 280;
const int NOTIFICATION_DURATION = 3000;
const int NOTIFICATION_FADE_DELAY = 1000;
const int NOTIFICATION_FADE_DURATION = 2000;
const int SETTINGS_COMMIT_TIMEOUT = 3000;
const int FPS_UPDATE_INTERVAL = 250;
const int STANDARD_MARGIN = 10;

// --- Forward Declarations ---
class NotificationWidget;
class CanvasWidget;
class FpsCounter;
class Command;
class SettingsChangeCommand;
class MainWindow;
class CommitOnFocusOutSpinBox;
class CommitOnFocusOutDoubleSpinBox;
class CommitOnFocusOutSlider;
class Ray;
class Polygon;
class Controller;

// --- Type Aliases ---
using SettingValue = std::variant<int, double, bool>;

// --- Default Settings Structure ---
struct ModeDefaults {
    int lightCount = 11;
    double lightSpread = 25.0;
    int staticCount = 1;
    double staticSpread = 15.0;
    int staticBrightness = 1 * SLIDER_PRECISION_FACTOR;
    bool polyDeleting = false;
    bool staticDeleting = false;
};

// =============================================================================
// Raycaster Logic
// =============================================================================
struct RC_QPointFLess {
    bool operator()(const QPointF& a, const QPointF& b) const {
        if (std::abs(a.x() - b.x()) > RC_SEGMENT_INTERSECTION_EPSILON) {
            return a.x() < b.x();
        }
        if (std::abs(a.y() - b.y()) > RC_SEGMENT_INTERSECTION_EPSILON) {
            return a.y() < b.y();
        }
        return false;
    }
};

struct RC_QPointFCloseEnough {
    bool operator()(const QPointF& a, const QPointF& b) const {
        QPointF diff = a - b;
        return QPointF::dotProduct(diff, diff) < RC_POINT_UNIQUENESS_THRESHOLD_SQUARED;
    }
};

std::optional<QPointF> RC_IntersectLineSegments(
    const QPointF& p1, const QPointF& p2, const QPointF& p3, const QPointF& p4) {
    QPointF s1 = p2 - p1;
    QPointF s2 = p4 - p3;
    double det = s1.x() * s2.y() - s1.y() * s2.x();
    if (std::abs(det) < 1e-9) {
        return std::nullopt;
    }
    double t = ((p3.x() - p1.x()) * s2.y() - (p3.y() - p1.y()) * s2.x()) / det;
    double u = ((p1.x() - p3.x()) * s1.y() - (p1.y() - p3.y()) * s1.x()) / -det;
    if (t >= -RC_SEGMENT_INTERSECTION_EPSILON && t <= 1.0 + RC_SEGMENT_INTERSECTION_EPSILON &&
        u >= -RC_SEGMENT_INTERSECTION_EPSILON && u <= 1.0 + RC_SEGMENT_INTERSECTION_EPSILON) {
        return p1 + t * s1;
    }
    return std::nullopt;
}

QPointF RC_closestPointOnSegment(const QPointF& p, const QPointF& seg_p1, const QPointF& seg_p2) {
    QPointF seg_vec = seg_p2 - seg_p1;
    double seg_len_sq = QPointF::dotProduct(seg_vec, seg_vec);
    if (seg_len_sq < 1e-9) {
        return seg_p1;
    }
    QPointF p_minus_seg_p1 = p - seg_p1;
    double t = QPointF::dotProduct(p_minus_seg_p1, seg_vec) / seg_len_sq;
    t = std::max(0.0, std::min(1.0, t));
    return seg_p1 + t * seg_vec;
}

QPointF RC_normalized(const QPointF& vec) {
    double len_sq = QPointF::dotProduct(vec, vec);
    if (len_sq < 1e-9) {
        return QPointF(0, 0);
    }
    double len = std::sqrt(len_sq);
    return vec / len;
}

class Ray {
   public:
    Ray(const QPointF& begin, const QPointF& end, double angle)
        : m_begin(begin), m_end(end), m_angle(angle) {
    }

    QPointF getBegin() const {
        return m_begin;
    }

    QPointF getEnd() const {
        return m_end;
    }

    double getAngle() const {
        return m_angle;
    }

    void setBegin(const QPointF& begin) {
        m_begin = begin;
        updateAngle();
    }

    void setEnd(const QPointF& end) {
        m_end = end;
        updateAngle();
    }

   private:
    void updateAngle() {
        QPointF dir = m_end - m_begin;
        m_angle = std::atan2(dir.y(), dir.x());
    }

    QPointF m_begin;
    QPointF m_end;
    double m_angle;
};

class Polygon {
   public:
    Polygon(const std::vector<QPointF>& vertices = {}) : m_vertices(vertices) {
    }

    const std::vector<QPointF>& getVertices() const {
        return m_vertices;
    }

    std::vector<QPointF>& getVerticesMutable() {
        return m_vertices;
    }

    void AddVertex(const QPointF& vertex) {
        m_vertices.push_back(vertex);
    }

    void UpdateLastVertex(const QPointF& new_vertex) {
        if (!m_vertices.empty()) {
            m_vertices.back() = new_vertex;
        }
    }

    std::optional<QPointF> IntersectRay(const Ray& ray) const {
        if (m_vertices.size() < 2) {
            return std::nullopt;
        }
        std::optional<QPointF> closest_intersection;
        double min_dist_sq_to_ray_begin = std::numeric_limits<double>::max();
        QPointF ray_p1 = ray.getBegin();
        QPointF ray_s1 = ray.getEnd() - ray_p1;
        for (size_t i = 0; i < m_vertices.size(); ++i) {
            QPointF edge_p1 = m_vertices[i];
            QPointF edge_p2 = m_vertices[(i + 1) % m_vertices.size()];
            QPointF edge_s2 = edge_p2 - edge_p1;
            double det = ray_s1.x() * edge_s2.y() - ray_s1.y() * edge_s2.x();
            if (std::abs(det) < 1e-9) {
                continue;
            }
            double t = ((edge_p1.x() - ray_p1.x()) * edge_s2.y() -
                        (edge_p1.y() - ray_p1.y()) * edge_s2.x()) /
                       det;
            double u = -((ray_p1.x() - edge_p1.x()) * ray_s1.y() -
                         (ray_p1.y() - edge_p1.y()) * ray_s1.x()) /
                       det;
            if (t >= -RC_SEGMENT_INTERSECTION_EPSILON && u >= -RC_SEGMENT_INTERSECTION_EPSILON &&
                u <= 1.0 + RC_SEGMENT_INTERSECTION_EPSILON) {
                QPointF intersection_point = ray_p1 + t * ray_s1;
                QPointF vec_to_intersection = intersection_point - ray.getBegin();
                double dist_sq = QPointF::dotProduct(vec_to_intersection, vec_to_intersection);
                if (dist_sq < min_dist_sq_to_ray_begin) {
                    min_dist_sq_to_ray_begin = dist_sq;
                    closest_intersection = intersection_point;
                }
            }
        }
        return closest_intersection;
    }

    bool isPointStrictlyInside(const QPointF& point) const {
        if (m_vertices.size() < 3) {
            return false;
        }
        int crossings = 0;
        for (size_t i = 0; i < m_vertices.size(); ++i) {
            QPointF p1 = m_vertices[i];
            QPointF p2 = m_vertices[(i + 1) % m_vertices.size()];
            double cross_product_z =
                (point.x() - p1.x()) * (p2.y() - p1.y()) - (point.y() - p1.y()) * (p2.x() - p1.x());
            if (std::abs(cross_product_z) < RC_SEGMENT_INTERSECTION_EPSILON) {
                if (std::min(p1.x(), p2.x()) - RC_SEGMENT_INTERSECTION_EPSILON <= point.x() &&
                    point.x() <= std::max(p1.x(), p2.x()) + RC_SEGMENT_INTERSECTION_EPSILON &&
                    std::min(p1.y(), p2.y()) - RC_SEGMENT_INTERSECTION_EPSILON <= point.y() &&
                    point.y() <= std::max(p1.y(), p2.y()) + RC_SEGMENT_INTERSECTION_EPSILON) {
                    return false;
                }
            }
            if ((p1.y() <= point.y() + RC_SEGMENT_INTERSECTION_EPSILON &&
                 p2.y() > point.y() + RC_SEGMENT_INTERSECTION_EPSILON) ||
                (p2.y() <= point.y() + RC_SEGMENT_INTERSECTION_EPSILON &&
                 p1.y() > point.y() + RC_SEGMENT_INTERSECTION_EPSILON)) {
                if (std::abs(p2.y() - p1.y()) > RC_SEGMENT_INTERSECTION_EPSILON) {
                    double vt = (point.y() - p1.y()) / (p2.y() - p1.y());
                    if (p1.x() + vt * (p2.x() - p1.x()) >
                        point.x() - RC_SEGMENT_INTERSECTION_EPSILON) {
                        crossings++;
                    }
                }
            }
        }
        return crossings % 2 == 1;
    }

   private:
    std::vector<QPointF> m_vertices;
};

class Controller {
   public:
    struct StaticLightSourceInfo {
        QPointF masterPosition;
        std::vector<QPointF> satellitePositions;
        int id;
        static int next_id_counter;

        StaticLightSourceInfo() : id(next_id_counter++) {
        }
    };

    Controller() {
        unsigned int num_hw_threads = std::thread::hardware_concurrency();
        m_num_worker_threads = (num_hw_threads > 1) ? (num_hw_threads - 1) : 1;
        if (m_num_worker_threads == 0) {
            m_num_worker_threads = 1;
        }
        ModeDefaults defaults;
        m_num_light_sources = defaults.lightCount;
        m_light_source_spread = defaults.lightSpread;
        m_master_light_pos = QPointF();
        m_light_sources.resize(m_num_light_sources);
        m_mouse_pos = QPointF(0, 0);
    }

    const std::vector<Polygon>& GetPolygons() const {
        return m_polygons_const_ref;
    }

    const std::vector<QPointF>& getLightSources() const {
        return m_light_sources;
    }

    const std::vector<StaticLightSourceInfo>& getStaticLightSourcesInfo() const {
        return m_static_light_sources_info;
    }

    const std::vector<QPointF>& getCurrentPolygonVertices() const {
        return m_current_polygon_vertices;
    }

    QPointF getMouseHintPosition() const {
        return m_mouse_pos;
    }

    int getNumLightSources() const {
        return m_num_light_sources;
    }

    double getLightSpread() const {
        return m_light_source_spread;
    }

    QPointF getMasterLightPosition() const {
        std::lock_guard<std::mutex> lock(m_geometry_mutex);
        return m_master_light_pos;
    }

    void setNumLightSources(int count) {
        std::lock_guard<std::mutex> lock(m_geometry_mutex);
        int clamped_count = std::max(1, std::min(count, MAX_EXTRA_LIGHT_SOURCES + 1));
        if (clamped_count != m_num_light_sources) {
            m_num_light_sources = clamped_count;
            if (!m_master_light_pos.isNull()) {
                generateGenericSatellitePositions(
                    m_master_light_pos, m_light_sources, m_num_light_sources,
                    m_light_source_spread);
            } else {
                m_light_sources.resize(m_num_light_sources);
            }
        }
    }

    void setLightSpread(double spread) {
        std::lock_guard<std::mutex> lock(m_geometry_mutex);
        double clamped_spread = std::max(0.1, spread);
        if (std::abs(clamped_spread - m_light_source_spread) > 1e-6) {
            m_light_source_spread = clamped_spread;
            if (!m_master_light_pos.isNull()) {
                generateGenericSatellitePositions(
                    m_master_light_pos, m_light_sources, m_num_light_sources,
                    m_light_source_spread);
            }
        }
    }

    void AddPolygonToScene(Polygon polygon) {
        if (polygon.getVertices().size() >= 3) {
            std::lock_guard<std::mutex> lock(m_geometry_mutex);
            m_polygons.push_back(std::move(polygon));
            m_geometry_dirty = true;
        }
    }

    void deletePolygonByIndex(int index) {
        std::lock_guard<std::mutex> lock(m_geometry_mutex);
        if (index > 0 && index < (int)m_polygons.size()) {
            m_polygons.erase(m_polygons.begin() + index);
            m_geometry_dirty = true;
        }
    }

    void addVertexToCurrentPolygon(const QPointF& pos) {
        m_current_polygon_vertices.push_back(pos);
    }

    void cancelCurrentPolygon() {
        m_current_polygon_vertices.clear();
    }

    void updateMousePosition(const QPointF& pos) {
        m_mouse_pos = pos;
    }

    bool finishCurrentPolygon() {
        if (m_current_polygon_vertices.size() < 3) {
            m_current_polygon_vertices.clear();
            return true;
        }
        Polygon new_polygon(m_current_polygon_vertices);
        bool collision_detected = false;
        std::vector<QPointF> all_active_light_points;
        {
            std::lock_guard<std::mutex> lock_dyn(m_geometry_mutex);
            if (!m_master_light_pos.isNull()) {
                all_active_light_points.push_back(m_master_light_pos);
            }
            for (const auto& p : m_light_sources) {
                if (!p.isNull()) {
                    all_active_light_points.push_back(p);
                }
            }
        }
        {
            std::lock_guard<std::mutex> lock_static(m_static_lights_mutex);
            for (const auto& sl_info : m_static_light_sources_info) {
                if (!sl_info.masterPosition.isNull()) {
                    all_active_light_points.push_back(sl_info.masterPosition);
                }
                for (const auto& sat_p : sl_info.satellitePositions) {
                    if (!sat_p.isNull()) {
                        all_active_light_points.push_back(sat_p);
                    }
                }
            }
        }
        for (const auto& light_pos : all_active_light_points) {
            if (new_polygon.isPointStrictlyInside(light_pos)) {
                collision_detected = true;
                break;
            }
        }
        if (collision_detected) {
            m_current_polygon_vertices.clear();
            return false;
        } else {
            AddPolygonToScene(std::move(new_polygon));
            m_current_polygon_vertices.clear();
            return true;
        }
    }

    QPointF findClosestPointOnPolygonBoundary(const QPointF& point, const Polygon& poly) const {
        QPointF closest_point_overall;
        double min_dist_sq_overall = std::numeric_limits<double>::max();
        bool found = false;
        const auto& vertices = poly.getVertices();
        if (vertices.size() < 2) {
            if (vertices.size() == 1) {
                return vertices[0];
            }
            return point;
        }
        for (size_t i = 0; i < vertices.size(); ++i) {
            QPointF p1 = vertices[i];
            QPointF p2 = vertices[(i + 1) % vertices.size()];
            QPointF current_closest_on_segment = RC_closestPointOnSegment(point, p1, p2);
            QPointF diff = point - current_closest_on_segment;
            double dist_sq = QPointF::dotProduct(diff, diff);
            if (!found || dist_sq < min_dist_sq_overall) {
                min_dist_sq_overall = dist_sq;
                closest_point_overall = current_closest_on_segment;
                found = true;
            }
        }
        return found ? closest_point_overall : point;
    }

    void generateGenericSatellitePositions(
        const QPointF& group_center_pos, std::vector<QPointF>& out_all_light_sources_in_group,
        int num_total_in_group, double spread_for_group) const {
        if (out_all_light_sources_in_group.size() != (size_t)num_total_in_group) {
            out_all_light_sources_in_group.resize(num_total_in_group);
        }
        if (num_total_in_group == 0) {
            return;
        }
        if (num_total_in_group == 1) {
            if (!out_all_light_sources_in_group.empty()) {
                out_all_light_sources_in_group[0] = group_center_pos;
            }
            return;
        }
        for (int i = 0; i < num_total_in_group; ++i) {
            if (spread_for_group < 1e-6) {
                if (i < (int)out_all_light_sources_in_group.size()) {
                    out_all_light_sources_in_group[i] = group_center_pos;
                }
            } else {
                double angle = 2.0 * M_PI * static_cast<double>(i) / num_total_in_group;
                if (i < (int)out_all_light_sources_in_group.size()) {
                    out_all_light_sources_in_group[i] =
                        group_center_pos +
                        QPointF(
                            std::cos(angle) * spread_for_group, std::sin(angle) * spread_for_group);
                }
            }
        }
    }

    void generateSatellitesForStaticSource(
        const QPointF& static_master_pos, std::vector<QPointF>& out_satellites_only,
        int num_satellites, double spread) const {
        if (out_satellites_only.size() != (size_t)num_satellites) {
            out_satellites_only.resize(num_satellites);
        }
        if (num_satellites == 0) {
            return;
        }
        for (int i = 0; i < num_satellites; ++i) {
            if (spread < 1e-6) {
                if (i < (int)out_satellites_only.size()) {
                    out_satellites_only[i] = static_master_pos;
                }
            } else {
                double angle_step = 2.0 * M_PI / num_satellites;
                double angle = static_cast<double>(i) * angle_step;
                if (i < (int)out_satellites_only.size()) {
                    out_satellites_only[i] =
                        static_master_pos +
                        QPointF(std::cos(angle) * spread, std::sin(angle) * spread);
                }
            }
        }
    }

    void setMasterLightSourcePosition(
        const QPointF& desired_master_pos, const QRectF& boundaries_rect) {
        std::lock_guard<std::mutex> lock(m_geometry_mutex);
        QPointF old_master_pos = m_master_light_pos;
        std::vector<QPointF> old_satellite_pos = m_light_sources;
        QPointF current_master_candidate = desired_master_pos;
        current_master_candidate.setX(std::max(
            boundaries_rect.left() + RC_COLLISION_OFFSET,
            std::min(current_master_candidate.x(), boundaries_rect.right() - RC_COLLISION_OFFSET)));
        current_master_candidate.setY(std::max(
            boundaries_rect.top() + RC_COLLISION_OFFSET,
            std::min(
                current_master_candidate.y(), boundaries_rect.bottom() - RC_COLLISION_OFFSET)));
        for (int iter = 0; iter < RC_MAX_MASTER_CORRECTION_ITERATIONS; ++iter) {
            bool master_collided_this_iter = false;
            for (size_t poly_idx = 1; poly_idx < m_polygons.size(); ++poly_idx) {
                const auto& obstacle_poly = m_polygons[poly_idx];
                if (obstacle_poly.isPointStrictlyInside(current_master_candidate)) {
                    master_collided_this_iter = true;
                    QPointF closest_pt =
                        findClosestPointOnPolygonBoundary(current_master_candidate, obstacle_poly);
                    QPointF push_vec = closest_pt - current_master_candidate;
                    if (QPointF::dotProduct(push_vec, push_vec) > 1e-9) {
                        current_master_candidate =
                            closest_pt + RC_normalized(push_vec) * RC_COLLISION_OFFSET;
                    } else {
                        QPointF poly_center(0, 0);
                        const auto& verts = obstacle_poly.getVertices();
                        if (!verts.empty()) {
                            for (const auto& v : verts) {
                                poly_center += v;
                            }
                            poly_center /= verts.size();
                        }
                        QPointF dir_from_center = current_master_candidate - poly_center;
                        if (QPointF::dotProduct(dir_from_center, dir_from_center) > 1e-9) {
                            current_master_candidate +=
                                RC_normalized(dir_from_center) * RC_COLLISION_OFFSET;
                        } else {
                            current_master_candidate += QPointF(RC_COLLISION_OFFSET, 0);
                        }
                    }
                    current_master_candidate.setX(std::max(
                        boundaries_rect.left() + RC_COLLISION_OFFSET,
                        std::min(
                            current_master_candidate.x(),
                            boundaries_rect.right() - RC_COLLISION_OFFSET)));
                    current_master_candidate.setY(std::max(
                        boundaries_rect.top() + RC_COLLISION_OFFSET,
                        std::min(
                            current_master_candidate.y(),
                            boundaries_rect.bottom() - RC_COLLISION_OFFSET)));
                    break;
                }
            }
            if (!master_collided_this_iter) {
                break;
            }
        }
        std::vector<QPointF> current_all_dynamic_lights_candidates;
        bool group_position_valid = false;
        for (int group_attempt = 0; group_attempt < RC_MAX_GROUP_CORRECTION_ATTEMPTS;
             ++group_attempt) {
            generateGenericSatellitePositions(
                current_master_candidate, current_all_dynamic_lights_candidates,
                m_num_light_sources, m_light_source_spread);
            bool any_collision_in_group = false;
            for (QPointF& light_candidate_pos : current_all_dynamic_lights_candidates) {
                light_candidate_pos.setX(std::max(
                    boundaries_rect.left() + RC_COLLISION_OFFSET,
                    std::min(
                        light_candidate_pos.x(), boundaries_rect.right() - RC_COLLISION_OFFSET)));
                light_candidate_pos.setY(std::max(
                    boundaries_rect.top() + RC_COLLISION_OFFSET,
                    std::min(
                        light_candidate_pos.y(), boundaries_rect.bottom() - RC_COLLISION_OFFSET)));
                for (size_t poly_idx = 1; poly_idx < m_polygons.size(); ++poly_idx) {
                    const auto& obstacle_poly = m_polygons[poly_idx];
                    if (obstacle_poly.isPointStrictlyInside(light_candidate_pos)) {
                        any_collision_in_group = true;
                        QPointF closest_pt =
                            findClosestPointOnPolygonBoundary(light_candidate_pos, obstacle_poly);
                        QPointF push_vec = closest_pt - light_candidate_pos;
                        current_master_candidate +=
                            RC_normalized(push_vec) * (RC_COLLISION_OFFSET * 0.5);
                        goto next_group_attempt;
                    }
                }
            }
            if (!any_collision_in_group) {
                group_position_valid = true;
                break;
            }
        next_group_attempt:;
        }
        if (group_position_valid) {
            m_master_light_pos = current_master_candidate;
            m_light_sources = current_all_dynamic_lights_candidates;
        } else {
            m_master_light_pos = old_master_pos;
            m_light_sources = old_satellite_pos;
        }
        EnsureBoundaryPolygonExists_NoLock(boundaries_rect);
    }

    void addStaticLightSource(const QPointF& master_pos, const QRectF& boundaries_rect) {
        std::lock_guard<std::mutex> lock_polygons(m_geometry_mutex);
        std::lock_guard<std::mutex> lock_static(m_static_lights_mutex);

        StaticLightSourceInfo new_sl;
        QPointF current_static_master_candidate = master_pos;

        const int static_sat_count = 6;
        const double static_sat_spread = 15.0;

        bool final_placement_ok = false;
        for (int group_attempt = 0; group_attempt < RC_MAX_GROUP_CORRECTION_ATTEMPTS;
             ++group_attempt) {
            current_static_master_candidate.setX(std::max(
                boundaries_rect.left() + RC_COLLISION_OFFSET,
                std::min(
                    current_static_master_candidate.x(),
                    boundaries_rect.right() - RC_COLLISION_OFFSET)));
            current_static_master_candidate.setY(std::max(
                boundaries_rect.top() + RC_COLLISION_OFFSET,
                std::min(
                    current_static_master_candidate.y(),
                    boundaries_rect.bottom() - RC_COLLISION_OFFSET)));

            new_sl.masterPosition = current_static_master_candidate;
            generateSatellitesForStaticSource(
                new_sl.masterPosition, new_sl.satellitePositions, static_sat_count,
                static_sat_spread);

            bool collision_this_attempt = false;

        any_collision_this_attempt_static_label:
            collision_this_attempt = false;

            new_sl.masterPosition.setX(std::max(
                boundaries_rect.left() + RC_COLLISION_OFFSET,
                std::min(
                    new_sl.masterPosition.x(), boundaries_rect.right() - RC_COLLISION_OFFSET)));
            new_sl.masterPosition.setY(std::max(
                boundaries_rect.top() + RC_COLLISION_OFFSET,
                std::min(
                    new_sl.masterPosition.y(), boundaries_rect.bottom() - RC_COLLISION_OFFSET)));
            for (size_t poly_idx = 1; poly_idx < m_polygons.size(); ++poly_idx) {
                if (m_polygons[poly_idx].isPointStrictlyInside(new_sl.masterPosition)) {
                    collision_this_attempt = true;
                    QPointF closest_pt = findClosestPointOnPolygonBoundary(
                        new_sl.masterPosition, m_polygons[poly_idx]);
                    QPointF push_vec = closest_pt - new_sl.masterPosition;
                    current_static_master_candidate =
                        closest_pt + RC_normalized(push_vec) * RC_COLLISION_OFFSET;
                    new_sl.masterPosition = current_static_master_candidate;
                    generateSatellitesForStaticSource(
                        new_sl.masterPosition, new_sl.satellitePositions, static_sat_count,
                        static_sat_spread);
                    goto any_collision_this_attempt_static_label;
                }
            }

            for (QPointF& sat_pos : new_sl.satellitePositions) {
                sat_pos.setX(std::max(
                    boundaries_rect.left() + RC_COLLISION_OFFSET,
                    std::min(sat_pos.x(), boundaries_rect.right() - RC_COLLISION_OFFSET)));
                sat_pos.setY(std::max(
                    boundaries_rect.top() + RC_COLLISION_OFFSET,
                    std::min(sat_pos.y(), boundaries_rect.bottom() - RC_COLLISION_OFFSET)));
                for (size_t poly_idx = 1; poly_idx < m_polygons.size(); ++poly_idx) {
                    if (m_polygons[poly_idx].isPointStrictlyInside(sat_pos)) {
                        collision_this_attempt = true;
                        QPointF closest_pt_to_sat =
                            findClosestPointOnPolygonBoundary(sat_pos, m_polygons[poly_idx]);
                        QPointF push_vec = closest_pt_to_sat - sat_pos;
                        current_static_master_candidate +=
                            RC_normalized(push_vec) * (RC_COLLISION_OFFSET * 0.5);
                        new_sl.masterPosition = current_static_master_candidate;
                        generateSatellitesForStaticSource(
                            new_sl.masterPosition, new_sl.satellitePositions, static_sat_count,
                            static_sat_spread);
                        goto any_collision_this_attempt_static_label;
                    }
                }
            }
            if (!collision_this_attempt) {
                final_placement_ok = true;
                break;
            }
        }

        if (final_placement_ok) {
            m_static_light_sources_info.push_back(new_sl);
        }
    }

    void deleteStaticLightSourceById(int id_to_delete) {
        std::lock_guard<std::mutex> lock(m_static_lights_mutex);
        m_static_light_sources_info.erase(
            std::remove_if(
                m_static_light_sources_info.begin(), m_static_light_sources_info.end(),
                [id_to_delete](const StaticLightSourceInfo& sl) { return sl.id == id_to_delete; }),
            m_static_light_sources_info.end());
    }

    void teleportMasterLightSource(const QPointF& pos, const QRectF& boundaries) {
        setMasterLightSourcePosition(pos, boundaries);
    }

    void PrepareCachedCastPoints(const QRectF& boundaries_rect) {
        std::vector<QPointF> all_cast_points;
        all_cast_points.reserve(512);
        all_cast_points.push_back(boundaries_rect.topLeft());
        all_cast_points.push_back(boundaries_rect.topRight());
        all_cast_points.push_back(boundaries_rect.bottomRight());
        all_cast_points.push_back(boundaries_rect.bottomLeft());
        for (const auto& polygon : m_polygons) {
            for (const auto& vertex : polygon.getVertices()) {
                all_cast_points.push_back(vertex);
            }
        }
        std::vector<std::pair<QPointF, QPointF>> all_edges;
        all_edges.reserve(m_polygons.size() * 5);
        for (const auto& polygon : m_polygons) {
            const auto& vertices = polygon.getVertices();
            if (vertices.size() >= 2) {
                for (size_t i = 0; i < vertices.size(); ++i) {
                    all_edges.emplace_back(vertices[i], vertices[(i + 1) % vertices.size()]);
                }
            }
        }
        for (size_t i = 0; i < all_edges.size(); ++i) {
            for (size_t j = i + 1; j < all_edges.size(); ++j) {
                std::optional<QPointF> intersection = RC_IntersectLineSegments(
                    all_edges[i].first, all_edges[i].second, all_edges[j].first,
                    all_edges[j].second);
                if (intersection) {
                    all_cast_points.push_back(*intersection);
                }
            }
        }
        if (!all_cast_points.empty()) {
            std::sort(all_cast_points.begin(), all_cast_points.end(), RC_QPointFLess());
            all_cast_points.erase(
                std::unique(
                    all_cast_points.begin(), all_cast_points.end(), RC_QPointFCloseEnough()),
                all_cast_points.end());
        }
        m_cached_cast_points.clear();
        m_cached_cast_points.reserve(all_cast_points.size());
        if (m_polygons.size() > 1) {
            for (const auto& cast_point : all_cast_points) {
                bool is_inside_any_obstacle = false;
                for (size_t poly_idx = 1; poly_idx < m_polygons.size(); ++poly_idx) {
                    if (m_polygons[poly_idx].isPointStrictlyInside(cast_point)) {
                        is_inside_any_obstacle = true;
                        break;
                    }
                }
                if (!is_inside_any_obstacle) {
                    m_cached_cast_points.push_back(cast_point);
                }
            }
        } else {
            m_cached_cast_points = all_cast_points;
        }
        m_geometry_dirty = false;
    }

    std::vector<Ray> CastRaysForSource(
        const QPointF& current_light_source, const std::vector<QPointF>& cast_points_cache) const {
        std::vector<Ray> rays;
        rays.reserve(cast_points_cache.size() * 3);
        for (const auto& point : cast_points_cache) {
            QPointF dir_to_point = point - current_light_source;
            double dist_to_point_sq = QPointF::dotProduct(dir_to_point, dir_to_point);
            if (dist_to_point_sq < 1e-8) {
                continue;
            }
            double original_angle = std::atan2(dir_to_point.y(), dir_to_point.x());
            QPointF norm_dir = RC_normalized(dir_to_point);
            rays.emplace_back(
                current_light_source, current_light_source + norm_dir * RC_FAR_POINT_MULTIPLIER,
                original_angle);
            double angle_plus = original_angle + RC_EPSILON_ANGLE;
            QPointF dir_plus(std::cos(angle_plus), std::sin(angle_plus));
            rays.emplace_back(
                current_light_source, current_light_source + dir_plus * RC_FAR_POINT_MULTIPLIER,
                angle_plus);
            double angle_minus = original_angle - RC_EPSILON_ANGLE;
            QPointF dir_minus(std::cos(angle_minus), std::sin(angle_minus));
            rays.emplace_back(
                current_light_source, current_light_source + dir_minus * RC_FAR_POINT_MULTIPLIER,
                angle_minus);
        }
        return rays;
    }

    void IntersectRaysForSource(
        std::vector<Ray>* rays, const std::vector<Polygon>& polygons_copy) const {
        if (!rays || polygons_copy.empty()) {
            return;
        }
        for (Ray& ray : *rays) {
            std::optional<QPointF> closest_overall_intersection;
            double min_dist_sq_overall =
                QPointF::dotProduct(ray.getEnd() - ray.getBegin(), ray.getEnd() - ray.getBegin());
            for (const auto& polygon : polygons_copy) {
                std::optional<QPointF> current_poly_intersection = polygon.IntersectRay(ray);
                if (current_poly_intersection) {
                    QPointF vec_to_intersect = *current_poly_intersection - ray.getBegin();
                    double dist_sq_current =
                        QPointF::dotProduct(vec_to_intersect, vec_to_intersect);
                    if (dist_sq_current < min_dist_sq_overall) {
                        QPointF ray_dir_norm = ray.getEnd() - ray.getBegin();
                        if (QPointF::dotProduct(vec_to_intersect, ray_dir_norm) >= -1e-9) {
                            min_dist_sq_overall = dist_sq_current;
                            closest_overall_intersection = *current_poly_intersection;
                        }
                    }
                }
            }
            if (closest_overall_intersection) {
                ray.setEnd(*closest_overall_intersection);
            }
        }
    }

    void RemoveAdjacentRays(std::vector<Ray>* rays_ptr) const {
        if (!rays_ptr || rays_ptr->empty()) {
            return;
        }
        std::vector<Ray>& rays = *rays_ptr;
        if (rays.size() < 2) {
            return;
        }
        std::sort(rays.begin(), rays.end(), [](const Ray& a, const Ray& b) {
            return a.getAngle() < b.getAngle();
        });
        std::vector<Ray> filtered_rays;
        if (!rays.empty()) {
            filtered_rays.push_back(rays.front());
            for (size_t i = 1; i < rays.size(); ++i) {
                const QPointF& end1 = filtered_rays.back().getEnd();
                const QPointF& end2 = rays[i].getEnd();
                QPointF diff = end1 - end2;
                if (QPointF::dotProduct(diff, diff) > RC_VERY_SMALL_DISTANCE_SQUARED) {
                    filtered_rays.push_back(rays[i]);
                } else {
                    QPointF prev_vec =
                        filtered_rays.back().getEnd() - filtered_rays.back().getBegin();
                    QPointF curr_vec = rays[i].getEnd() - rays[i].getBegin();
                    if (QPointF::dotProduct(curr_vec, curr_vec) >
                        QPointF::dotProduct(prev_vec, prev_vec)) {
                        filtered_rays.back() = rays[i];
                    }
                }
            }
        }
        *rays_ptr = std::move(filtered_rays);
    }

    Polygon CalculateSingleLightArea(
        const QPointF& light_source_pos, const std::vector<QPointF>& cast_points_cache_copy,
        const std::vector<Polygon>& polygons_copy, const QRectF& boundaries_rect_for_check) const {
        if (light_source_pos.isNull()) {
            return Polygon(std::vector<QPointF>{});
        }
        if (!boundaries_rect_for_check.contains(light_source_pos)) {
            return Polygon(std::vector<QPointF>{});
        }
        for (size_t poly_idx = 1; poly_idx < polygons_copy.size(); ++poly_idx) {
            if (polygons_copy[poly_idx].isPointStrictlyInside(light_source_pos)) {
                return Polygon(std::vector<QPointF>{});
            }
        }
        std::vector<Ray> rays_vec = CastRaysForSource(light_source_pos, cast_points_cache_copy);
        IntersectRaysForSource(&rays_vec, polygons_copy);
        std::sort(rays_vec.begin(), rays_vec.end(), [](const Ray& a, const Ray& b) {
            return a.getAngle() < b.getAngle();
        });
        RemoveAdjacentRays(&rays_vec);
        std::vector<QPointF> light_vertices;
        light_vertices.reserve(rays_vec.size());
        for (const auto& ray : rays_vec) {
            light_vertices.push_back(ray.getEnd());
        }
        if (light_vertices.size() < 3) {
            return Polygon(std::vector<QPointF>{});
        }
        return Polygon(light_vertices);
    }

    std::vector<Polygon> CreateAllLightAreas(const QRectF& boundaries_rect) {
        std::vector<Polygon> all_light_areas_result;
        std::vector<Polygon> polygons_copy_for_threads;
        std::vector<QPointF> cast_points_cache_copy_for_threads;
        std::vector<QPointF> all_active_light_points_for_threads;

        {
            std::lock_guard<std::mutex> lock(m_geometry_mutex);
            if (m_light_sources.size() != (size_t)m_num_light_sources && m_num_light_sources >= 0) {
                generateGenericSatellitePositions(
                    m_master_light_pos, m_light_sources, m_num_light_sources,
                    m_light_source_spread);
            }
            for (const auto& dyn_ls : m_light_sources) {
                if (!dyn_ls.isNull()) {
                    all_active_light_points_for_threads.push_back(dyn_ls);
                }
            }
        }
        {
            std::lock_guard<std::mutex> lock(m_static_lights_mutex);
            for (const auto& static_info : m_static_light_sources_info) {
                if (!static_info.masterPosition.isNull()) {
                    all_active_light_points_for_threads.push_back(static_info.masterPosition);
                }
                for (const auto& sat_pos : static_info.satellitePositions) {
                    if (!sat_pos.isNull()) {
                        all_active_light_points_for_threads.push_back(sat_pos);
                    }
                }
            }
        }

        if (all_active_light_points_for_threads.empty()) {
            return all_light_areas_result;
        }
        all_light_areas_result.resize(all_active_light_points_for_threads.size());

        {
            std::lock_guard<std::mutex> lock(m_geometry_mutex);
            EnsureBoundaryPolygonExists_NoLock(boundaries_rect);
            if (m_geometry_dirty || m_cached_cast_points.empty()) {
                PrepareCachedCastPoints(boundaries_rect);
            }
            polygons_copy_for_threads = m_polygons;
            m_polygons_const_ref = m_polygons;
            cast_points_cache_copy_for_threads = m_cached_cast_points;
        }

        std::vector<std::thread> threads;
        threads.reserve(m_num_worker_threads);
        size_t num_total_sources_to_calc = all_active_light_points_for_threads.size();
        size_t sources_per_thread =
            (num_total_sources_to_calc + m_num_worker_threads - 1) / m_num_worker_threads;
        if (sources_per_thread == 0 && num_total_sources_to_calc > 0) {
            sources_per_thread = 1;
        }

        for (unsigned int i = 0; i < m_num_worker_threads; ++i) {
            size_t start_idx = i * sources_per_thread;
            size_t end_idx = std::min((i + 1) * sources_per_thread, num_total_sources_to_calc);
            if (start_idx >= end_idx) {
                continue;
            }
            threads.emplace_back([this, start_idx, end_idx, &all_light_areas_result,
                                  polygons_copy = polygons_copy_for_threads,
                                  cast_points_cache_copy = cast_points_cache_copy_for_threads,
                                  light_points_copy = all_active_light_points_for_threads,
                                  boundaries = boundaries_rect]() {
                for (size_t k = start_idx; k < end_idx; ++k) {
                    if (k < all_light_areas_result.size()) {
                        all_light_areas_result[k] = CalculateSingleLightArea(
                            light_points_copy[k], cast_points_cache_copy, polygons_copy,
                            boundaries);
                    }
                }
            });
        }
        for (auto& t : threads) {
            if (t.joinable()) {
                t.join();
            }
        }
        return all_light_areas_result;
    }

    void EnsureBoundaryPolygonExists_NoLock(const QRectF& rect) {
        std::vector<QPointF> boundary_vertices = {
          rect.topLeft(), rect.topRight(), rect.bottomRight(), rect.bottomLeft()};
        if (m_polygons.empty()) {
            m_polygons.emplace_back(boundary_vertices);
        } else {
            if (m_polygons[0].getVertices().size() != 4 ||
                m_polygons[0].getVertices()[0] != rect.topLeft()) {
                m_polygons.insert(m_polygons.begin(), Polygon(boundary_vertices));
            } else {
                m_polygons[0].getVerticesMutable() = boundary_vertices;
            }
        }
        m_geometry_dirty = true;
    }

    void ClearObstacles() {
        std::lock_guard<std::mutex> lock(m_geometry_mutex);
        if (!m_polygons.empty()) {
            Polygon boundary = m_polygons[0];
            m_polygons.clear();
            m_polygons.push_back(boundary);
        }
        m_geometry_dirty = true;
    }

   private:
    std::vector<Polygon> m_polygons;
    std::vector<Polygon> m_polygons_const_ref;
    QPointF m_master_light_pos;
    std::vector<QPointF> m_light_sources;
    std::vector<StaticLightSourceInfo> m_static_light_sources_info;
    mutable std::mutex m_static_lights_mutex;
    std::vector<QPointF> m_cached_cast_points;
    bool m_geometry_dirty = true;
    mutable std::mutex m_geometry_mutex;
    unsigned int m_num_worker_threads = 1;
    std::vector<QPointF> m_current_polygon_vertices;
    QPointF m_mouse_pos;
    int m_num_light_sources;
    double m_light_source_spread;
};

int Controller::StaticLightSourceInfo::next_id_counter = 0;

// =============================================================================
// GUI Logic
// =============================================================================
class FpsCounter {
   public:
    FpsCounter()
        : m_frameCount(0), m_currentFps(0.0), m_elapsedAccumulator(0), m_lastFrameTimestamp(0) {
        m_timer.start();
    }

    void reportFrameRendered() {
        m_frameCount++;
        qint64 currentTime = m_timer.elapsed();
        if (m_lastFrameTimestamp == 0) {
            m_lastFrameTimestamp = currentTime;
            return;
        }
        qint64 frameDelta = currentTime - m_lastFrameTimestamp;
        m_lastFrameTimestamp = currentTime;
        if (frameDelta > 0) {
            m_frameTimeHistory.push_back(frameDelta);
            m_totalFrameTimeHistory += frameDelta;
            while (m_totalFrameTimeHistory > 1000 && m_frameTimeHistory.size() > 1) {
                m_totalFrameTimeHistory -= m_frameTimeHistory.front();
                m_frameTimeHistory.erase(m_frameTimeHistory.begin());
            }
            if (!m_frameTimeHistory.empty() && m_totalFrameTimeHistory > 0) {
                m_currentFps = static_cast<double>(m_frameTimeHistory.size() * 1000.0) /
                               m_totalFrameTimeHistory;
            } else if (frameDelta > 0) {
                m_currentFps = 1000.0 / frameDelta;
            }
        }
    }

    double getFps() const {
        return m_currentFps;
    }

   private:
    QElapsedTimer m_timer;
    int m_frameCount;
    double m_currentFps;
    qint64 m_elapsedAccumulator;
    qint64 m_lastFrameTimestamp;
    std::vector<qint64> m_frameTimeHistory;
    qint64 m_totalFrameTimeHistory = 0;
};

class Command {
   public:
    virtual ~Command() = default;
    virtual void execute() = 0;
    virtual void undo() = 0;
};

class NotificationWidget : public QLabel {
   public:
    explicit NotificationWidget(QWidget* parent = nullptr);
    void showTemporary(
        const QString& message, int durationMs = NOTIFICATION_DURATION,
        int fadeDelayMs = NOTIFICATION_FADE_DELAY);

   protected:
    void enterEvent(QEnterEvent* event) override;
    void leaveEvent(QEvent* event) override;
    void timerEvent(QTimerEvent* event) override;
    void paintEvent(QPaintEvent* event) override;

   private:
    enum class State { Hidden, Showing, Fading } currentState = State::Hidden;
    int displayTimerId = 0;
    int fadeTimerId = 0;
    int fadeDuration = NOTIFICATION_FADE_DURATION;
    int displayDuration = NOTIFICATION_FADE_DELAY;
    QElapsedTimer fadeElapsedTimer;
    qreal currentOpacity = 1.0;
    void startDisplayTimer();
    void startFadeTimer();
    void stopTimers();
    void positionWidget();

    static qreal easeOutQuad(qreal t) {
        return -t * (t - 2.0);
    }
};

NotificationWidget::NotificationWidget(QWidget* parent) : QLabel(parent) {
    setStyleSheet(
        R"( QLabel { background-color: rgba(30, 30, 30, 245); color: #e0e0e0; padding: 10px 18px; border-radius: 6px; font-size: 10pt; } )");
    setAttribute(Qt::WA_TranslucentBackground);
    setWindowFlags(
        Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint | Qt::WindowDoesNotAcceptFocus);
    hide();
}

void NotificationWidget::positionWidget() {
    if (!parentWidget()) {
        return;
    }
    QWidget* mw = parentWidget();
    while (mw->parentWidget() && mw->parentWidget()->isWidgetType()) {
        mw = mw->parentWidget();
    }
    QPoint pbr = mw->mapToGlobal(mw->rect().bottomRight());
    int m = STANDARD_MARGIN + 5;
    move(pbr.x() - width() - m, pbr.y() - height() - m);
}

void NotificationWidget::paintEvent(QPaintEvent* event) {
    Q_UNUSED(event);
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setOpacity(currentOpacity);
    painter.setPen(Qt::NoPen);
    painter.setBrush(QColor(30, 30, 30, int(245 * currentOpacity)));
    painter.drawRoundedRect(rect(), 6, 6);
    QString currentText = text();
    if (!currentText.isEmpty()) {
        QRect textRect = contentsRect();
        int flags = alignment() | Qt::TextWordWrap;
        painter.setFont(font());
        painter.setPen(QColor(0, 0, 0, int(255 * currentOpacity)));
        const int outlineOffset = 1;
        for (int dx = -outlineOffset; dx <= outlineOffset; ++dx) {
            for (int dy = -outlineOffset; dy <= outlineOffset; ++dy) {
                if (dx == 0 && dy == 0) {
                    continue;
                }
                if (std::abs(dx) + std::abs(dy) <= outlineOffset + 1) {
                    painter.drawText(textRect.translated(dx, dy), flags, currentText);
                }
            }
        }
        painter.setPen(QColor(224, 224, 224, int(255 * currentOpacity)));
        painter.drawText(textRect, flags, currentText);
    }
}

void NotificationWidget::showTemporary(const QString& message, int durationMs, int fadeDelayMs) {
    stopTimers();
    fadeDuration = qMax(100, durationMs - fadeDelayMs);
    displayDuration = qMax(100, fadeDelayMs);
    setText(message);
    adjustSize();
    positionWidget();
    currentOpacity = 1.0;
    setWindowOpacity(currentOpacity);
    currentState = State::Showing;
    show();
    raise();
    if (!rect().contains(mapFromGlobal(QCursor::pos()))) {
        startDisplayTimer();
    }
}

void NotificationWidget::startDisplayTimer() {
    if (displayTimerId == 0) {
        displayTimerId = startTimer(displayDuration);
    }
}

void NotificationWidget::startFadeTimer() {
    if (fadeTimerId == 0) {
        fadeElapsedTimer.start();
        fadeTimerId = startTimer(16);
    }
}

void NotificationWidget::stopTimers() {
    if (displayTimerId != 0) {
        killTimer(displayTimerId);
        displayTimerId = 0;
    }
    if (fadeTimerId != 0) {
        killTimer(fadeTimerId);
        fadeTimerId = 0;
    }
}

void NotificationWidget::enterEvent(QEnterEvent* event) {
    if (currentState != State::Hidden) {
        stopTimers();
        if (currentState == State::Fading) {
            currentOpacity = 1.0;
            setWindowOpacity(currentOpacity);
            currentState = State::Showing;
        }
    }
    QLabel::enterEvent(event);
}

void NotificationWidget::leaveEvent(QEvent* event) {
    if (currentState == State::Showing) {
        startDisplayTimer();
    }
    QLabel::leaveEvent(event);
}

void NotificationWidget::timerEvent(QTimerEvent* event) {
    if (event->timerId() == displayTimerId) {
        killTimer(displayTimerId);
        displayTimerId = 0;
        if (currentState == State::Showing) {
            currentState = State::Fading;
            startFadeTimer();
        }
    } else if (event->timerId() == fadeTimerId) {
        if (currentState == State::Fading) {
            qint64 el = fadeElapsedTimer.elapsed();
            qreal t = qBound(0.0, (qreal)el / fadeDuration, 1.0);
            currentOpacity = 1.0 - easeOutQuad(t);
            setWindowOpacity(currentOpacity);
            if (t >= 1.0) {
                stopTimers();
                currentState = State::Hidden;
                hide();
            }
        } else {
            killTimer(fadeTimerId);
            fadeTimerId = 0;
        }
    } else {
        QLabel::timerEvent(event);
    }
}

class CanvasWidget : public QWidget {
   public:
    explicit CanvasWidget(
        std::function<void()> commitCallback, FpsCounter* fpsCounterRef, Controller* controllerRef,
        QPushButton* polyDelBtnRef, QPushButton* staticDelBtnRef, NotificationWidget* notifier,
        QWidget* parent = nullptr);

    Controller* getController() {
        return m_rc_controller;
    }

    void setCurrentOperatingMode(int mode) {
        m_current_mode = mode;
        update();
    }

    int getHoveredPolygonIndex() const {
        return m_hovered_polygon_index;
    }

    void resetHoveredPolygonIndex() {
        if (m_hovered_polygon_index != -1) {
            m_hovered_polygon_index = -1;
            update();
        }
    }

    int getHoveredStaticLightId() const {
        return m_hovered_static_light_id;
    }

    void resetHoveredStaticLightId() {
        if (m_hovered_static_light_id != -1) {
            m_hovered_static_light_id = -1;
            update();
        }
    }

   protected:
    void paintEvent(QPaintEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;
    void enterEvent(QEnterEvent* event) override;
    void leaveEvent(QEvent* event) override;

   private:
    std::function<void()> commitSettingsCallback;
    FpsCounter* fpsCounter;
    Controller* m_rc_controller;
    int m_current_mode = 0;
    bool m_is_dragging_light_source = false;
    int m_hovered_polygon_index = -1;
    int m_hovered_static_light_id = -1;
    QPushButton* m_poly_delete_button_ref;
    QPushButton* m_static_delete_button_ref;
    NotificationWidget* m_notifier_ref;
};

CanvasWidget::CanvasWidget(
    std::function<void()> commitCallback, FpsCounter* fpsCounterRef, Controller* controllerRef,
    QPushButton* polyDelBtnRef, QPushButton* staticDelBtnRef, NotificationWidget* notifier,
    QWidget* parent)
    : QWidget(parent)
    , commitSettingsCallback(commitCallback)
    , fpsCounter(fpsCounterRef)
    , m_rc_controller(controllerRef)
    , m_poly_delete_button_ref(polyDelBtnRef)
    , m_static_delete_button_ref(staticDelBtnRef)
    , m_notifier_ref(notifier) {
    setFocusPolicy(Qt::StrongFocus);
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    setMouseTracking(true);
}

void CanvasWidget::paintEvent(QPaintEvent* event) {
    if (fpsCounter) {
        fpsCounter->reportFrameRendered();
    }
    Q_UNUSED(event);
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.fillRect(rect(), QColor(20, 20, 25));
    if (m_rc_controller) {
        const auto& polygons = m_rc_controller->GetPolygons();
        for (size_t i = 0; i < polygons.size(); ++i) {
            const auto& vertices = polygons[i].getVertices();
            if (vertices.empty()) {
                continue;
            }
            QPolygonF qpoly;
            for (const auto& v : vertices) {
                qpoly << v;
            }
            if (i == 0) {
                painter.setPen(QPen(Qt::darkGray, 1, Qt::DotLine));
                painter.setBrush(Qt::NoBrush);
                if (vertices.size() >= 2) {
                    painter.drawPolygon(qpoly);
                }
            } else {
                bool is_poly_delete_mode =
                    m_current_mode == 1 && m_poly_delete_button_ref->isChecked();
                bool is_hovered_poly = (int)i == m_hovered_polygon_index;
                if (is_poly_delete_mode && is_hovered_poly) {
                    painter.setPen(QPen(QColor(180, 0, 0), 2));
                    painter.setBrush(QColor(80, 60, 60, 100));
                } else {
                    painter.setPen(Qt::black);
                    painter.setBrush(QColor(50, 50, 60));
                }
                if (vertices.size() >= 3) {
                    painter.drawPolygon(qpoly);
                }
            }
        }
        const auto& current_poly_vertices = m_rc_controller->getCurrentPolygonVertices();
        if (m_current_mode == 1 && m_poly_delete_button_ref &&
            !m_poly_delete_button_ref->isChecked() && !current_poly_vertices.empty()) {
            painter.setPen(QPen(Qt::cyan, 1.5));
            painter.setBrush(Qt::NoBrush);
            for (size_t k = 0; k < current_poly_vertices.size(); ++k) {
                painter.drawEllipse(current_poly_vertices[k], 3, 3);
                if (k > 0) {
                    painter.drawLine(current_poly_vertices[k - 1], current_poly_vertices[k]);
                }
            }
            painter.drawLine(current_poly_vertices.back(), m_rc_controller->getMouseHintPosition());
            painter.setPen(QPen(Qt::magenta, 1, Qt::DashLine));
            painter.drawEllipse(m_rc_controller->getMouseHintPosition(), 2, 2);
        }

        std::vector<Polygon> light_areas = m_rc_controller->CreateAllLightAreas(rect());
        const auto& dynamic_light_sources = m_rc_controller->getLightSources();
        const auto& static_light_sources_info = m_rc_controller->getStaticLightSourcesInfo();

        int current_light_area_idx = 0;

        int num_dyn_sources = m_rc_controller->getNumLightSources();
        if (num_dyn_sources <= 0) {
            num_dyn_sources = 1;
        }
        int alpha_per_dyn_source;
        double target_combined_opacity_ratio_dyn = 180.0 / 255.0;
        if (num_dyn_sources == 1) {
            alpha_per_dyn_source = 120;
        } else {
            double alpha_norm = 1.0 - std::pow(
                                          1.0 - target_combined_opacity_ratio_dyn,
                                          1.0 / static_cast<double>(num_dyn_sources));
            alpha_per_dyn_source = static_cast<int>(alpha_norm * 255.0);
            alpha_per_dyn_source = std::max(8, std::min(alpha_per_dyn_source, 130));
        }

        for (size_t i = 0;
             i < dynamic_light_sources.size() && current_light_area_idx < (int)light_areas.size();
             ++i) {
            if (dynamic_light_sources[i].isNull()) {
                current_light_area_idx++;
                continue;
            }
            const auto& light_area = light_areas[current_light_area_idx++];
            const auto& light_vertices = light_area.getVertices();
            if (light_vertices.size() >= 3) {
                QPolygonF light_qpoly;
                for (const auto& v : light_vertices) {
                    light_qpoly << v;
                }
                QPainterPath path;
                if (!light_qpoly.isEmpty()) {
                    path.moveTo(light_qpoly.first());
                    for (int k = 1; k < light_qpoly.size(); ++k) {
                        path.lineTo(light_qpoly.at(k));
                    }
                    path.closeSubpath();
                }
                painter.setPen(Qt::NoPen);
                painter.setBrush(QColor(255, 255, 220, alpha_per_dyn_source));
                painter.drawPath(path);
            }
        }

        const int static_light_alpha = 35;
        for (const auto& sl_info : static_light_sources_info) {
            if (current_light_area_idx < (int)light_areas.size() &&
                !sl_info.masterPosition.isNull()) {
                const auto& light_area = light_areas[current_light_area_idx++];
                const auto& light_vertices = light_area.getVertices();
                if (light_vertices.size() >= 3) {
                    QPolygonF qpoly;
                    for (const auto& v : light_vertices) {
                        qpoly << v;
                    }
                    QPainterPath path;
                    if (!qpoly.isEmpty()) {
                        path.moveTo(qpoly.first());
                        for (int k = 1; k < qpoly.size(); ++k) {
                            path.lineTo(qpoly.at(k));
                        }
                        path.closeSubpath();
                    }
                    painter.setPen(Qt::NoPen);
                    painter.setBrush(QColor(255, 192, 203, static_light_alpha));
                    painter.drawPath(path);
                }
            }
            for (size_t sat_idx = 0; sat_idx < sl_info.satellitePositions.size() &&
                                     current_light_area_idx < (int)light_areas.size();
                 ++sat_idx) {
                if (sl_info.satellitePositions[sat_idx].isNull()) {
                    current_light_area_idx++;
                    continue;
                }
                const auto& light_area = light_areas[current_light_area_idx++];
                const auto& light_vertices = light_area.getVertices();
                if (light_vertices.size() >= 3) {
                    QPolygonF qpoly;
                    for (const auto& v : light_vertices) {
                        qpoly << v;
                    }
                    QPainterPath path;
                    if (!qpoly.isEmpty()) {
                        path.moveTo(qpoly.first());
                        for (int k = 1; k < qpoly.size(); ++k) {
                            path.lineTo(qpoly.at(k));
                        }
                        path.closeSubpath();
                    }
                    painter.setPen(Qt::NoPen);
                    painter.setBrush(QColor(255, 192, 203, static_light_alpha));
                    painter.drawPath(path);
                }
            }
        }

        painter.setPen(Qt::yellow);
        painter.setBrush(Qt::yellow);
        for (const auto& ls_pos : m_rc_controller->getLightSources()) {
            if (!ls_pos.isNull()) {
                painter.drawEllipse(ls_pos, 2, 2);
            }
        }
        QPointF masterPos = m_rc_controller->getMasterLightPosition();
        if (!masterPos.isNull()) {
            painter.setPen(Qt::red);
            painter.setBrush(Qt::red);
            painter.drawEllipse(masterPos, 4, 4);
        }

        bool is_static_delete_mode = m_current_mode == 2 && m_static_delete_button_ref->isChecked();
        for (const auto& sl_info : static_light_sources_info) {
            if (!sl_info.masterPosition.isNull()) {
                bool is_hovered = sl_info.id == m_hovered_static_light_id;
                if (is_static_delete_mode && is_hovered) {
                    painter.setPen(QPen(QColor(200, 0, 200), 2));
                    painter.setBrush(QColor(100, 0, 100, 100));
                } else {
                    painter.setPen(QColor(255, 105, 180));
                    painter.setBrush(QColor(255, 105, 180));
                }
                painter.drawEllipse(sl_info.masterPosition, 3, 3);
            }
            painter.setPen(QColor(255, 150, 210));
            painter.setBrush(QColor(255, 150, 210));
            for (const auto& sat_pos : sl_info.satellitePositions) {
                if (!sat_pos.isNull()) {
                    painter.drawEllipse(sat_pos, 1.5, 1.5);
                }
            }
        }

    } else {
        painter.setPen(Qt::gray);
        painter.setFont(QFont("Arial", 12));
        painter.drawText(rect(), Qt::AlignCenter, "Raycaster Controller not available.");
    }
}

void CanvasWidget::mousePressEvent(QMouseEvent* event) {
    if (commitSettingsCallback) {
        commitSettingsCallback();
    }
    if (!m_rc_controller || !m_poly_delete_button_ref || !m_static_delete_button_ref ||
        !m_notifier_ref) {
        return;
    }
    QPointF pos = event->position();
    bool needs_update = false;
    if (m_current_mode == 0) {
        if (event->button() == Qt::LeftButton) {
            m_is_dragging_light_source = true;
            m_rc_controller->setMasterLightSourcePosition(pos, rect());
            needs_update = true;
        } else if (event->button() == Qt::RightButton) {
            m_rc_controller->teleportMasterLightSource(pos, rect());
            needs_update = true;
        }
    } else if (m_current_mode == 1) {
        bool is_delete_mode = m_poly_delete_button_ref->isChecked();
        if (event->button() == Qt::LeftButton) {
            if (is_delete_mode) {
                if (m_hovered_polygon_index != -1) {
                    m_rc_controller->deletePolygonByIndex(m_hovered_polygon_index);
                    m_hovered_polygon_index = -1;
                    needs_update = true;
                }
            } else {
                m_rc_controller->addVertexToCurrentPolygon(pos);
                needs_update = true;
            }
        } else if (event->button() == Qt::RightButton) {
            if (!is_delete_mode) {
                if (!m_rc_controller->finishCurrentPolygon()) {
                    m_notifier_ref->showTemporary(
                        "Polygon creation cancelled:\nintersects light source.");
                }
                needs_update = true;
            }
        }
    } else if (m_current_mode == 2) {
        bool is_delete_mode_static = m_static_delete_button_ref->isChecked();
        if (event->button() == Qt::LeftButton) {
            if (is_delete_mode_static) {
                if (m_hovered_static_light_id != -1) {
                    m_rc_controller->deleteStaticLightSourceById(m_hovered_static_light_id);
                    resetHoveredStaticLightId();
                    needs_update = true;
                }
            } else {
                m_rc_controller->addStaticLightSource(pos, rect());
                needs_update = true;
            }
        }
    }
    if (needs_update) {
        update();
    }
    QWidget::mousePressEvent(event);
}

void CanvasWidget::mouseMoveEvent(QMouseEvent* event) {
    if (!m_rc_controller) {
        return;
    }
    QPointF pos = event->position();
    bool needs_update = false;
    m_rc_controller->updateMousePosition(pos);
    int old_hover_index = m_hovered_polygon_index;
    m_hovered_polygon_index = -1;
    const auto& polygons = m_rc_controller->GetPolygons();
    for (size_t i = 1; i < polygons.size(); ++i) {
        const auto& vertices = polygons[i].getVertices();
        if (vertices.size() >= 3) {
            QPolygonF qpoly;
            for (const auto& v : vertices) {
                qpoly << v;
            }
            if (qpoly.containsPoint(pos, Qt::OddEvenFill)) {
                m_hovered_polygon_index = static_cast<int>(i);
                break;
            }
        }
    }
    if (m_hovered_polygon_index != old_hover_index) {
        needs_update = true;
    }
    if (m_current_mode == 1 && m_poly_delete_button_ref && !m_poly_delete_button_ref->isChecked()) {
        needs_update = true;
    }
    if (m_current_mode == 0 && m_is_dragging_light_source && (event->buttons() & Qt::LeftButton)) {
        m_rc_controller->setMasterLightSourcePosition(pos, rect());
        needs_update = true;
    }
    if (m_current_mode == 2) {
        int old_hover_static_id = m_hovered_static_light_id;
        m_hovered_static_light_id = -1;
        const auto& static_sources = m_rc_controller->getStaticLightSourcesInfo();
        for (const auto& sl_info : static_sources) {
            if (!sl_info.masterPosition.isNull()) {
                QPointF diff = sl_info.masterPosition - pos;
                if (QPointF::dotProduct(diff, diff) < 5 * 5) {
                    m_hovered_static_light_id = sl_info.id;
                    break;
                }
            }
        }
        if (m_hovered_static_light_id != old_hover_static_id) {
            needs_update = true;
        }
    } else {
        if (m_hovered_static_light_id != -1) {
            m_hovered_static_light_id = -1;
            needs_update = true;
        }
    }
    if (needs_update) {
        update();
    }
    QWidget::mouseMoveEvent(event);
}

void CanvasWidget::mouseReleaseEvent(QMouseEvent* event) {
    if (m_current_mode == 0 && event->button() == Qt::LeftButton) {
        m_is_dragging_light_source = false;
    }
    QWidget::mouseReleaseEvent(event);
}

void CanvasWidget::enterEvent(QEnterEvent* event) {
    if (commitSettingsCallback) {
        commitSettingsCallback();
    }
    if (m_rc_controller) {
        m_rc_controller->updateMousePosition(mapFromGlobal(QCursor::pos()));
        if (m_hovered_polygon_index != -1) {
            m_hovered_polygon_index = -1;
            update();
        }
        if (m_hovered_static_light_id != -1) {
            m_hovered_static_light_id = -1;
            update();
        }
    }
    QWidget::enterEvent(event);
}

void CanvasWidget::leaveEvent(QEvent* event) {
    if (m_hovered_polygon_index != -1) {
        m_hovered_polygon_index = -1;
        update();
    }
    if (m_hovered_static_light_id != -1) {
        m_hovered_static_light_id = -1;
        update();
    }
    QWidget::leaveEvent(event);
}

// --- CommitOnFocusOut Widgets ---
class CommitOnFocusOutMixin {
   public:
    void setCommitCallback(std::function<void()> cb) {
        commitCallback = cb;
    }

   protected:
    std::function<void()> commitCallback;

    virtual void focusOutEvent(QFocusEvent* event) {
        if (commitCallback) {
            commitCallback();
        }
    }
};

class CommitOnFocusOutSpinBox : public QSpinBox, public CommitOnFocusOutMixin {
   public:
    using QSpinBox::QSpinBox;

   protected:
    void focusOutEvent(QFocusEvent* event) override {
        CommitOnFocusOutMixin::focusOutEvent(event);
        QSpinBox::focusOutEvent(event);
    }
};

class CommitOnFocusOutDoubleSpinBox : public QDoubleSpinBox, public CommitOnFocusOutMixin {
   public:
    using QDoubleSpinBox::QDoubleSpinBox;

   protected:
    void focusOutEvent(QFocusEvent* event) override {
        CommitOnFocusOutMixin::focusOutEvent(event);
        QDoubleSpinBox::focusOutEvent(event);
    }
};

class CommitOnFocusOutSlider : public QSlider, public CommitOnFocusOutMixin {
   public:
    using QSlider::QSlider;

   protected:
    void mouseReleaseEvent(QMouseEvent* event) override {
        QSlider::mouseReleaseEvent(event);
        if (commitCallback) {
            commitCallback();
        }
    }

    void focusOutEvent(QFocusEvent* event) override {
        CommitOnFocusOutMixin::focusOutEvent(event);
        QSlider::focusOutEvent(event);
    }
};

// --- MainWindow Definition ---
class MainWindow : public QMainWindow {
   public:
    MainWindow(QWidget* parent = nullptr);
    ~MainWindow() override;
    void initializeModeState();

    NotificationWidget* getNotifier() {
        return notificationWidget;
    }

    CanvasWidget* getCanvas() {
        return canvasWidget;
    }

    void commitPendingSettingsChange();

   protected:
    void keyPressEvent(QKeyEvent* event) override;
    void resizeEvent(QResizeEvent* event) override;

   private:
    friend class SettingsChangeCommand;
    void setupUI();
    void createControlPanel();
    QWidget* createModeControlContainer();
    QWidget* createLightModeControls();
    QWidget* createPolygonModeControls();
    QWidget* createStaticLightModeControls();
    QLabel* createHintsLabel();
    void applyStyles();
    void setupConnections();
    void loadDefaults();
    void applyDefaults(int modeIndex);
    QWidget* centralWidget;
    QHBoxLayout* mainLayout;
    CanvasWidget* canvasWidget;
    QWidget* controlPanelContainer;
    QHBoxLayout* controlPanelMainLayout;
    QWidget* controlPanelWidget;
    QVBoxLayout* controlPanelLayout;
    QPushButton* togglePanelButton;
    QTimer* panelAnimationTimer;
    int panelAnimationTargetWidth;
    bool isControlPanelCollapsed = false;
    QComboBox* modeComboBox;
    QStackedWidget* modeStackedWidget;
    QLabel* hintsLabel;
    QPushButton* polyDrawDeleteButton;
    CommitOnFocusOutSpinBox* lightSourceCountSpinBox;
    CommitOnFocusOutDoubleSpinBox* lightSourceDistanceSpinBox;
    CommitOnFocusOutSpinBox* staticSourceCountSpinBox;
    CommitOnFocusOutDoubleSpinBox* staticSourceDistanceSpinBox;
    CommitOnFocusOutSlider* staticLightBrightnessSlider;
    QLabel* staticLightBrightnessValueLabel;
    QPushButton* staticPlaceDeleteButton;
    NotificationWidget* notificationWidget;
    bool firstCollapseNotificationShown = false;
    std::vector<std::unique_ptr<Command>> undoStacks[3];
    ModeDefaults defaultSettings;
    QTimer* settingsCommitTimer;
    QWidget* lastEditedWidget = nullptr;
    SettingValue originalSettingValue;
    bool isEditingSettings = false;
    FpsCounter fpsCounterInstance;
    QTimer* fpsUpdateTimer;
    QLabel* fpsLabelOverlay;
    void handleModeChange(int index);
    void handlePolyDrawDeleteToggle(bool checked);
    void handleStaticPlaceDeleteToggle(bool checked);
    void handleBrightnessChange(int value);
    void showFeatureInDevelopmentMessage(const QString& feature = "This feature");
    void toggleControlPanel();
    void animatePanel();
    void updateHintsLabel(int modeIndex);
    void executeCommand(std::unique_ptr<Command> cmd);
    void undoLastCommand();
    void clearUndoStack(int modeIndex);
    void handleSettingAboutToChange(QWidget* widget);
    void handleSettingChanged(QWidget* widget, SettingValue currentValue);
    void updateFpsDisplay();
    void positionFpsLabel();
    void resetEditTracking();
    Controller m_raycaster_controller;
};

// --- SettingsChangeCommand Definition ---
class SettingsChangeCommand : public Command {
   public:
    SettingsChangeCommand(
        QWidget* widget, SettingValue oldVal, SettingValue newVal, MainWindow* context)
        : targetWidget(widget), oldValue(oldVal), newValue(newVal), mainWindowContext(context) {
    }

    void execute() override {
        applyValue(newValue);
    }

    void undo() override {
        applyValue(oldValue);
    }

   private:
    QWidget* targetWidget;
    SettingValue oldValue;
    SettingValue newValue;
    MainWindow* mainWindowContext;

    void applyValue(const SettingValue& val) {
        if (!targetWidget || !mainWindowContext) {
            return;
        }
        std::visit(
            [this](auto&& arg) {
                using T = std::decay_t<decltype(arg)>;
                bool changed_in_controller = false;
                if constexpr (std::is_same_v<T, int>) {
                    if (targetWidget == mainWindowContext->lightSourceCountSpinBox) {
                        mainWindowContext->m_raycaster_controller.setNumLightSources(arg);
                        changed_in_controller = true;
                    } else if (auto* s = qobject_cast<QSpinBox*>(targetWidget)) {
                        s->setValue(arg);
                    } else if (auto* sl = qobject_cast<QSlider*>(targetWidget)) {
                        sl->setValue(arg);
                        if (sl == mainWindowContext->staticLightBrightnessSlider) {
                            mainWindowContext->handleBrightnessChange(arg);
                        }
                    }
                } else if constexpr (std::is_same_v<T, double>) {
                    if (targetWidget == mainWindowContext->lightSourceDistanceSpinBox) {
                        mainWindowContext->m_raycaster_controller.setLightSpread(arg);
                        changed_in_controller = true;
                    } else if (auto* ds = qobject_cast<QDoubleSpinBox*>(targetWidget)) {
                        ds->setValue(arg);
                    }
                } else if constexpr (std::is_same_v<T, bool>) {
                    if (auto* b = qobject_cast<QPushButton*>(targetWidget)) {
                        if (b->isCheckable()) {
                            bool blocked = b->blockSignals(true);
                            b->setChecked(arg);
                            b->blockSignals(blocked);
                        }
                    }
                }
                if (changed_in_controller && mainWindowContext->canvasWidget) {
                    mainWindowContext->canvasWidget->update();
                }
            },
            val);
    }
};

//==============================================================================
// MainWindow Implementation
//==============================================================================
MainWindow::MainWindow(QWidget* parent) : QMainWindow(parent) {
    try {
        loadDefaults();
        m_raycaster_controller.setNumLightSources(defaultSettings.lightCount);
        m_raycaster_controller.setLightSpread(defaultSettings.lightSpread);
        setupUI();
        setWindowTitle("Raycaster UI - Final");
        applyStyles();
        resize(1100, 750);
        settingsCommitTimer = new QTimer(this);
        settingsCommitTimer->setSingleShot(true);
        settingsCommitTimer->setInterval(SETTINGS_COMMIT_TIMEOUT);
        connect(settingsCommitTimer, &QTimer::timeout, this, [this]() {
            this->commitPendingSettingsChange();
        });
        panelAnimationTimer = new QTimer(this);
        panelAnimationTimer->setInterval(16);
        connect(panelAnimationTimer, &QTimer::timeout, this, &MainWindow::animatePanel);
        fpsUpdateTimer = new QTimer(this);
        fpsUpdateTimer->setInterval(FPS_UPDATE_INTERVAL);
        connect(fpsUpdateTimer, &QTimer::timeout, this, &MainWindow::updateFpsDisplay);
        fpsUpdateTimer->start();
    } catch (const std::exception& e) {
        throw;
    } catch (...) {
        throw;
    }
}

MainWindow::~MainWindow() {
}

void MainWindow::initializeModeState() {
    handleModeChange(0);
    if (canvasWidget) {
        QTimer::singleShot(50, this, [this]() {
            if (canvasWidget) {
                QRectF canvasRect = canvasWidget->rect();
                QPointF centerPos = canvasRect.center();
                if (!centerPos.isNull() && !canvasRect.isEmpty()) {
                    m_raycaster_controller.setMasterLightSourcePosition(centerPos, canvasRect);
                    canvasWidget->update();
                } else {
                    QPointF fallbackCenter(this->width() / 2.0, this->height() / 2.0);
                    if (fallbackCenter.isNull() || this->rect().isEmpty()) {
                        fallbackCenter = QPointF(400, 300);
                    }
                    m_raycaster_controller.setMasterLightSourcePosition(
                        fallbackCenter,
                        this->rect().isEmpty() ? QRectF(0, 0, 800, 600) : this->rect());
                    canvasWidget->update();
                }
            }
        });
    }
}

void MainWindow::loadDefaults() {
}

void MainWindow::applyDefaults(int modeIndex) {
    resetEditTracking();
    lightSourceCountSpinBox->blockSignals(true);
    lightSourceDistanceSpinBox->blockSignals(true);
    polyDrawDeleteButton->blockSignals(true);
    staticPlaceDeleteButton->blockSignals(true);
    ModeDefaults defaults;
    lightSourceCountSpinBox->setValue(defaults.lightCount);
    lightSourceDistanceSpinBox->setValue(defaults.lightSpread);
    polyDrawDeleteButton->setChecked(defaults.polyDeleting);
    staticPlaceDeleteButton->setChecked(defaults.staticDeleting);
    m_raycaster_controller.setNumLightSources(defaults.lightCount);
    m_raycaster_controller.setLightSpread(defaults.lightSpread);
    handlePolyDrawDeleteToggle(defaults.polyDeleting);
    handleStaticPlaceDeleteToggle(defaults.staticDeleting);
    lightSourceCountSpinBox->blockSignals(false);
    lightSourceDistanceSpinBox->blockSignals(false);
    polyDrawDeleteButton->blockSignals(false);
    staticPlaceDeleteButton->blockSignals(false);
    notificationWidget->showTemporary("Settings reset to default.");
    if (canvasWidget) {
        canvasWidget->update();
    }
}

void MainWindow::applyStyles() {
    QString styleSheetTemplate = QLatin1String(
        R"( QMainWindow{background-color:#2e2e2e} QWidget{color:#e0e0e0;font-size:9pt} #ControlPanelWidget{background-color:#353535; border-right: 1px solid #4a4a4a;} #TogglePanelButton{background-color:#404040; border:none; border-left: 1px solid #555; border-right: none; color:#aaa; font-weight:bold; padding:5px 2px; min-width: %1px; max-width: %1px;} #TogglePanelButton:hover{background-color:#505050;color:#ccc} #FpsLabel { background-color: rgba(20, 20, 20, 190); color: #a0e0a0; padding: 3px 8px; border-radius: 4px; font-size: 10pt; } QGroupBox{font-weight:bold;border:1px solid #555;border-radius:5px;margin-top:1ex;padding-top:15px;padding-left:5px;padding-right:5px;padding-bottom:8px;background-color:#3a3a3a} QGroupBox::title{subcontrol-origin:margin;subcontrol-position:top left;padding:0 5px 0 5px;left:10px;color:#f0f0f0;font-size:10pt} QLabel{color:#c0c0c0;padding-top:3px;margin-bottom:2px} #HintsLabel{color:#a0a0a0;font-style:italic;padding:10px 5px;border-top:1px solid #4a4a4a;margin-top:15px;background-color:#383838} QComboBox,QSpinBox,QDoubleSpinBox{background-color:#444;border:1px solid #666;border-radius:3px;padding:4px;color:#e0e0e0;min-height:20px} QSlider{min-height:20px} QComboBox::drop-down{border:none;background-color:#555;width:20px} QComboBox::down-arrow{width:10px;height:10px} QSpinBox::up-button,QSpinBox::down-button,QDoubleSpinBox::up-button,QDoubleSpinBox::down-button{subcontrol-origin:border;background-color:#555;border-left:1px solid #666;width:18px} QSpinBox::up-arrow,QDoubleSpinBox::up-arrow{width:10px;height:10px} QSpinBox::down-arrow,QDoubleSpinBox::down-arrow{width:10px;height:10px} QPushButton{background-color:#5a5a5a;color:#e0e0e0;border:1px solid #777;border-radius:4px;padding:6px 12px;min-width:80px;min-height:22px} QPushButton:hover{background-color:#6a6a6a;border-color:#888} QPushButton:pressed{background-color:#4a4a4a} QPushButton:checked{background-color:#a03030;border-color:#c05050;color:#ffffff;font-weight:bold} QPushButton:disabled{background-color:#444;color:#888;border-color:#555} QSlider::groove:horizontal{border:1px solid #666;height:8px;background:#3a3a3a;margin:2px 0;border-radius:4px} QSlider::handle:horizontal{background:#8a8a8a;border:1px solid #999;width:16px;margin:-4px 0;border-radius:8px} QSlider::handle:horizontal:hover{background:#9a9a9a} CanvasWidget{background-color:#1e1e1e;border:1px solid #444} )");
    setStyleSheet(styleSheetTemplate.arg(PANEL_TOGGLE_BUTTON_WIDTH));
}

void MainWindow::setupUI() {
    centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);
    mainLayout = new QHBoxLayout(centralWidget);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);
    centralWidget->setFocusPolicy(Qt::ClickFocus);
    createControlPanel();
    notificationWidget = new NotificationWidget(this);
    canvasWidget = new CanvasWidget(
        [this]() { this->commitPendingSettingsChange(); }, &fpsCounterInstance,
        &m_raycaster_controller, polyDrawDeleteButton, staticPlaceDeleteButton, notificationWidget,
        this);
    mainLayout->addWidget(canvasWidget, 1);
    controlPanelContainer = new QWidget(this);
    controlPanelContainer->setFocusPolicy(Qt::ClickFocus);
    controlPanelMainLayout = new QHBoxLayout(controlPanelContainer);
    controlPanelMainLayout->setContentsMargins(0, 0, 0, 0);
    controlPanelMainLayout->setSpacing(0);
    togglePanelButton = new QPushButton(">", this);
    togglePanelButton->setObjectName("TogglePanelButton");
    togglePanelButton->setToolTip("Collapse/Expand Panel");
    togglePanelButton->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);
    connect(
        togglePanelButton, &QPushButton::clicked, this, [this]() { this->toggleControlPanel(); });
    controlPanelWidget->setObjectName("ControlPanelWidget");
    controlPanelWidget->setFixedWidth(PANEL_EXPANDED_WIDTH);
    isControlPanelCollapsed = false;
    controlPanelMainLayout->addWidget(togglePanelButton);
    controlPanelMainLayout->addWidget(controlPanelWidget);
    mainLayout->addWidget(controlPanelContainer);
    controlPanelContainer->setFixedWidth(PANEL_EXPANDED_WIDTH + PANEL_TOGGLE_BUTTON_WIDTH);
    notificationWidget->hide();
    fpsLabelOverlay = new QLabel("FPS: ---", this);
    fpsLabelOverlay->setObjectName("FpsLabel");
    fpsLabelOverlay->setMinimumSize(80, 20);
    fpsLabelOverlay->adjustSize();
    fpsLabelOverlay->show();
    positionFpsLabel();
    fpsLabelOverlay->raise();
    setupConnections();
}

void MainWindow::setupConnections() {
    connect(
        modeComboBox, QOverload<int>::of(&QComboBox::currentIndexChanged), this,
        [this](int index) { this->handleModeChange(index); });
    auto setupCommitCallback = [this](QWidget* widget) {
        if (auto* w = dynamic_cast<CommitOnFocusOutMixin*>(widget)) {
            w->setCommitCallback([this]() { this->commitPendingSettingsChange(); });
        } else if (auto* btn = qobject_cast<QPushButton*>(widget)) {
            if (btn->isCheckable()) {
                connect(btn, &QPushButton::toggled, this, [this, btn](bool checked) {
                    handleSettingAboutToChange(btn);
                    handleSettingChanged(btn, checked);
                    commitPendingSettingsChange();
                });
            }
        }
    };
    auto connectSettingWidget = [this, setupCommitCallback](QWidget* widget) {
        if (!widget) {
            return;
        }
        setupCommitCallback(widget);
        if (auto* s = qobject_cast<QSpinBox*>(widget)) {
            connect(s, QOverload<int>::of(&QSpinBox::valueChanged), this, [this, s](int val) {
                handleSettingChanged(s, val);
            });
        } else if (auto* ds = qobject_cast<QDoubleSpinBox*>(widget)) {
            connect(
                ds, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this,
                [this, ds](double val) { handleSettingChanged(ds, val); });
        } else if (auto* sl = qobject_cast<QSlider*>(widget)) {
            connect(sl, &QSlider::valueChanged, this, [this, sl](int val) {
                handleSettingChanged(sl, val);
            });
        }
    };
    connectSettingWidget(lightSourceCountSpinBox);
    connectSettingWidget(lightSourceDistanceSpinBox);
    connectSettingWidget(polyDrawDeleteButton);
    connectSettingWidget(staticPlaceDeleteButton);
    connect(
        polyDrawDeleteButton, &QPushButton::toggled, this, &MainWindow::handlePolyDrawDeleteToggle);
    connect(
        staticPlaceDeleteButton, &QPushButton::toggled, this,
        &MainWindow::handleStaticPlaceDeleteToggle);
}

void MainWindow::createControlPanel() {
    controlPanelWidget = new QWidget(this);
    controlPanelLayout = new QVBoxLayout(controlPanelWidget);
    controlPanelLayout->setContentsMargins(8, 8, 8, 8);
    controlPanelLayout->setSpacing(15);
    QLabel* modeLabel = new QLabel("Operating Mode:", this);
    modeComboBox = new QComboBox(this);
    modeComboBox->addItem("Dynamic Light");
    modeComboBox->addItem("Draw Polygons");
    modeComboBox->addItem("Place Static Lights");
    modeComboBox->setToolTip("Select interaction mode (Arrows)");
    controlPanelLayout->addWidget(modeLabel);
    controlPanelLayout->addWidget(modeComboBox);
    controlPanelLayout->addWidget(createModeControlContainer(), 1);
}

QWidget* MainWindow::createModeControlContainer() {
    QWidget* c = new QWidget(this);
    QVBoxLayout* l = new QVBoxLayout(c);
    l->setContentsMargins(0, 0, 0, 0);
    l->setSpacing(0);
    modeStackedWidget = new QStackedWidget(this);
    modeStackedWidget->addWidget(createLightModeControls());
    modeStackedWidget->addWidget(createPolygonModeControls());
    modeStackedWidget->addWidget(createStaticLightModeControls());
    hintsLabel = createHintsLabel();
    l->addWidget(modeStackedWidget);
    l->addWidget(hintsLabel);
    l->addStretch(1);
    return c;
}

QLabel* MainWindow::createHintsLabel() {
    QLabel* l = new QLabel(this);
    l->setObjectName("HintsLabel");
    l->setWordWrap(true);
    l->setAlignment(Qt::AlignTop | Qt::AlignLeft);
    l->setText("Hints will appear here.");
    l->setMinimumHeight(100);
    return l;
}

QWidget* MainWindow::createLightModeControls() {
    QWidget* c = new QWidget();
    QVBoxLayout* l = new QVBoxLayout(c);
    l->setContentsMargins(0, 0, 0, 0);
    l->setSpacing(10);
    QGroupBox* gb = new QGroupBox("Dynamic Light Source");
    QFormLayout* fl = new QFormLayout(gb);
    fl->setHorizontalSpacing(10);
    fl->setVerticalSpacing(8);
    fl->setRowWrapPolicy(QFormLayout::WrapLongRows);
    QLabel* cl =
        new QLabel("Total Sources (10-" + QString::number(MAX_EXTRA_LIGHT_SOURCES + 1) + "):");
    cl->setToolTip(
        "Total number of light sources (1 main + 0 to " + QString::number(MAX_EXTRA_LIGHT_SOURCES) +
        " additional).\nChanges apply immediately.");
    lightSourceCountSpinBox = new CommitOnFocusOutSpinBox();
    lightSourceCountSpinBox->setRange(1, MAX_EXTRA_LIGHT_SOURCES + 1);
    lightSourceCountSpinBox->setValue(defaultSettings.lightCount);
    lightSourceCountSpinBox->setToolTip(cl->toolTip());
    QLabel* dl = new QLabel("Spread:");
    dl->setToolTip(
        "Distance additional sources spread around the main one.\nChanges apply immediately.");
    lightSourceDistanceSpinBox = new CommitOnFocusOutDoubleSpinBox();
    lightSourceDistanceSpinBox->setRange(0.1, 50.0);
    lightSourceDistanceSpinBox->setDecimals(1);
    lightSourceDistanceSpinBox->setSingleStep(0.5);
    lightSourceDistanceSpinBox->setValue(defaultSettings.lightSpread);
    lightSourceDistanceSpinBox->setToolTip(dl->toolTip());
    fl->addRow(cl, lightSourceCountSpinBox);
    fl->addRow(dl, lightSourceDistanceSpinBox);
    l->addWidget(gb);
    l->addStretch(1);
    return c;
}

QWidget* MainWindow::createPolygonModeControls() {
    QWidget* c = new QWidget();
    QVBoxLayout* l = new QVBoxLayout(c);
    l->setContentsMargins(0, 0, 0, 0);
    l->setSpacing(10);
    QGroupBox* gb = new QGroupBox("Polygon Drawing");
    QVBoxLayout* gl = new QVBoxLayout(gb);
    gl->setSpacing(10);
    polyDrawDeleteButton = new QPushButton("Drawing Mode");
    polyDrawDeleteButton->setCheckable(true);
    polyDrawDeleteButton->setChecked(defaultSettings.polyDeleting);
    polyDrawDeleteButton->setToolTip(
        "Toggle between Drawing mode and Deleting mode.\nIn Deleting mode, hover to highlight and "
        "LMB click to delete a polygon.\nOr press 'D' key while hovering to delete.");
    gl->addWidget(polyDrawDeleteButton);
    l->addWidget(gb);
    l->addStretch(1);
    return c;
}

QWidget* MainWindow::createStaticLightModeControls() {
    QWidget* c = new QWidget();
    QVBoxLayout* l = new QVBoxLayout(c);
    l->setContentsMargins(0, 0, 0, 0);
    l->setSpacing(10);
    QGroupBox* gb = new QGroupBox("Static Light Sources");
    QVBoxLayout* gl = new QVBoxLayout(gb);
    gl->setSpacing(10);
    staticPlaceDeleteButton = new QPushButton("Place Mode");
    staticPlaceDeleteButton->setCheckable(true);
    staticPlaceDeleteButton->setChecked(defaultSettings.staticDeleting);
    staticPlaceDeleteButton->setToolTip(
        "Toggle between Placing new static light groups and Deleting them.\nIn Deleting mode, "
        "hover and LMB click or press 'D' to delete.");
    gl->addWidget(staticPlaceDeleteButton);
    l->addWidget(gb);
    l->addStretch(1);
    return c;
}

void MainWindow::resizeEvent(QResizeEvent* event) {
    QMainWindow::resizeEvent(event);
    positionFpsLabel();
}

void MainWindow::positionFpsLabel() {
    if (!fpsLabelOverlay) {
        return;
    }
    fpsLabelOverlay->move(width() - fpsLabelOverlay->width() - STANDARD_MARGIN, STANDARD_MARGIN);
    fpsLabelOverlay->raise();
}

void MainWindow::updateFpsDisplay() {
    QString fpsText = QString("FPS: %1").arg(fpsCounterInstance.getFps(), 0, 'f', 0);
    fpsLabelOverlay->setText(fpsText);
}

void MainWindow::keyPressEvent(QKeyEvent* event) {
    bool editWasActive = isEditingSettings;
    if (editWasActive && event->key() == Qt::Key_Escape && lastEditedWidget &&
        lastEditedWidget->hasFocus()) {
        resetEditTracking();
        if (lastEditedWidget) {
            SettingsChangeCommand tempCmd(
                lastEditedWidget, SettingValue{}, originalSettingValue, this);
            tempCmd.undo();
        }
        notificationWidget->showTemporary("Edit cancelled");
        event->accept();
        return;
    }
    commitPendingSettingsChange();
    int currentMode = modeComboBox->currentIndex();
    bool ctrl = event->modifiers() & Qt::ControlModifier;

    if (!ctrl && (event->key() == Qt::Key_Up || event->key() == Qt::Key_Down ||
                  event->key() == Qt::Key_Left || event->key() == Qt::Key_Right)) {
        int cnt = modeComboBox->count();
        if (cnt > 0) {
            int next = currentMode;
            if (event->key() == Qt::Key_Up || event->key() == Qt::Key_Left) {
                next = (currentMode - 1 + cnt) % cnt;
            } else {
                next = (currentMode + 1) % cnt;
            }
            modeComboBox->setCurrentIndex(next);
            event->accept();
            return;
        }
    }

    if (canvasWidget && canvasWidget->hasFocus()) {
        bool needs_update = false;
        if (currentMode == 1) {
            bool is_delete_mode = polyDrawDeleteButton->isChecked();
            if (!is_delete_mode && event->key() == Qt::Key_A) {
                m_raycaster_controller.addVertexToCurrentPolygon(
                    canvasWidget->mapFromGlobal(QCursor::pos()));
                needs_update = true;
            } else if (
                !is_delete_mode &&
                (event->key() == Qt::Key_Return || event->key() == Qt::Key_Enter)) {
                if (!m_raycaster_controller.finishCurrentPolygon()) {
                    notificationWidget->showTemporary(
                        "Polygon creation cancelled:\nintersects light source.");
                }
                needs_update = true;
            } else if (event->key() == Qt::Key_Escape) {
                m_raycaster_controller.cancelCurrentPolygon();
                needs_update = true;
            }
        }
        if (event->key() == Qt::Key_D) {
            int hoveredPolyIndex = canvasWidget->getHoveredPolygonIndex();
            int hoveredStaticLightId = canvasWidget->getHoveredStaticLightId();

            if (hoveredPolyIndex != -1) {
                m_raycaster_controller.deletePolygonByIndex(hoveredPolyIndex);
                canvasWidget->resetHoveredPolygonIndex();
                notificationWidget->showTemporary("Polygon deleted.");
                needs_update = true;
            } else if (hoveredStaticLightId != -1 && currentMode == 2) {
                m_raycaster_controller.deleteStaticLightSourceById(hoveredStaticLightId);
                canvasWidget->resetHoveredStaticLightId();
                notificationWidget->showTemporary("Static light source deleted.");
                needs_update = true;
            }
        }
        if (event->key() == Qt::Key_R) {
            m_raycaster_controller.ClearObstacles();
            QPointF centerPos = canvasWidget->rect().center();
            if (!centerPos.isNull() && !canvasWidget->rect().isEmpty()) {
                m_raycaster_controller.setMasterLightSourcePosition(
                    centerPos, canvasWidget->rect());
            }
            notificationWidget->showTemporary("Obstacles cleared, light reset.");
            needs_update = true;
        }
        if (needs_update) {
            canvasWidget->update();
            event->accept();
            return;
        }
    }
    switch (currentMode) {
        case 1:
            if (event->key() == Qt::Key_Delete) {
                showFeatureInDevelopmentMessage("Delete Polygon (use button/D key)");
                event->accept();
                return;
            }
            break;
        case 2:
            if (event->key() == Qt::Key_Delete) {
                showFeatureInDevelopmentMessage("Delete Static Light (use button/D key)");
                event->accept();
                return;
            }
            break;
    }
    QMainWindow::keyPressEvent(event);
}

void MainWindow::handleModeChange(int index) {
    commitPendingSettingsChange();
    modeStackedWidget->setCurrentIndex(index);
    updateHintsLabel(index);
    if (canvasWidget) {
        canvasWidget->setCurrentOperatingMode(index);
        switch (index) {
            case 0:
                canvasWidget->setCursor(Qt::CrossCursor);
                break;
            case 1:
                handlePolyDrawDeleteToggle(polyDrawDeleteButton->isChecked());
                break;
            case 2:
                handleStaticPlaceDeleteToggle(staticPlaceDeleteButton->isChecked());
                break;
        }
        canvasWidget->setFocus();
        canvasWidget->update();
    }
    if (polyDrawDeleteButton && index != 1 && polyDrawDeleteButton->isChecked()) {
        polyDrawDeleteButton->setChecked(false);
    }
    if (staticPlaceDeleteButton && index != 2 && staticPlaceDeleteButton->isChecked()) {
        staticPlaceDeleteButton->setChecked(false);
    }

    notificationWidget->showTemporary(
        QString("Switched to %1 Mode").arg(modeComboBox->itemText(index)));
}

void MainWindow::updateHintsLabel(int modeIndex) {
    QString hint =
        "Global: Arrows: Switch Mode | R: Clear Polygons\n"
        "D: Delete Hovered Polygon/Static Light (in Static Mode)\n\n";
    switch (modeIndex) {
        case 0:
            hint +=
                "Mode: Dynamic Light\n"
                "LMB Drag: Move Light | RMB Click: Teleport Light";
            break;
        case 1:
            hint +=
                "Mode: Draw Polygons\n"
                "[Drawing] LMB/A: Add Vertex | Enter/RMB: Finish | Esc: Cancel\n"
                "[Deleting] Hover: Highlight | LMB Click: Delete Hovered";
            break;
        case 2:
            hint +=
                "Mode: Place Static Lights\n"
                "[Placing] LMB Click: Add Static Light Group\n"
                "[Deleting] Hover: Highlight | LMB Click: Delete Hovered";
            break;
    }
    hintsLabel->setText(hint);
}

void MainWindow::handlePolyDrawDeleteToggle(bool checked) {
    if (modeComboBox->currentIndex() == 1 && canvasWidget) {
        polyDrawDeleteButton->setText(checked ? "Deleting Mode" : "Drawing Mode");
        canvasWidget->setCursor(Qt::CrossCursor);
        if (!checked) {
            canvasWidget->resetHoveredPolygonIndex();
        }
        canvasWidget->update();
    } else if (checked) {
        polyDrawDeleteButton->blockSignals(true);
        polyDrawDeleteButton->setChecked(false);
        polyDrawDeleteButton->blockSignals(false);
    }
}

void MainWindow::handleStaticPlaceDeleteToggle(bool checked) {
    if (modeComboBox->currentIndex() == 2 && canvasWidget) {
        staticPlaceDeleteButton->setText(
            checked ? "Deleting Mode (Static)" : "Place Mode (Static)");
        canvasWidget->setCursor(Qt::CrossCursor);
        if (!checked) {
            canvasWidget->resetHoveredStaticLightId();
        }
        canvasWidget->update();
    } else if (checked) {
        staticPlaceDeleteButton->blockSignals(true);
        staticPlaceDeleteButton->setChecked(false);
        staticPlaceDeleteButton->blockSignals(false);
    }
}

void MainWindow::handleBrightnessChange(int value) {
    Q_UNUSED(value);
}

void MainWindow::showFeatureInDevelopmentMessage(const QString& feature) {
    notificationWidget->showTemporary(QString("%1 is under development.").arg(feature));
}

void MainWindow::toggleControlPanel() {
    commitPendingSettingsChange();
    bool collapsing = !isControlPanelCollapsed;
    panelAnimationTargetWidth = collapsing ? PANEL_COLLAPSED_WIDTH : PANEL_EXPANDED_WIDTH;
    controlPanelContainer->setMaximumWidth(PANEL_EXPANDED_WIDTH + PANEL_TOGGLE_BUTTON_WIDTH);
    controlPanelContainer->setMinimumWidth(PANEL_COLLAPSED_WIDTH + PANEL_TOGGLE_BUTTON_WIDTH);
    panelAnimationTimer->start();
    if (collapsing && !firstCollapseNotificationShown) {
        notificationWidget->showTemporary(
            "Hint: Use Arrow keys\nto switch modes when panel is closed.");
        firstCollapseNotificationShown = true;
    }
}

void MainWindow::animatePanel() {
    int currentContainerWidth = controlPanelContainer->width();
    int targetContainerWidth = panelAnimationTargetWidth + PANEL_TOGGLE_BUTTON_WIDTH;
    int step = qMax(
        5, (PANEL_EXPANDED_WIDTH /
            (PANEL_ANIMATION_DURATION / qMax(1, panelAnimationTimer->interval()))));
    int newContainerWidth = (targetContainerWidth > currentContainerWidth)
                                ? qMin(currentContainerWidth + step, targetContainerWidth)
                                : qMax(currentContainerWidth - step, targetContainerWidth);
    controlPanelContainer->setFixedWidth(newContainerWidth);
    controlPanelWidget->setFixedWidth(newContainerWidth - PANEL_TOGGLE_BUTTON_WIDTH);
    if (newContainerWidth == targetContainerWidth) {
        panelAnimationTimer->stop();
        isControlPanelCollapsed = (panelAnimationTargetWidth == PANEL_COLLAPSED_WIDTH);
        togglePanelButton->setText(isControlPanelCollapsed ? "<" : ">");
        bool vis_final = !isControlPanelCollapsed;
        modeComboBox->setVisible(vis_final);
        modeStackedWidget->setVisible(vis_final);
        hintsLabel->setVisible(vis_final);
    }
}

void MainWindow::executeCommand(std::unique_ptr<Command> cmd) {
    if (!cmd) {
        return;
    }
    cmd->execute();
}

void MainWindow::undoLastCommand() {
    notificationWidget->showTemporary("Undo for settings is disabled.");
}

void MainWindow::clearUndoStack(int modeIndex) {
    Q_UNUSED(modeIndex);
}

void MainWindow::handleSettingAboutToChange(QWidget* widget) {
    if (!widget || isEditingSettings) {
        return;
    }
    isEditingSettings = true;
    lastEditedWidget = widget;
    if (auto* s = qobject_cast<QSpinBox*>(widget)) {
        originalSettingValue = s->value();
    } else if (auto* ds = qobject_cast<QDoubleSpinBox*>(widget)) {
        originalSettingValue = ds->value();
    } else if (auto* sl = qobject_cast<QSlider*>(widget)) {
        originalSettingValue = sl->value();
    } else if (auto* b = qobject_cast<QPushButton*>(widget)) {
        if (b->isCheckable()) {
            originalSettingValue = b->isChecked();
        } else {
            resetEditTracking();
            return;
        }
    } else {
        resetEditTracking();
        return;
    }
}

void MainWindow::handleSettingChanged(QWidget* widget, SettingValue currentValue) {
    Q_UNUSED(currentValue);
    if (!widget) {
        return;
    }
    if (!isEditingSettings || lastEditedWidget != widget) {
        handleSettingAboutToChange(widget);
        if (!isEditingSettings) {
            return;
        }
    }
    settingsCommitTimer->start();
}

void MainWindow::resetEditTracking() {
    if (isEditingSettings) {
        settingsCommitTimer->stop();
        isEditingSettings = false;
        lastEditedWidget = nullptr;
    }
}

void MainWindow::commitPendingSettingsChange() {
    if (!isEditingSettings || !lastEditedWidget) {
        resetEditTracking();
        return;
    }
    SettingValue fVal;
    bool ok = true;
    if (auto* s = qobject_cast<QSpinBox*>(lastEditedWidget)) {
        fVal = s->value();
    } else if (auto* ds = qobject_cast<QDoubleSpinBox*>(lastEditedWidget)) {
        fVal = ds->value();
    } else if (auto* sl = qobject_cast<QSlider*>(lastEditedWidget)) {
        fVal = sl->value();
    } else if (auto* b = qobject_cast<QPushButton*>(lastEditedWidget)) {
        if (b->isCheckable()) {
            fVal = b->isChecked();
        } else {
            ok = false;
        }
    } else {
        ok = false;
    }
    if (!ok) {
        resetEditTracking();
        return;
    }
    if (originalSettingValue != fVal) {
        SettingsChangeCommand cmd(lastEditedWidget, originalSettingValue, fVal, this);
        cmd.execute();
    }
    resetEditTracking();
}

//==============================================================================
// main Function
//==============================================================================
int main(int argc, char* argv[]) {
    QApplication a(argc, argv);
    int result = -1;
    try {
        MainWindow w;
        w.show();
        QTimer::singleShot(50, &w, &MainWindow::initializeModeState);
        result = a.exec();
    } catch (const std::exception& e) {
        // Exception message will be lost, but per instruction, no debug output.
        return 1;
    } catch (...) {
        return 2;
    }
    return result;
}
