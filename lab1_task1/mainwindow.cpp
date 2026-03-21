#include "mainwindow.h"

#include <QWidget>
#include <QLabel>
#include <QSpinBox>
#include <QPushButton>
#include <QVector>
#include <QListWidget>
#include <QListWidgetItem>
#include <QColor>
#include <QBoxLayout>
#include <QGroupBox>
#include <QComboBox>
#include <QLineEdit>
#include <QProgressBar>
#include <QRandomGenerator>
#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    QWidget *central = new QWidget(this);
    setCentralWidget(central);

    auto *mainLayout = new QVBoxLayout(central);
    mainLayout->setContentsMargins(10, 10, 10, 10);
    mainLayout->setSpacing(10);

    auto *topRow = new QHBoxLayout;
    auto *countLabel = new QLabel("Number of tickets:", central);
    countSpinBox = new QSpinBox(central);
    countSpinBox->setRange(0, 200);

    topRow->addWidget(countLabel);
    topRow->addWidget(countSpinBox);
    topRow->addStretch();

    mainLayout->addLayout(topRow);

    auto *contentRow = new QHBoxLayout;
    contentRow->setSpacing(12);
    mainLayout->addLayout(contentRow);

    ticketList = new QListWidget(central);
    ticketList->setMinimumWidth(260);
    contentRow->addWidget(ticketList, 1);

    auto *questionBox = new QGroupBox("Question view", central);
    auto *questionLayout = new QVBoxLayout(questionBox);
    questionLayout->setSpacing(8);

    numberLabel = new QLabel("Number: -", questionBox);
    nameLabel = new QLabel("Name: -", questionBox);

    nameEdit = new QLineEdit(questionBox);
    nameEdit->setPlaceholderText("Enter new ticket name and press Enter");

    statusCombo = new QComboBox(questionBox);
    statusCombo->addItem("Default", 0);
    statusCombo->addItem("Need repeat", 1);
    statusCombo->addItem("Done", 2);

    nextButton = new QPushButton("Next random question", questionBox);
    previousButton = new QPushButton("Previous question", questionBox);

    totalProgress = new QProgressBar(questionBox);
    greenProgress = new QProgressBar(questionBox);

    totalProgress->setFormat("Total progress: %p%");
    greenProgress->setFormat("Green progress: %p%");

    questionLayout->addWidget(numberLabel);
    questionLayout->addWidget(nameLabel);
    questionLayout->addWidget(nameEdit);
    questionLayout->addWidget(statusCombo);
    questionLayout->addWidget(nextButton);
    questionLayout->addWidget(previousButton);

    auto *totalLabel = new QLabel("Total progress", questionBox);
    auto *greenLabel = new QLabel("Fully learned", questionBox);

    questionLayout->addWidget(totalLabel);
    questionLayout->addWidget(totalProgress);
    questionLayout->addWidget(greenLabel);
    questionLayout->addWidget(greenProgress);

    questionLayout->addStretch();

    contentRow->addWidget(questionBox, 1);

    rebuildTickets(0);
    updateProgressBars();
    updateButtons();

    connect(countSpinBox, &QSpinBox::valueChanged, this, [this](int value) {
        rebuildTickets(value);
    });

    connect(ticketList, &QListWidget::itemClicked, this, [this](QListWidgetItem *item) {
        int index = ticketList->row(item);
        showTicket(index);
    });

    connect(ticketList, &QListWidget::itemDoubleClicked, this, [this](QListWidgetItem *item) {
        int index = ticketList->row(item);
        if (index < 0 || index >= tickets.size()) return;

        int currentStatus = tickets[index].status;
        int nextStatus = (currentStatus == 2 ? 1 : 2);

        setTicketStatus(index, nextStatus);
        showTicket(index);
    });

    connect(statusCombo, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, [this](int comboIndex) {
        if (currentIndex < 0 || currentIndex >= tickets.size()) return;

        int status = statusCombo->itemData(comboIndex).toInt();
        setTicketStatus(currentIndex, status);
        showTicket(currentIndex);
    });

    connect(nameEdit, &QLineEdit::returnPressed, this, [this]() {
        if (currentIndex < 0 || currentIndex >= tickets.size()) return;
        if (!nameEdit->hasFocus()) return;

        QString text = nameEdit->text().trimmed();
        if (text.isEmpty()) return;

        tickets[currentIndex].name = text;
        updateListItem(currentIndex);
        showTicket(currentIndex);
    });

    connect(nextButton, &QPushButton::clicked, this, [this]() {
        int nextIndex = randomAvailableTicket();
        if (nextIndex == -1) {
            QMessageBox::information(this, "Done!", "All tickets are learned!");
            return;
        }

        if (currentIndex != -1) {
            history.push_back(currentIndex);
            updateButtons();
        }

        showTicket(nextIndex);
        ticketList->setCurrentRow(nextIndex);
    });

    connect(previousButton, &QPushButton::clicked, this, [this]() {
        if (history.isEmpty()) return;

        int prevIndex = history.takeLast();
        updateButtons();
        showTicket(prevIndex);
        ticketList->setCurrentRow(prevIndex);
    });
}

