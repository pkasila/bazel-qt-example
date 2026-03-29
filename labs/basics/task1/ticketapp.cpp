#include "ticketapp.h"
#include <QVBoxLayout>
#include <QHBoxLayout>

TicketApp::TicketApp(QWidget *parent) : QWidget(parent) {
    QVBoxLayout *mainLayout = new QVBoxLayout(this);

    ticketCount = new QSpinBox;
    ticketCount->setMinimum(1);
    ticketCount->setMaximum(100);
    mainLayout->addWidget(ticketCount);

    ticketList = new QListWidget;
    mainLayout->addWidget(ticketList);

    QGroupBox *questionView = new QGroupBox("Текущий билет");
    QVBoxLayout *questionLayout = new QVBoxLayout(questionView);

    questionLabel = new QLabel("Выберите билет");
    questionLayout->addWidget(questionLabel);

    nameEdit = new QLineEdit;
    questionLayout->addWidget(nameEdit);

    questionView->setLayout(questionLayout);
    mainLayout->addWidget(questionView);

    QHBoxLayout *buttonLayout = new QHBoxLayout;
    nextButton = new QPushButton("Следующий билет");
    prevButton = new QPushButton("Предыдущий билет");
    buttonLayout->addWidget(prevButton);
    buttonLayout->addWidget(nextButton);
    mainLayout->addLayout(buttonLayout);

    totalProgress = new QProgressBar;
    greenProgress = new QProgressBar;
    mainLayout->addWidget(totalProgress);
    mainLayout->addWidget(greenProgress);

    connect(ticketCount, QOverload<int>::of(&QSpinBox::valueChanged), this, &TicketApp::updateTicketList);
    connect(nextButton, &QPushButton::clicked, this, &TicketApp::nextTicket);
    connect(prevButton, &QPushButton::clicked, this, &TicketApp::prevTicket);
    connect(ticketList, &QListWidget::itemClicked, this, &TicketApp::selectTicket);
    connect(ticketList, &QListWidget::itemDoubleClicked, this, &TicketApp::toggleTicketStatus);
    connect(nameEdit, &QLineEdit::editingFinished, this, &TicketApp::updateTicketName);

    updateTicketList(10);
}

void TicketApp::updateTicketList(int count) {
    ticketList->clear();
    ticketNames.clear();
    ticketStatus.clear();
    for (int i = 1; i <= count; ++i) {
        QString name = QString("Билет %1").arg(i);
        ticketNames[i] = name;
        ticketStatus[i] = "default";
        QListWidgetItem *item = new QListWidgetItem(name);
        item->setBackground(Qt::gray);
        ticketList->addItem(item);
    }
    updateProgress();
}

void TicketApp::selectTicket(QListWidgetItem *item) {
    currentIndex = ticketList->row(item) + 1;
    questionLabel->setText(ticketNames[currentIndex]);
    nameEdit->setText(ticketNames[currentIndex]);
}

void TicketApp::updateTicketName() {
    if (currentIndex > 0) {
        ticketNames[currentIndex] = nameEdit->text();
        ticketList->item(currentIndex - 1)->setText(nameEdit->text());
    }
}

void TicketApp::toggleTicketStatus(QListWidgetItem *item) {
    int index = ticketList->row(item) + 1;
    if (ticketStatus[index] == "default") {
        ticketStatus[index] = "yellow";
        item->setBackground(Qt::yellow);
    } else if (ticketStatus[index] == "yellow") {
        ticketStatus[index] = "green";
        item->setBackground(Qt::green);
    } else {
        ticketStatus[index] = "default";
        item->setBackground(Qt::gray);
    }
    updateProgress();
}

void TicketApp::nextTicket() {
    int count = ticketList->count();
    if (count == 0) return;
    int newIndex = QRandomGenerator::global()->bounded(count) + 1;
    while (ticketStatus[newIndex] == "green") {
        newIndex = QRandomGenerator::global()->bounded(count) + 1;
    }
    currentIndex = newIndex;
    questionLabel->setText(ticketNames[currentIndex]);
    nameEdit->setText(ticketNames[currentIndex]);
}

void TicketApp::prevTicket() {
    if (currentIndex > 1) {
        currentIndex--;
        questionLabel->setText(ticketNames[currentIndex]);
        nameEdit->setText(ticketNames[currentIndex]);
    }
}

void TicketApp::updateProgress() {
    int total = ticketList->count();
    int completed = 0;
    int fullyCompleted = 0;
    for (auto status : ticketStatus) {
        if (status == "yellow" || status == "green") completed++;
        if (status == "green") fullyCompleted++;
    }
    totalProgress->setValue((completed * 100) / total);
    greenProgress->setValue((fullyCompleted * 100) / total);
}
