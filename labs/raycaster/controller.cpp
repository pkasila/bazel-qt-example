#include "controller.h"
#include <algorithm>
#include <unordered_set>
#include <QDebug>

const std::vector<Polygon>& Controller::GetPolygons() {
    return polygons;
}

void Controller::AddPolygon(const Polygon& p) {
    polygons.push_back(p);
}

void Controller::AddVertexToLastPolygon(const QPointF& new_vertex) {
    if (polygons.empty()) {
        return;
    }
    polygons.back().AddVertex(new_vertex);
}

void Controller::UpdateLastPolygon(const QPointF& new_vertex) {
    if (polygons.empty()) {
        return;
    }
    polygons.back().UpdateLastVertex(new_vertex);
}

std::vector<QPointF>& Controller::getLights() {
    return static_light_sources;
}

void Controller::setLights(const std::vector<QPointF>& p) {
    static_light_sources = p;
}

QPointF& Controller::getLight() {
    return light_source;
}
void Controller::setLight(const QPointF& p) {
    light_source = p;
}

std::vector<Ray> Controller::CastRaysLight() {
    std::vector<Ray> ans;
    float he = 532;
    float wi = 790;
    QPointF BottomRight(wi, he), BottomLeft(0, he), TopRight(wi, 0), TopLeft(0, 0);
    Ray r(light_source, TopLeft);
    ans.push_back(r);
    ans.push_back(r.Rotate(0.0001));
    ans.push_back(r.Rotate(-0.0001));
    r.setEnd(TopRight);
    ans.push_back(r);
    ans.push_back(r.Rotate(0.0001));
    ans.push_back(r.Rotate(-0.0001));
    r.setEnd(BottomLeft);
    ans.push_back(r);
    ans.push_back(r.Rotate(0.0001));
    ans.push_back(r.Rotate(-0.0001));
    r.setEnd(BottomRight);
    ans.push_back(r);
    ans.push_back(r.Rotate(0.0001));
    ans.push_back(r.Rotate(-0.0001));
    for (auto p : polygons) {
        for (int i = 0; i < p.getVertices().size(); ++i) {
            Ray r(light_source, p.getVertices()[i]);
            ans.push_back(r);
            ans.push_back(r.Rotate(0.0001));
            ans.push_back(r.Rotate(-0.0001));
        }
    }
    /*for (int i = 0; i < lights.size(); ++i) {
        Ray r(lights[i], TopLeft);
        ans.push_back(r);
        ans.push_back(r.Rotate(0.0001));
        ans.push_back(r.Rotate(-0.0001));
        r.setEnd(TopRight);
        ans.push_back(r);
        ans.push_back(r.Rotate(0.0001));
        ans.push_back(r.Rotate(-0.0001));
        r.setEnd(BottomLeft);
        ans.push_back(r);
        ans.push_back(r.Rotate(0.0001));
        ans.push_back(r.Rotate(-0.0001));
        r.setEnd(BottomRight);
        ans.push_back(r);
        ans.push_back(r.Rotate(0.0001));
        ans.push_back(r.Rotate(-0.0001));
        for (auto p : polygons) {
            for (int j = 0; j < p.getVertices().size(); ++j) {
                Ray r(lights[i], p.getVertices()[j]);
                ans.push_back(r);
                ans.push_back(r.Rotate(0.0001));
                ans.push_back(r.Rotate(-0.0001));
            }
        }
    }*/
    return ans;
}

