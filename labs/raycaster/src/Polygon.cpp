#include "Polygon.h"
//Constructors
Polygon::Polygon(const QVector<QPointF>& vertices) : vertices(vertices) {}

Polygon::Polygon() : Polygon(QVector<QPointF>()) {}

//Getters
const QVector<QPointF>& Polygon::getVertices() const {
    return this->vertices;
}

QPointF Polygon::getLastVertex() const {
    return this->vertices.last();
}

QVector<Segment> Polygon::getSides() const noexcept {
    QVector<Segment> sides;
    for (int i = 0; i < this->vertices.count() - 1; ++i) {
        Segment side = Segment(vertices[i], vertices[i + 1]);
        sides.push_back(side);
    }
    Segment lastSide = Segment(vertices.first(), vertices.last());
    sides.push_back(lastSide);
    return sides;
}

//Public methods
void Polygon::addVertex(const QPointF& newVertex) {
    vertices.push_back(newVertex);
}

bool Polygon::isPointPolygonVertex(const QPointF &point) const {
    bool isPointPolygonVertex = false;
    for (const QPointF& vertex : this->getVertices()) {
        if (vertex == point) {
            isPointPolygonVertex = true;
            break;
        }
    }
    return isPointPolygonVertex;
}

std::optional<QPointF> Polygon::intersectRay (const Ray& ray) const {
    QPointF beginPoint = ray.getBeginPoint();

    if (vertices.count() < 2) {
        return std::nullopt;
    }

    QVector<QPointF> intersectionPoints;
    for (int i = 0; i < vertices.count() - 1; ++i) {
        Segment polygonSide = Segment(vertices[i], vertices[i + 1]);
        std::optional<QPointF> intersection = polygonSide.findIntersectionWithRay(ray);
        if (intersection) {
            intersectionPoints.push_back(*intersection);
        }
    }

    if (vertices.count() > 2) {
        Segment polygonSide = Segment(vertices.first(), vertices.last());
        std::optional<QPointF> intersection = polygonSide.findIntersectionWithRay(ray);
        if (intersection) {
            intersectionPoints.push_back(*intersection);
        }
    }

    if (intersectionPoints.count() == 0) {
        return std::nullopt;
    }

    QPointF closestIntersectionPoint = findClosestPointToTheGiven(beginPoint,
                                                                  intersectionPoints);
    return closestIntersectionPoint;
}
