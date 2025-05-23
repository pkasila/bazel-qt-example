#include "polygon.h"

#include <QDebug>

Polygon::Polygon(const std::vector<QPointF>& vertices) {
    vertices_ = vertices;
}

void Polygon::AddVertex(const QPointF& vertex) {
    vertices_.push_back(vertex);
}

void Polygon::UpdateLastVertex(const QPointF& new_vertex) {
    vertices_[vertices_.size() - 1] = new_vertex;
}

double Distanse(QPointF point1, QPointF point2) {
    return (point1.x() - point2.x()) * (point1.x() - point2.x()) + (point1.y() - point2.y()) * (point1.y() - point2.y());
}

double Polygon::IntersectRay(const Ray& ray) {
    double t_min = 100000000;
    for (int i = 0; i < vertices_.size(); ++i) {
        QPointF a = vertices_[i];
        QPointF b = vertices_[(i + 1) % vertices_.size()];

        double xd = b.x() - a.x();
        double yd = b.y() - a.y();

        // if (abs(xd / ray.GetXDir() - yd / ray.GetYDir()) < 0.00001) {
        //     continue;
        // }





        QPointF p = ray.GetBegin();
        QPointF r(ray.GetXDir(), ray.GetYDir());
        QPointF q(a);
        QPointF s(b - a);

        double rxs = r.x()*s.y() - r.y()*s.x();
        double q_p_x_r = (q - p).x()*r.y() - (q - p).y()*r.x();

        if (fabs(rxs) < 1e-8) continue;





        double t2 = (ray.GetXDir() * (static_cast<double>(a.y()) - ray.GetBeginY()) + ray.GetYDir() * (ray.GetBeginX() - static_cast<double>(a.x()))) / (xd * ray.GetYDir() - yd * ray.GetXDir());
        double t1 = (static_cast<double>(a.x()) + xd * t2 - ray.GetBeginX()) / ray.GetXDir();

        if (t1 <= 0 || t2 < 0 || t2 > 1) {
            continue;
        }

        if (t_min > t1) {
            t_min = t1;
        }

        // qDebug() << t1 << " " << t2 << " " << t_min << "\n";
    }

    return t_min;
}

void Polygon::FinishPolygon() {
    vertices_.push_back(vertices_[0]);
}
