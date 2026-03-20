#include "ticket_review_app.h"

#include <QFormLayout>
#include <QFrame>
#include <QHBoxLayout>
#include <QPainter>
#include <QRandomGenerator>
#include <QSignalBlocker>
#include <QStyledItemDelegate>
#include <QVBoxLayout>

namespace {

constexpr int kDefaultTicketCount = 10;
constexpr int kMinimumTicketCount = 1;
constexpr int kMaximumTicketCount = 500;
constexpr int kTicketItemWidth = 160;
constexpr int kTicketItemHeight = 52;

QColor StatusColor(TicketManager::Status status) {
  switch (status) {
    case TicketManager::Status::kDefault:
      return QColor("#d1d5db");
    case TicketManager::Status::kYellow:
      return QColor("#fde68a");
    case TicketManager::Status::kGreen:
      return QColor("#86efac");
  }
  return QColor("#d1d5db");
}

class TicketItemDelegate final : public QStyledItemDelegate {
 public:
  explicit TicketItemDelegate(QObject* parent = nullptr)
      : QStyledItemDelegate(parent) {}

  void paint(QPainter* painter,
             const QStyleOptionViewItem& option,
             const QModelIndex& index) const override {
    painter->save();
    painter->setRenderHint(QPainter::Antialiasing, true);

    const QVariant status_data =
        index.data(static_cast<int>(Qt::UserRole) + 1);
    const auto status =
        static_cast<TicketManager::Status>(status_data.toInt());

    QRectF card_rect = option.rect.adjusted(4, 4, -4, -4);
    QPen border_pen(QColor("#cbd5e1"));
    border_pen.setWidth(1);

    if ((option.state & QStyle::State_Selected) != QStyle::State_None) {
      border_pen.setColor(QColor("#1f2937"));
      border_pen.setWidth(2);
    }

    painter->setPen(border_pen);
    painter->setBrush(StatusColor(status));
    painter->drawRoundedRect(card_rect, 10, 10);

    painter->setPen(QColor("#111827"));
    painter->drawText(card_rect.adjusted(12, 0, -12, 0),
                      Qt::AlignCenter | Qt::TextWordWrap,
                      index.data(Qt::DisplayRole).toString());

    painter->restore();
  }

  QSize sizeHint(const QStyleOptionViewItem& option,
                 const QModelIndex& index) const override {
    Q_UNUSED(option)
    Q_UNUSED(index)
    return {kTicketItemWidth, kTicketItemHeight};
  }
};

}  // namespace

TicketReviewApp::TicketReviewApp(QWidget* parent)
    : QWidget(parent), ticket_manager_(kDefaultTicketCount) {
  BuildUi();
  ConnectSignals();
  ResetTickets(kDefaultTicketCount);

  setWindowTitle("Повторение билетов");
  resize(1080, 640);
}

