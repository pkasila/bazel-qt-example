#ifndef MAINWINDOW
#define MAINWINDOW

#include "canvas/canvas.h"

#include <QComboBox>
#include <QLabel>
#include <QMainWindow>
#include <QPushButton>
#include <QTimer>

class MainWindow : public QMainWindow {  // NOLINT
    Q_OBJECT
    // todo: try Q_PROPERTY
   public:
    MainWindow();
    ~MainWindow() override = default;
   private slots:
    void OnModeChanged();
    void OnCanvasReset();
    void OnFinishPolygon();
    void UpdateFPS();

   private:  // NOLINT
    void SetupIU();

    QComboBox* mode_selector_;
    QLabel* fps_label_;
    QTimer* fps_timer_;
    Canvas* canvas_;
};

#endif
