#include "DrawingArea.h"

DrawingArea::DrawingArea(QWidget *parent): QWidget(parent),
    mode(Mode::Polygons), polygonInProgress(false) {
    setMouseTracking(true);
}

void DrawingArea::setMode(Mode newMode) {
    this->mode = newMode;
}

Polygon DrawingArea::getFictitiousPerimeterPolygon() const {
    double width = this->width();
    double height = this->height();
    QVector<QPointF> fictitiousPolygonVertices = {QPointF(0, 0),
                                                  QPointF(width, 0),
                                                  QPointF(width, height),
                                                  QPointF(0, height)};
    return Polygon(fictitiousPolygonVertices);
}

QPoint DrawingArea::getCursorPositionInDrawingArea() const {
    return mapFromGlobal(QCursor::pos());
}

bool DrawingArea::isPointInsideDrawingArea(const QPointF& point) const {
    bool isXCordValid = (0 < point.x()) &&
                        (point.x() < this->width());
    bool isYCordValid = (0 < point.y()) &&
                        (point.y() < this->height());
    return isXCordValid && isYCordValid;
}

bool DrawingArea::isCursorInsideDrawingArea() const {
    QPointF cursorPosition = getCursorPositionInDrawingArea();
    return isPointInsideDrawingArea(cursorPosition);
}

void DrawingArea::determineRaysEndPoints(QVector<Ray>& rays) const {
    Polygon fictitiousPolygon = getFictitiousPerimeterPolygon();
    for (Ray& ray : rays) {
        if (!ray.isEndPointDetermined()) {
            std::optional<QPointF> intersection = fictitiousPolygon.intersectRay(ray);
            ray.setEndPoint(*intersection);
        }
    }
}

void DrawingArea::paintEvent(QPaintEvent *) {
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    controller.addPolygon(getFictitiousPerimeterPolygon());

    QImage lightLayer(size(), QImage::Format_ARGB32_Premultiplied);
    lightLayer.fill(Qt::transparent);

    if (mode == Mode::Light && isCursorInsideDrawingArea()) {
        QPointF lightSource = controller.getLightSource();
        painter.setBrush(CustomColors::lightPointColor);
        painter.setPen(Qt::NoPen);
        painter.drawEllipse(lightSource, Proportions::lightCircleRadius,
                                         Proportions::lightCircleRadius);
        painter.setBrush(Qt::NoBrush);

        //Drawing light
        QPainter lightPainter(&lightLayer);
        lightPainter.setRenderHint(QPainter::Antialiasing);
        lightPainter.setCompositionMode(QPainter::CompositionMode_Plus);
        lightPainter.setBrush(CustomColors::lightColor);
        lightPainter.setPen(Qt::NoPen);

        //Light sources
        QVector<QPointF> lightSources = getIncidentalLightSources(lightSource);
        lightSources.push_back(lightSource);

        for (const QPointF& lightSource : std::as_const(lightSources)) {
            Polygon lightPolygon = getLightPolygonForLightSource(lightSource);
            lightPainter.drawPolygon(lightPolygon.getVertices());
        }

        lightPainter.end();
    }

    //Completed  polygons
    const QVector<Polygon>& polygons = controller.getPolygons();
    painter.setPen(QPen(Qt::black, DrawingParameters::completedPolygonThickness));
    painter.setBrush(Qt::NoBrush);
    for (int i = 0; i < polygons.count() - 1 - (polygonInProgress ? 1 : 0); ++i) {
        const QVector<QPointF>& points = polygons[i].getVertices();
        if (points.size() >= 2) {
            painter.drawPolygon(points);
        }
    }

    //Draw active polygon
    if (polygonInProgress && polygons.size() > 1) {
        //Get current polygon vertices
        const QVector<QPointF>& points = polygons[polygons.size() - 2].getVertices();
        painter.setPen(QPen(Qt::black, DrawingParameters::activePolygonThickness));
        if (points.count() >= 2) {
            painter.drawPolyline(points);
        } else if (points.count() == 1) {
            painter.drawEllipse(points[0], Proportions::drawingCircleRadius,
                                           Proportions::drawingCircleRadius);
        }
    }
    controller.popPolygon(); //pop the fictitious polygon

    painter.drawImage(0, 0, lightLayer);
    painter.end();
}

Polygon DrawingArea::getLightPolygonForLightSource(const QPointF& lightSource) const {
    QVector<Ray> castedRays = controller.castRays(lightSource);
    determineRaysEndPoints(castedRays);
    controller.intersectRays(castedRays);
    controller.removeAdjacentRays(castedRays, Geometry::closeRaysEpsilon);
    Polygon lightPolygon = controller.createLightPolygon(castedRays);
    return lightPolygon;
}

void DrawingArea::mousePressEvent(QMouseEvent *event) {
    QPointF cursorPos = event->pos();
    if (mode == Mode::Light) {
        controller.setLightSource(cursorPos);
    } else if (mode == Mode::Polygons) {
        if (event->button() == Qt::LeftButton) {
            doLeftButton(cursorPos);
        } else if (event->button() == Qt::RightButton && polygonInProgress) {
            //Finishing the polygon
            doRightButton();
        }
    }
    update();
}

void DrawingArea::doLeftButton(const QPointF& cursorPos) {
    if (polygonInProgress) {
        Segment newSide = Segment(cursorPos, controller.getLastPolygon().getLastVertex());
        if (controller.isNewSideValid(newSide)) {
            controller.addVertexToLastPolygon(cursorPos);
        }
    } else {
        if (controller.isNewPolygonStartingPointValid(cursorPos)) {
            controller.addPolygon(Polygon());
            polygonInProgress = true;
            controller.addVertexToLastPolygon(cursorPos);
        }
    }
}

void DrawingArea::doRightButton() {
    if (controller.getLastPolygon().getVertices().count() == 1) {
        controller.popPolygon(); //a point is not a polygon
    } else {
        Segment newSide = Segment(controller.getLastPolygon().getVertices()[0],
                                  controller.getLastPolygon().getLastVertex());
        if (controller.isNewSideValid(newSide)) {
            polygonInProgress = false;
        }
    }
}

void DrawingArea::mouseMoveEvent(QMouseEvent *event) {
    if (mode == Mode::Light && isCursorInsideDrawingArea()) {
        controller.setLightSource(event->pos());
        update();
    }
}

QVector<QPointF> DrawingArea::getIncidentalLightSources(const QPointF& mainLightSource) const {
    QVector<QPointF> incidentalLightSources;
    for (const QPointF& shift : DrawingParameters::shifts) {
        QPointF newLightSource = mainLightSource + shift;
        if (isPointInsideDrawingArea(newLightSource)) {
            incidentalLightSources.push_back(newLightSource);
        }
    }
    return incidentalLightSources;
}