void TicketReviewApp::BuildUi() {
  auto* main_layout = new QHBoxLayout(this);
  main_layout->setContentsMargins(18, 18, 18, 18);
  main_layout->setSpacing(18);

  auto* left_panel_layout = new QVBoxLayout();
  left_panel_layout->setSpacing(14);

  auto* count_layout = new QHBoxLayout();
  auto* count_label = new QLabel("Количество билетов:");
  count_spin_box_ = new QSpinBox();
  count_spin_box_->setRange(kMinimumTicketCount, kMaximumTicketCount);
  count_spin_box_->setValue(kDefaultTicketCount);
  count_spin_box_->setMinimumWidth(110);
  count_layout->addWidget(count_label);
  count_layout->addWidget(count_spin_box_);
  count_layout->addStretch();

  tickets_view_ = new QListWidget();
  tickets_view_->setItemDelegate(new TicketItemDelegate(tickets_view_));
  tickets_view_->setViewMode(QListView::IconMode);
  tickets_view_->setResizeMode(QListView::Adjust);
  tickets_view_->setMovement(QListView::Static);
  tickets_view_->setSelectionMode(QAbstractItemView::SingleSelection);
  tickets_view_->setWrapping(true);
  tickets_view_->setSpacing(8);
  tickets_view_->setUniformItemSizes(true);
  tickets_view_->setFrameShape(QFrame::StyledPanel);
  tickets_view_->setStyleSheet(
      "QListWidget {"
      "  background-color: #f8fafc;"
      "  border: 1px solid #cbd5e1;"
      "  border-radius: 12px;"
      "  padding: 8px;"
      "}");

  auto* total_progress_label = new QLabel("Общий прогресс");
  total_progress_bar_ = new QProgressBar();
  total_progress_bar_->setTextVisible(true);
  total_progress_bar_->setStyleSheet(
      "QProgressBar {"
      "  border: 1px solid #cbd5e1;"
      "  border-radius: 8px;"
      "  background-color: #ffffff;"
      "  text-align: center;"
      "}"
      "QProgressBar::chunk {"
      "  border-radius: 8px;"
      "  background-color: #f59e0b;"
      "}");

  auto* green_progress_label = new QLabel("Завершено полностью");
  green_progress_bar_ = new QProgressBar();
  green_progress_bar_->setTextVisible(true);
  green_progress_bar_->setStyleSheet(
      "QProgressBar {"
      "  border: 1px solid #cbd5e1;"
      "  border-radius: 8px;"
      "  background-color: #ffffff;"
      "  text-align: center;"
      "}"
      "QProgressBar::chunk {"
      "  border-radius: 8px;"
      "  background-color: #22c55e;"
      "}");

  left_panel_layout->addLayout(count_layout);
  left_panel_layout->addWidget(tickets_view_, 1);
  left_panel_layout->addWidget(total_progress_label);
  left_panel_layout->addWidget(total_progress_bar_);
  left_panel_layout->addWidget(green_progress_label);
  left_panel_layout->addWidget(green_progress_bar_);

  question_group_ = new QGroupBox("Текущий билет");
  question_group_->setStyleSheet(
      "QGroupBox {"
      "  border: 1px solid #cbd5e1;"
      "  border-radius: 12px;"
      "  margin-top: 12px;"
      "  font-weight: 600;"
      "  background-color: #ffffff;"
      "}"
      "QGroupBox::title {"
      "  left: 14px;"
      "  padding: 0 6px;"
      "}");

  auto* question_layout = new QVBoxLayout(question_group_);
  question_layout->setContentsMargins(18, 24, 18, 18);
  question_layout->setSpacing(16);

  auto* info_layout = new QFormLayout();
  info_layout->setLabelAlignment(Qt::AlignLeft);
  info_layout->setFormAlignment(Qt::AlignTop);
  info_layout->setHorizontalSpacing(12);
  info_layout->setVerticalSpacing(12);

  number_value_label_ = new QLabel();
  name_value_label_ = new QLabel();
  name_value_label_->setWordWrap(true);
  name_edit_ = new QLineEdit();
  name_edit_->setPlaceholderText("Введите новое имя билета и нажмите Enter");

  status_combo_box_ = new QComboBox();
  status_combo_box_->addItem(StatusToText(TicketManager::Status::kDefault));
  status_combo_box_->addItem(StatusToText(TicketManager::Status::kYellow));
  status_combo_box_->addItem(StatusToText(TicketManager::Status::kGreen));

  info_layout->addRow("Номер:", number_value_label_);
  info_layout->addRow("Имя:", name_value_label_);
  info_layout->addRow("Переименовать:", name_edit_);
  info_layout->addRow("Статус:", status_combo_box_);

  auto* buttons_layout = new QHBoxLayout();
  previous_question_button_ = new QPushButton("Предыдущий билет");
  next_question_button_ = new QPushButton("Случайный следующий билет");
  previous_question_button_->setMinimumHeight(40);
  next_question_button_->setMinimumHeight(40);
  buttons_layout->addWidget(previous_question_button_);
  buttons_layout->addWidget(next_question_button_);

  question_layout->addLayout(info_layout);
  question_layout->addStretch();
  question_layout->addLayout(buttons_layout);

  main_layout->addLayout(left_panel_layout, 5);
  main_layout->addWidget(question_group_, 4);

  setStyleSheet(
      "QWidget {"
      "  font-family: Inter, Arial, sans-serif;"
      "  font-size: 14px;"
      "  color: #111827;"
      "  background-color: #eef2ff;"
      "}"
      "QLineEdit, QSpinBox, QComboBox {"
      "  min-height: 36px;"
      "  border: 1px solid #cbd5e1;"
      "  border-radius: 10px;"
      "  background-color: #ffffff;"
      "  padding: 0 10px;"
      "}"
      "QPushButton {"
      "  border: none;"
      "  border-radius: 10px;"
      "  padding: 0 16px;"
      "  background-color: #2563eb;"
      "  color: #ffffff;"
      "  font-weight: 600;"
      "}"
      "QPushButton:disabled {"
      "  background-color: #94a3b8;"
      "}"
      "QPushButton:hover:!disabled {"
      "  background-color: #1d4ed8;"
      "}");
}

