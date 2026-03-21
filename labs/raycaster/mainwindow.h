#pragma once
#include "canvas.h"

#include <QCheckBox>
#include <QLabel>
#include <QMainWindow>
#include <QPushButton>
#include <QRadioButton>
#include <QSlider>

class MainWindow : public QMainWindow {
    Q_OBJECT
   public:
    MainWindow(QWidget* parent = nullptr);

   protected:
    void keyPressEvent(QKeyEvent* event) override;

   private:
    Canvas* canvas;
    QRadioButton *rbLight, *rbPoly, *rbStatic;
    QCheckBox *cbToggle, *cbPhoto;
    QPushButton* btnHelp;
    QSlider* sliderDetail;
    QLabel* lblDetail;

    void showHelp();
};