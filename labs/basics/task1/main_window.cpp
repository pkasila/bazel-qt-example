// NOLINTBEGIN(cppcoreguidelines-owning-memory)
#include "main_window.h"

#include <QApplication>
#include <QComboBox>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QListWidget>
#include <QProgressBar>
#include <QPushButton>
#include <QRandomGenerator>
#include <QSpinBox>
#include <QStyleHints>
#include <QTimer>
#include <QVBoxLayout>
#include <cmath>
#include <ranges>

MainWindow::MainWindow() {  // NOLINT
    SetupUI();
}

MainWindow::~MainWindow() = default;

void MainWindow::SetupUI() {
    // TOP LAYOUT (progress bars)
    auto* top_widget = new QWidget(this);
    auto* top_layout = new QVBoxLayout(top_widget);
    total_progress_bar_ = new QProgressBar(top_widget);
    green_progress_bar_ = new QProgressBar(top_widget);
    total_progress_bar_->setTextVisible(true);
    total_progress_bar_->setFormat("%p% завершено/в прогрессе");
    total_progress_bar_->setRange(0, 100);
    total_progress_bar_->setValue(0);
    green_progress_bar_->setTextVisible(true);
    green_progress_bar_->setFormat("%p% завершено");
    green_progress_bar_->setRange(0, 100);
    green_progress_bar_->setValue(0);
    top_layout->addWidget(total_progress_bar_);
    top_layout->addWidget(green_progress_bar_);

    // UP LAYOUT (view)
    auto* view = new QWidget(this);
    auto* up_layout = new QVBoxLayout(view);
    count_spin_box_ = new QSpinBox(view);
    count_spin_box_->setMaximumWidth(200);
    ticket_list_ = new QListWidget(view);
    up_layout->addWidget(count_spin_box_);
    up_layout->setAlignment(count_spin_box_, Qt::AlignLeft);
    up_layout->addWidget(ticket_list_);

    // DOWN LAYOUT (question_view)
    question_view_ = new QGroupBox(this);
    auto* down_layout = new QVBoxLayout(question_view_);
    index_label_ = new QLabel("Номер: ");
    name_label_ = new QLabel("Название: ");
    hint_label_ = new QLabel("Подсказка (не более 200 символов): ");
    hint_label_->setMaximumWidth(1000);

    name_edit_ = new QLineEdit(question_view_);
    hint_edit_ = new QLineEdit(question_view_);
    status_combo_box_ = new QComboBox(question_view_);
    status_combo_box_->addItems({"Не начато", "В процессе", "Завершено"});
    down_layout->addWidget(index_label_);
    down_layout->addWidget(name_label_);
    down_layout->addWidget(name_edit_);
    down_layout->addWidget(hint_label_);
    down_layout->addWidget(hint_edit_);
    down_layout->addWidget(status_combo_box_);
    down_layout->addStretch();

    // MIDDLE LAYOUT
    auto* middle_widget = new QWidget(this);
    auto* middle_layout = new QVBoxLayout(middle_widget);
    middle_layout->addWidget(view);
    middle_layout->addWidget(question_view_);

    // BOTTOM LAYOUT
    auto* bottom_widget = new QWidget(this);
    auto* bottom_layout = new QHBoxLayout(bottom_widget);
    previous_button_ = new QPushButton("Предыдущий");
    next_button_ = new QPushButton("Следующий");
    bottom_layout->addWidget(previous_button_);
    bottom_layout->addWidget(next_button_);

    // MAIN LAYOUT
    auto* central_widget = new QWidget(this);
    setCentralWidget(central_widget);
    auto* main_layout = new QVBoxLayout(central_widget);
    main_layout->addWidget(top_widget);
    main_layout->addWidget(middle_widget);
    main_layout->addWidget(bottom_widget);
    central_widget->setLayout(main_layout);

    // CONNECTIONS
    connect(count_spin_box_, &QSpinBox::valueChanged, this, &MainWindow::OnCountChanged);
    connect(
        ticket_list_, &QListWidget::itemSelectionChanged, this,
        &MainWindow::OnItemSelectionChanged);
    connect(ticket_list_, &QListWidget::itemDoubleClicked, this, &MainWindow::OnItemDoubleClicked);
    connect(name_edit_, &QLineEdit::returnPressed, this, &MainWindow::OnNameEditChanged);
    connect(status_combo_box_, &QComboBox::currentIndexChanged, this, &MainWindow::OnStatusChanged);
    connect(next_button_, &QPushButton::clicked, this, &MainWindow::OnNextClicked);
    connect(previous_button_, &QPushButton::clicked, this, &MainWindow::OnPreviousClicked);
    connect(hint_edit_, &QLineEdit::returnPressed, this, &MainWindow::OnHintEditChanged);
}

void MainWindow::OnCountChanged(int count) {
    tickets_.clear();
    for (size_t i : std::ranges::iota_view(0, count)) {
        Ticket ticket(i, QString("Билет %1").arg(i + 1), TicketStatus::Default, "");
        tickets_.append(ticket);
    }
    history_.clear();
    completed_ = 0;
    partial_ = 0;
    current_index_ = -1;
    is_programmatic_selection_ = false;
    UpdateTicketList();
    UpdateQuestionView();
    UpdateProgress();
}

