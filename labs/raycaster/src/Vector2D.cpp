#include "Vector2D.h"
//Constructor
Vector2D::Vector2D(double x_cord, double y_cord) : x_cord(x_cord),
    y_cord(y_cord) {}

Vector2D::Vector2D() : Vector2D(1.0, 0.0) {}

//Getters
double Vector2D::getXCord() const {
    return this->x_cord;
}

double Vector2D::getYCord() const {
    return this->y_cord;
}
