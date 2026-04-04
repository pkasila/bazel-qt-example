#ifndef TICKET_REVIEW_APP_H
#define TICKET_REVIEW_APP_H

#include <QMainWindow>
#include <QSpinBox>
#include <QListWidget>
#include <QLabel>
#include <QLineEdit>
#include <QComboBox>
#include <QPushButton>
#include <QProgressBar>
#include "ticket_manager.h"

class TicketReviewApp : public QMainWindow {
  Q_OBJECT

 public:
  explicit TicketReviewApp(QWidget* parent = nullptr);

 private:
  void SetupUI();
  void ConnectSignals();
  void RefreshTicketList();
  void UpdateTicketDetails(int ticket_id);
  void UpdateProgressBars();

  void OnTicketCountChanged(int new_count);
  void OnListItemClicked(QListWidgetItem* item);
  void OnListItemDoubleClicked(QListWidgetItem* item);
  void OnStatusChanged(int new_status_index);
  void OnNameEditReturnPressed();
  void OnNextButtonClicked();
  void OnPreviousButtonClicked();

  TicketManager manager_;
  int current_ticket_id_;

  // UI Elements
  QSpinBox* count_spinbox_;
  QListWidget* ticket_list_;
  QLabel* ticket_number_label_;
  QLabel* ticket_name_label_;
  QLineEdit* ticket_name_edit_;
  QComboBox* ticket_status_combo_;
  QPushButton* next_button_;
  QPushButton* previous_button_;
  QProgressBar* total_progress_bar_;
  QProgressBar* mastered_progress_bar_;
  QLabel* total_progress_label_;
  QLabel* mastered_progress_label_;
};

#endif  // TICKET_REVIEW_APP_H
