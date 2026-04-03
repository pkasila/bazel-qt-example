#include "lightsource.h"

LightSource::LightSource() {
}

LightSource::LightSource(QPointF position, QColor color, int power)
    : position_(position), color_(color), power_(power){
}

void LightSource::SetPosition(QPointF new_pos) {
    position_ = new_pos;
}

void LightSource::SetLightColor(QColor new_color) {
    color_ = new_color;
}

void LightSource::SetPower(int power) {
    power_ = std::max(0, std::min(100, power));
    color_.setAlpha(255.0*power_/100);
}

void LightSource::IncreasePower() {
    power_ = std::min(power_+10, 100);
}

void LightSource::DecreasePower() {
    power_ = std::max(power_-10, 0);
}

QPointF LightSource::Position() const {
    return position_;
}

QColor LightSource::LightColor() const {
    return color_;
}

int LightSource::Power() const {
    return power_;
}

double LightSource::LightAreaRadius() {
    return light_area_radius_;
}

int LightSource::LightsNumber() {
    return lights_number_;
}

double LightSource::LampRadius() {
    return lamp_radius_;
}


