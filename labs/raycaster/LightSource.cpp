//
// Created by Zazerg on 4/2/25.
//

#include "LightSource.h"

LightSource::LightSource(QColor c1, QColor c2, int r, QPointF p, QSize w) : primaryColor_(c1), halfShadeColor_(c2), radius_(r), position_(p), updated_(false), image_(w, QImage::Format_RGB32) {
}

void LightSource::moveTo(QPointF to) {
    position_ = to;
    updated_ = false;
}

const QColor& LightSource::getColor1() const{
    return primaryColor_;
}

const QColor& LightSource::getColor2() const {
    return halfShadeColor_;
}

int LightSource::getRadius() const {
    return radius_;
}

const QPointF& LightSource::getPosition() const {
    return position_;
}

bool LightSource::updated() const {
    return updated_;
}

void LightSource::setUpdated(bool n) {
    updated_ = n;
}

QImage& LightSource::getImage() {
    return image_;
}

void LightSource::setRadius(int b) {
    radius_ = b;
}
