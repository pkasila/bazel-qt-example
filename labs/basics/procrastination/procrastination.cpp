#include "procrastination.h"

#include <QApplication>
#include <QBrush>
#include <QColor>
#include <QComboBox>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QLabel>
#include <QLineEdit>
#include <QMainWindow>
#include <QProgressBar>
#include <QPushButton>
#include <QRandomGenerator>
#include <QSpinBox>
#include <QTableWidget>
#include <QVBoxLayout>
#include <ctime>
#include <stdexcept>

Ticket::Ticket(const size_t& number) : number_(number), name_(QString("Билет %1").arg(number_)) {
}

void Ticket::setNumber(const size_t& number) {
    number_ = number;
    name_ = QString("Билет %1").arg(number_);
}

const size_t& Ticket::getNumber() const {
    return number_;
}

void Ticket::setName(const QString& name) {
    name_ = name;
}

const QString& Ticket::getName() const {
    return name_;
}

void Ticket::setState(const TicketState& state) {
    state_ = state;
}

const TicketState& Ticket::getState() const {
    return state_;
}

void Ticket::nextState() {
    switch (state_) {
        case TicketState::Default:
            state_ = TicketState::Done;
            break;
        case TicketState::InProgress:
            state_ = TicketState::Done;
            break;
        case TicketState::Done:
            state_ = TicketState::InProgress;
            break;
    }
}

MainWindow::MainWindow(QWidget* parent)  // NOLINT
    : QMainWindow(parent)
    , count_(new QSpinBox)
    , view_(new QTableWidget)
    , ticket_view_(new QGroupBox)
    , total_progress_(new QProgressBar)
    , green_progress_(new QProgressBar) {
    auto* central = new QWidget(this);  // NOLINT
    setCentralWidget(central);

    view_->setColumnCount(1);
    view_->horizontalHeader()->setStretchLastSection(true);
    view_->verticalHeader()->setVisible(false);
    view_->horizontalHeader()->setVisible(false);
    view_->setSelectionMode(QAbstractItemView::NoSelection);
    view_->setEditTriggers(QAbstractItemView::NoEditTriggers);

    hideTicketView();

    auto* bottom_layout = new QHBoxLayout;  // NOLINT

    count_->setMinimum(1);
    count_->setMaximum(1000);
    count_->setValue(3);

    tickets_.reserve(1000);
    undone_tickets_ind_.reserve(1000);

    reset_button_ = new QPushButton("Сбросить прогресс");       // NOLINT

    prev_ticket_button_ = new QPushButton("Предыдущий билет");  // NOLINT
    next_ticket_button_ = new QPushButton("Следующий билет");   // NOLINT

    auto* count_edit_layout = new QHBoxLayout;

    auto* count_edit_label = new QLabel(QString("Количество билетов:"));       // NOLINT
    count_edit_label->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

    count_edit_layout->addWidget(count_edit_label);
    count_edit_layout->addWidget(count_);
    count_edit_layout->setSpacing(0);

    bottom_layout->addLayout(count_edit_layout);
    bottom_layout->addWidget(reset_button_);
    bottom_layout->addWidget(prev_ticket_button_);
    bottom_layout->addWidget(next_ticket_button_);

    auto* progress_layout = new QVBoxLayout;  // NOLINT

    total_progress_->setMinimum(0);
    total_progress_->setMaximum(count_->value());

    green_progress_->setMinimum(0);
    green_progress_->setMaximum(count_->value());

    auto* total_progress_label = new QLabel(QString("Просмотренные билеты: "));    // NOLINT
    auto* green_progress_label = new QLabel(QString("Выученные билеты: "));  // NOLINT

    total_progress_label->setAlignment(Qt::AlignCenter);
    green_progress_label->setAlignment(Qt::AlignCenter);

    progress_layout->addWidget(total_progress_label);
    progress_layout->addWidget(total_progress_);
    progress_layout->addWidget(green_progress_label);
    progress_layout->addWidget(green_progress_);

    auto* main_layout = new QVBoxLayout;  // NOLINT
    main_layout->addWidget(ticket_view_);
    main_layout->addWidget(view_);
    main_layout->addLayout(progress_layout);
    main_layout->addLayout(bottom_layout);

    connect(count_, &QSpinBox::valueChanged, this, &MainWindow::countChanged);
    connect(reset_button_, &QPushButton::clicked, this, &MainWindow::resetProgress);
    connect(view_, &QTableWidget::cellClicked, this, &MainWindow::ticketChosen);
    connect(view_, &QTableWidget::cellDoubleClicked, this, &MainWindow::toNextTicketState);
    connect(this, &MainWindow::progressChanged, this, &MainWindow::updateProgressBars);
    connect(prev_ticket_button_, &QPushButton::clicked, this, &MainWindow::choosePrevTicket);
    connect(next_ticket_button_, &QPushButton::clicked, this, &MainWindow::chooseNextTicket);

    countChanged();

    updateProgressBars(seen_tickets_, done_tickets_);

    central->setLayout(main_layout);
}

