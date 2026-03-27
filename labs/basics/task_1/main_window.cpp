#include <iostream>
#include "main_window.h"
#include "Ticket.h"
#include <random>
#include <string>
#include <QtWidgets>

MainWindow::MainWindow() {
    // default selected items
    selected_row_ = -1;
    selected_column_ = -1;

    QWidget *central_widget = new QWidget;
    setCentralWidget(central_widget);

    // count of tickets
    count_box_ = new QSpinBox(this);
    count_box_->setValue(0);

    // represent the info of ticket
    view_ = new QTableWidget(this);
    view_->setEditTriggers(QAbstractItemView::NoEditTriggers);
    view_->setColumnCount(3);
    view_->setHorizontalHeaderLabels({"Номер", "Название", "Статус"});
    view_->verticalHeader()->setVisible(false);

    view_->setSelectionBehavior(QAbstractItemView::SelectRows);
    view_->setSelectionMode(QAbstractItemView::SingleSelection);
    view_->setSelectionBehavior(QAbstractItemView::SelectRows);

    view_->setColumnWidth(0, 60);
    view_->setColumnWidth(2, 100);

    QHeaderView *header = view_->horizontalHeader();
    header->setSectionResizeMode(0, QHeaderView::Fixed);
    header->setSectionResizeMode(1, QHeaderView::Stretch);
    header->setSectionResizeMode(2, QHeaderView::Fixed);
    header->setDefaultAlignment(Qt::AlignCenter);

    // labels that represent selected number and name of ticket
    number_label_ = new QLabel("Номер", this);
    name_label_ = new QLabel("Название", this);

    // field for editing name of selected ticket
    name_edit_ = new QLineEdit(this);

    // set a status of selected ticket
    status_box_ = new QComboBox(this);
    status_box_->addItems({"Не выучен", "Повторить", "Выучен"});

    QGroupBox *question_view = new QGroupBox(this);
    QVBoxLayout *qv_layout = new QVBoxLayout();
    qv_layout->addWidget(number_label_);
    qv_layout->addWidget(name_label_);
    qv_layout->addWidget(name_edit_);
    qv_layout->addWidget(status_box_);
    question_view->setLayout(qv_layout);

    // next not green question
    next_question_button_ = new QPushButton("Новый билет", this);
    next_question_button_->setFixedSize(140, 40);

    // previous question
    previous_question_button_ = new QPushButton("Предыдущий билет", this);
    previous_question_button_->setFixedSize(140, 40);

    // layout for buttons
    QHBoxLayout *buttons_layout = new QHBoxLayout();
    buttons_layout->addStretch();
    buttons_layout->addWidget(next_question_button_);
    buttons_layout->addSpacing(20);
    buttons_layout->addWidget(previous_question_button_);
    buttons_layout->addStretch();

    // progress based on not grey tickets
    total_progress_ = new QProgressBar(this);
    total_progress_->setValue(0);
    total_progress_->setFormat("%p%");
    total_progress_->setFixedHeight(20);
    total_progress_->setStyleSheet("QProgressBar {"
        "height: 20px;"
        "border-radius: 10px;"
        "background-color: #dcdcdc;"
        "}"
        "QProgressBar::chunk {"
        "background-color: #E0CE06;" // yellow
        "border-radius: 10px;"
        "}");

    // progress based on green tickets
    green_progress_ = new QProgressBar(this);
    green_progress_->setValue(0);
    green_progress_->setFormat("%p%");
    green_progress_->setFixedHeight(20);
    green_progress_->setStyleSheet("QProgressBar {"
        "height: 20px;"
        "border-radius: 10px;"
        "background-color: #dcdcdc;"
        "}"
        "QProgressBar::chunk {"
        "background-color: #27A82B;" // green
        "border-radius: 10px;"
        "}");

    // main layout
    QVBoxLayout *main_layout = new QVBoxLayout();
    main_layout->setContentsMargins(5, 5, 5, 5);

    // count layout
    QHBoxLayout *count_layout = new QHBoxLayout();
    QLabel *count_label = new QLabel("Количество билетов", this);
    count_layout->addStretch();
    count_layout->addWidget(count_label);
    count_layout->addWidget(count_box_);
    count_layout->addStretch();

    // form contents of the app
    main_layout->addLayout(count_layout);
    main_layout->addSpacing(10);
    main_layout->addWidget(view_);
    main_layout->addSpacing(10);
    main_layout->addLayout(buttons_layout);
    main_layout->addSpacing(10);
    main_layout->addWidget(question_view);
    main_layout->addSpacing(10);
    main_layout->addWidget(total_progress_);
    main_layout->addWidget(green_progress_);

    central_widget->setLayout(main_layout);

    createActions();

    setWindowTitle(tr("Билеты МА"));
}

