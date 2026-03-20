#include "ticket_review_app.h"

#include <QBrush>
#include <QColor>
#include <QFormLayout>
#include <QFrame>
#include <QSignalBlocker>

#include <algorithm>

namespace {
QColor StatusColor(TicketStatus status) {
  if (status == TicketStatus::kYellow) {
    return QColor(QStringLiteral("#f8e58c"));
  }
  if (status == TicketStatus::kGreen) {
    return QColor(QStringLiteral("#9fe0b4"));
  }
  return QColor(QStringLiteral("#d9dee8"));
}

QString StatusText(TicketStatus status) {
  if (status == TicketStatus::kYellow) {
    return QString::fromUtf8("Нужно повторить ещё раз");
  }
  if (status == TicketStatus::kGreen) {
    return QString::fromUtf8("Повторён полностью");
  }
  return QString::fromUtf8("Ещё не повторяли");
}
}

TicketReviewApp::TicketReviewApp(QWidget* parent)
    : QWidget(parent),
      manager_(20),
      current_index_(-1),
      history_(),
      root_layout_(nullptr),
      setup_group_(nullptr),
      setup_layout_(nullptr),
      count_label_(nullptr),
      count_spin_box_(nullptr),
      content_layout_(nullptr),
      view_group_(nullptr),
      view_layout_(nullptr),
      view_(nullptr),
      right_panel_(nullptr),
      right_layout_(nullptr),
      question_group_(nullptr),
      question_layout_(nullptr),
      number_title_label_(nullptr),
      number_value_label_(nullptr),
      name_title_label_(nullptr),
      name_value_label_(nullptr),
      edit_title_label_(nullptr),
      name_edit_(nullptr),
      status_title_label_(nullptr),
      status_combo_box_(nullptr),
      button_layout_(nullptr),
      next_question_button_(nullptr),
      previous_question_button_(nullptr),
      progress_group_(nullptr),
      progress_layout_(nullptr),
      total_progress_label_(nullptr),
      total_progress_bar_(nullptr),
      green_progress_label_(nullptr),
      green_progress_bar_(nullptr) {
  BuildInterface();
  ApplyStyle();
  ConnectSignals();
  RebuildView();
  SelectTicket(0, false);
  setMinimumSize(980, 640);
  setWindowTitle(QString::fromUtf8("Лаба 1 — Прокрастинация"));
}

void TicketReviewApp::resizeEvent(QResizeEvent* event) {
  content_layout_->setDirection(width() < 960 ? QBoxLayout::TopToBottom
                                              : QBoxLayout::LeftToRight);
  UpdateViewGrid();
  QWidget::resizeEvent(event);
}

