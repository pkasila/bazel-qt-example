#include "mainwindow.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QRandomGenerator>

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {
    QWidget *central = new QWidget(this);
    setCentralWidget(central);
    setWindowTitle("Подготовка к матану");

    countSpinBox = new QSpinBox();
    countSpinBox->setRange(1, 100);
    ticketList = new QListWidget();
    totalProgress = new QProgressBar();
    greenProgress = new QProgressBar();
    
    QVBoxLayout *leftLayout = new QVBoxLayout();
    leftLayout->addWidget(new QLabel("Кол-во билетов:"));
    leftLayout->addWidget(countSpinBox);
    leftLayout->addWidget(ticketList);
    leftLayout->addWidget(new QLabel("Общий прогресс:"));
    leftLayout->addWidget(totalProgress);
    leftLayout->addWidget(new QLabel("Зеленый прогресс:"));
    leftLayout->addWidget(greenProgress);

    detailBox = new QGroupBox("Информация о билете");
    QVBoxLayout *detailLayout = new QVBoxLayout();
    lblNumber = new QLabel("№ -");
    lblName = new QLabel("Выберите билет");
    nameEdit = new QLineEdit();
    nameEdit->setPlaceholderText("Новое имя...");
    statusCombo = new QComboBox();
    statusCombo->addItems({"Не начато", "Повторить (желтый)", "Готово (зеленый)"});
    
    btnNext = new QPushButton("Следующий (рандом)");
    btnPrev = new QPushButton("Предыдущий");

    detailLayout->addWidget(lblNumber);
    detailLayout->addWidget(lblName);
    detailLayout->addWidget(nameEdit);
    detailLayout->addWidget(statusCombo);
    detailLayout->addWidget(btnNext);
    detailLayout->addWidget(btnPrev);
    detailBox->setLayout(detailLayout);

    QHBoxLayout *mainLayout = new QHBoxLayout(central);
    mainLayout->addLayout(leftLayout, 1);
    mainLayout->addWidget(detailBox, 1);

    connect(countSpinBox, QOverload<int>::of(&QSpinBox::valueChanged), this, &MainWindow::setupTickets);
    connect(ticketList, &QListWidget::itemClicked, this, &MainWindow::onTicketClicked);
    connect(ticketList, &QListWidget::itemDoubleClicked, this, &MainWindow::onTicketDoubleClicked);
    connect(nameEdit, &QLineEdit::returnPressed, this, &MainWindow::updateTicketName);
    connect(statusCombo, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &MainWindow::onStatusChanged);
    connect(btnNext, &QPushButton::clicked, this, &MainWindow::nextRandomTicket);
    connect(btnPrev, &QPushButton::clicked, this, &MainWindow::previousTicket);

    setupTickets(countSpinBox->value());
}

void MainWindow::setupTickets(int count) {
    tickets.clear();
    ticketList->clear();
    history.clear();
    currentIndex = -1;

    for(int i = 0; i < count; ++i) {
        Ticket t = {i + 1, QString("Билет %1").arg(i + 1), 0};
        tickets.append(t);
        QListWidgetItem *item = new QListWidgetItem(t.name);
        item->setBackground(Qt::lightGray);
        ticketList->addItem(item);
    }
    updateProgress();
}

void MainWindow::updateView() {
    for(int i = 0; i < tickets.size(); ++i) {
        QListWidgetItem *item = ticketList->item(i);
        item->setText(tickets[i].name);
        if(tickets[i].status == 0) item->setBackground(Qt::lightGray);
        else if(tickets[i].status == 1) item->setBackground(Qt::yellow);
        else if(tickets[i].status == 2) item->setBackground(Qt::green);
    }
    updateProgress();
}

void MainWindow::displayTicket(int index) {
    if(index < 0 || index >= tickets.size()) return;
    currentIndex = index;
    lblNumber->setText(QString("Номер: %1").arg(tickets[index].number));
    lblName->setText(tickets[index].name);
    
    statusCombo->blockSignals(true);
    statusCombo->setCurrentIndex(tickets[index].status);
    statusCombo->blockSignals(false);
    
    ticketList->setCurrentRow(index);
}

void MainWindow::onTicketClicked(QListWidgetItem *item) {
    displayTicket(ticketList->row(item));
}

void MainWindow::onTicketDoubleClicked(QListWidgetItem *item) {
    int idx = ticketList->row(item);
    if(tickets[idx].status == 2) tickets[idx].status = 1;
    else tickets[idx].status = 2;
    updateView();
    displayTicket(idx);
}

void MainWindow::updateTicketName() {
    if(currentIndex != -1 && !nameEdit->text().isEmpty()) {
        tickets[currentIndex].name = nameEdit->text();
        updateView();
        lblName->setText(tickets[currentIndex].name);
        nameEdit->clear();
    }
}

void MainWindow::onStatusChanged(int index) {
    if(currentIndex != -1) {
        tickets[currentIndex].status = index;
        updateView();
    }
}

void MainWindow::nextRandomTicket() {
    QVector<int> available;
    for(int i = 0; i < tickets.size(); ++i) {
        if(tickets[i].status < 2) available.append(i);
    }
    
    if(!available.isEmpty()) {
        if(currentIndex != -1) history.push(currentIndex);
        int randIdx = available[QRandomGenerator::global()->bounded(available.size())];
        displayTicket(randIdx);
    }
}

void MainWindow::previousTicket() {
    if(!history.isEmpty()) {
        displayTicket(history.pop());
    }
}

void MainWindow::updateProgress() {
    int started = 0, done = 0;
    for(const auto &t : tickets) {
        if(t.status > 0) started++;
        if(t.status == 2) done++;
    }
    totalProgress->setRange(0, tickets.size());
    totalProgress->setValue(started);
    greenProgress->setRange(0, tickets.size());
    greenProgress->setValue(done);
}