void MainWindow::updateRowColor(QColor color) {
    for (int i = 0; i < view_->columnCount(); ++i) {
        view_->item(selected_row_, i)->setBackground(color);
    }
}

// update information of selected ticket by its row
void MainWindow::updateTicket(int row) {
    if (row >= 0 && row < tickets_.size()) {
        Ticket &ticket = tickets_[row];

        // delete old ticket
        revision_tickets_.erase(ticket);

        // update progress bars counts
        auto new_status = static_cast<Ticket::Status>(status_box_->currentIndex());
        if (ticket.getStatus() != new_status) {
            if (new_status == Ticket::DEFAULT) {
                grey_count_++;
            } else if (ticket.getStatus() == Ticket::DEFAULT) {
                grey_count_--;
            }
        }

        // set new info
        ticket.setName(ticket.getName());
        ticket.setStatus(static_cast<Ticket::Status>(status_box_->currentIndex()));

        // insert new ticket
        if (ticket.getStatus() != Ticket::GREEN) {
            revision_tickets_.insert(ticket);
        }

        updateProgress();
    }
}

// update progress bars
void MainWindow::updateProgress() {
    total_progress_->setValue((tickets_.size() - grey_count_) * 100 / tickets_.size());
    green_progress_->setValue((tickets_.size() - revision_tickets_.size()) * 100 / tickets_.size());
}

// update ticket visually in the app
void MainWindow::onEnterPressed() {
    if (selected_row_ >= 0) {
        // update view table
        view_->setItem(selected_row_, 1, new QTableWidgetItem(name_edit_->text()));

        // update row color
        int new_status = status_box_->currentIndex();
        QColor selected_color = status_data[new_status].second;
        for (int i = 0; i < view_->columnCount(); ++i) {
            view_->item(selected_row_, i)->setBackground(selected_color);
        }

        // update name label
        name_label_->setText(name_edit_->text());

        updateTicket(selected_row_);
    }
}

// change status
void MainWindow::statusChanged() {
    if (selected_row_ >= 0) {
        // get current color in format of int
        int new_status = status_box_->currentIndex();

        // update status in view table
        view_->setItem(selected_row_, 2, new QTableWidgetItem(status_data[new_status].first.c_str()));

        // update row color
        QColor selected_color = status_data[new_status].second;
        updateRowColor(selected_color);

        // update name label
        name_label_->setText(name_edit_->text());

        updateTicket(selected_row_);
    }
}

// select ticket by certain row of view table
void MainWindow::chooseTicket(int row) {
    // push ticket to stack
    tickets_stack_.push(selected_ticket_);
    std::cout << "add to stack in choose ticket: " << tickets_stack_.top().getNumber() << std::endl;

    // remember selected ticket
    selected_ticket_ = tickets_[row];

    onCellClicked(row);
}

// update labels by info of selected ticket
void MainWindow::onCellClicked(int row) {
    view_->selectRow(row);
    selected_row_ = row;

    // update labels (and status box)
    QTableWidgetItem *item = view_->item(row, 1);
    if (item) {
        name_label_->setText(item->text());
    } else {
        name_label_->setText("");
    }
    number_label_->setText(QString::number(row + 1));
    name_edit_->setText(item ? item->text() : "");
    status_box_->setCurrentIndex(tickets_[row].getStatus());
    std::cout << "end of cell clicked" << std::endl;
}