void TicketReviewApp::BuildInterface() {
  root_layout_ = new QVBoxLayout(this);
  root_layout_->setContentsMargins(20, 20, 20, 20);
  root_layout_->setSpacing(16);

  setup_group_ = new QGroupBox(QString::fromUtf8("Параметры"), this);
  setup_layout_ = new QHBoxLayout(setup_group_);
  setup_layout_->setContentsMargins(16, 16, 16, 16);
  setup_layout_->setSpacing(12);

  count_label_ = new QLabel(QString::fromUtf8("Количество билетов"),
                            setup_group_);
  count_spin_box_ = new QSpinBox(setup_group_);
  count_spin_box_->setRange(1, 200);
  count_spin_box_->setValue(manager_.GetCount());
  count_spin_box_->setAccelerated(true);

  setup_layout_->addWidget(count_label_);
  setup_layout_->addWidget(count_spin_box_);
  setup_layout_->addStretch(1);

  content_layout_ = new QBoxLayout(QBoxLayout::LeftToRight);
  content_layout_->setSpacing(16);

  view_group_ = new QGroupBox(QString::fromUtf8("Прогресс по билетам"), this);
  view_layout_ = new QVBoxLayout(view_group_);
  view_layout_->setContentsMargins(16, 16, 16, 16);
  view_layout_->setSpacing(12);

  view_ = new QListWidget(view_group_);
  view_->setViewMode(QListView::IconMode);
  view_->setFlow(QListView::LeftToRight);
  view_->setMovement(QListView::Static);
  view_->setResizeMode(QListView::Adjust);
  view_->setUniformItemSizes(true);
  view_->setSpacing(10);
  view_->setWrapping(true);
  view_->setSelectionMode(QAbstractItemView::SingleSelection);
  view_->setWordWrap(true);
  view_->setFrameShape(QFrame::NoFrame);

  view_layout_->addWidget(view_);

  right_panel_ = new QWidget(this);
  right_layout_ = new QVBoxLayout(right_panel_);
  right_layout_->setContentsMargins(0, 0, 0, 0);
  right_layout_->setSpacing(16);

  question_group_ = new QGroupBox(QString::fromUtf8("Текущий билет"),
                                  right_panel_);
  question_layout_ = new QVBoxLayout(question_group_);
  question_layout_->setContentsMargins(16, 16, 16, 16);
  question_layout_->setSpacing(14);

  QFormLayout* info_layout = new QFormLayout();
  info_layout->setSpacing(12);
  info_layout->setLabelAlignment(Qt::AlignLeft | Qt::AlignVCenter);
  info_layout->setFormAlignment(Qt::AlignTop);

  number_title_label_ = new QLabel(QString::fromUtf8("Номер"), question_group_);
  number_value_label_ = new QLabel(question_group_);
  name_title_label_ = new QLabel(QString::fromUtf8("Имя"), question_group_);
  name_value_label_ = new QLabel(question_group_);
  name_value_label_->setWordWrap(true);
  edit_title_label_ = new QLabel(QString::fromUtf8("Новое имя"),
                                 question_group_);
  name_edit_ = new QLineEdit(question_group_);
  name_edit_->setPlaceholderText(
      QString::fromUtf8("Введите имя и нажмите Enter"));
  status_title_label_ = new QLabel(QString::fromUtf8("Статус"),
                                   question_group_);
  status_combo_box_ = new QComboBox(question_group_);
  status_combo_box_->addItem(QString::fromUtf8("По умолчанию"),
                             static_cast<int>(TicketStatus::kDefault));
  status_combo_box_->addItem(QString::fromUtf8("Жёлтый"),
                             static_cast<int>(TicketStatus::kYellow));
  status_combo_box_->addItem(QString::fromUtf8("Зелёный"),
                             static_cast<int>(TicketStatus::kGreen));

  info_layout->addRow(number_title_label_, number_value_label_);
  info_layout->addRow(name_title_label_, name_value_label_);
  info_layout->addRow(edit_title_label_, name_edit_);
  info_layout->addRow(status_title_label_, status_combo_box_);

  question_layout_->addLayout(info_layout);

  button_layout_ = new QHBoxLayout();
  button_layout_->setSpacing(12);
  previous_question_button_ =
      new QPushButton(QString::fromUtf8("Предыдущий билет"), question_group_);
  next_question_button_ =
      new QPushButton(QString::fromUtf8("Следующий случайный билет"),
                      question_group_);
  previous_question_button_->setMinimumHeight(40);
  next_question_button_->setMinimumHeight(40);
  button_layout_->addWidget(previous_question_button_);
  button_layout_->addWidget(next_question_button_);

  question_layout_->addLayout(button_layout_);
  question_layout_->addStretch(1);

  progress_group_ = new QGroupBox(QString::fromUtf8("Полосы прогресса"),
                                  right_panel_);
  progress_layout_ = new QVBoxLayout(progress_group_);
  progress_layout_->setContentsMargins(16, 16, 16, 16);
  progress_layout_->setSpacing(12);

  total_progress_label_ =
      new QLabel(QString::fromUtf8("Общий прогресс"), progress_group_);
  total_progress_bar_ = new QProgressBar(progress_group_);
  total_progress_bar_->setTextVisible(true);
  total_progress_bar_->setMinimumHeight(26);

  green_progress_label_ =
      new QLabel(QString::fromUtf8("Полностью повторено"), progress_group_);
  green_progress_bar_ = new QProgressBar(progress_group_);
  green_progress_bar_->setTextVisible(true);
  green_progress_bar_->setMinimumHeight(26);

  progress_layout_->addWidget(total_progress_label_);
  progress_layout_->addWidget(total_progress_bar_);
  progress_layout_->addWidget(green_progress_label_);
  progress_layout_->addWidget(green_progress_bar_);

  right_layout_->addWidget(question_group_);
  right_layout_->addWidget(progress_group_);

  content_layout_->addWidget(view_group_, 6);
  content_layout_->addWidget(right_panel_, 5);

  root_layout_->addWidget(setup_group_);
  root_layout_->addLayout(content_layout_, 1);
}

