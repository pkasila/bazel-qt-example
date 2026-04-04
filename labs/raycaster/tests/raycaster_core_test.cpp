#include "labs/raycaster/core/controller.h"
#include "labs/raycaster/core/geometry_utils.h"
#include "labs/raycaster/core/polygon.h"

#include <QPointF>
#include <cmath>
#include <cstdlib>
#include <iostream>
#include <string_view>
#include <vector>

static bool Check(bool condition, std::string_view message) {
    if (!condition) {
        std::cerr << "FAILED: " << message << '\n';
        return false;
    }
    return true;
}

static bool CheckAlmostEqual(double lhs, double rhs, double epsilon, std::string_view message) {
    return Check(std::abs(lhs - rhs) <= epsilon, message);
}

static bool TestGeometryHelpers() {
    using raycaster::geom::HasSignificantArea;
    using raycaster::geom::IntersectRaySegment;
    using raycaster::geom::IsSimplePolygon;
    using raycaster::geom::PolygonsIntersect;

    const auto intersection = IntersectRaySegment(
        QPointF(0.0, 0.0), QPointF(10.0, 0.0), QPointF(5.0, -2.0), QPointF(5.0, 2.0));

    if (!Check(intersection.has_value(), "ray should intersect vertical segment")) {
        return false;
    }
    const QPointF intersection_point = intersection.value_or(QPointF());
    if (!CheckAlmostEqual(intersection_point.x(), 5.0, 1e-6, "intersection x should be 5")) {
        return false;
    }
    if (!CheckAlmostEqual(intersection_point.y(), 0.0, 1e-6, "intersection y should be 0")) {
        return false;
    }

    const std::vector<QPointF> triangle = {
      QPointF(0.0, 0.0),
      QPointF(4.0, 0.0),
      QPointF(0.0, 3.0),
    };
    if (!Check(HasSignificantArea(triangle), "triangle should have area")) {
        return false;
    }
    if (!Check(IsSimplePolygon(triangle), "triangle should be simple")) {
        return false;
    }

    const std::vector<QPointF> collinear = {
      QPointF(0.0, 0.0),
      QPointF(1.0, 0.0),
      QPointF(2.0, 0.0),
    };
    if (!Check(!HasSignificantArea(collinear), "collinear polygon should be degenerate")) {
        return false;
    }
    if (!Check(!IsSimplePolygon(collinear), "degenerate polygon should be rejected")) {
        return false;
    }

    const std::vector<QPointF> bow = {
      QPointF(0.0, 0.0),
      QPointF(3.0, 3.0),
      QPointF(0.0, 3.0),
      QPointF(3.0, 0.0),
    };
    if (!Check(!IsSimplePolygon(bow), "self-intersecting polygon should be rejected")) {
        return false;
    }

    const std::vector<QPointF> square_a = {
      QPointF(0.0, 0.0),
      QPointF(4.0, 0.0),
      QPointF(4.0, 4.0),
      QPointF(0.0, 4.0),
    };
    const std::vector<QPointF> square_b = {
      QPointF(3.0, 3.0),
      QPointF(6.0, 3.0),
      QPointF(6.0, 6.0),
      QPointF(3.0, 6.0),
    };
    return Check(PolygonsIntersect(square_a, square_b), "overlapping polygons should intersect");
}

static bool TestControllerLogic() {
    raycaster::Controller controller;
    controller.SetSceneBounds(0.0, 0.0, 100.0, 100.0);

    if (!Check(controller.CastRays().size() == 12U, "boundary polygon should contribute 12 rays")) {
        return false;
    }
    if (!Check(controller.AreSoftShadowsEnabled(), "soft shadows should be enabled by default")) {
        return false;
    }
    if (!Check(
            controller.GetDynamicLightSources().size() > 1U,
            "soft shadows should use several lights")) {
        return false;
    }

    controller.SetSoftShadowsEnabled(false);
    if (!Check(
            controller.GetDynamicLightSources().size() == 1U,
            "single-light mode should keep one active source")) {
        return false;
    }
    controller.SetSoftShadowsEnabled(true);

    controller.AddPolygon(
        raycaster::Polygon({
          QPointF(10.0, 10.0),
          QPointF(40.0, 10.0),
          QPointF(40.0, 40.0),
          QPointF(10.0, 40.0),
        }));
    if (!Check(controller.FinalizeLastPolygon(), "valid polygon should be finalized")) {
        return false;
    }

    const QPointF current_light = controller.GetLightSource();
    controller.AddPolygon(
        raycaster::Polygon({
          QPointF(current_light.x() - 6.0, current_light.y() - 6.0),
          QPointF(current_light.x() + 6.0, current_light.y() - 6.0),
          QPointF(current_light.x() + 6.0, current_light.y() + 6.0),
          QPointF(current_light.x() - 6.0, current_light.y() + 6.0),
        }));
    if (!Check(
            !controller.FinalizeLastPolygon(),
            "polygon covering current light should be rejected")) {
        return false;
    }

    controller.SetLightSource(QPointF(20.0, 20.0));
    if (!Check(
            controller.GetLightSource() != QPointF(20.0, 20.0),
            "light should not move inside obstacle")) {
        return false;
    }

    controller.AddStaticLight(QPointF(20.0, 20.0));
    if (!Check(
            controller.GetStaticLights().empty(),
            "static light inside obstacle should be rejected")) {
        return false;
    }

    controller.AddPolygon(
        raycaster::Polygon({
          QPointF(30.0, 30.0),
          QPointF(60.0, 30.0),
          QPointF(60.0, 60.0),
          QPointF(30.0, 60.0),
        }));
    if (!Check(!controller.FinalizeLastPolygon(), "intersecting polygon should be rejected")) {
        return false;
    }

    controller.AddPolygon(
        raycaster::Polygon({
          QPointF(70.0, 10.0),
          QPointF(80.0, 10.0),
          QPointF(90.0, 10.0),
        }));
    if (!Check(!controller.FinalizeLastPolygon(), "degenerate polygon should be rejected")) {
        return false;
    }

    controller.SetSceneBounds(0.0, 0.0, 60.0, 60.0);
    const QPointF light = controller.GetLightSource();
    if (!Check(
            light.x() >= 0.0 && light.x() <= 60.0,
            "light should stay inside resized scene horizontally")) {
        return false;
    }
    if (!Check(
            light.y() >= 0.0 && light.y() <= 60.0,
            "light should stay inside resized scene vertically")) {
        return false;
    }

    const auto light_area = controller.CreateLightArea();
    return Check(
        light_area.GetVertices().size() >= 3U, "light area should contain at least 3 vertices");
}

int main() {
    try {
        if (!TestGeometryHelpers()) {
            return EXIT_FAILURE;
        }
        if (!TestControllerLogic()) {
            return EXIT_FAILURE;
        }
        return EXIT_SUCCESS;
    } catch (const std::exception& error) {
        std::cerr << "FAILED: unexpected exception: " << error.what() << '\n';
        return EXIT_FAILURE;
    } catch (...) {
        std::cerr << "FAILED: unknown exception\n";
        return EXIT_FAILURE;
    }
}
