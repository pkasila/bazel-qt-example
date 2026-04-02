#ifndef RAYCASTER_HPP
#define RAYCASTER_HPP

#include <QQuickPaintedItem>
#include "camera.hpp"
#include "polygon.hpp"

namespace rc {
    class Raycaster;
}

class rc::Raycaster : public QQuickPaintedItem {
    Q_OBJECT
    Q_PROPERTY(int mode MEMBER mode NOTIFY modeChanged);
    Q_PROPERTY(QPointF lightPosition MEMBER lightPosition NOTIFY lightPositionChanged);
    Q_PROPERTY(QString fpsText MEMBER fpsText NOTIFY fpsTextChanged);
    QML_ELEMENT

public:
    Raycaster(QQuickItem* parent = nullptr);
    void paint(QPainter* painter) override;

    // Camera
    Q_INVOKABLE void shift(QPointF mouse);
    Q_INVOKABLE void resetShift();
    Q_INVOKABLE void rescale(double factor, QPointF mouse);

    // Polygon editing
    Q_INVOKABLE void newVertex(QPointF point);
    Q_INVOKABLE void setLastVertex(QPointF point);
    Q_INVOKABLE void finishPolygon();

    Q_INVOKABLE void addStaticLight();
    Q_INVOKABLE void clear();

signals:
    void modeChanged();
    void lightPositionChanged();
    void fpsTextChanged();

public slots:
    void redraw();

private:
    void drawPolygons(QPainter* painter);
    void drawLights(QPainter* painter);
    void drawLightSource(QPainter* painter, QPointF source);
    std::vector<QPointF> getLightPolygon(const QPointF& source);
    void rebuildExtraPoints();

    std::vector<Polygon> polygons;
    Camera cam;

    QPointF lastMouse;
    bool shiftActive = false;

    std::vector<QPointF> staticLights;
    QPointF lightPosition;
    int mode = 0;

    std::vector<QPointF> extraPoints;

    QString fpsText = "∞ FPS";
    double frameTime = 0;
};

#endif // RAYCASTER_HPP