void TicketReviewApp::ConnectSignals() {
  connect(count_spin_box_, &QSpinBox::valueChanged, this,
          [this](int value) { ApplyTicketCount(value); });

  connect(view_, &QListWidget::currentRowChanged, this,
          [this](int row) { SelectTicket(row, true); });

  connect(view_, &QListWidget::itemDoubleClicked, this,
          [this](QListWidgetItem* item) {
            const int row = view_->row(item);
            manager_.ToggleStatusByDoubleClick(row);
            UpdateViewItem(row);
            UpdateProgressBars();
            UpdateQuestionView();
            UpdateButtons();
          });

  connect(name_edit_, &QLineEdit::returnPressed, this,
          [this]() { SubmitEditedName(); });

  connect(status_combo_box_, &QComboBox::currentIndexChanged, this,
          [this](int index) {
            if (index < 0) {
              return;
            }
            const auto status = static_cast<TicketStatus>(
                status_combo_box_->itemData(index).toInt());
            ChangeCurrentStatus(status);
          });

  connect(next_question_button_, &QPushButton::clicked, this,
          [this]() { OpenRandomTicket(); });

  connect(previous_question_button_, &QPushButton::clicked, this,
          [this]() { OpenPreviousTicket(); });
}

void TicketReviewApp::ApplyStyle() {
  setStyleSheet(QString::fromUtf8(
      "QWidget { background-color: #eef3f9; color: #243042; }"
      "QGroupBox { background-color: #ffffff; border: 1px solid #d7dfec; "
      "border-radius: 18px; margin-top: 14px; font-weight: 600; }"
      "QGroupBox::title { left: 14px; padding: 0 6px; color: #456087; }"
      "QLabel { font-size: 14px; }"
      "QSpinBox, QLineEdit, QComboBox { background-color: #f9fbff; border: "
      "1px solid #c9d6ea; border-radius: 10px; padding: 6px 8px; min-height: "
      "20px; }"
      "QPushButton { background-color: #2563eb; color: #ffffff; border: none; "
      "border-radius: 10px; padding: 8px 14px; font-weight: 600; }"
      "QPushButton:hover { background-color: #1d4fd0; }"
      "QPushButton:disabled { background-color: #aeb8ca; }"
      "QListWidget { background-color: #f8fbff; border: 1px solid #dde5f1; "
      "border-radius: 16px; padding: 8px; }"
      "QListWidget::item { border: 2px solid transparent; border-radius: 14px; "
      "padding: 6px; }"
      "QListWidget::item:selected { border: 2px solid #2563eb; }"
      "QProgressBar { background-color: #f8fbff; border: 1px solid #d8e1ef; "
      "border-radius: 10px; text-align: center; }"
      "QProgressBar::chunk { background-color: #2563eb; border-radius: 9px; }"));

  green_progress_bar_->setStyleSheet(QString::fromUtf8(
      "QProgressBar { background-color: #f8fbff; border: 1px solid #d8e1ef; "
      "border-radius: 10px; text-align: center; }"
      "QProgressBar::chunk { background-color: #2f9e5b; border-radius: 9px; }"));
}

void TicketReviewApp::RebuildView() {
  QSignalBlocker blocker(view_);
  view_->clear();

  for (int index = 0; index < manager_.GetCount(); ++index) {
    auto* item = new QListWidgetItem();
    item->setTextAlignment(Qt::AlignCenter);
    item->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
    item->setSizeHint(QSize(170, 84));
    view_->addItem(item);
    UpdateViewItem(index);
  }

  UpdateViewGrid();
}

void TicketReviewApp::UpdateViewItem(int index) {
  if (index < 0 || index >= view_->count()) {
    return;
  }

  QListWidgetItem* item = view_->item(index);
  const QString title = QString::fromUtf8("№%1").arg(index + 1);
  const QString subtitle = manager_.GetName(index);
  item->setText(title + QString::fromUtf8("\n") + subtitle);
  item->setBackground(QBrush(StatusColor(manager_.GetStatus(index))));
  item->setForeground(QBrush(QColor(QStringLiteral("#17212f"))));
}