std::vector<std::vector<Ray>> Controller::CastRaysL() {
    std::vector<std::vector<Ray>> ans;
    float he = 532;
    float wi = 790;
    QPointF BottomRight(wi, he), BottomLeft(0, he), TopRight(wi, 0), TopLeft(0, 0);
    for (int i = 0; i < lights.size(); ++i) {
        std::vector<Ray> wh;
        Ray r(lights[i], TopLeft);
        wh.push_back(r);
        wh.push_back(r.Rotate(0.0001));
        wh.push_back(r.Rotate(-0.0001));
        r.setEnd(TopRight);
        wh.push_back(r);
        wh.push_back(r.Rotate(0.0001));
        wh.push_back(r.Rotate(-0.0001));
        r.setEnd(BottomLeft);
        wh.push_back(r);
        wh.push_back(r.Rotate(0.0001));
        wh.push_back(r.Rotate(-0.0001));
        r.setEnd(BottomRight);
        wh.push_back(r);
        wh.push_back(r.Rotate(0.0001));
        wh.push_back(r.Rotate(-0.0001));
        for (auto p : polygons) {
            for (int j = 0; j < p.getVertices().size(); ++j) {
                Ray r(lights[i], p.getVertices()[j]);
                wh.push_back(r);
                wh.push_back(r.Rotate(0.0001));
                wh.push_back(r.Rotate(-0.0001));
            }
        }
        ans.push_back(wh);
    }
    return ans;
}
std::vector<std::vector<Ray>> Controller::CastRaysLights() {
    std::vector<std::vector<Ray>> ans;
    for (int i = 0; i < static_light_sources.size(); ++i) {
        std::vector<Ray> a;
        auto l = static_light_sources[i];
        float he = 532;
        float wi = 790;
        QPointF BottomRight(wi, he), BottomLeft(0, he), TopRight(wi, 0), TopLeft(0, 0);
        Ray r(l, TopLeft);
        a.push_back(r);
        a.push_back(r.Rotate(0.0001));
        a.push_back(r.Rotate(-0.0001));
        r.setEnd(TopRight);
        a.push_back(r);
        a.push_back(r.Rotate(0.0001));
        a.push_back(r.Rotate(-0.0001));
        r.setEnd(BottomLeft);
        a.push_back(r);
        a.push_back(r.Rotate(0.0001));
        a.push_back(r.Rotate(-0.0001));
        r.setEnd(BottomRight);
        a.push_back(r);
        a.push_back(r.Rotate(0.0001));
        a.push_back(r.Rotate(-0.0001));
        for (auto p : polygons) {
            for (int i = 0; i < p.getVertices().size(); ++i) {
                Ray r(l, p.getVertices()[i]);
                a.push_back(r);
                a.push_back(r.Rotate(0.0001));
                a.push_back(r.Rotate(-0.0001));
            }
        }
        ans.push_back(a);
    }
    return ans;
}

std::vector<std::vector<Ray>> Controller::CastRaysBoundedLights() {
    std::vector<std::vector<Ray>> ans;
    for (int i = 0; i < bounded_lights.size(); ++i) {
        std::vector<Ray> a;
        auto l = bounded_lights[i].first;
        float len = bounded_lights[i].second;
        float he = 532;
        float wi = 790;
        QPointF BottomRight(wi, he), BottomLeft(0, he), TopRight(wi, 0), TopLeft(0, 0);
        Ray r(l, TopLeft);
        a.push_back(r);
        a.push_back(r.Rotate(0.0001));
        a.push_back(r.Rotate(-0.0001));
        r.setEnd(TopRight);
        a.push_back(r);
        a.push_back(r.Rotate(0.0001));
        a.push_back(r.Rotate(-0.0001));
        r.setEnd(BottomLeft);
        a.push_back(r);
        a.push_back(r.Rotate(0.0001));
        a.push_back(r.Rotate(-0.0001));
        r.setEnd(BottomRight);
        a.push_back(r);
        a.push_back(r.Rotate(0.0001));
        a.push_back(r.Rotate(-0.0001));
        QPointF start(l.x() + len, l.y());
        r.setEnd(start);
        for (int i = 0; i < 180; ++i) {
            a.push_back(r.Rotate(M_PI * i * 2 / 180));
        }
        for (auto p : polygons) {
            for (int i = 0; i < p.getVertices().size(); ++i) {
                Ray r(l, p.getVertices()[i]);
                a.push_back(r);
                a.push_back(r.Rotate(0.0001));
                a.push_back(r.Rotate(-0.0001));
            }
        }
        ans.push_back(a);
    }
    return ans;
}

