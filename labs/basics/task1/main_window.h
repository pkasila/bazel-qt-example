#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "ticket.h"

#include <QMainWindow>

QT_BEGIN_NAMESPACE
class QProgressBar;
class QSpinBox;
class QListWidget;
class QGroupBox;
class QLabel;
class QPushButton;
class QLineEdit;
class QComboBox;
class QListWidgetItem;
class QTimer;
QT_END_NAMESPACE

class MainWindow : public QMainWindow {  // NOLINT
    Q_OBJECT

   public:
    MainWindow();
    ~MainWindow() override;
   private slots:
    void OnCountChanged(int count);
    void OnItemSelectionChanged();
    void OnNameEditChanged();
    void OnStatusChanged(int index);
    void OnItemDoubleClicked(QListWidgetItem* item);
    void OnNextClicked();
    void OnPreviousClicked();
    void OnHintEditChanged();

   private:  // NOLINT
    void SetupUI();
    void UpdateTicketList();
    void UpdateQuestionView();
    void UpdateProgress();

    QGroupBox* question_view_;
    QLabel* index_label_;
    QLabel* name_label_;
    QLabel* hint_label_;
    QLineEdit* name_edit_;
    QLineEdit* hint_edit_;
    QComboBox* status_combo_box_;
    QSpinBox* count_spin_box_;
    QListWidget* ticket_list_;
    QPushButton* previous_button_;
    QPushButton* next_button_;
    QProgressBar* total_progress_bar_;
    QProgressBar* green_progress_bar_;
    QVector<Ticket> tickets_;
    QVector<int> history_;
    int current_index_ = -1;
    int completed_ = 0;
    int partial_ = 0;
    bool is_programmatic_selection_ = false;
};

#endif