void TicketReviewApp::ConnectSignals() {
  connect(count_spin_box_,
          qOverload<int>(&QSpinBox::valueChanged),
          this,
          &TicketReviewApp::ResetTickets);

  connect(tickets_view_,
          &QListWidget::itemClicked,
          this,
          [this](QListWidgetItem* item) {
            ShowTicket(tickets_view_->row(item), true);
          });

  connect(tickets_view_,
          &QListWidget::itemDoubleClicked,
          this,
          [this](QListWidgetItem* item) {
            const int index = tickets_view_->row(item);
            ticket_manager_.ToggleStatusFromView(index);
            RefreshTicketItem(index);
            RefreshQuestionView();
            RefreshProgressBars();
            RefreshButtons();
            tickets_view_->viewport()->update();
          });

  connect(name_edit_,
          &QLineEdit::returnPressed,
          this,
          &TicketReviewApp::CommitNameEdit);

  connect(status_combo_box_,
          qOverload<int>(&QComboBox::currentIndexChanged),
          this,
          [this](int index) {
            SetCurrentTicketStatus(IndexToStatus(index));
          });

  connect(next_question_button_,
          &QPushButton::clicked,
          this,
          &TicketReviewApp::SelectRandomNextTicket);

  connect(previous_question_button_,
          &QPushButton::clicked,
          this,
          &TicketReviewApp::ShowPreviousTicket);
}

void TicketReviewApp::ResetTickets(int ticket_count) {
  ticket_manager_.Reset(ticket_count);
  selection_history_.clear();
  history_position_ = -1;
  current_ticket_index_ = ticket_count > 0 ? 0 : -1;

  if (current_ticket_index_ >= 0) {
    selection_history_.push_back(current_ticket_index_);
    history_position_ = 0;
  }

  RebuildView();
  RefreshAll();
}

void TicketReviewApp::RebuildView() {
  tickets_view_->clear();

  for (int index = 0; index < ticket_manager_.GetTicketCount(); ++index) {
    auto* item = new QListWidgetItem(ticket_manager_.GetTicket(index).name);
    item->setTextAlignment(Qt::AlignCenter);
    item->setSizeHint({kTicketItemWidth, kTicketItemHeight});
    tickets_view_->addItem(item);
    RefreshTicketItem(index);
  }

  if (current_ticket_index_ >= 0) {
    tickets_view_->setCurrentRow(current_ticket_index_);
  }
}

void TicketReviewApp::RefreshTicketItem(int index) {
  if (!ticket_manager_.IsValidIndex(index)) {
    return;
  }

  QListWidgetItem* item = tickets_view_->item(index);
  if (item == nullptr) {
    return;
  }

  const TicketManager::Ticket& ticket = ticket_manager_.GetTicket(index);
  item->setText(ticket.name);
  item->setData(static_cast<int>(Qt::UserRole) + 1,
                static_cast<int>(ticket.status));
}

void TicketReviewApp::RefreshQuestionView() {
  const bool has_ticket = ticket_manager_.IsValidIndex(current_ticket_index_);
  question_group_->setEnabled(has_ticket);

  if (!has_ticket) {
    number_value_label_->setText("—");
    name_value_label_->setText("—");
    name_edit_->clear();
    QSignalBlocker blocker(status_combo_box_);
    status_combo_box_->setCurrentIndex(0);
    return;
  }

  const TicketManager::Ticket& ticket =
      ticket_manager_.GetTicket(current_ticket_index_);
  number_value_label_->setText(QString::number(current_ticket_index_ + 1));
  name_value_label_->setText(ticket.name);

  {
    QSignalBlocker blocker(name_edit_);
    name_edit_->setText(ticket.name);
  }

  {
    QSignalBlocker blocker(status_combo_box_);
    status_combo_box_->setCurrentIndex(StatusToIndex(ticket.status));
  }
}