void Controller::IntersectRays(std::vector<Ray>* rays) {
    for (auto& ray : *rays) {
        std::vector<std::pair<QPointF, float>> v;
        QPointF point;
        float he = 532;
        float wi = 790;
        QPointF BottomRight(wi, he), BottomLeft(0, he), TopRight(wi, 0), TopLeft(0, 0);
        auto p1 = ray.intersection(BottomRight, BottomLeft);
        auto p2 = ray.intersection(BottomLeft, TopLeft);
        auto p3 = ray.intersection(TopLeft, TopRight);
        auto p4 = ray.intersection(TopRight, BottomRight);
        if (p1.has_value()) {
            point = p1.value().first;
        } else if (p2.has_value()) {
            point = p2.value().first;
        } else if (p3.has_value()) {
            point = p3.value().first;
        } else if (p4.has_value()){
            point = p4.value().first;
        } else {
            point = ray.getEnd();
        }
        v.push_back(std::make_pair(point, (point.x() - ray.getBegin().x()) * (point.x() - ray.getBegin().x()) +
                                                 (point.y() - ray.getBegin().y()) * (point.y() - ray.getBegin().y())));
        for (auto p : polygons) {
            auto i = p.IntersectRay(ray);
            if (!i.has_value()) continue;
            auto a = i.value();
            float len = (ray.getBegin().x() - a.x()) * (ray.getBegin().x() - a.x()) + (ray.getBegin().y() - a.y()) * (ray.getBegin().y() - a.y());
            v.push_back(std::make_pair(a, len));
        }
        point = v[0].first;
        float min = v[0].second;
        for (auto& r : v) {
            if (r.second < min) {
                min = r.second;
                point = r.first;
            }
        }
        ray.setEnd(point);
    }
}

/*
void Controller::IntersectRaysB(std::vector<Ray>* rays, float len) {
    for (auto& ray : *rays) {
        std::vector<std::pair<QPointF, float>> v;
        float he = 532;
        float wi = 790;
        QPointF point;
        QPointF BottomRight(wi, he), BottomLeft(0, he), TopRight(wi, 0), TopLeft(0, 0);
        auto p1 = ray.intersection(BottomRight, BottomLeft);
        auto p2 = ray.intersection(BottomLeft, TopLeft);
        auto p3 = ray.intersection(TopLeft, TopRight);
        auto p4 = ray.intersection(TopRight, BottomRight);
        if (p1.has_value()) {
            point = p1.value().first;
        } else if (p2.has_value()) {
            point = p2.value().first;
        } else if (p3.has_value()) {
            point = p3.value().first;
        } else if (p4.has_value()){
            point = p4.value().first;
        } else {
            point = ray.getEnd();
        }
        v.push_back(std::make_pair(point, (point.x() - ray.getBegin().x()) * (point.x() - ray.getBegin().x()) +
                                              (point.y() - ray.getBegin().y()) * (point.y() - ray.getBegin().y())));
        for (auto p : polygons) {
            auto i = p.IntersectRay(ray);
            if (!i.has_value()) continue;
            auto a = i.value();
            float l = (ray.getBegin().x() - a.x()) * (ray.getBegin().x() - a.x()) + (ray.getBegin().y() - a.y()) * (ray.getBegin().y() - a.y());
            v.push_back(std::make_pair(a, l));
        }
        auto poin = v[0].first;
        float min = v[0].second;
        for (auto& r : v) {
            if (r.second < min) {
                min = r.second;
                poin = r.first;
            }
        }
        if (min > len) {
            min = len;
            ray.setEnd(ray.getBegin() + (ray.getEnd() - ray.getBegin()) * len / std::sqrt(min));
        } else {
            ray.setEnd(poin);
        }
    }
}
*/

void Controller::IntersectRaysB(std::vector<Ray>* rays, float maxLength) {
    for (auto& ray : *rays) {
        std::vector<std::pair<QPointF, float>> intersections;
        QPointF point;
        float height = 532;
        float width = 790;

        QPointF BottomRight(width, height), BottomLeft(0, height), TopRight(width, 0), TopLeft(0, 0);

        auto borders = {
            ray.intersection(BottomRight, BottomLeft),
            ray.intersection(BottomLeft, TopLeft),
            ray.intersection(TopLeft, TopRight),
            ray.intersection(TopRight, BottomRight)
        };

        for (const auto& p : borders) {
            if (p.has_value()) {
                point = p.value().first;
                float distSquared = std::pow(point.x() - ray.getBegin().x(), 2) +
                                    std::pow(point.y() - ray.getBegin().y(), 2);
                intersections.emplace_back(point, distSquared);
                break;
            }
        }

        if (intersections.empty()) {
            point = ray.getEnd();
            float distSquared = std::pow(point.x() - ray.getBegin().x(), 2) +
                                std::pow(point.y() - ray.getBegin().y(), 2);
            intersections.emplace_back(point, distSquared);
        }

        for (auto& polygon : polygons) {
            auto result = polygon.IntersectRay(ray);
            if (!result.has_value()) continue;

            QPointF intersectionPoint = result.value();
            float distSquared = std::pow(intersectionPoint.x() - ray.getBegin().x(), 2) +
                                std::pow(intersectionPoint.y() - ray.getBegin().y(), 2);
            intersections.emplace_back(intersectionPoint, distSquared);
        }

        QPointF closestPoint = intersections[0].first;
        float minDistSquared = intersections[0].second;
        for (const auto& pair : intersections) {
            if (pair.second < minDistSquared) {
                minDistSquared = pair.second;
                closestPoint = pair.first;
            }
        }

        float maxDistSquared = maxLength * maxLength;
        if (minDistSquared > maxDistSquared) {
            QPointF direction = ray.getEnd() - ray.getBegin();
            float directionLength = std::sqrt(std::pow(direction.x(), 2) + std::pow(direction.y(), 2));
            if (directionLength >= 5) {
                QPointF newEnd = ray.getBegin() + direction * (maxLength / directionLength);
                ray.setEnd(newEnd);
            } else {
                return;
                ray.setEnd(ray.getBegin());
            }
        } else {
            ray.setEnd(closestPoint);
        }
    }
}

