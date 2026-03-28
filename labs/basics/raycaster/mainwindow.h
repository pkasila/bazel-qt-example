#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QComboBox>
#include "customgraphicsview.h"
#include "controller.h"

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    CustomGraphicsView *graphicsView_;
    QComboBox *modeComboBox_;
    Controller controller_;

    std::vector<QPointF> currentPolygon_;
    QPointF lightPos_;
    bool isLightMode_ = true;
    bool isDrawingPolygon_ = false;
    bool isFirstClick_ = true;

    void updateView();
    void handlePolygonDrawing(QPointF scenePos, Qt::MouseButton button);

private slots:
    void onModeChanged(int index);
};

#endif // MAINWINDOW_H
