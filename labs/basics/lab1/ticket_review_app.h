#ifndef TICKET_REVIEW_APP_H_
#define TICKET_REVIEW_APP_H_

#include <QComboBox>
#include <QGroupBox>
#include <QLabel>
#include <QLineEdit>
#include <QListWidget>
#include <QProgressBar>
#include <QPushButton>
#include <QSpinBox>
#include <QVector>
#include <QWidget>

#include "ticket_manager.h"

class TicketReviewApp : public QWidget {
  Q_OBJECT

 public:
  explicit TicketReviewApp(QWidget* parent = nullptr);

 private:
  void BuildUi();
  void ConnectSignals();

  void ResetTickets(int ticket_count);
  void RebuildView();
  void RefreshTicketItem(int index);
  void RefreshQuestionView();
  void RefreshProgressBars();
  void RefreshButtons();
  void RefreshAll();

  void ShowTicket(int index, bool add_to_history);
  void AddToHistory(int index);
  void SelectRandomNextTicket();
  void ShowPreviousTicket();
  void CommitNameEdit();
  void SetCurrentTicketStatus(TicketManager::Status status);

  static QString StatusToText(TicketManager::Status status);
  static TicketManager::Status IndexToStatus(int index);
  static int StatusToIndex(TicketManager::Status status);

  TicketManager ticket_manager_;
  int current_ticket_index_ = -1;
  QVector<int> selection_history_;
  int history_position_ = -1;

  QSpinBox* count_spin_box_ = nullptr;
  QListWidget* tickets_view_ = nullptr;
  QGroupBox* question_group_ = nullptr;
  QLabel* number_value_label_ = nullptr;
  QLabel* name_value_label_ = nullptr;
  QLineEdit* name_edit_ = nullptr;
  QComboBox* status_combo_box_ = nullptr;
  QPushButton* next_question_button_ = nullptr;
  QPushButton* previous_question_button_ = nullptr;
  QProgressBar* total_progress_bar_ = nullptr;
  QProgressBar* green_progress_bar_ = nullptr;
};

#endif  // TICKET_REVIEW_APP_H_
