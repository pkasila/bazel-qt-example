#ifndef MAINWINDOW_H_
#define MAINWINDOW_H_

#include "SlotMachineWidget.h"

#include <QCheckBox>
#include <QLabel>
#include <QProgressBar>
#include <QPushButton>
#include <QSpinBox>
#include <QString>
#include <QTimer>
#include <QWidget>

class MainWindow
    : public QWidget {  // NOLINT(cppcoreguidelines-special-member-functions,hicpp-special-member-functions)
    Q_OBJECT

   public:
    explicit MainWindow(QWidget* parent = nullptr);

   private slots:
    void OnSpinClicked();
    void OnSpinningStopped();
    void OnBetChanged(int bet);
    void OnAutoPlayToggled(bool checked);
    void OnAutoPlayTimerTick();
    void OnRestartClicked();

   private:  // NOLINT(readability-redundant-access-specifiers)
    void SetupUi();
    void UpdateControls();
    void ProcessResults();

    SlotMachineWidget* slot_machine_{nullptr};
    QPushButton* spin_button_{nullptr};
    QSpinBox* bet_spin_box_{nullptr};
    QLabel* info_label_{nullptr};
    QCheckBox* auto_play_check_{nullptr};
    QProgressBar* balance_bar_{nullptr};
    QTimer* auto_play_timer_{nullptr};
    QPushButton* restart_button_{nullptr};

    int balance_{1000};
    int current_bet_{0};
};

#endif  // MAINWINDOW_H_
