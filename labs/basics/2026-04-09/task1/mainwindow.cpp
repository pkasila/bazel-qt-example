#include "mainwindow.h"

#include <QAbstractItemView>
#include <QColor>
#include <QComboBox>
#include <QFormLayout>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QLabel>
#include <QLineEdit>
#include <QProgressBar>
#include <QPushButton>
#include <QRandomGenerator>
#include <QSignalBlocker>
#include <QSpinBox>
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QVBoxLayout>
#include <QWidget>

namespace {
QString DefaultTicketName(const int number) {
    return QString("Билет %1").arg(number);
}
}  // namespace

MainWindow::MainWindow(QWidget* parent) : QMainWindow(parent) {
    SetupUi();
    SetupConnections();

    count_spin_box_->setMinimum(1);
    count_spin_box_->setMaximum(1000); 
    count_spin_box_->setValue(10);

    RebuildTickets(count_spin_box_->value());
}

void MainWindow::SetupUi() {
    auto* central_widget = new QWidget(this);
    setCentralWidget(central_widget);
    setWindowTitle("Повторение билетов");

    count_spin_box_ = new QSpinBox(this);
    view_table_widget_ = new QTableWidget(this);

    number_value_label_ = new QLabel("-", this);
    name_value_label_ = new QLabel("-", this);
    name_line_edit_ = new QLineEdit(this);
    status_combo_box_ = new QComboBox(this);

    next_question_button_ = new QPushButton("Следующий случайный билет", this);
    previous_question_button_ = new QPushButton("Предыдущий билет", this);

    total_progress_bar_ = new QProgressBar(this);
    green_progress_bar_ = new QProgressBar(this);

    view_table_widget_->setColumnCount(1);
    view_table_widget_->setHorizontalHeaderLabels({"Билеты"});
    view_table_widget_->horizontalHeader()->setStretchLastSection(true);
    view_table_widget_->verticalHeader()->setVisible(false);
    view_table_widget_->setSelectionBehavior(QAbstractItemView::SelectRows);
    view_table_widget_->setSelectionMode(QAbstractItemView::SingleSelection);
    view_table_widget_->setEditTriggers(QAbstractItemView::NoEditTriggers);

    status_combo_box_->addItem("Не повторял");
    status_combo_box_->addItem("Повторить ещё");
    status_combo_box_->addItem("Повторён");

    auto* top_layout = new QHBoxLayout();
    top_layout->addWidget(new QLabel("Количество билетов:", this));
    top_layout->addWidget(count_spin_box_);
    top_layout->addStretch();

    auto* question_group_box = new QGroupBox("Текущий билет", this);
    auto* question_layout = new QFormLayout(question_group_box);
    question_layout->addRow("Номер:", number_value_label_);
    question_layout->addRow("Имя:", name_value_label_);
    question_layout->addRow("Новое имя:", name_line_edit_);
    question_layout->addRow("Статус:", status_combo_box_);

    auto* buttons_layout = new QHBoxLayout();
    buttons_layout->addWidget(previous_question_button_);
    buttons_layout->addWidget(next_question_button_);

    auto* right_layout = new QVBoxLayout();
    right_layout->addWidget(question_group_box);
    right_layout->addLayout(buttons_layout);
    right_layout->addWidget(total_progress_bar_);
    right_layout->addWidget(green_progress_bar_);
    right_layout->addStretch();

    auto* main_layout = new QHBoxLayout();
    main_layout->addWidget(view_table_widget_, 3);
    main_layout->addLayout(right_layout, 2);

    auto* root_layout = new QVBoxLayout(central_widget);
    root_layout->addLayout(top_layout);
    root_layout->addLayout(main_layout);

    total_progress_bar_->setMinimum(0);
    green_progress_bar_->setMinimum(0);
}

void MainWindow::SetupConnections() {
    connect(count_spin_box_,
            &QSpinBox::valueChanged,
            this,
            &MainWindow::OnTicketCountChanged);

    connect(view_table_widget_,
            &QTableWidget::cellClicked,
            this,
            &MainWindow::OnTicketCellClicked);

    connect(view_table_widget_,
            &QTableWidget::cellDoubleClicked,
            this,
            &MainWindow::OnTicketCellDoubleClicked);

    connect(status_combo_box_,
            &QComboBox::currentIndexChanged,
            this,
            &MainWindow::OnStatusComboBoxChanged);

    connect(name_line_edit_,
            &QLineEdit::returnPressed,
            this,
            &MainWindow::OnNameEditReturnPressed);

    connect(next_question_button_,
            &QPushButton::clicked,
            this,
            &MainWindow::OnNextQuestionClicked);

    connect(previous_question_button_,
            &QPushButton::clicked,
            this,
            &MainWindow::OnPreviousQuestionClicked);
}

void MainWindow::RebuildTickets(const int count) {
    tickets_.clear();
    tickets_.resize(count);
    history_.clear();
    current_index_ = -1;

    for (int index = 0; index < count; ++index) {
        tickets_[index].name = DefaultTicketName(index + 1);
        tickets_[index].status = TicketStatus::kDefault;
    }

    view_table_widget_->clearContents();
    view_table_widget_->setRowCount(count);

    for (int index = 0; index < count; ++index) {
        UpdateTableRow(index);
    }

    total_progress_bar_->setMaximum(count);
    green_progress_bar_->setMaximum(count);
    UpdateProgressBars();

    if (count > 0) {
        SelectTicket(0, false);
    } else {
        ClearQuestionView();
    }
}