void TicketReviewApp::UpdateAllViewItems() {
  for (int index = 0; index < manager_.GetCount(); ++index) {
    UpdateViewItem(index);
  }
}

void TicketReviewApp::UpdateQuestionView() {
  if (current_index_ < 0 || current_index_ >= manager_.GetCount()) {
    return;
  }

  const QString ticket_name = manager_.GetName(current_index_);
  const TicketStatus ticket_status = manager_.GetStatus(current_index_);

  number_value_label_->setText(QString::number(current_index_ + 1));
  name_value_label_->setText(ticket_name);

  {
    QSignalBlocker blocker(name_edit_);
    name_edit_->setText(ticket_name);
  }

  {
    QSignalBlocker blocker(status_combo_box_);
    status_combo_box_->setCurrentIndex(StatusComboIndex(ticket_status));
  }

  question_group_->setTitle(QString::fromUtf8("Текущий билет — %1")
                                .arg(StatusText(ticket_status)));
}

void TicketReviewApp::UpdateProgressBars() {
  const int count = manager_.GetCount();
  total_progress_bar_->setRange(0, count);
  green_progress_bar_->setRange(0, count);
  total_progress_bar_->setValue(manager_.GetTotalProgress());
  green_progress_bar_->setValue(manager_.GetGreenProgress());
  total_progress_bar_->setFormat(QString::fromUtf8("%v из %m"));
  green_progress_bar_->setFormat(QString::fromUtf8("%v из %m"));
}

void TicketReviewApp::UpdateButtons() {
  previous_question_button_->setEnabled(!history_.empty());
  next_question_button_->setEnabled(manager_.HasEligibleTickets());
}

void TicketReviewApp::UpdateViewGrid() {
  const int available_width = std::max(view_->viewport()->width(), 360);
  const int column_count = std::max(2, available_width / 175);
  const int horizontal_padding = 24;
  const int spacing = view_->spacing();
  const int cell_width = std::max(
      140, (available_width - horizontal_padding - spacing * (column_count - 1)) /
               column_count);
  view_->setGridSize(QSize(cell_width, 88));
}

void TicketReviewApp::SelectTicket(int index, bool record_history) {
  if (index < 0 || index >= manager_.GetCount()) {
    return;
  }

  if (index == current_index_) {
    UpdateQuestionView();
    UpdateButtons();
    return;
  }

  if (record_history && current_index_ >= 0) {
    history_.push_back(current_index_);
  }

  current_index_ = index;

  {
    QSignalBlocker blocker(view_);
    view_->setCurrentRow(current_index_);
  }

  UpdateQuestionView();
  UpdateProgressBars();
  UpdateButtons();
}

void TicketReviewApp::ApplyTicketCount(int count) {
  manager_.Reset(count);
  history_.clear();
  current_index_ = -1;
  RebuildView();
  SelectTicket(0, false);
  UpdateAllViewItems();
}

void TicketReviewApp::SubmitEditedName() {
  if (current_index_ < 0 || !name_edit_->hasFocus()) {
    return;
  }

  const QString new_name = name_edit_->text().trimmed();
  if (new_name.isEmpty()) {
    return;
  }

  manager_.SetName(current_index_, new_name);
  UpdateViewItem(current_index_);
  UpdateQuestionView();
}

void TicketReviewApp::ChangeCurrentStatus(TicketStatus status) {
  if (current_index_ < 0) {
    return;
  }

  manager_.SetStatus(current_index_, status);
  UpdateViewItem(current_index_);
  UpdateQuestionView();
  UpdateProgressBars();
  UpdateButtons();
}

void TicketReviewApp::OpenRandomTicket() {
  const int next_index = manager_.PickRandomEligible(current_index_);
  if (next_index < 0) {
    return;
  }
  SelectTicket(next_index, true);
}

void TicketReviewApp::OpenPreviousTicket() {
  while (!history_.empty() && history_.back() == current_index_) {
    history_.pop_back();
  }

  if (history_.empty()) {
    UpdateButtons();
    return;
  }

  const int previous_index = history_.back();
  history_.pop_back();
  SelectTicket(previous_index, false);
}

int TicketReviewApp::StatusComboIndex(TicketStatus status) const {
  if (status == TicketStatus::kYellow) {
    return 1;
  }
  if (status == TicketStatus::kGreen) {
    return 2;
  }
  return 0;
}
