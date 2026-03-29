//
// Created by Zazerg on 4/2/25.
//

#ifndef LIGHTSOURCE_H
#define LIGHTSOURCE_H
#include <qcolor.h>
#include <qimage.h>
#include <qpoint.h>

// #include "RenderArea.h"


class LightSource {
public:
    LightSource(QColor c1, QColor c2, int r, QPointF p, QSize w);

    void moveTo(QPointF);
    const QColor& getColor1() const;
    const QColor& getColor2() const;
    int getRadius() const;
    const QPointF& getPosition() const;
    bool updated() const;
    void setUpdated(bool n);
    QImage& getImage();

    void setRadius(int b);

private:
    QColor primaryColor_, halfShadeColor_;
    int radius_;
    QPointF position_;
    QImage image_;
    bool updated_;
};



#endif //LIGHTSOURCE_H
