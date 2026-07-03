#ifndef DRAWINGAREA_H
#define DRAWINGAREA_H
//Qt libraries
#include <QWidget>
#include <QPoint>
#include <QPointF>
#include <QPainter>
#include <QPen>
#include <QMouseEvent>
#include <QCursor>
#include <QImage>
//Custom libraries
#include "Controller.h"
#include "Polygon.h"
#include "Mode.h"

class DrawingArea : public QWidget {
    Q_OBJECT

public:
    DrawingArea(QWidget *parent = nullptr);
    void setMode(Mode mode);

protected:
    void paintEvent(QPaintEvent *) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;

private:
    Controller controller;
    Mode mode;
    bool polygonInProgress;

private:
    void determineRaysEndPoints(QVector<Ray>& rays) const;
    Polygon getFictitiousPerimeterPolygon() const;
    QPoint getCursorPositionInDrawingArea() const;
    bool isPointInsideDrawingArea(const QPointF& point) const;
    bool isCursorInsideDrawingArea() const;
    QVector<QPointF> getIncidentalLightSources(const QPointF& mainLightSource) const;
    Polygon getLightPolygonForLightSource(const QPointF& lightSource) const;
    void doLeftButton(const QPointF& cursorPos);
    void doRightButton();
};

#endif // DRAWINGAREA_H
