#include "mainwindow.h"

#include "task2widget.h"

#include <QAbstractItemView>
#include <QFormLayout>
#include <QHBoxLayout>
#include <QRandomGenerator>
#include <QTabWidget>
#include <QVBoxLayout>

MainWindow::MainWindow(QWidget* parent) : QMainWindow(parent) {
    setupUi();
    updateTicketCount(count_spinbox->value());
}

MainWindow::~MainWindow() {
}

void MainWindow::setupUi() {
    QTabWidget* tabs = new QTabWidget(this);
    setCentralWidget(tabs);

    QWidget* task1Widget = new QWidget();
    QVBoxLayout* main_layout = new QVBoxLayout(task1Widget);
    main_layout->setSpacing(15);
    main_layout->setContentsMargins(20, 20, 20, 20);

    count_spinbox = new QSpinBox();
    count_spinbox->setMinimum(1);
    count_spinbox->setMaximum(1000);
    count_spinbox->setValue(10);

    view_list = new QListWidget();
    view_list->setSelectionMode(QAbstractItemView::NoSelection);
    view_list->setFocusPolicy(Qt::NoFocus);

    question_view = new QGroupBox("📋 Текущий билет");
    QFormLayout* question_layout = new QFormLayout(question_view);
    question_layout->setVerticalSpacing(10);
    number_label = new QLabel("Номер: -");
    number_label->setStyleSheet("font-weight: bold; color: #0d47a1; transition: color 0.3s;");
    name_label = new QLabel("-");
    name_label->setStyleSheet("font-size: 32px; font-weight: bold; color: #1a1a1a;");
    name_edit = new QLineEdit();
    status_combo = new QComboBox();
    status_combo->addItems({"Дефолтный", "Желтый", "Зеленый"});

    question_layout->addRow(number_label);
    question_layout->addRow(new QLabel("Имя:"), name_label);
    question_layout->addRow(new QLabel("Изменить имя:"), name_edit);
    question_layout->addRow(new QLabel("Статус:"), status_combo);

    QHBoxLayout* nav_layout = new QHBoxLayout();
    nav_layout->setSpacing(10);
    prev_button = new QPushButton("⬅️ Предыдущий");
    next_button = new QPushButton("🎲 Следующий случайный");
    nav_layout->addWidget(prev_button);
    nav_layout->addWidget(next_button);

    total_progress = new QProgressBar();
    total_progress->setFormat("📊 Общий прогресс: %p%");

    green_progress = new QProgressBar();
    green_progress->setFormat("✅ Выучено: %p%");
    green_progress->setStyleSheet("QProgressBar::chunk {background-color: #28a745;}");

    main_layout->addWidget(new QLabel("<b>Количество всех билетов:</b>"));
    main_layout->addWidget(count_spinbox);
    main_layout->addWidget(view_list);
    main_layout->addWidget(question_view);
    main_layout->addLayout(nav_layout);
    main_layout->addWidget(total_progress);
    main_layout->addWidget(green_progress);

    tabs->addTab(task1Widget, "Задание 1: Билеты");

    Task2Widget* task2Widget = new Task2Widget();
    tabs->addTab(task2Widget, "Задание 2: Калькулятор Отчисления");

    connect(
        count_spinbox, QOverload<int>::of(&QSpinBox::valueChanged), this,
        &MainWindow::updateTicketCount);
    connect(view_list, &QListWidget::itemClicked, this, &MainWindow::selectTicket);
    connect(view_list, &QListWidget::itemDoubleClicked, this, &MainWindow::toggleTicketStatus);
    connect(name_edit, &QLineEdit::returnPressed, this, &MainWindow::updateTicketName);
    connect(
        status_combo, QOverload<int>::of(&QComboBox::currentIndexChanged), this,
        &MainWindow::updateTicketStatus);
    connect(next_button, &QPushButton::clicked, this, &MainWindow::nextRandomTicket);
    connect(prev_button, &QPushButton::clicked, this, &MainWindow::previousTicket);

    QString globalStyle = R"(
        QMainWindow {
            background-color: #f0f2f5;
        }
        QTabWidget::pane {
            border: 1px solid #ced4da;
            background-color: white;
            border-radius: 8px;
            top: -1px;
        }
        QTabBar::tab {
            background: #e9ecef;
            border: 1px solid #ced4da;
            padding: 10px 20px;
            border-top-left-radius: 8px;
            border-top-right-radius: 8px;
            margin-right: 2px;
        }
        QTabBar::tab:selected {
            background: white;
            border-bottom-color: white;
            font-weight: bold;
        }
        QGroupBox {
            font-weight: bold;
            border: 2px solid #dee2e6;
            border-radius: 10px;
            margin-top: 1.5ex;
            background-color: #ffffff;
            padding: 15px;
        }
        QGroupBox::title {
            subcontrol-origin: margin;
            subcontrol-position: top center;
            padding: 0 10px;
            color: #495057;
        }
        QPushButton {
            background-color: #007bff;
            color: white;
            border-radius: 6px;
            padding: 8px 16px;
            font-weight: bold;
            border: none;
        }
        QPushButton:hover {
            background-color: #0056b3;
        }
        QPushButton:pressed {
            background-color: #004085;
        }
        QProgressBar {
            border: none;
            border-radius: 8px;
            background-color: #e9ecef;
            text-align: center;
            height: 25px;
            font-weight: bold;
        }
        QProgressBar::chunk {
            background-color: #007bff;
            border-radius: 8px;
        }
        QListWidget {
            border: 2px solid #dee2e6;
            border-radius: 8px;
            background-color: white;
            padding: 5px;
        }
        QListWidget::item:hover {
            background-color: #f8f9fa;
        }
        QLineEdit, QSpinBox, QComboBox {
            border: 2px solid #ced4da;
            border-radius: 6px;
            padding: 6px;
            background-color: white;
        }
        QLineEdit:focus, QSpinBox:focus, QComboBox:focus {
            border-color: #80bdff;
        }
    )";
    this->setStyleSheet(globalStyle);

    resize(900, 750);
}

