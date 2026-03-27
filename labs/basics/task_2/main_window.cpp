#include <QtWidgets>
#include "main_window.h"

#include <iostream>

#include "Utils.h"


MainWindow::MainWindow() {
    // load data from json
    schedule_ = Utils::readScheduleFromFile(file_name_);
    auto list = schedule_.dates;
    for (auto obj: list) {
        events_map_[obj.date] = obj.events;
    }

    QWidget *central_widget = new QWidget;
    setCentralWidget(central_widget);

    calendar_ = new QCalendarWidget();

    // represent events of selected date
    events_table_ = new QTableWidget();
    events_table_->setSelectionBehavior(QAbstractItemView::SelectRows);
    events_table_->setSelectionMode(QAbstractItemView::SingleSelection);
    events_table_->setSelectionBehavior(QAbstractItemView::SelectRows);
    events_table_->setEditTriggers(QAbstractItemView::NoEditTriggers);
    events_table_->setColumnCount(3);
    events_table_->setHorizontalHeaderLabels({"Дата", "Время", "Событие"});
    events_table_->verticalHeader()->setVisible(false);

    events_table_->setColumnWidth(0, 100);
    events_table_->setColumnWidth(1, 80);

    QHeaderView *header = events_table_->horizontalHeader();
    header->setSectionResizeMode(0, QHeaderView::Fixed);
    header->setSectionResizeMode(1, QHeaderView::Fixed);
    header->setSectionResizeMode(2, QHeaderView::Stretch);
    header->setDefaultAlignment(Qt::AlignCenter);

    events_table_->verticalHeader()->setVisible(false);

    auto *time_layout = new QVBoxLayout();

    // choose time of event (selected day)
    time_ = new QLabel("Время:");
    time_edit_ = new QTimeEdit();
    time_edit_->setTime(QTime::currentTime());

    auto *event_layout = new QVBoxLayout();

    // add event
    event_label_ = new QLabel("Напоминание (dd-MM-yyyy, hh-mm)");
    event_edit_ = new QLineEdit();

    // submit event (selected date and time)
    submit_event_button_ = new QPushButton("Добавить событие");

    // delete event (selected cell of the table)
    delete_event_button_ = new QPushButton("Удалить выбранное событие");

    // for backapace
    delete_event_button_->setShortcut(QKeySequence("Backspace"));

    // layouts
    auto *main_layout = new QVBoxLayout();
    main_layout->setContentsMargins(5, 5, 5, 5);
    main_layout->addWidget(calendar_);
    main_layout->addWidget(events_table_);

    time_layout->addWidget(time_);
    time_layout->addWidget(time_edit_);
    main_layout->addLayout(time_layout);

    event_layout->addWidget(event_label_);
    event_layout->addWidget(event_edit_);
    main_layout->addLayout(event_layout);

    main_layout->addWidget(submit_event_button_);
    main_layout->addWidget(delete_event_button_);

    central_widget->setLayout(main_layout);

    createActions();

    // load data in order to show events of current day
    updateEventsTable();
}

// update/create a table of events with its date and time (sorted by time)
void MainWindow::updateEventsTable() {
    events_table_->clear();
    events_table_->setHorizontalHeaderLabels({"Дата", "Время", "Событие"});

    std::cout << "count of events: " << events_map_.size() << std::endl;
    if (events_map_.contains(selected_date_)) {
        auto selected_date_events = events_map_[selected_date_];
        events_table_->setRowCount(selected_date_events.size());
        int row = 0;

        for (auto event: selected_date_events) {
            QTableWidgetItem *item0 = new QTableWidgetItem(selected_date_.toString("dd-MM-yyyy"));
            item0->setTextAlignment(Qt::AlignCenter);
            events_table_->setItem(row, 0, item0);

            QTableWidgetItem *item1 = new QTableWidgetItem(event.time);
            item1->setTextAlignment(Qt::AlignCenter);
            events_table_->setItem(row, 1, item1);

            QTableWidgetItem *item2 = new QTableWidgetItem(event.task);
            item2->setTextAlignment(Qt::AlignCenter);
            events_table_->setItem(row, 2, item2);

            row++;
        }
    }
}

// after choosing a cell of the calendar it shows table of events and update task_label
void MainWindow::onDateClicked() {
    selected_date_ = calendar_->selectedDate();
    updateTaskLabel();

    // load events of current date
    updateEventsTable();
}

void MainWindow::setNewTime() {
    selected_time_ = time_edit_->time().toString("hh:mm");
    updateTaskLabel();
}

void MainWindow::updateTaskLabel() {
    event_label_->setText(QString("Напоминание (%1, %2):")
        .arg(selected_date_.toString("dd-MM-yyyy"))
        .arg(selected_time_));
}

void MainWindow::addNewEvent() {
    auto &selected_date_events = events_map_[selected_date_];
    auto it = selected_date_events.begin();

    // search the time which is greater than selected_time and insert the event before that time
    while (it != selected_date_events.end()) {
        if (it->time.compare(selected_time_) > 0) {
            break;
        }
        ++it;
    }
    auto event_label = !event_edit_->text().isEmpty() ? event_edit_->text() : "Безымянное событие";
    selected_date_events.insert(it, Event(selected_time_, event_label));
    updateEventsTable();
}

void MainWindow::deleteSelectedEvent() {

    auto &selected_date_events = events_map_[selected_date_];
    std::cout << events_table_->currentRow() << std::endl;
    if (events_table_->currentRow() == -1) {
        return;
    }
    selected_date_events.erase(selected_date_events.begin() + events_table_->currentRow());
    updateEventsTable();
}

// after work of the app it has to save all events to json
MainWindow::~MainWindow() {
    QList<DateSchedule> new_list;
    for (auto event: events_map_) {
        if (!event.second.isEmpty()) {
            new_list.push_back(DateSchedule(event.first, event.second));
        }
    }
    schedule_.dates = new_list;
    Utils::writeScheduleToFile(file_name_, schedule_);
}

void MainWindow::createActions() {
    connect(calendar_, &QCalendarWidget::clicked, this, &MainWindow::onDateClicked);
    connect(time_edit_, &QTimeEdit::timeChanged, this, &MainWindow::setNewTime);

    // for sumbit button
    connect(submit_event_button_, &QPushButton::clicked, this, &MainWindow::addNewEvent);
    // for return
    connect(event_edit_, &QLineEdit::returnPressed, this, &MainWindow::addNewEvent);

    connect(delete_event_button_, &QPushButton::clicked, this, &MainWindow::deleteSelectedEvent);

}
