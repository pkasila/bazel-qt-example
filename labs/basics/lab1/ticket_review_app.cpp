#include "ticket_review_app.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QListWidgetItem>
#include <QGroupBox>
#include <QColor>

TicketReviewApp::TicketReviewApp(QWidget* parent)
    : QMainWindow(parent), manager_(0), current_ticket_id_(-1) {
  SetupUI();
  ConnectSignals();
}

void TicketReviewApp::SetupUI() {
  auto* central_widget = new QWidget(this);
  auto* main_layout = new QHBoxLayout(central_widget);

  // Left panel: count and ticket list
  auto* left_layout = new QVBoxLayout();

  auto* count_layout = new QHBoxLayout();
  count_layout->addWidget(new QLabel("Number of tickets:"));
  count_spinbox_ = new QSpinBox();
  count_spinbox_->setMinimum(1);
  count_spinbox_->setMaximum(100);
  count_spinbox_->setValue(10);
  count_layout->addWidget(count_spinbox_);
  left_layout->addLayout(count_layout);

  ticket_list_ = new QListWidget();
  left_layout->addWidget(ticket_list_);

  main_layout->addLayout(left_layout, 1);

  // Right panel: ticket details and progress
  auto* right_layout = new QVBoxLayout();

  // Ticket info group
  auto* ticket_group = new QGroupBox("Ticket Information");
  auto* ticket_layout = new QVBoxLayout(ticket_group);

  auto* number_layout = new QHBoxLayout();
  number_layout->addWidget(new QLabel("Ticket number:"));
  ticket_number_label_ = new QLabel("—");
  number_layout->addWidget(ticket_number_label_);
  number_layout->addStretch();
  ticket_layout->addLayout(number_layout);

  auto* name_layout = new QHBoxLayout();
  name_layout->addWidget(new QLabel("Name:"));
  ticket_name_label_ = new QLabel("—");
  name_layout->addWidget(ticket_name_label_);
  name_layout->addStretch();
  ticket_layout->addLayout(name_layout);

  auto* name_edit_layout = new QHBoxLayout();
  name_edit_layout->addWidget(new QLabel("Edit name:"));
  ticket_name_edit_ = new QLineEdit();
  ticket_name_edit_->setPlaceholderText("Enter new name...");
  name_edit_layout->addWidget(ticket_name_edit_);
  ticket_layout->addLayout(name_edit_layout);

  auto* status_layout = new QHBoxLayout();
  status_layout->addWidget(new QLabel("Status:"));
  ticket_status_combo_ = new QComboBox();
  ticket_status_combo_->addItem("Default");
  ticket_status_combo_->addItem("Repeated");
  ticket_status_combo_->addItem("Mastered");
  status_layout->addWidget(ticket_status_combo_);
  status_layout->addStretch();
  ticket_layout->addLayout(status_layout);

  right_layout->addWidget(ticket_group);

  // Navigation group
  auto* navigation_group = new QGroupBox("Navigation");
  auto* navigation_layout = new QVBoxLayout(navigation_group);

  previous_button_ = new QPushButton("← Previous");
  next_button_ = new QPushButton("Next →");

  auto* nav_buttons_layout = new QHBoxLayout();
  nav_buttons_layout->addWidget(previous_button_);
  nav_buttons_layout->addWidget(next_button_);
  navigation_layout->addLayout(nav_buttons_layout);

  right_layout->addWidget(navigation_group);

  // Progress group
  auto* progress_group = new QGroupBox("Progress");
  auto* progress_layout = new QVBoxLayout(progress_group);

  total_progress_label_ = new QLabel("Total progress:");
  progress_layout->addWidget(total_progress_label_);
  total_progress_bar_ = new QProgressBar();
  progress_layout->addWidget(total_progress_bar_);

  mastered_progress_label_ = new QLabel("Mastered progress:");
  progress_layout->addWidget(mastered_progress_label_);
  mastered_progress_bar_ = new QProgressBar();
  progress_layout->addWidget(mastered_progress_bar_);

  right_layout->addWidget(progress_group);

  right_layout->addStretch();

  main_layout->addLayout(right_layout, 1);

  setCentralWidget(central_widget);
  setWindowTitle("Ticket Review Assistant");
  resize(1000, 600);
}

void TicketReviewApp::ConnectSignals() {
  connect(count_spinbox_, QOverload<int>::of(&QSpinBox::valueChanged), this,
          &TicketReviewApp::OnTicketCountChanged);

  connect(ticket_list_, &QListWidget::itemClicked, this,
          &TicketReviewApp::OnListItemClicked);
  connect(ticket_list_, &QListWidget::itemDoubleClicked, this,
          &TicketReviewApp::OnListItemDoubleClicked);

  connect(ticket_status_combo_, QOverload<int>::of(&QComboBox::currentIndexChanged),
          this, &TicketReviewApp::OnStatusChanged);

  connect(ticket_name_edit_, &QLineEdit::returnPressed, this,
          &TicketReviewApp::OnNameEditReturnPressed);

  connect(next_button_, &QPushButton::clicked, this,
          &TicketReviewApp::OnNextButtonClicked);
  connect(previous_button_, &QPushButton::clicked, this,
          &TicketReviewApp::OnPreviousButtonClicked);
}

