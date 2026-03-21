#include "mainwindow.h"
#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {
    auto *centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);
    auto *mainLayout = new QHBoxLayout(centralWidget);
    auto *leftLayout = new QVBoxLayout();

    leftLayout->addWidget(new QLabel("Выберите дату рождения:"));
    birthDateEdit = new QDateEdit(QDate::currentDate(), this);
    birthDateEdit->setCalendarPopup(true);
    birthDateEdit->setDisplayFormat("dd.MM.yyyy");

    calendar = new QCalendarWidget(this);
    leftLayout->addWidget(birthDateEdit);
    leftLayout->addWidget(calendar);

    calcButton = new QPushButton("Узнать результат!", this);
    calcButton->setStyleSheet("background-color: #4CAF50; color: white; font-weight: bold; height: 40px;");
    leftLayout->addWidget(calcButton);

    resultLabel = new QLabel("Результат появится здесь", this);
    resultLabel->setAlignment(Qt::AlignCenter);
    resultLabel->setWordWrap(true);
    resultLabel->setStyleSheet("font-size: 18px; color: blue; border: 1px solid gray; padding: 10px;");
    leftLayout->addWidget(resultLabel);

    auto *rightLayout = new QVBoxLayout();
    rightLayout->addWidget(new QLabel("Возможные варианты:"));
    
    rightLayout->addWidget(new QLabel("Кто ты из..."));
    categoryEdit = new QLineEdit("Veperr", this);
    rightLayout->addWidget(categoryEdit);
    
    rightLayout->addWidget(new QLabel("Варианты"));
    outcomesList = new QListWidget(this);
    outcomesList->addItems({"kai angel", "9mice", "golemitka"}); 
    
    rightLayout->addWidget(outcomesList);

    auto *editLayout = new QHBoxLayout();
    newOutcomeEdit = new QLineEdit(this);
    newOutcomeEdit->setPlaceholderText("Имя героя...");
    addBtn = new QPushButton("Добавить", this);
    editLayout->addWidget(newOutcomeEdit);
    editLayout->addWidget(addBtn);
    
    delBtn = new QPushButton("Удалить выбранное", this);

    rightLayout->addLayout(editLayout);
    rightLayout->addWidget(delBtn);

    mainLayout->addLayout(leftLayout, 1);
    mainLayout->addLayout(rightLayout, 1);

    connect(addBtn, &QPushButton::clicked, this, &MainWindow::onAddOutcome);
    connect(delBtn, &QPushButton::clicked, this, &MainWindow::onDeleteOutcome);
    connect(calcButton, &QPushButton::clicked, this, &MainWindow::onCalculate);

    connect(birthDateEdit, &QDateEdit::dateChanged, this, &MainWindow::syncCalendarToDate);
    connect(calendar, &QCalendarWidget::clicked, this, &MainWindow::syncDateToCalendar);
}

void MainWindow::onAddOutcome() {
    QString text = newOutcomeEdit->text().trimmed();
    if (!text.isEmpty()) {
        outcomesList->addItem(text);
        newOutcomeEdit->clear();
    }
}

void MainWindow::onDeleteOutcome() {
    delete outcomesList->currentItem();
}

void MainWindow::syncCalendarToDate() {
    calendar->setSelectedDate(birthDateEdit->date());
}

void MainWindow::syncDateToCalendar() {
    birthDateEdit->setDate(calendar->selectedDate());
}

void MainWindow::onCalculate() {
    int count = outcomesList->count();
    if (count == 0) {
        QMessageBox::warning(this, "Ошибка", "Добавьте хотя бы одного героя в список справа!");
        return;
    }

    QDate date = birthDateEdit->date();
    int magicNumber = date.day() + date.month() + date.year();
    int index = magicNumber % count;

    QString category = categoryEdit->text();
    QString hero = outcomesList->item(index)->text();

    resultLabel->setText(QString("Ты " + hero + "!!"));
}