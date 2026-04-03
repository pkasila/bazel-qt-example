#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "canvas.h"
#include "overcanvas.h"
#include "painttoolbar.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    //Ui::MainWindow *ui;
    //Canvas *canvas;
    OverCanvas *overCanvas;
    PaintToolBar *toolBar;
};
#endif // MAINWINDOW_H
