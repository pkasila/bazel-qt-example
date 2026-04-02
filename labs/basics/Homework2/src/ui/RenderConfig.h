#pragma once

#include <QColor>

namespace render_config {

inline const QColor kBackgroundColor = QColor(18, 18, 22);

inline const QColor kPolygonEdgeColor = QColor(210, 210, 220);
inline const QColor kPolygonVertexColor = QColor(255, 140, 90);
inline const QColor kPreviewColor = QColor(100, 180, 255);

inline const QColor kMainLightSourceColor = QColor(150, 150, 150);
inline const QColor kSecondaryLightSourceColor = QColor(180, 180, 180);
inline const QColor kLightAreaColor = QColor(255, 255, 255, 50);

inline constexpr int kMainLightSourceRadius = 3;
inline constexpr int kSecondaryLightSourceRadius = 3;
inline constexpr int kVertexRadius = 4;

inline constexpr double kEdgeWidth = 2.0;
inline constexpr double kPreviewWidth = 2.0;

inline constexpr int kBoundaryMargin = 2;
inline constexpr double kLightSourceWallMargin = 3.0;

inline const QColor kStaticMainLightSourceColor = kMainLightSourceColor;
inline const QColor kStaticSecondaryLightSourceColor = kSecondaryLightSourceColor;
inline const QColor kStaticLightAreaColor = kLightAreaColor;

inline constexpr int kStaticMainLightSourceRadius = 3;
inline constexpr int kStaticSecondaryLightSourceRadius = 3;
}