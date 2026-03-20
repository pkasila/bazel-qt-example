//
// Created by blalex on 3/12/25.
//

#include "main_window.h"

#include <QHBoxLayout>
#include <QLineEdit>
#include <QMessageBox>
#include <QPushButton>
#include <QWidget>
#include <cstddef>
#include <qcombobox.h>
#include <qcontainerfwd.h>
#include <qdebug.h>
#include <qdir.h>
#include <qgroupbox.h>
#include <qlabel.h>
#include <qlistwidget.h>
#include <qlogging.h>
#include <qmainwindow.h>
#include <qnamespace.h>
#include <qobject.h>
#include <qprogressbar.h>
#include <qspinbox.h>
#include <qtextedit.h>
#include <random>

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
    , count_(new QSpinBox(this))
    , view_(new QListWidget(this))
    , total_progress_(new QProgressBar(this))
    , green_progress_(new QProgressBar(this))
    , question_view_(new QGroupBox("Enter new ticket name", this))
    , number_label_(new QLabel("№", this))
    , name_label_(new QLabel("Ticket name", this))
    , name_edit_(new QLineEdit("Enter new name", this))
    , status_(new QComboBox(this))
    , next_button_(new QPushButton("Next ticket", this))
    , previous_button_(new QPushButton("Previous ticket", this))
    , load_button_(new QPushButton("Load ticket's", this))
    , save_button_(new QPushButton("Save ticket's", this))
    , description_label_(new QLabel("Description", this))
    , description_(new QTextEdit("Enter information about ticket", this)) {
    CreateWidgets();
    SetupLayout();

    connect(count_, QOverload<int>::of(&QSpinBox::valueChanged), this, &MainWindow::UpdateTickets);
    connect(view_, &QListWidget::itemClicked, this, &MainWindow::OnItemClicked);
    connect(view_, &QListWidget::itemDoubleClicked, this, &MainWindow::OnItemDoubleClicked);
    connect(name_edit_, &QLineEdit::editingFinished, this, &MainWindow::ChangeName);
    connect(status_, &QComboBox::currentIndexChanged, this, &MainWindow::ChangeType);
    connect(next_button_, &QPushButton::clicked, this, &MainWindow::NextTicket);
    connect(previous_button_, &QPushButton::clicked, this, &MainWindow::PreviousTicket);
    connect(description_, &QTextEdit::textChanged, this, &MainWindow::ChangeText);
    connect(load_button_, &QPushButton::clicked, this, &MainWindow::LoadTickets);
    connect(save_button_, &QPushButton::clicked, this, &MainWindow::SaveTickets);

    setWindowTitle("Procrastination");
    resize(500, 700);
}

void MainWindow::CreateWidgets() const {
    status_->addItems({"Default", "Yellow", "Green"});
}

void MainWindow::SetupLayout() {
    count_->setMaximum(1000);
    green_progress_->setStyleSheet(
        "QProgressBar {"
        "border: 2px solid grey;"
        "border-radius: 5px;"
        "}"
        "QProgressBar::chunk {"
        "background: #32CD32;"
        "}");
    green_progress_->setAlignment(Qt::AlignCenter);
    total_progress_->setStyleSheet(
        "QProgressBar {"
        "border: 2px solid grey;"
        "border-radius: 5px;"
        "}"
        "QProgressBar::chunk {"
        "background: #FFD700;"
        "}");
    total_progress_->setAlignment(Qt::AlignCenter);

    auto* main_layout = new QVBoxLayout;
    main_layout->addWidget(total_progress_);
    main_layout->addWidget(green_progress_);
    main_layout->addWidget(count_);
    main_layout->addWidget(view_);

    auto* question_layout = new QGridLayout(question_view_);
    question_layout->addWidget(name_label_, 0, 0);
    question_layout->addWidget(number_label_, 0, 1);
    question_layout->addWidget(name_edit_, 1, 0);
    question_layout->addWidget(status_, 1, 1);

    auto* button_layout = new QHBoxLayout;
    button_layout->addWidget(next_button_);
    button_layout->addWidget(previous_button_);
    button_layout->addWidget(load_button_);
    button_layout->addWidget(save_button_);

    main_layout->addWidget(question_view_);
    main_layout->addWidget(description_label_);
    main_layout->addWidget(description_);
    main_layout->addLayout(button_layout);

    auto* central_widget = new QWidget(this);
    central_widget->setLayout(main_layout);
    setCentralWidget(central_widget);
}

void MainWindow::UpdateTickets(const size_t ticket_count) {
    auto current_count = std::size(tickets_);
    ticket_count_ = ticket_count;

    if (ticket_count >= current_count + 1) {
        tickets_.resize(ticket_count);
        for (size_t i = current_count; i < ticket_count; i++) {
            const Ticket ticket{
              .name = "Ticket №" + QString::number(i + 1), .status = TicketStatus::Default};
            tickets_[i] = ticket;
        }
    }

    view_->clear();
    for (size_t i = 0; i < ticket_count_; i++) {
        auto* item = new QListWidgetItem(tickets_[i].name);

        item->setData(Qt::UserRole, static_cast<int>(i));
        view_->addItem(item);
        switch (tickets_[i].status) {
            case TicketStatus::Default:
                // item->setBackground(Qt::gray);
                break;
            case TicketStatus::Yellow:
                item->setBackground(QColor("#FFD700"));
                break;
            case TicketStatus::Green:
                item->setBackground(Qt::green);
                break;
            default:
                item->setBackground(Qt::black);
        }
    }
    if (ticket_count != 0) {
        UpdateProgressBar();
    }
}

