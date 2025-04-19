#include "Controller.h"
//Constructors
Controller::Controller(const QVector<Polygon>& polygons) :
    polygons(polygons), lightSource(QPointF()) {}

Controller::Controller() : Controller(QVector<Polygon>()) {}
//Getters
const QVector<Polygon>& Controller::getPolygons() const {
    return this->polygons;
}

QPointF Controller::getLightSource() const {
    return this->lightSource;
}

const Polygon& Controller::getLastPolygon() const {
    return this->polygons.last();
}

//Setters
void Controller::setLightSource(const QPointF& lightSource) {
    this->lightSource = lightSource;
}

//Methods
void Controller::addPolygon(const Polygon& polygon) {
    this->polygons.push_back(polygon);
}

void Controller::popPolygon() {
    this->polygons.pop_back();
}

void Controller::addVertexToLastPolygon(const QPointF& vertex) {
    this->polygons.last().addVertex(vertex);
}

QVector<Ray> Controller::castRays(const QPointF& lightSource) const {
    QVector<Ray> rays;
    for (const Polygon& polygon : getPolygons()) {
        for (const QPointF& vertex : polygon.getVertices()) {
            Ray newRay = Ray(lightSource, vertex);
            Ray rotatedRay_1 = newRay.rotate(Geometry::vertexRayRotationAngle);
            Ray rotatedRay_2 = newRay.rotate(-Geometry::vertexRayRotationAngle);
            rays.push_back(newRay);
            rays.push_back(rotatedRay_1);
            rays.push_back(rotatedRay_2);
        }
    }
    return rays;
}

void Controller::intersectRays(QVector<Ray>& rays) const {
    for (Ray& ray : rays) {
        for (const Polygon& polygon : getPolygons()) {
            auto intersection = polygon.intersectRay(ray);
            if (intersection != std::nullopt && ray.isEndPointDetermined()) {
                if (getDistance(lightSource, *intersection) <
                    getDistance(lightSource, *ray.getEndPoint())) {
                    ray.setEndPoint(*intersection);
                }
            }
        }
    }
}

void Controller::removeAdjacentRays(QVector<Ray>& rays, double threshold) const {
    QVector<Ray> filtered;
    for (int i = 0; i < rays.count(); ++i) {
        QPointF endPointA = *rays[i].getEndPoint();
        bool isCloseToAnotherRay = false;
        for (const Ray& keptRay : filtered) {
            if (getDistance(*keptRay.getEndPoint(), endPointA) < threshold) {
                isCloseToAnotherRay = true;
                break;
            }
        }
        if (!isCloseToAnotherRay) {
            filtered.push_back(rays[i]);
        }
    }
    rays = std::move(filtered);
}

Polygon Controller::createLightPolygon(QVector<Ray>& rays) const {
    std::sort(rays.begin(), rays.end(), [](const Ray& a, const Ray& b) {
        Vector2D dirA = a.getDirectionVector();
        Vector2D dirB = b.getDirectionVector();

        double angleA = normalizeAngle(std::atan2(dirA.getYCord(), dirA.getXCord()));
        double angleB = normalizeAngle(std::atan2(dirB.getYCord(), dirB.getXCord()));
        return angleA < angleB;
    });

    QVector<QPointF> verticesOfLightPolygon;
    for (const Ray& ray : std::as_const(rays)) {
        verticesOfLightPolygon.push_back(*ray.getEndPoint());
    }
    return Polygon(verticesOfLightPolygon);
}

bool Controller::isNewSideValid(const Segment& newSide) const {
    //Intersections with completed polygons are forbidden
    bool isNewSideAvailable = true;
    for (int i = 0; i < polygons.count() - 1; ++i) {
        Polygon currPolygon = polygons[i];
        QVector<Segment> currPolygonSides = currPolygon.getSides();
        for (const Segment& side : std::as_const(currPolygonSides)) {
            if (side.findIntersectionWithSegment(newSide)) {
                isNewSideAvailable = false;
                break;
            }
        }
        if (isNewSideAvailable == false) {
            break;
        }
    }

    /* New sides of active polygon are allowed to have
       intersection with another side only in a vertex */
    Polygon activePolygon = polygons.last();
    QVector<Segment> activePolygonSides = activePolygon.getSides();
    for (const Segment& side : std::as_const(activePolygonSides)) {
        auto intersection = side.findIntersectionWithSegment(newSide);
        if (intersection) {
            if (activePolygon.isPointPolygonVertex(*intersection) == false) {
                isNewSideAvailable = false;
                break;
            }
        }
    }

    return isNewSideAvailable;
}

bool Controller::isNewPolygonStartingPointValid(const QPointF& point) const {
    bool isNewPolygonStartingPointValid = true;
    for (const Polygon& polygon : this->getPolygons()) {
        QVector<Segment> polygonSides = polygon.getSides();
        for (const Segment& side : std::as_const(polygonSides)) {
            if (side.isPointOnSegment(point)) {
                isNewPolygonStartingPointValid = false;
                break;
            }
        }
        if (isNewPolygonStartingPointValid == false) {
            break;
        }
    }
    return isNewPolygonStartingPointValid;
}