void MainWindow::updateButtons()
{
    previousButton->setEnabled(!history.isEmpty());

    bool hasTickets = !tickets.isEmpty();
    nextButton->setEnabled(hasTickets);
    statusCombo->setEnabled(currentIndex >= 0);
    nameEdit->setEnabled(currentIndex >= 0);
}

void MainWindow::rebuildTickets(int count)
{
    tickets.clear();
    history.clear();
    currentIndex = -1;

    for (int i = 0; i < count; ++i) {
        Ticket t;
        t.number = i + 1;
        t.name = QString("Ticket %1").arg(i + 1);
        t.status = 0;
        tickets.push_back(t);
    }

    refreshList();

    numberLabel->setText("Number: -");
    nameLabel->setText("Name: -");
    nameEdit->clear();
    statusCombo->setCurrentIndex(0);

    updateProgressBars();
    updateButtons();
}

void MainWindow::refreshList()
{
    ticketList->clear();

    for (int i = 0; i < tickets.size(); ++i) {
        auto *item = new QListWidgetItem(tickets[i].name);
        ticketList->addItem(item);
        updateListItem(i);
    }
}

void MainWindow::updateListItem(int index)
{
    if (index < 0 || index >= tickets.size()) return;
    QListWidgetItem *item = ticketList->item(index);
    if (!item) return;

    item->setText(tickets[index].name);

    if (tickets[index].status == 0) {
        item->setBackground(QColor("#d0d4da"));
        item->setForeground(QColor("#1f2328"));
    } else if (tickets[index].status == 1) {
        item->setBackground(QColor("#f4d35e"));
        item->setForeground(QColor("#3b2f00"));
    } else {
        item->setBackground(QColor("#7bd389"));
        item->setForeground(QColor("#0f3d1e"));
    }
}

void MainWindow::showTicket(int index)
{
    if (index < 0 || index >= tickets.size()) return;

    currentIndex = index;

    const Ticket &t = tickets[index];

    numberLabel->setText(QString("Number: %1").arg(t.number));
    nameLabel->setText(QString("Name: %1").arg(t.name));
    nameEdit->setText(t.name);

    int comboIndex = statusCombo->findData(t.status);
    if (comboIndex != -1) {
        statusCombo->blockSignals(true);
        statusCombo->setCurrentIndex(comboIndex);
        statusCombo->blockSignals(false);
    }

    updateButtons();
}

void MainWindow::updateProgressBars()
{
    int totalCount = tickets.size();
    if (totalCount == 0) {
        totalProgress->setValue(0);
        greenProgress->setValue(0);
        return;
    }

    int totalDone = 0;
    int greenDone = 0;

    for (const Ticket &t : tickets) {
        if (t.status == 1 || t.status == 2) {
            ++totalDone;
        }
        if (t.status == 2) {
            ++greenDone;
        }
    }

    totalProgress->setValue(100 * totalDone / totalCount);
    greenProgress->setValue(100 * greenDone / totalCount);
}

void MainWindow::setTicketStatus(int index, int status)
{
    if (index < 0 || index >= tickets.size()) return;

    tickets[index].status = status;
    updateListItem(index);
    updateProgressBars();
}

int MainWindow::randomAvailableTicket() const
{
    QVector<int> available;

    for (int i = 0; i < tickets.size(); ++i) {
        if (tickets[i].status == 0 || tickets[i].status == 1) {
            available.push_back(i);
        }
    }

    if (available.isEmpty()) {
        if (currentIndex >= 0 && currentIndex < tickets.size() &&
            (tickets[currentIndex].status == 0 || tickets[currentIndex].status == 1)) {
            return currentIndex;
        }
        return -1;
    }

    int pos = QRandomGenerator::global()->bounded(available.size());
    return available[pos];
}