void MainWindow::OnItemClicked(const QListWidgetItem* item) {
    const size_t index = item->data(Qt::UserRole).toUInt();
    tickets_index_history_.push_back(index);
    LoadTicket(static_cast<int>(index));
}

void MainWindow::ChangeName() {
    if (ticket_count_ != 0 && user_change_) {
        tickets_[current_ticket_].name = name_edit_->text();
        UpdateTickets(ticket_count_);
    }
}

void MainWindow::LoadTicket(int index) {
    if (index < tickets_.size()) {
        user_change_ = false;
        number_label_->setText("№" + QString::number(index + 1));
        name_edit_->setText(tickets_[index].name);
        description_->setText(tickets_[index].description);
        status_->setCurrentIndex(static_cast<int>(tickets_[index].status));
        current_ticket_ = index;
        user_change_ = true;
    }
}

void MainWindow::OnItemDoubleClicked(const QListWidgetItem* item) {
    if (const size_t index = item->data(Qt::UserRole).toUInt(); index < tickets_.size()) {
        switch (tickets_[index].status) {
            case TicketStatus::Default:
                tickets_[index].status = TicketStatus::Green;
                break;
            case TicketStatus::Yellow:
                tickets_[index].status = TicketStatus::Green;
                break;
            case TicketStatus::Green:
                tickets_[index].status = TicketStatus::Yellow;
                break;
            default:
                tickets_[index].status = TicketStatus::Default;
        }
        current_ticket_ = index;
        UpdateTickets(ticket_count_);
        LoadTicket(static_cast<int>(index));
    }
}

void MainWindow::ChangeType(const int index) {
    if (index != -1 && user_change_ && std::size(tickets_) != 0) {
        tickets_[current_ticket_].status = static_cast<TicketStatus>(index);
        UpdateTickets(ticket_count_);
    }
}

void MainWindow::UpdateProgressBar() const {
    size_t green_counter = 0;
    size_t default_counter = 0;
    for (size_t i = 0; i < ticket_count_; i++) {
        if (tickets_[i].status == TicketStatus::Default) {
            default_counter++;
        }
        if (tickets_[i].status == TicketStatus::Green) {
            green_counter++;
        }
    }

    total_progress_->setValue(round((ticket_count_ - default_counter) * 100 / ticket_count_));
    green_progress_->setValue(round((green_counter) * 100 / ticket_count_));
}

void MainWindow::NextTicket() {
    std::vector<int> valid_indices;

    for (size_t i = 0; i < ticket_count_; ++i) {
        if (tickets_[i].status == TicketStatus::Default ||
            tickets_[i].status == TicketStatus::Yellow) {
            valid_indices.push_back(i);
        }
    }

    if (valid_indices.empty()) {
        QMessageBox::information(nullptr, "Information", "All tickets learned!");
        return;
    }

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dist(0, valid_indices.size() - 1);

    auto index = valid_indices[dist(gen)];
    tickets_index_history_.push_back(index);
    tickets_[index].status = TicketStatus::Yellow;
    UpdateTickets(ticket_count_);
    view_->item(index)->setSelected(true);
    LoadTicket(index);
}

void MainWindow::PreviousTicket() {
    if (tickets_index_history_.empty()) {
        QMessageBox::information(nullptr, "Information", "History is clear");
        return;
    }

    auto index = tickets_index_history_.back();
    tickets_index_history_.pop_back();

    if (index > ticket_count_ - 1) {
        PreviousTicket();
        return;
    }

    view_->item(index)->setSelected(true);
    LoadTicket(index);
}

void MainWindow::ChangeText() {
    if (ticket_count_ != 0 && user_change_) {
        tickets_[current_ticket_].description = description_->toPlainText();
    }
}

void MainWindow::SaveTickets() {
    QFile file(filepath_);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        qWarning() << "Open error:" << filepath_;
        return;
    }

    QTextStream out(&file);
    for (size_t i = 0; i < ticket_count_; ++i) {
        out << tickets_[i].name << "||" << static_cast<int>(tickets_[i].status) << "||" << tickets_[i].description << "\n";
    }
}

void MainWindow::LoadTickets() {
    QFile file(filepath_);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qWarning() << "Open error:" << filepath_;
        return;
    }

    tickets_.clear();
    QTextStream in(&file);
    while (!in.atEnd()) {
        const QString line = in.readLine().trimmed();
        if (line.isEmpty()) {
            continue;
        }

        QStringList parts = line.split("||");
        if (parts.size() != 3) {
            qWarning() << "Wrong format at" << line << "line";
            continue;
        }

        Ticket ticket{
          .name = parts[0],
          .status = static_cast<TicketStatus>(parts[1].toInt()),
          .description = parts[2]};
        tickets_.push_back(ticket);
    }
    tickets_index_history_.clear();
    ticket_count_ = std::size(tickets_);
    UpdateTickets(ticket_count_);
    count_->setValue(static_cast<int>(ticket_count_));
}