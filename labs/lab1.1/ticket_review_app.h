#ifndef TICKET_REVIEW_APP_H_
#define TICKET_REVIEW_APP_H_

#include <QBoxLayout>
#include <QComboBox>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QVBoxLayout>
#include <QListWidget>
#include <QProgressBar>
#include <QPushButton>
#include <QResizeEvent>
#include <QSpinBox>
#include <QWidget>

#include <vector>

#include "ticket_manager.h"

class TicketReviewApp : public QWidget {
 public:
  explicit TicketReviewApp(QWidget* parent = nullptr);

 protected:
  void resizeEvent(QResizeEvent* event) override;

 private:
  void BuildInterface();
  void ConnectSignals();
  void ApplyStyle();
  void RebuildView();
  void UpdateViewItem(int index);
  void UpdateAllViewItems();
  void UpdateQuestionView();
  void UpdateProgressBars();
  void UpdateButtons();
  void UpdateViewGrid();
  void SelectTicket(int index, bool record_history);
  void ApplyTicketCount(int count);
  void SubmitEditedName();
  void ChangeCurrentStatus(TicketStatus status);
  void OpenRandomTicket();
  void OpenPreviousTicket();
  int StatusComboIndex(TicketStatus status) const;

  TicketManager manager_;
  int current_index_;
  std::vector<int> history_;

  QVBoxLayout* root_layout_;
  QGroupBox* setup_group_;
  QHBoxLayout* setup_layout_;
  QLabel* count_label_;
  QSpinBox* count_spin_box_;
  QBoxLayout* content_layout_;
  QGroupBox* view_group_;
  QVBoxLayout* view_layout_;
  QListWidget* view_;
  QWidget* right_panel_;
  QVBoxLayout* right_layout_;
  QGroupBox* question_group_;
  QVBoxLayout* question_layout_;
  QLabel* number_title_label_;
  QLabel* number_value_label_;
  QLabel* name_title_label_;
  QLabel* name_value_label_;
  QLabel* edit_title_label_;
  QLineEdit* name_edit_;
  QLabel* status_title_label_;
  QComboBox* status_combo_box_;
  QHBoxLayout* button_layout_;
  QPushButton* next_question_button_;
  QPushButton* previous_question_button_;
  QGroupBox* progress_group_;
  QVBoxLayout* progress_layout_;
  QLabel* total_progress_label_;
  QProgressBar* total_progress_bar_;
  QLabel* green_progress_label_;
  QProgressBar* green_progress_bar_;
};

#endif