void MainWindow::updateTicketCount(int count) {
    tickets.clear();
    view_list->clear();
    history.clear();
    current_ticket_index = -1;

    for (int i = 0; i < count; ++i) {
        Ticket t;
        t.name = QString("Билет %1").arg(i + 1);
        t.status = 0;
        tickets.push_back(t);

        QListWidgetItem* item = new QListWidgetItem(t.name);
        item->setBackground(QColor("#eeeeee"));
        item->setForeground(Qt::black);
        view_list->addItem(item);
    }
    updateProgress();

    number_label->setText("Номер: -");
    name_label->setText("-");
    name_edit->clear();
    status_combo->setCurrentIndex(0);
}

void MainWindow::selectTicket(QListWidgetItem* item) {
    int index = view_list->row(item);
    if (index < 0 || index >= tickets.size()) {
        return;
    }

    current_ticket_index = index;
    Ticket& t = tickets[index];

    number_label->setText(QString("Номер: %1").arg(index + 1));
    name_label->setText(t.name);
    name_edit->setText(t.name);

    status_combo->blockSignals(true);
    status_combo->setCurrentIndex(t.status);
    status_combo->blockSignals(false);
}

void MainWindow::updateTicketName() {
    if (current_ticket_index < 0 || name_edit->text().isEmpty()) {
        return;
    }

    tickets[current_ticket_index].name = name_edit->text();
    name_label->setText(name_edit->text());
    view_list->item(current_ticket_index)->setText(name_edit->text());
}

void MainWindow::updateTicketStatus(int status) {
    if (current_ticket_index < 0) {
        return;
    }

    tickets[current_ticket_index].status = status;
    QListWidgetItem* item = view_list->item(current_ticket_index);

    if (status == 0) {
        item->setBackground(QColor("#eeeeee"));
        item->setForeground(Qt::black);
    } else if (status == 1) {
        item->setBackground(QColor("#fff59d"));
        item->setForeground(Qt::black);
    } else if (status == 2) {
        item->setBackground(QColor("#a5d6a7"));
        item->setForeground(Qt::black);
    }

    updateProgress();
}

void MainWindow::toggleTicketStatus(QListWidgetItem* item) {
    int index = view_list->row(item);
    if (index < 0 || index >= tickets.size()) {
        return;
    }

    int current_status = tickets[index].status;
    int new_status = (current_status == 2) ? 1 : 2;

    selectTicket(item);
    status_combo->setCurrentIndex(new_status);
}

void MainWindow::nextRandomTicket() {
    std::vector<int> available;
    for (int i = 0; i < tickets.size(); ++i) {
        if (tickets[i].status == 0 || tickets[i].status == 1) {
            available.push_back(i);
        }
    }

    if (available.empty()) {
        return;
    }

    if (current_ticket_index >= 0) {
        history.push_back(current_ticket_index);
    }

    int rand_idx = QRandomGenerator::global()->bounded(static_cast<int>(available.size()));
    int next_ticket = available[rand_idx];

    view_list->setCurrentRow(next_ticket);
    selectTicket(view_list->item(next_ticket));
}

void MainWindow::previousTicket() {
    if (history.empty()) {
        return;
    }

    int prev_ticket = history.back();
    history.pop_back();

    view_list->setCurrentRow(prev_ticket);
    selectTicket(view_list->item(prev_ticket));
}

void MainWindow::updateProgress() {
    int total_learning = 0;
    int green_learning = 0;
    int count = tickets.size();

    for (const auto& t : tickets) {
        if (t.status == 1 || t.status == 2) {
            total_learning++;
        }
        if (t.status == 2) {
            green_learning++;
        }
    }

    if (count > 0) {
        total_progress->setMaximum(count);
        total_progress->setValue(total_learning);

        green_progress->setMaximum(count);
        green_progress->setValue(green_learning);
    }
}
