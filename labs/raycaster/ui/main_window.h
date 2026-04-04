#pragma once

#include "labs/raycaster/ui/render_widget.h"

#include <QMainWindow>

class QCheckBox;
class QComboBox;
class QLabel;
class QPushButton;

namespace raycaster {

class MainWindow : public QMainWindow {
   public:
    MainWindow();

   private:
    static InteractionMode ModeFromIndex(int index);
    void UpdateHintText();

    RenderWidget* render_widget_;
    QCheckBox* soft_shadows_check_box_;
    QCheckBox* cat_check_box_;
    QComboBox* mode_combo_box_;
    QLabel* hint_label_;
};

}  // namespace raycaster