void MainWindow::OnItemSelectionChanged() {
    if (is_programmatic_selection_) {
        return;
    }
    QList<QListWidgetItem*> selected_items = ticket_list_->selectedItems();
    if (selected_items.isEmpty()) {
        current_index_ = -1;
        return;
    }
    history_.append(current_index_);
    current_index_ = selected_items.first()->data(Qt::UserRole).toInt();
    UpdateQuestionView();
}

void MainWindow::OnItemDoubleClicked(QListWidgetItem* item) {
    int index = item->data(Qt::UserRole).toInt();
    Ticket& ticket = tickets_[index];
    auto [_, name, status, hint] = ticket.GetPrivate();
    TicketStatus new_status = TicketStatus::Default;
    if (status == TicketStatus::Green) {
        new_status = TicketStatus::Yellow;
        partial_++;
        completed_--;
    } else {
        new_status = TicketStatus::Green;
        if (status == TicketStatus::Yellow) {
            partial_--;
        }
        completed_++;
    }
    ticket.ChangeStatus(new_status);
    ticket_list_->item(index)->setBackground(ticket.GetStatusColor());
    UpdateProgress();
}

void MainWindow::OnNameEditChanged() {
    if (current_index_ == -1) {
        return;
    }
    Ticket& current_ticket = tickets_[current_index_];
    QString name = name_edit_->text();
    if (name.isEmpty()) {
        return;
    }
    current_ticket.Rename(name_edit_->text());
    name_label_->setText("Название: " + name);
    ticket_list_->item(current_index_)->setText(name);
}

void MainWindow::OnStatusChanged(int index) {
    if (current_index_ == -1) {
        return;
    }
    Ticket& ticket = tickets_[current_index_];
    auto [_, name, status, hint] = ticket.GetPrivate();
    if (status == TicketStatus::Green) {
        completed_--;
    } else if (status == TicketStatus::Yellow) {
        partial_--;
    }
    auto new_status = static_cast<TicketStatus>(index);
    if (new_status == TicketStatus::Green) {
        completed_++;
    } else if (new_status == TicketStatus::Yellow) {
        partial_++;
    }
    ticket.ChangeStatus(new_status);
    ticket_list_->item(current_index_)->setBackground(ticket.GetStatusColor());
    UpdateProgress();
}

void MainWindow::OnNextClicked() {
    QVector<int> available;
    for (const auto& ticket : tickets_) {
        auto [index, name, status, _] = ticket.GetPrivate();
        if (status != TicketStatus::Green) {
            available.append(static_cast<int>(index));
        }
    }

    if (available.isEmpty()) {
        return;
    }

    int random_index = static_cast<int>(QRandomGenerator::global()->bounded(available.size()));
    history_.append(current_index_);
    current_index_ = available[random_index];
    UpdateQuestionView();
    is_programmatic_selection_ = true;
    ticket_list_->setCurrentRow(current_index_);
    is_programmatic_selection_ = false;
}

void MainWindow::OnPreviousClicked() {
    if (history_.isEmpty()) {
        return;
    }
    current_index_ = history_.takeLast();
    UpdateQuestionView();
    is_programmatic_selection_ = true;
    ticket_list_->setCurrentRow(current_index_);
    is_programmatic_selection_ = false;
}

void MainWindow::OnHintEditChanged() {
    if (current_index_ == -1) {
        return;
    }
    Ticket& ticket = tickets_[current_index_];
    ticket.SetHint(hint_edit_->text());
    hint_label_->setText("Подсказка: " + hint_edit_->text());
}

void MainWindow::UpdateTicketList() {
    ticket_list_->clear();
    for (const auto& ticket : tickets_) {
        auto [index, name, status, _] = ticket.GetPrivate();
        auto* item = new QListWidgetItem(name);
        item->setData(Qt::UserRole, static_cast<int>(index));
        item->setBackground(ticket.GetStatusColor());
        ticket_list_->addItem(item);
    }
}

void MainWindow::UpdateQuestionView() {
    if (current_index_ < 0 || current_index_ >= tickets_.size()) {
        return;
    }
    auto [index, name, status, hint] = tickets_[current_index_].GetPrivate();
    index_label_->setText(QString("Номер: %1").arg(index + 1));
    name_label_->setText(QString("Название: %1").arg(name));
    name_edit_->setText(name);
    hint_label_->setText(QString("Подсказка: %1").arg(hint));
    hint_edit_->clear();
    status_combo_box_->setCurrentIndex(static_cast<int>(status));
}

void MainWindow::UpdateProgress() {
    if (tickets_.isEmpty()) {
        total_progress_bar_->setValue(0);
        green_progress_bar_->setValue(0);
        return;
    }
    int n = static_cast<int>(tickets_.size());
    int completed = 100 * completed_ / n;
    int total = 100 * (completed_ + partial_) / n;
    total_progress_bar_->setValue(total);
    green_progress_bar_->setValue(completed);
}

// NOLINTEND(cppcoreguidelines-owning-memory)