void TicketReviewApp::RefreshTicketList() {
  ticket_list_->clear();

  for (int i = 0; i < manager_.GetCapacity(); ++i) {
    QString item_text = manager_.GetName(i);
    auto item = new QListWidgetItem(item_text);

    switch (manager_.GetStatus(i)) {
      case TicketManager::Status::kDefault:
        item->setBackground(Qt::lightGray);
        break;
      case TicketManager::Status::kRepeated:
        item->setBackground(Qt::yellow);
        break;
      case TicketManager::Status::kMastered:
        item->setBackground(Qt::green);
        break;
    }

    ticket_list_->addItem(item);
  }
}

void TicketReviewApp::UpdateTicketDetails(int ticket_id) {
  current_ticket_id_ = ticket_id;

  if (ticket_id < 0 || ticket_id >= manager_.GetCapacity()) {
    ticket_number_label_->setText("—");
    ticket_name_label_->setText("—");
    ticket_status_combo_->blockSignals(true);
    ticket_status_combo_->setCurrentIndex(0);
    ticket_status_combo_->blockSignals(false);
    ticket_name_edit_->clear();
    return;
  }

  ticket_number_label_->setText(QString::number(ticket_id + 1));
  ticket_name_label_->setText(manager_.GetName(ticket_id));

  ticket_status_combo_->blockSignals(true);
  ticket_status_combo_->setCurrentIndex(
      static_cast<int>(manager_.GetStatus(ticket_id)));
  ticket_status_combo_->blockSignals(false);

  ticket_name_edit_->clear();

  if (ticket_id >= 0 && ticket_id < ticket_list_->count()) {
    ticket_list_->setCurrentRow(ticket_id);
  }
}

void TicketReviewApp::UpdateProgressBars() {
  int capacity = manager_.GetCapacity();
  if (capacity == 0) {
    total_progress_bar_->setMaximum(1);
    total_progress_bar_->setValue(0);
    mastered_progress_bar_->setMaximum(1);
    mastered_progress_bar_->setValue(0);
    return;
  }

  int total_progress = manager_.GetTotalProgress();
  int mastered_progress = manager_.GetMasteredProgress();

  total_progress_bar_->setMaximum(capacity);
  total_progress_bar_->setValue(total_progress);
  total_progress_label_->setText(
      QString("Total progress: %1/%2").arg(total_progress).arg(capacity));

  mastered_progress_bar_->setMaximum(capacity);
  mastered_progress_bar_->setValue(mastered_progress);
  mastered_progress_label_->setText(
      QString("Mastered progress: %1/%2").arg(mastered_progress).arg(capacity));
}

void TicketReviewApp::OnTicketCountChanged(int new_count) {
  manager_.SetCapacity(new_count);

  for (int i = 0; i < new_count; ++i) {
    if (manager_.GetName(i).isEmpty()) {
      manager_.SetName(i, QString("Ticket %1").arg(i + 1));
    }
  }

  RefreshTicketList();
  UpdateProgressBars();
  current_ticket_id_ = -1;
  UpdateTicketDetails(-1);
}

void TicketReviewApp::OnListItemClicked(QListWidgetItem* item) {
  int index = ticket_list_->row(item);
  UpdateTicketDetails(index);
}

void TicketReviewApp::OnListItemDoubleClicked(QListWidgetItem* item) {
  int index = ticket_list_->row(item);
  auto current_status = manager_.GetStatus(index);

  if (current_status == TicketManager::Status::kMastered) {
    manager_.SetStatus(index, TicketManager::Status::kRepeated);
  } else {
    manager_.SetStatus(index, TicketManager::Status::kMastered);
  }

  RefreshTicketList();
  UpdateProgressBars();
  UpdateTicketDetails(index);
}

void TicketReviewApp::OnStatusChanged(int new_status_index) {
  if (current_ticket_id_ < 0) {
    return;
  }

  auto new_status = static_cast<TicketManager::Status>(new_status_index);
  manager_.SetStatus(current_ticket_id_, new_status);

  RefreshTicketList();
  UpdateProgressBars();
}

void TicketReviewApp::OnNameEditReturnPressed() {
  if (current_ticket_id_ < 0) {
    return;
  }

  QString new_name = ticket_name_edit_->text().trimmed();
  if (new_name.isEmpty()) {
    return;
  }

  manager_.SetName(current_ticket_id_, new_name);
  ticket_name_label_->setText(new_name);
  RefreshTicketList();
  ticket_name_edit_->clear();
}

void TicketReviewApp::OnNextButtonClicked() {
  int next_ticket_id = manager_.GetRandomNonMasteredIndex();
  if (next_ticket_id >= 0) {
    UpdateTicketDetails(next_ticket_id);
  }
}

void TicketReviewApp::OnPreviousButtonClicked() {
  auto non_mastered = manager_.GetNonMasteredIndices();
  if (!non_mastered.empty()) {
    if (current_ticket_id_ > 0) {
      int prev_id = current_ticket_id_ - 1;
      while (prev_id >= 0) {
        if (manager_.GetStatus(prev_id) != TicketManager::Status::kMastered) {
          UpdateTicketDetails(prev_id);
          return;
        }
        prev_id--;
      }
    }
    UpdateTicketDetails(non_mastered[0]);
  }
}
