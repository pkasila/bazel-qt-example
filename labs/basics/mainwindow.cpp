#include "mainwindow.h"

#include <iostream>
#include <qboxlayout.h>
#include <qgroupbox.h>
#include <qlabel.h>
#include <qlineedit.h>
#include <qlistwidget.h>
#include <QPushButton>
#include <qstatusbar.h>

MainWindow::MainWindow() : QMainWindow(nullptr)
{
    setWindowTitle("Tickets");
    setMinimumSize(400,400);

    setStyleSheet("color: black");

    QWidget *widget = new QWidget(this);
    setCentralWidget(widget);
    QVBoxLayout *layout = new QVBoxLayout(widget);

    gen = std::mt19937(std::random_device{}());

    count = new QSpinBox(widget);
    count->setMinimum(1);
    view = new QListWidget(widget);
    view->setViewMode(QListWidget::ViewMode::IconMode);
    view->setDragEnabled(false);
    view->setGridSize(QSize(100, 100));
    view->setSelectionMode(QListWidget::NoSelection);
    view->setResizeMode(QListWidget::Adjust);
    next = new QPushButton("Next question", widget);
    next->setEnabled(false);
    previous = new QPushButton("Previous question", widget);
    progresstotal = new QProgressBar(widget);
    progressgreen = new QProgressBar(widget);
    progressgreen->setMinimum(0);
    progresstotal->setMinimum(0);

    QPalette p = palette();
    p.setColor(QPalette::Highlight, Qt::green);
    p.setColor(QPalette::HighlightedText, Qt::black);
    progresstotal->setPalette(p);
    QPalette p2 = palette();
    p2.setColor(QPalette::Highlight, Qt::yellow);
    p2.setColor(QPalette::HighlightedText, Qt::black);
    progresstotal->setPalette(p2);
    progressgreen->setPalette(p);
    // progresstotal->setStyleSheet("background-color: rgb(255, 255, 255);" "QProgressBar::chunk { background: QLinearGradient(x1: 0, x2: 1, y1 :0, y2: 0, stop: 0 #FF0000, stop: 1 #00FF00); }");
    // progressgreen->setStyleSheet("background-color: rgb(220, 255, 220);" "QProgressBar::chunk { background-color: rgb(0,255,0); }");

    question_view = new QGroupBox(widget);
    number = new QLabel("Number", question_view);
    name = new QLabel("Name", question_view);
    line_edit = new QLineEdit("123", question_view);
    status = new QComboBox(question_view);
    status->addItem("White");
    status->addItem("Yellow");
    status->addItem("Green");
    QVBoxLayout* question_layout = new QVBoxLayout(question_view);
    question_layout->addWidget(number);
    question_layout->addWidget(name);
    question_layout->addWidget(line_edit);
    question_layout->addWidget(status);
    question_view->setLayout(question_layout);
    question_view->setEnabled(false);
    QHBoxLayout *hlayout = new QHBoxLayout(widget);
    layout->addWidget(count);
    hlayout->addWidget(next);
    hlayout->addWidget(previous);
    layout->addLayout(hlayout);
    layout->addWidget(count);
    QHBoxLayout *hlayout2 = new QHBoxLayout(widget);
    hlayout2->addWidget(view);
    hlayout2->addWidget(question_view);
    layout->addLayout(hlayout2);
    layout->addWidget(progresstotal);
    layout->addWidget(progressgreen);

    widget->setLayout(layout);

    changeCount();

    connect(count, &QSpinBox::editingFinished, this, &MainWindow::changeCount);
    connect(view, &QListWidget::itemDoubleClicked, this, &MainWindow::doubleClick);
    connect(view, &QListWidget::itemClicked, this, &MainWindow::singleClick);
    connect(line_edit, &QLineEdit::returnPressed, this, &MainWindow::changeName);
    connect(status, &QComboBox::currentIndexChanged, this, &MainWindow::changeColor);
    connect(next, &QPushButton::pressed, this, &MainWindow::tonext);
    connect(previous, &QPushButton::pressed, this, &MainWindow::toprev);
    connect(view, &QListWidget::currentItemChanged, this, &MainWindow::currentChanged);
}

void MainWindow::changeCount() {
    question_view->setEnabled(false);
    tickets.resize(count->value());
    while (view->count() > count->value()) {
        view->takeItem(view->count() - 1);
    }
    while (view->count() < count->value()) {
        QListWidgetItem* item = new QListWidgetItem(std::to_string(view->count() + 1).data());
        tickets[view->count()].name = ("Ticket " + std::to_string(view->count() + 1)).data();
        tickets[view->count()].number = view->count();
        item->setSizeHint(QSize(100, 100));
        item->setBackground(QBrush(Qt::white));
        view->addItem(item);
    }
    def.clear();
    ng.clear();
    for (auto& i : tickets) {
        i.name = ("Ticket " + std::to_string(i.number + 1)).data();
        i.state = 0;
        view->item(i.number)->setBackground(QBrush(Qt::white));
        def.insert(i.number);
        ng.insert(i.number);
    }
    next->setEnabled(ng.size() != 0);
    updateBars();
}

void MainWindow::singleClick(QListWidgetItem* item) {
    number->setText(std::to_string(view->currentRow() + 1).data());
    ticket t = tickets[view->currentRow()];
    active = view->currentRow();
    name->setText(t.name);
    status->setCurrentIndex(t.state);
    question_view->setEnabled(true);
}

void MainWindow::changeName() {
    tickets[active].name = line_edit->text();
    name->setText(tickets[active].name);
}

void MainWindow::doubleClick(QListWidgetItem* item) {
    int current = view->currentRow();
    if (tickets[current].state == 2) {
        setItemColor(current, 1);
        status->setCurrentIndex(tickets[current].state);
    }
    else {
        setItemColor(current, 2);
        status->setCurrentIndex(tickets[current].state);
    }
}

void MainWindow::changeColor() {
    setItemColor(active, status->currentIndex());
}

void MainWindow::tonext() {
    if (ng.size() == 0) {
        return;
    }
    auto it = ng.begin();
    std::advance(it, std::uniform_int_distribution<>(0, ng.size() - 1)(gen));
    int ind = *it;
    view->setCurrentRow(ind);
    view->item(ind)->setSelected(true);
    view->setFocus();
    // view->currentItem()->setSelected(true);
    singleClick(view->currentItem());
}

void MainWindow::toprev() {
    int now = view->currentRow();
    if (history.empty()) {return;}
    history.pop();
    while (!history.empty()) {
        if ((history.top()) < (view->count()) &&  (history.top() != now)) {
            view->setCurrentRow(history.top());
            view->setFocus();
            singleClick(view->currentItem());
            return;
        }
        history.pop();
    }
}

void MainWindow::updateBars() {
    progressgreen->setMaximum(view->count());
    progresstotal->setMaximum(view->count());
    progressgreen->setValue(view->count() - ng.size());
    progresstotal->setValue(view->count() - def.size());
}

void MainWindow::setItemColor(int ind, int col) {
    ng.erase(ind);
    def.erase(ind);
    switch (col) {
        case 0:
            def.insert(ind);
            ng.insert(ind);
            view->item(ind)->setBackground(QBrush(Qt::white));
        break;
        case 1:
            ng.insert(ind);
            view->item(ind)->setBackground(QBrush(Qt::yellow));
        break;
        case 2:
            view->item(ind)->setBackground(QBrush(Qt::green));
        break;
    }
    tickets[ind].state = col;
    next->setEnabled(!ng.empty());
    updateBars();
}

void MainWindow::currentChanged() {
    history.push(view->currentRow());
}
