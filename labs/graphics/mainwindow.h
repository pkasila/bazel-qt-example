#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "controller.h"

#include <QComboBox>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QMainWindow>
#include <QMouseEvent>
#include <QResizeEvent>

class MainWindow : public QMainWindow {
    Q_OBJECT

   public:
    MainWindow(QWidget* parent = nullptr);
    ~MainWindow();

   protected:
    void resizeEvent(QResizeEvent* event) override;
    bool eventFilter(QObject* obj, QEvent* event) override;

   private slots:
    void onModeChanged(int index);
    void onSceneClicked(const QPointF& point);

   private:
    void setupUI();
    void updateScene();
    void drawPolygons();
    void drawLightSource();
    void updateBoundingPolygon();
    void drawLightArea();
    QGraphicsScene* scene_;
    QGraphicsView* view_;
    QComboBox* mode_;
    Controller controller_;
    bool is_drawing_poly_ = false;
    QPointF cur_mouse_pos_;
};

#endif  // MAINWINDOW_H