void MainWindow::UpdateTableRow(const int index) {
    if (index < 0 || index >= tickets_.size()) {
        return;
    }

    QTableWidgetItem* item = view_table_widget_->item(index, 0);
    if (item == nullptr) {
        item = new QTableWidgetItem();
        view_table_widget_->setItem(index, 0, item);
    }

    const Ticket& ticket = tickets_[index];
    item->setText(ticket.name);

    switch (ticket.status) {
        case TicketStatus::kDefault:
            item->setBackground(QColor(180, 180, 180));
            item->setToolTip(QString("№%1\nСтатус: Не повторял").arg(index + 1));
            break;
        case TicketStatus::kYellow:
            item->setBackground(QColor(255, 230, 120));
            item->setToolTip(QString("№%1\nСтатус: Повторить ещё").arg(index + 1));
            break;
        case TicketStatus::kGreen:
            item->setBackground(QColor(140, 220, 140));
            item->setToolTip(QString("№%1\nСтатус: Повторён").arg(index + 1));
            break;
    }
}

void MainWindow::UpdateQuestionView() {
    if (current_index_ < 0 || current_index_ >= tickets_.size()) {
        ClearQuestionView();
        return;
    }

    const Ticket& ticket = tickets_[current_index_];

    number_value_label_->setText(QString::number(current_index_ + 1));
    name_value_label_->setText(ticket.name);

    {
        const QSignalBlocker blocker(status_combo_box_);
        switch (ticket.status) {
            case TicketStatus::kDefault:
                status_combo_box_->setCurrentIndex(0);
                break;
            case TicketStatus::kYellow:
                status_combo_box_->setCurrentIndex(1);
                break;
            case TicketStatus::kGreen:
                status_combo_box_->setCurrentIndex(2);
                break;
        }
    }

    name_line_edit_->clear();
}

void MainWindow::ClearQuestionView() {
    number_value_label_->setText("-");
    name_value_label_->setText("-");
    name_line_edit_->clear();

    const QSignalBlocker blocker(status_combo_box_);
    status_combo_box_->setCurrentIndex(0);
}

void MainWindow::SelectTicket(const int index, const bool push_to_history) {
    if (index < 0 || index >= tickets_.size()) {
        return;
    }

    if (push_to_history && current_index_ >= 0 && current_index_ != index) {
        history_.push_back(current_index_);
    }

    current_index_ = index;
    view_table_widget_->selectRow(index);
    UpdateQuestionView();
}

void MainWindow::SetTicketStatus(const int index, const TicketStatus new_status) {
    if (index < 0 || index >= tickets_.size()) {
        return;
    }

    tickets_[index].status = new_status;
    UpdateTableRow(index);
    UpdateProgressBars();

    if (index == current_index_) {
        UpdateQuestionView();
    }
}

void MainWindow::UpdateProgressBars() {
    int total_done = 0;
    int green_done = 0;

    for (const Ticket& ticket : tickets_) {
        if (ticket.status == TicketStatus::kYellow ||
            ticket.status == TicketStatus::kGreen) {
            ++total_done;
        }

        if (ticket.status == TicketStatus::kGreen) {
            ++green_done;
        }
    }

    total_progress_bar_->setValue(total_done);
    green_progress_bar_->setValue(green_done);

    total_progress_bar_->setFormat(
        QString("Общий прогресс: %1 / %2").arg(total_done).arg(tickets_.size()));
    green_progress_bar_->setFormat(
        QString("Повторено полностью: %1 / %2").arg(green_done).arg(tickets_.size()));
}

void MainWindow::OnTicketCountChanged(const int value) {
    RebuildTickets(value);
}

void MainWindow::OnTicketCellClicked(const int row, const int /*column*/) {
    SelectTicket(row, true);
}

void MainWindow::OnTicketCellDoubleClicked(const int row, const int /*column*/) {
    if (row < 0 || row >= tickets_.size()) {
        return;
    }

    const TicketStatus current_status = tickets_[row].status;
    const TicketStatus next_status =
        current_status == TicketStatus::kGreen ? TicketStatus::kYellow
                                               : TicketStatus::kGreen;

    SetTicketStatus(row, next_status);
    SelectTicket(row, false);
}

void MainWindow::OnStatusComboBoxChanged(const int index) {
    if (current_index_ < 0 || current_index_ >= tickets_.size()) {
        return;
    }

    TicketStatus new_status = TicketStatus::kDefault;
    switch (index) {
        case 1:
            new_status = TicketStatus::kYellow;
            break;
        case 2:
            new_status = TicketStatus::kGreen;
            break;
        case 0:
        default:
            new_status = TicketStatus::kDefault;
            break;
    }

    SetTicketStatus(current_index_, new_status);
}

void MainWindow::OnNameEditReturnPressed() {
    if (!name_line_edit_->hasFocus()) {
        return;
    }

    if (current_index_ < 0 || current_index_ >= tickets_.size()) {
        return;
    }

    const QString new_name = name_line_edit_->text().trimmed();
    if (new_name.isEmpty()) {
        return;
    }

    tickets_[current_index_].name = new_name;
    name_value_label_->setText(new_name);
    UpdateTableRow(current_index_);
    name_line_edit_->clear();
}

void MainWindow::OnNextQuestionClicked() {
    QVector<int> available_indexes;
    available_indexes.reserve(tickets_.size());

    for (int index = 0; index < tickets_.size(); ++index) {
        if (tickets_[index].status == TicketStatus::kDefault ||
            tickets_[index].status == TicketStatus::kYellow) {
            available_indexes.push_back(index);
        }
    }

    if (available_indexes.isEmpty()) {
        return;
    }

    const int random_position =
        QRandomGenerator::global()->bounded(available_indexes.size());
    SelectTicket(available_indexes[random_position], true);
}

void MainWindow::OnPreviousQuestionClicked() {
    if (history_.isEmpty()) {
        return;
    }

    const int previous_index = history_.takeLast();
    SelectTicket(previous_index, false);
}