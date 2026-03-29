#ifndef UTILS_H
#define UTILS_H

#include <QColor>
#include <cmath>
#include <numbers>

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

#endif  // UTILS_H