void TicketReviewApp::RefreshProgressBars() {
  const int ticket_count = ticket_manager_.GetTicketCount();
  const int reviewed_count = ticket_manager_.CountReviewed();
  const int green_count = ticket_manager_.CountGreen();

  total_progress_bar_->setRange(0, ticket_count);
  total_progress_bar_->setValue(reviewed_count);
  total_progress_bar_->setFormat(
      QString("%1 из %2").arg(reviewed_count).arg(ticket_count));

  green_progress_bar_->setRange(0, ticket_count);
  green_progress_bar_->setValue(green_count);
  green_progress_bar_->setFormat(
      QString("%1 из %2").arg(green_count).arg(ticket_count));
}

void TicketReviewApp::RefreshButtons() {
  previous_question_button_->setEnabled(history_position_ > 0);
  next_question_button_->setEnabled(
      !ticket_manager_.GetAvailableForRandom(current_ticket_index_).isEmpty());
}

void TicketReviewApp::RefreshAll() {
  RefreshQuestionView();
  RefreshProgressBars();
  RefreshButtons();
  tickets_view_->viewport()->update();
}

void TicketReviewApp::ShowTicket(int index, bool add_to_history) {
  if (!ticket_manager_.IsValidIndex(index)) {
    return;
  }

  current_ticket_index_ = index;
  tickets_view_->setCurrentRow(index);
  if (add_to_history) {
    AddToHistory(index);
  }
  RefreshQuestionView();
  RefreshButtons();
  tickets_view_->viewport()->update();
}

void TicketReviewApp::AddToHistory(int index) {
  if (history_position_ >= 0 && history_position_ < selection_history_.size() &&
      selection_history_[history_position_] == index) {
    return;
  }

  if (history_position_ + 1 < selection_history_.size()) {
    selection_history_.resize(history_position_ + 1);
  }

  selection_history_.push_back(index);
  history_position_ = selection_history_.size() - 1;
}

void TicketReviewApp::SelectRandomNextTicket() {
  QVector<int> available_indices =
      ticket_manager_.GetAvailableForRandom(current_ticket_index_);
  if (available_indices.isEmpty()) {
    return;
  }

  const int random_index =
      QRandomGenerator::global()->bounded(available_indices.size());
  ShowTicket(available_indices[random_index], true);
}

void TicketReviewApp::ShowPreviousTicket() {
  if (history_position_ <= 0) {
    return;
  }

  --history_position_;
  ShowTicket(selection_history_[history_position_], false);
}

void TicketReviewApp::CommitNameEdit() {
  if (!name_edit_->hasFocus() ||
      !ticket_manager_.IsValidIndex(current_ticket_index_)) {
    return;
  }

  const QString new_name = name_edit_->text().trimmed();
  if (new_name.isEmpty()) {
    return;
  }

  ticket_manager_.SetName(current_ticket_index_, new_name);
  RefreshTicketItem(current_ticket_index_);
  RefreshQuestionView();
  tickets_view_->viewport()->update();
}

void TicketReviewApp::SetCurrentTicketStatus(TicketManager::Status status) {
  if (!ticket_manager_.IsValidIndex(current_ticket_index_)) {
    return;
  }

  ticket_manager_.SetStatus(current_ticket_index_, status);
  RefreshTicketItem(current_ticket_index_);
  RefreshProgressBars();
  RefreshButtons();
  tickets_view_->viewport()->update();
}

QString TicketReviewApp::StatusToText(TicketManager::Status status) {
  switch (status) {
    case TicketManager::Status::kDefault:
      return "Не повторял";
    case TicketManager::Status::kYellow:
      return "Надо повторить ещё";
    case TicketManager::Status::kGreen:
      return "Повторён";
  }
  return "Не повторял";
}

TicketManager::Status TicketReviewApp::IndexToStatus(int index) {
  switch (index) {
    case 0:
      return TicketManager::Status::kDefault;
    case 1:
      return TicketManager::Status::kYellow;
    case 2:
      return TicketManager::Status::kGreen;
    default:
      return TicketManager::Status::kDefault;
  }
}

int TicketReviewApp::StatusToIndex(TicketManager::Status status) {
  switch (status) {
    case TicketManager::Status::kDefault:
      return 0;
    case TicketManager::Status::kYellow:
      return 1;
    case TicketManager::Status::kGreen:
      return 2;
  }
  return 0;
}
