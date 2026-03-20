#ifndef SPACEAPP_H
#define SPACEAPP_H

#include <QWidget>
#include <QSlider>
#include <QLCDNumber>
#include <QCheckBox>
#include <QPushButton>
#include <QRadioButton>
#include <QDial>
#include <QLabel>

class SpaceApp : public QWidget {
    Q_OBJECT
public:
    SpaceApp(QWidget *parent = nullptr);
};

#endif