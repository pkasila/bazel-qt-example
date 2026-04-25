#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QComboBox>
#include <QMainWindow>

#include "raycaster_widget.h"

class MainWindow : public QMainWindow {
  Q_OBJECT

 public:
  explicit MainWindow(QWidget* parent = nullptr);

 private slots:
  void OnModeChanged(int index);

 private:
  void SetupUi();

  RaycasterWidget* raycaster_;
  QComboBox* mode_combo_;
};

#endif
