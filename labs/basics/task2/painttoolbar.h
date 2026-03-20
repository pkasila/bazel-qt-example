#ifndef PAINTTOOLBAR_H
#define PAINTTOOLBAR_H

#include "canvas.h"

#include <QDebug>
#include <QWidget>
#include <QRadioButton>
#include <QLabel>
#include <QButtonGroup>
#include <QPainter>
#include <QMainWindow>
#include <QComboBox>

class PaintToolBar;

class ColorWidget : public QWidget {
    Q_OBJECT
public:
    ColorWidget(QWidget *parent = nullptr);
    void setColor(QColor newColor);
    void randFill();
    ~ColorWidget() = default;
    friend class PaintToolBar;
protected:
    void mousePressEvent(QMouseEvent *event) override ;
    void paintEvent(QPaintEvent *event) override {
        if (!random) {
            QPainter painter(this);
            painter.fillRect(rect(), color);
        } else {
            randFill();
        }
    }
private:
    QColor color;
    bool random = false;
    PaintToolBar *toolBar;
};



class PaintToolBar : public QWidget
{
    Q_OBJECT
public:
    PaintToolBar(QWidget *parent = nullptr, Canvas *canvas = nullptr);

    void setColor();
    void setPenWidth(int newPenWidthIndex = 1);

    ~PaintToolBar();

protected:
    void paintEvent(QPaintEvent *event) override;
private:
    QButtonGroup *buttonGroupToolType;
    QRadioButton *radioButtonPencil;
    QRadioButton *radioButtonBrush;
    QLabel *labelToolType;
    Canvas *canvas;
    ColorWidget *currentColor;
    ColorWidget *randomColor;
    QColor color;
    int penWidth;
    QComboBox *comboBoxPenWidth;

};



#endif // PAINTTOOLBAR_H