// change status by double click
void MainWindow::onCellDoubleClicked() {
    // push ticket to stack
    tickets_stack_.push(selected_ticket_);

    selected_ticket_ = tickets_[selected_row_];

    // update status
    Ticket::Status new_status;
    if (selected_ticket_.getStatus() == Ticket::GREEN) {
        new_status = Ticket::YELLOW;
    } else {
        new_status = Ticket::GREEN;
    }

    status_box_->setCurrentIndex(new_status);
    view_->setItem(selected_row_, 2, new QTableWidgetItem(status_data[new_status].first.c_str()));

    // update row color
    updateRowColor(status_data[new_status].second);

    updateTicket(selected_row_);
}

// update/create table of tickets
void MainWindow::updateTable() {

    // set default values of widgets
    number_label_->setText("");
    name_label_->setText("");
    name_edit_->setText("");
    green_progress_->setValue(0);
    total_progress_->setValue(0);

    if (!view_) {
        std::cerr << "Ошибка: view не инициализирован" << std::endl;
        return;
    }

    // clear containers
    while (!tickets_stack_.empty()) {
        tickets_stack_.pop();
    }
    std::cout << "stack: " << tickets_stack_.size() << std::endl;
    if (!revision_tickets_.empty()) {
        revision_tickets_.clear();
    }

    // set row count
    view_->setRowCount(count_box_->value());


    grey_count_ = count_box_->value();

    // set data
    for (int row = 0; row < count_box_->value(); row++) {
        QTableWidgetItem *item0 = new QTableWidgetItem(
            QString("%1").arg(row + 1));
        view_->setItem(row, 0, item0);

        QTableWidgetItem *item1 = new QTableWidgetItem(
            QString("Билет %1").arg(row + 1));
        view_->setItem(row, 1, item1);

        QTableWidgetItem *item2 = new QTableWidgetItem(
            QString("Не выучен"));
        view_->setItem(row, 2, item2);

        auto ticket = Ticket(row + 1, "Билет" + std::to_string(row + 1));
        std::cout << "ticket " << ticket.getName() << std::endl;

        // add ticket to containers
        tickets_.push_back(ticket);
        revision_tickets_.emplace(ticket);

        // update row color
        for (int i = 0; i < view_->columnCount(); ++i) {
            view_->item(row, i)->setBackground(status_data[0].second);
        }
    }
    selected_ticket_ = tickets_[0];
    onCellClicked(0);

}

// get random not green ticket
void MainWindow::getRandomTicket() {
    // check if exists not green ticket
    if (revision_tickets_.empty()) {
        std::cout << "Все билеты повторены" << std::endl;
        return;
    }
    // selected ticket is random ticket
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> distr(0, revision_tickets_.size() - 1);
    int const random_number = distr(gen);
    tickets_stack_.push(selected_ticket_);
    std::cout << "add to stack in random ticket: " << selected_ticket_.getNumber() << std::endl;

    auto it = revision_tickets_.begin();
    std::advance(it, random_number);
    selected_ticket_ = *it;

    // choose the ticket (imitate clicking)
    onCellClicked(selected_ticket_.getNumber() - 1);
}

// get previous ticket
void MainWindow::getPreviousTicket() {

    // check if was some ticket chosen before
    if (tickets_stack_.empty()) {
        std::cout << "Не один билет не был выбран" << std::endl;
        return;
    }
    // get previous ticket as top element of stack
    selected_ticket_ = tickets_stack_.top();
    std::cout << "selected ticket : " << selected_ticket_.getNumber() << std::endl;

    tickets_stack_.pop();

    // choose the ticket (imitate clicking)
    onCellClicked(selected_ticket_.getNumber() - 1);
}


void MainWindow::createActions() {
    if (count_box_ && view_ && name_edit_) {
        connect(count_box_, QOverload<int>::of(&QSpinBox::valueChanged),
                this, &MainWindow::updateTable);
        connect(name_edit_, &QLineEdit::returnPressed,
                this, &MainWindow::onEnterPressed);
        connect(view_, &QTableWidget::cellClicked,
                this, &MainWindow::chooseTicket);
        connect(view_, &QTableWidget::cellDoubleClicked,
                this, &MainWindow::onCellDoubleClicked);
        connect(next_question_button_, &QPushButton::clicked,
                this, &MainWindow::getRandomTicket);
        connect(previous_question_button_, &QPushButton::clicked,
                this, &MainWindow::getPreviousTicket);
        connect(status_box_, &QComboBox::currentTextChanged,
                this, &MainWindow::statusChanged);
    }
}
