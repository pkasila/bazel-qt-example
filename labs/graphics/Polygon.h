//
// Created by Kirill Smychok on 6.04.25.
//

#ifndef POLYGON_H
#define POLYGON_H
#include <vector>
#include <QtCore/qmetatype.h>

#include "Ray.h"


class Polygon {

public:
    Polygon(const std::vector<QPointF>& vertices);
    Polygon(): vertices_(std::vector<QPointF>()) {}

    std::vector<QPointF> GetVertices();

    void AddVertex(const QPointF& vertex);
    void UpdateLastVertex(const QPointF& new_vertex);
    std::optional<QPointF> IntersectRay(const Ray& ray);
    bool IsEmpty();
    void Clear();

    static double GetDistance(const QPointF &p1, const QPointF &p2);

    static bool IsVertexIncluded(QPointF p1, QPointF p2, QPointF possible_included);

    bool IsIntersected(QPointF p1, QPointF p2, QPointF p3, QPointF p4, QPointF &intersection);


private:
    std::vector<QPointF> vertices_;
    QPointF last_vertex_ = QPointF(-10, -10);
};



#endif //POLYGON_H