QPointF meanOf(std::vector<QPointF>& points) {
    QPointF sum;
    for (auto& p : points) {
        sum.setX(sum.x() + p.x());
        sum.setY(sum.y() + p.y());
    }
    sum.setX(sum.x() / points.size());
    sum.setY(sum.y() / points.size());
    std::vector<std::pair<QPointF, float>> v;
    for (auto& p : points) {
        auto len = (sum.x() - p.x()) * (sum.x() - p.x()) +
                   (sum.y() - p.y()) * (sum.y() - p.y());
        v.push_back(std::make_pair(p, len));
    }
    std::sort(v.begin(), v.end(), [&] (std::pair<QPointF, float>& a, std::pair<QPointF, float>& b) {
        return a.second < b.second;
    }); // можно и за О(1) но мне лень, так как в целом не очень много точек(ладно шучу на самом деле просто Я Ваня Клохов
    return v[0].first;
}

void delLish(std::vector<QPointF>* points) {
    std::vector<QPointF> poi;
    std::unordered_set<int> set;
    float DELTA = 5;
    set.insert(0);
    for (int i = 1; i < points->size(); ++i) {
        if (set.find(i) == set.end()) {
            bool add = true;
            for (auto& integer : set) {
                QPointF point = points->at(integer);
                float len = (point.x() - points->at(i).x()) * (point.x() - points->at(i).x()) +
                            (point.y() - points->at(i).y()) * (point.y() - points->at(i).y());
                if (len < DELTA * DELTA) {
                    add = false;
                    break;
                }
            }
            if (add) {
                set.insert(i);
            }
        }
    }
    std::vector<QPointF> ans;
    for (const auto& in : set) {
        ans.push_back(points->at(in));
    }
    qDebug() << "\n";
    *points = ans;
}

void Controller::RemoveAdjacentRays(std::vector<Ray>* rays) {
    std::vector<QPointF> ends;
    std::vector<Ray> ans;
    for (auto& ray : *rays) {
        ends.push_back(ray.getEnd());
    }
    delLish(&ends);
    Ray r;
    r.setBegin(light_source);
    for (auto& point : ends) {
        r.setEnd(point);
        ans.push_back(r);
    }
    *rays = ans;
}

/*void Controller::RemoveAdjacentRaysStatic(std::vector<Ray>* rays) {
    for (auto& light : static_light_sources) {
        std::vector<QPointF> ends;
        std::vector<Ray> ans;
        delLish(&ends);
        Ray r;
        r.setBegin(light_source);
        for (auto& point : ends) {
            r.setEnd(point);
            ans.push_back(r);
        }
        *rays = ans;
    }
}*/

Polygon Controller::CreateLightArea(std::vector<Ray>* rays) {
    std::vector<std::pair<Ray, float>> v;
    for (auto& ray : *rays) {
        v.push_back(std::make_pair(ray, ray.getAngle()));
    }
    std::sort(v.begin(), v.end(), [&] (std::pair<Ray, float>& a, std::pair<Ray, float>& b) {
        return a.second < b.second;
    });
    Polygon p;
    for (int i = 0; i < v.size(); ++i) {
        p.AddVertex(v[i].first.getEnd());
    }
    return p;
}

std::vector<QPointF>& Controller::getL() {
    return lights;
}
