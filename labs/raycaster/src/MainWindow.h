#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include <QWidget>
#include <QMainWindow>
#include <QFrame>
#include <QToolButton>
#include <QIcon>
#include <QString>
#include <QHBoxLayout>
#include <QVBoxLayout>
//
#include "DrawingArea.h"
#include "ProjectConstants.h"
#include "Mode.h"

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);

private:
    DrawingArea *drawingArea;
    QToolButton *lightButton;
    QToolButton *polygonButton;

private:
    void setupToolbar();
    void updateModeButtons(Mode mode);
};
#endif // MAINWINDOW_H
