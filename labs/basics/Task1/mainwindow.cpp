#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QApplication>

#include <QMainWindow>
#include <QSpinBox>
#include <QListWidget>
#include <QPushButton>
#include <QProgressBar>
#include <QLineEdit>
#include <QLabel>
#include <QGroupBox>
#include <QComboBox>
#include <QMap>
#include <QGridLayout>
#include <QRandomGenerator>

void MainWindow::setupUI(){
    QWidget *CentralWidget = new QWidget(this);
    QVBoxLayout *MainLayout = new QVBoxLayout;
    CountSpinBox = new QSpinBox(this);
    CountSpinBox->setMinimum(1);
    CountSpinBox->setMaximum(100);
    connect(CountSpinBox, SIGNAL(valueChanged(int)), this, SLOT(UpdateTicketCount()));
    MainLayout->addWidget(CountSpinBox);


    TotalProgress = new QProgressBar(this);
    GreenProgress = new QProgressBar(this);
    MainLayout->addWidget(TotalProgress);
    MainLayout->addWidget(GreenProgress);



    TicketList = new QListWidget(this);
    MainLayout->addWidget(TicketList);
    connect(TicketList, SIGNAL(itemDoubleClicked(QListWidgetItem*)), this, SLOT(ChangeTicketStatus(QListWidgetItem*)));

    TicketNumberLabel = new QLabel(this);
    MainLayout->addWidget(TicketNumberLabel);


    TicketNameEdit = new QLineEdit(this);
    connect(TicketNameEdit, &QLineEdit::editingFinished, this, &MainWindow::ChangeTicketName);
    MainLayout->addWidget(TicketNameEdit);


    QHBoxLayout *buttonLayout = new QHBoxLayout();
    PrevButton = new QPushButton("Previous", this);
    NextButton = new QPushButton("Next", this);
    connect(PrevButton, &QPushButton::clicked, this, &MainWindow::PrevTicket);
    connect(NextButton, &QPushButton::clicked, this, &MainWindow::NextTicket);
    buttonLayout->addWidget(PrevButton);
    buttonLayout->addWidget(NextButton);
    MainLayout->addLayout(buttonLayout);

    statusDropdown = new QComboBox(this);
    statusDropdown->addItem("Default");
    statusDropdown->addItem("Yellow");
    statusDropdown->addItem("Green");
    connect(statusDropdown, SIGNAL(currentIndexChanged(int)), this, SLOT(ChangeTicketStatusFromDropdown()));
    MainLayout->addWidget(statusDropdown);

    CentralWidget->setLayout(MainLayout);
    setCentralWidget(CentralWidget);
}

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    setupUI();
}

void MainWindow::InitializeTickets(int count) {
    TicketList->clear();
    TicketStatuses.clear();
    for (int i = 1; i <= count; i++) {
        QListWidgetItem *item = new QListWidgetItem("Ticket " + QString::number(i), TicketList);
        item->setBackground(Qt::gray);
        TicketStatuses[item] = TicketStatus::Gray;
    }
}

void MainWindow::UpdateTicketCount(){
    InitializeTickets(CountSpinBox->value());
    History.clear();
    nonGreenTickets.clear();
    HistoryIndex = -1;
    TicketNameEdit->clear();
    TotalProgress->setValue(0);
        GreenProgress->setValue(0);
}

void MainWindow::ChangeTicketStatus(QListWidgetItem *item) {

    TicketStatus &status = TicketStatuses[item];
    if (status == TicketStatus::Green) {
        item->setBackground(QBrush(Qt::yellow));
        status = TicketStatus::Yellow;
    }
    else if (status == TicketStatus::Gray) {
        item->setBackground(QBrush(Qt::green));
        status = TicketStatus::Green;
    }
    else {
        item->setBackground(QBrush(Qt::gray));
        status = TicketStatus::Gray;
    }
    History.push_back(item);
    HistoryIndex++;
    TicketList->setCurrentItem(History[HistoryIndex]);
    UpdateNonGreenTickets();
    UpdateProgress();
    TicketNameEdit->setText(TicketList->currentItem()->text());
    TicketNumberLabel->setText(TicketList->currentItem()->text());
}

void MainWindow::UpdateNonGreenTickets() {
    nonGreenTickets.clear();
    for (auto it = TicketStatuses.begin(); it != TicketStatuses.end(); ++it){
        if (it.value() != TicketStatus::Green) {
            nonGreenTickets.append(it.key());
        }
    }
}

void MainWindow::NextTicket() {
    UpdateNonGreenTickets();
    if(nonGreenTickets.size()<=1) return;
    HistoryIndex++;
    if(HistoryIndex==History.size()){
        int Index = QRandomGenerator::global()->bounded(nonGreenTickets.size());
        while(TicketList->currentItem()==nonGreenTickets.at(Index)){
        Index = QRandomGenerator::global()->bounded(nonGreenTickets.size());
        }
        TicketList->setCurrentItem(nonGreenTickets.at(Index));

        History.push_back(nonGreenTickets.at(Index));
    }
    else{
        TicketList->setCurrentItem(History[HistoryIndex]);
    }
    TicketNameEdit->setText(TicketList->currentItem()->text());
    TicketNumberLabel->setText(TicketList->currentItem()->text());
}

void MainWindow::PrevTicket(){
    if(HistoryIndex > 0) {
        HistoryIndex--;
        TicketList->setCurrentItem(History[HistoryIndex]);
        TicketNameEdit->setText(TicketList->currentItem()->text());
        TicketNumberLabel->setText(TicketList->currentItem()->text());
    }
}

void MainWindow::UpdateProgress() {
    int total = TicketList->count();
    int Completed = 0;
    int GreenCompleted = 0;

    for (auto status : TicketStatuses) {
        if (status != TicketStatus::Gray)
            Completed++;
        if (status == TicketStatus::Green)
            GreenCompleted++;
    }

    TotalProgress->setValue(total > 0 ? (Completed * 100 / total) : 0);
    GreenProgress->setValue(total > 0 ? (GreenCompleted * 100 / total) : 0);
}

void MainWindow::ChangeTicketName() {
    if(!TicketList->currentItem()) return;
    TicketList->currentItem()->setText(TicketNameEdit->text());
    TicketNumberLabel->setText(TicketList->currentItem()->text());
}

void MainWindow::UpdateTicketInfo(QListWidgetItem *item) {
    if(!item) return;
    TicketNumberLabel->setText(item->text());
    TicketNameEdit->setText(item->text());
    statusDropdown->setCurrentIndex(static_cast<int>(TicketStatuses[item]));
}

void MainWindow::ChangeTicketStatusFromDropdown()
{
    QListWidgetItem *item = History[HistoryIndex];
    TicketStatus &status = TicketStatuses[item];
    if (statusDropdown->currentIndex()==0) {
        item->setBackground(QBrush(Qt::gray));
        status = TicketStatus::Gray;
    }
    else if (statusDropdown->currentIndex()==2) {
        item->setBackground(QBrush(Qt::green));
        status = TicketStatus::Green;
    }
    else {
        item->setBackground(QBrush(Qt::yellow));
        status = TicketStatus::Yellow;
    }
    UpdateNonGreenTickets();
    UpdateProgress();
}

MainWindow::~MainWindow()
{
    delete ui;
}