void MainWindow::countChanged() {
    tickets_.clear();
    tickets_.resize(count_->value());

    hideTicketView(false);
    seen_tickets_ind_.clear();

    for (size_t i = 0; i < tickets_.size(); i++) {
        tickets_.at(i).setNumber(i + 1);
    }
    resetView();

    seen_tickets_ = 0;
    done_tickets_ = 0;
    total_progress_->setMaximum(count_->value());
    green_progress_->setMaximum(count_->value());
    emit progressChanged(seen_tickets_, done_tickets_);
}

void MainWindow::resetProgress() {
    for (auto& ticket : tickets_) {
        stateChanged(ticket.getState(), TicketState::Default);
        ticket.setState(TicketState::Default);
    }
    resetView();
    hideTicketView(false);
    seen_tickets_ind_.clear();

    seen_tickets_ = 0;
    done_tickets_ = 0;
    emit progressChanged(seen_tickets_, done_tickets_);
}

void MainWindow::ticketChosen(const int row, const int /**/) {
    showTicketView(tickets_.at(row));
}

void MainWindow::toNextTicketState(const int row, const int /**/) {
    const TicketState prev_state = tickets_.at(row).getState();
    tickets_.at(row).nextState();
    stateChanged(prev_state, tickets_.at(row).getState());
    if (chosen_ticket_ == &tickets_.at(row)) {
        showTicketView(tickets_.at(row), false);
    }
    resetView();
}

void MainWindow::updateTicketName(Ticket& ticket, const QString& new_name) const {
    ticket.setName(new_name);
    resetView();
}

void MainWindow::updateTicketState(Ticket& ticket, const TicketState new_state) {
    stateChanged(ticket.getState(), new_state);
    ticket.setState(new_state);
    resetView();
}

void MainWindow::updateProgressBars(const size_t& seen, const size_t& done) const {
    total_progress_->setValue(seen);
    green_progress_->setValue(done);
}

void MainWindow::choosePrevTicket() {
    if (seen_tickets_ind_.empty()) {
        return;
    }
    const size_t prev_ticket_ind = seen_tickets_ind_.back();
    seen_tickets_ind_.pop_back();
    showTicketView(tickets_.at(prev_ticket_ind), false);
}

void MainWindow::chooseNextTicket() {
    undone_tickets_ind_.clear();
    size_t chosen_ticket_ind = -1;
    if (chosen_ticket_ != nullptr) {
        chosen_ticket_ind = chosen_ticket_->getNumber() - 1;
    }
    for (size_t i = 0; i < tickets_.size(); i++) {
        if (tickets_.at(i).getState() == TicketState::Done) {
            continue;
        }
        if (i == chosen_ticket_ind) {
            continue;
        }
        undone_tickets_ind_.emplace_back(i);
    }
    if (undone_tickets_ind_.empty()) {
        return;
    }
    const size_t ind = random_.bounded(static_cast<quint64>(undone_tickets_ind_.size()));
    showTicketView(tickets_.at(undone_tickets_ind_.at(ind)));
}

void MainWindow::deleteLayout(QLayout* layout) {
    if (layout == nullptr) {
        return;
    }

    QLayoutItem* item;
    while ((item = layout->takeAt(0)) != nullptr) {
        if (item->widget() != nullptr) {
            delete item->widget();
        } else if (item->layout() != nullptr) {
            deleteLayout(item->layout());
            continue;
        } else if (item->spacerItem() != nullptr) {
            delete item->spacerItem();
            continue;
        } else {
            throw std::runtime_error("MainWindow::deleteLayout QLayoutItem is not valid");
        }
        delete item;
    }

    delete layout;
}

