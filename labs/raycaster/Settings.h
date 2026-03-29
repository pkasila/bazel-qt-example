//
// Created by Zazerg on 4/3/25.
//

#ifndef SETTINGS_H
#define SETTINGS_H
#include <qcolor.h>
#include <qpixmap.h>

enum class Mode {
    DRAW, LIGHT, STATIC_LIGHT, MOVE_SCREEN
};

class Settings {
public:
    Mode mode = Mode::DRAW;
    QColor color1 = QColor(120,60,60), color2 = QColor(60,30,30);
    int radius = 400;
    bool enable_aa_ =
#ifdef USE_OPENGL
        true
    #else
            false
    #endif
        ;
    QPixmap bg = QPixmap(":/bg");
    QSize bgSize = bg.size();
};



#endif //SETTINGS_H
