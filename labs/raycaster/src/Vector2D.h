#ifndef VECTOR2D_H
#define VECTOR2D_H
//Qt libraries
#include <QPointF>

class Vector2D {
public:
    //Constructor
    Vector2D(double x_cord, double y_cord);
    Vector2D();
    //Getters
    double getXCord() const;
    double getYCord() const;

private:
    double x_cord;
    double y_cord;
};

#endif // VECTOR2D_H
