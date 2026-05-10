#pragma once

#include "polygon.h"

#include <CGAL/Boolean_set_operations_2.h>
#include <CGAL/Exact_predicates_exact_constructions_kernel.h>
#include <CGAL/Polygon_2.h>
#include <CGAL/enum.h>
#include <QPoint>
#include <vector>

class Validator {
   public:
    using Kernel = CGAL::Exact_predicates_exact_constructions_kernel;
    using Point = CGAL::Point_2<Kernel>;
    using CgalPolygon = CGAL::Polygon_2<Kernel>;

    static bool IsValidPolygonRobust(const std::vector<QPoint>& pts) {
        if (pts.size() <= 3) {
            return true;
        }

        CgalPolygon poly;
        for (const auto& p : pts) {
            poly.push_back(Point(p.x(), p.y()));
        }

        if (!poly.is_simple()) {
            return false;
        }

        if (poly.area() == 0) {
            return false;
        }

        return true;
    }

    static bool DoPolygonsIntersect(const Polygon& poly1, const Polygon& poly2) {
        CgalPolygon cgal_poly2;
        for (const auto& p : poly2.GetAllVertices()) {
            cgal_poly2.push_back(Point(p.x(), p.y()));
        }

        if (cgal_poly2.is_empty()) {
            return false;
        }

        if (cgal_poly2.is_clockwise_oriented()) {
            cgal_poly2.reverse_orientation();
        }

        const auto& v1 = poly1.GetAllVertices();
        if (v1.size() < 2) {
            return false;
        }

        for (size_t i = 0; i < v1.size(); ++i) {
            Point p1(v1[i].x(), v1[i].y());
            Point p2(v1[(i + 1) % v1.size()].x(), v1[(i + 1) % v1.size()].y());

            if (p1 == p2) {
                continue;
            }

            CGAL::Segment_2<Kernel> s(p1, p2);

            for (auto edge = cgal_poly2.edges_begin(); edge != cgal_poly2.edges_end(); ++edge) {
                if (CGAL::do_intersect(s, *edge)) {
                    return true;
                }
            }

            if (cgal_poly2.bounded_side(p1) == CGAL::ON_BOUNDED_SIDE) {
                return true;
            }
        }

        return false;
    }

    static bool IsPointOutside(const Polygon& poly, const QPoint& point) {
        CgalPolygon cgal_poly;
        for (const auto& p : poly.GetAllVertices()) {
            cgal_poly.push_back(Point(p.x(), p.y()));
        }

        if (cgal_poly.is_empty()) {
            return true;
        }

        if (!cgal_poly.is_simple()) {
            return true;
        }
        return cgal_poly.bounded_side(Point(point.x(), point.y())) == CGAL::ON_UNBOUNDED_SIDE;
    }

    static bool IsLightPositionValid(const std::vector<Polygon>& polygons, const QPoint& pos) {
        if (polygons.empty()) {
            return true;
        }

        if (IsPointOutside(polygons[0], pos)) {
            return false;
        }
        for (size_t i = 1; i < polygons.size(); ++i) {
            if (!IsPointOutside(polygons[i], pos)) {
                return false;
            }
        }
        return true;
    }

    static bool CanAddPolygonVertex(
        const std::vector<Polygon>& polygons, const Polygon& current_poly) {
        if (!IsValidPolygonRobust(current_poly.GetAllVertices())) {
            return false;
        }

        if (polygons.size() <= 2) {
            return true;
        }

        for (size_t i = 1; i < polygons.size() - 1; ++i) {
            if (DoPolygonsIntersect(current_poly, polygons[i])) {
                return false;
            }
        }
        return true;
    }
};