#ifndef PROJECTCONSTANTS_H
#define PROJECTCONSTANTS_H
#include <QString>
#include <QColor>
#include <QSize>
#include <QMargins>

namespace Geometry {
    inline constexpr double EPSILON = 1e-9;
    inline constexpr double closeRaysEpsilon = 1e-2;
    inline constexpr double vertexRayRotationAngle = 1e-4;
}

namespace Proportions {
    inline const QSize mainWindowSize = QSize(800, 600);
    inline const QSize minMainWindowSize = QSize(300, 200);
    inline const QSize drawingAreaMinSize = QSize(100, 100);

    inline const QSize lightIconSize = QSize(35, 35);
    inline const QSize lightButtonSize = QSize(50, 50);
    inline const QSize polygonIconSize = QSize(30, 30);
    inline const QSize polygonButtonSize = QSize(50, 50);

    inline const QMargins drawingAreaFrameMargins = QMargins(0, 0, 0, 0);

    inline constexpr int lightCircleRadius = 5;
    inline constexpr int smallLightCircleRadius = 2;
    inline constexpr int drawingCircleRadius = 3;
}

namespace DrawingParameters {
    inline constexpr int completedPolygonThickness = 2;
    inline constexpr int activePolygonThickness = 1;

    inline constexpr double lightShift = 6;
    inline const QVector<QPointF> shifts = {QPointF(DrawingParameters::lightShift, 0),
                                            QPointF(0, DrawingParameters::lightShift),
                                            QPointF(-DrawingParameters::lightShift, 0),
                                            QPointF(0, -DrawingParameters::lightShift)};
}

namespace Titles {
    inline const QString mainWindowTitle = "Raycaster";
}

namespace Texts {
    inline const QString lightButtonComment = "Light Mode";
    inline const QString polygonButtonComment = "Polygon Mode";
}

namespace Paths {
    inline const QString lightIconPath = ":/icons/icons/light.png";
    inline const QString polygonIconPath = ":/icons/icons/polygon.png";
}

namespace CustomColors {
    inline const QColor lightPointColor(128, 0, 32);
    inline const QColor lightColor(255, 200, 0, 20);
}

namespace Styles {
    inline const QString lightButtonStyle = "QToolButton {"
                                            "    background-color: #F5F5F5;"
                                            "    border: 1px solid #D1D1D1;"
                                            "    padding: 5px;"
                                            "}"
                                            "QToolButton:hover {"
                                            "    background-color: #E0E0E0;"
                                            "}"
                                            "QToolButton:pressed {"
                                            "    background-color: #D1D1D1;"
                                            "}"
                                            "QToolButton:checked {"
                                            "    background-color: #CCCCCC;"
                                            "}";

    inline const QString polygonButtonStyle = "QToolButton {"
                                              "    background-color: #F5F5F5;"
                                              "    border: 1px solid #D1D1D1;"
                                              "    padding: 5px;"
                                              "}"
                                              "QToolButton:hover {"
                                              "    background-color: #E0E0E0;"
                                              "}"
                                              "QToolButton:pressed {"
                                              "    background-color: #D1D1D1;"
                                              "}"
                                              "QToolButton:checked {"
                                              "    background-color: #CCCCCC;"
                                              "}";

    inline const QString drawingAreaFrameStyle = "QFrame { border: 2px solid grey; }";

}

#endif // PROJECTCONSTANTS_H