void MainWindow::resetView() const {
    view_->setRowCount(tickets_.size());
    for (size_t row = 0; row < tickets_.size(); row++) {
        auto* ticket = new QTableWidgetItem(tickets_.at(row).getName());
        switch (tickets_.at(row).getState()) {
            case TicketState::Default:
                ticket->setBackground(QBrush(QColor(128, 128, 128)));
                ticket->setForeground(QBrush(QColor(255, 255, 255)));
                break;
            case TicketState::InProgress:
                ticket->setBackground(QBrush(QColor(255, 255, 0)));
                ticket->setForeground(QBrush(QColor(0, 0, 0)));
                break;
            case TicketState::Done:
                ticket->setBackground(QBrush(QColor(0, 255, 0)));
                ticket->setForeground(QBrush(QColor(0, 0, 0)));
                break;
        };
        view_->setItem(row, 0, ticket);
    }
    view_->resizeRowsToContents();
}

void MainWindow::showTicketView(Ticket& ticket, bool forward) {
    if (chosen_ticket_ != nullptr && chosen_ticket_->getNumber() == ticket.getNumber()) {
        forward = false;
    }

    hideTicketView(forward);
    ticket_view_->setFlat(false);

    chosen_ticket_ = &ticket;

    view_->setCurrentCell(ticket.getNumber() - 1, 0);
    view_->setFocus();

    auto* layout = new QVBoxLayout;                                             // NOLINT

    auto* number = new QLabel(QString("Билет No.%1").arg(ticket.getNumber()));  // NOLINT
    auto* name = new QLabel(ticket.getName());                                  // NOLINT

    layout->addWidget(number);
    layout->addWidget(name);

    auto* name_edit_layout = new QHBoxLayout;                                // NOLINT

    auto* name_edit_label = new QLabel(QString("Новое название билета: "));  // NOLINT
    auto* name_edit = new QLineEdit;                                         // NOLINT

    connect(name_edit, &QLineEdit::returnPressed, [&ticket, name, name_edit, this]() {
        if (!name_edit->text().isEmpty()) {
            name->setText(name_edit->text());
            this->updateTicketName(ticket, name_edit->text());
        }
    });

    name_edit_layout->addWidget(name_edit_label);
    name_edit_layout->addWidget(name_edit);
    name_edit_layout->addStretch(1);

    layout->addLayout(name_edit_layout);

    auto* state_edit_layout = new QHBoxLayout;                        // NOLINT

    auto* state_edit_label = new QLabel(QString("Статус билета: "));  // NOLINT
    auto* state_edit = new QComboBox;                                 // NOLINT

    state_edit->addItem("Не повторял");
    state_edit->addItem("Плохо помню");
    state_edit->addItem("Запомнил");
    state_edit->setCurrentIndex(static_cast<int>(ticket.getState()));

    connect(state_edit, &QComboBox::currentIndexChanged, [&ticket, state_edit, this]() {
        this->updateTicketState(ticket, static_cast<TicketState>(state_edit->currentIndex()));
    });

    state_edit_layout->addWidget(state_edit_label);
    state_edit_layout->addWidget(state_edit);
    state_edit_layout->addStretch(1);

    layout->addLayout(state_edit_layout);

    ticket_view_->setLayout(layout);
}

void MainWindow::hideTicketView(const bool forward) {
    deleteLayout(ticket_view_->layout());
    if (chosen_ticket_ != nullptr && forward) {
        if (seen_tickets_ind_.empty() ||
            seen_tickets_ind_.back() != chosen_ticket_->getNumber() - 1) {
            if (seen_tickets_ind_.size() == k_max_roll_back_) {
                seen_tickets_ind_.pop_front();
            }
            seen_tickets_ind_.emplace_back(chosen_ticket_->getNumber() - 1);
        }
    }
    chosen_ticket_ = nullptr;
    ticket_view_->setFlat(true);
}

void MainWindow::stateChanged(const TicketState& prev_state, const TicketState& new_state) {
    switch (static_cast<int>(prev_state)) {
        case 1:
            seen_tickets_--;
            break;
        case 2:
            seen_tickets_--;
            done_tickets_--;
        default:
            break;
    }
    switch (static_cast<int>(new_state)) {
        case 1:
            seen_tickets_++;
            break;
        case 2:
            seen_tickets_++;
            done_tickets_++;
        default:
            break;
    }
    emit progressChanged(seen_tickets_, done_tickets_);
}
