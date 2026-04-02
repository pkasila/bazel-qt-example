#ifndef LIGHTSOURCE_H
#define LIGHTSOURCE_H

#include <QPointF>
#include <QColor>

class LightSource {
public:
    LightSource();
    LightSource(QPointF position, QColor color = default_color_, int power = 50);
    void SetPosition(QPointF new_pos);
    void SetLightColor(QColor new_color);
    void SetPower(int power);
    void IncreasePower();
    void DecreasePower();
    [[nodiscard]]QPointF Position() const;
    [[nodiscard]]QColor LightColor() const;
    [[nodiscard]]int Power() const;
    [[nodiscard]]static double LightAreaRadius();
    static int LightsNumber() ;
    static double LampRadius() ;

private:
    const static int lights_number_ = 8;
    constexpr const static double lamp_radius_ = 10;
    constexpr const static double light_area_radius_ = 1000;
    int power_ = 40; // 0 <= % <= 100, step = 10;
    constexpr const static QColor default_color_ = QColor(/*135, 206, 250*/60, 165, 240, 40);
    QColor color_ = default_color_;
    QPointF position_;
};

#endif  // LIGHTSOURCE_H
