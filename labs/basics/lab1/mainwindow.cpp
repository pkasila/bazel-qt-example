#include <QRandomGenerator>

#include "mainwindow.h"

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {
    QWidget *centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);
    QHBoxLayout *mainLayout = new QHBoxLayout(centralWidget);

    // left
    QVBoxLayout *leftLayout = new QVBoxLayout();

    QLabel *countLabel = new QLabel("Количество билетов:", this);

    countSpinBox = new QSpinBox(this);
    countSpinBox->setRange(0, 1000);

    ticketsListWidget = new QListWidget(this);

    started = new QProgressBar(this);
    started->setFormat("Начато: %p%");
    started->setAlignment(Qt::AlignCenter); 

    mastered = new QProgressBar(this);
    mastered->setFormat("Выучено: %p%");
    mastered->setAlignment(Qt::AlignCenter);

    leftLayout->addWidget(countLabel);
    leftLayout->addWidget(countSpinBox);
    leftLayout->addWidget(ticketsListWidget);
    leftLayout->addWidget(started);
    leftLayout->addWidget(mastered);

    //right
    questionGroupBox = new QGroupBox("Детали билета", this);
    QVBoxLayout *rightLayout = new QVBoxLayout(questionGroupBox);

    numberLabel = new QLabel("Номер билета: ", this);
    nameLabel = new QLabel("Название: ", this);
    nameEdit = new QLineEdit(this);
    nameEdit->setPlaceholderText("Новое название (нажми Enter)");
    statusEdit = new QComboBox(this);
    statusEdit->addItems({"Не начат (Серый)", "Повторить (Желтый)", "Выучен (Зеленый)"});

    rightLayout->addWidget(numberLabel);
    rightLayout->addWidget(nameLabel);
    rightLayout->addWidget(nameEdit);
    rightLayout->addWidget(statusEdit);

    QHBoxLayout *buttonLayout = new QHBoxLayout();
    next = new QPushButton("Следующий", this);
    prev = new QPushButton("Предыдущий", this);
    buttonLayout->addWidget(next);
    buttonLayout->addWidget(prev);

    rightLayout->addLayout(buttonLayout);

    clear = new QPushButton("Очистить", this);
    rightLayout->addWidget(clear);

    rightLayout->addStretch();

    //left and right
    mainLayout->addLayout(leftLayout, 1);
    mainLayout->addWidget(questionGroupBox, 2);

    connect(countSpinBox, &QSpinBox::valueChanged, this, &MainWindow::onCountChanged);
    connect(next, &QPushButton::clicked, this, &MainWindow::onNextClicked);
    connect(prev, &QPushButton::clicked, this, &MainWindow::onPrevClicked);
    connect(clear, &QPushButton::clicked, this, &MainWindow::onClearClicked);

    connect(ticketsListWidget, &QListWidget::currentRowChanged, this, &MainWindow::onListRowChanged);
    connect(statusEdit, &QComboBox::currentIndexChanged, this, &MainWindow::onStatusIndexChanged);
    connect(ticketsListWidget, &QListWidget::itemDoubleClicked, this, &MainWindow::onItemDoubleClicked);

    connect(nameEdit, &QLineEdit::returnPressed, this, &MainWindow::onNameEditReturnPressed);
    
    onCountChanged(10);
    countSpinBox->setValue(10);
}

void MainWindow::onCountChanged(int count) {
    int currentSize = tickets.size();

    if (count > currentSize) {
        for (int i = currentSize; i < count; ++i) {
            Ticket newTicket;
            newTicket.name = "Билет " + QString::number(i + 1);
            newTicket.status = TicketStatus::kNotStart;

            tickets.append(newTicket);
            ticketsListWidget->addItem(newTicket.name);
            QListWidgetItem *item = ticketsListWidget->item(i);
            if (item) item->setBackground(QBrush(Qt::gray));
        }
    } 
    else if (count < currentSize) {
        while (tickets.size() > count) {
            tickets.removeLast();
            QListWidgetItem *item = ticketsListWidget->takeItem(ticketsListWidget->count() - 1);
            delete item;
        }

        if (currentTicketId >= count) {
            currentTicketId = -1;
            numberLabel->setText("Номер билета: -");
            nameLabel->setText("Название: -");
        }
    }
    started->setMaximum(count);
    mastered->setMaximum(count);
    
    updateProgressBars();
}

void MainWindow::onNextClicked() {
    QVector<int> available;
    
    for (int i = 0; i < tickets.size(); ++i) {
        if (tickets[i].status != TicketStatus::kMastered && i != currentTicketId) {
            available.append(i);
        }
    }

    if (available.isEmpty()) {
        return;
    }

    int randomIndex = QRandomGenerator::global()->bounded(available.size());
    int nextTicketId = available[randomIndex];

    history.append(nextTicketId);
    historyIndex = history.size() - 1;

    ticketsListWidget->setCurrentRow(nextTicketId);
}

void MainWindow::onPrevClicked() {
    if (historyIndex <= 0) {
        return;
    }
    --historyIndex;
    int prevTicketId = history[historyIndex];
    history.pop_back();
    while (historyIndex > 0 && (prevTicketId >= tickets.size() || prevTicketId == currentTicketId)) {
        --historyIndex;
        prevTicketId = history[historyIndex];
        history.pop_back();
    }
    if (historyIndex > 0 || prevTicketId < tickets.size())
    {
        ticketsListWidget->setCurrentRow(prevTicketId);
    }

}

void MainWindow::onClearClicked() {
    for (int i = 0; i < tickets.size(); i++) {
        tickets[i].status = TicketStatus::kNotStart;
        tickets[i].name = "Билет " + QString::number(i + 1);

        QListWidgetItem *item = ticketsListWidget->item(i);
        item->setBackground(QBrush(Qt::gray));
        item->setText("Билет " + QString::number(i + 1));
    }

    updateProgressBars();
}

void MainWindow::onListRowChanged(int row) {
    if (row < 0 || row >= tickets.size()) {
        return; 
    }

    currentTicketId = row;
    numberLabel->setText("Номер билета: " + QString::number(row + 1));
    nameLabel->setText(tickets[row].name);

    statusEdit->blockSignals(true); 
    statusEdit->setCurrentIndex(static_cast<int>(tickets[row].status));
    statusEdit->blockSignals(false);

    history.append(row);
    historyIndex = history.size() - 1;
}

void MainWindow::onStatusIndexChanged(int index) {
    if (currentTicketId < 0 || currentTicketId >= tickets.size()) {
        return;
    }

    tickets[currentTicketId].status = static_cast<TicketStatus>(index);
    QListWidgetItem *item = ticketsListWidget->item(currentTicketId);
    if (!item) return; 

    if (index == 0) {
        item->setBackground(QBrush(Qt::gray));
    } else if (index == 1) {
        item->setBackground(QBrush(Qt::yellow));
    } else {
        item->setBackground(QBrush(Qt::green));
    }

    updateProgressBars();
}

void MainWindow::onItemDoubleClicked(QListWidgetItem *item) {
    if (!item || currentTicketId < 0) return;

    TicketStatus currentStatus = tickets[currentTicketId].status;
    int newIndex = 0;

    if (currentStatus == TicketStatus::kNotStart || currentStatus == TicketStatus::kRepeat) {
        newIndex = 2;
    } else {
        newIndex = 1;
    }
    
    statusEdit->setCurrentIndex(newIndex);
}

void MainWindow::updateProgressBars() {
    int startedCount = 0;
    int masteredCount = 0;

    for (int i = 0; i < tickets.size(); ++i) {
        if (tickets[i].status == TicketStatus::kRepeat || tickets[i].status == TicketStatus::kMastered) {
            startedCount++;
        }
        if (tickets[i].status == TicketStatus::kMastered) {
            masteredCount++;
        }
    }

    started->setValue(startedCount);
    mastered->setValue(masteredCount);
}

void MainWindow::onNameEditReturnPressed() {
    QString newName = nameEdit->text();
    
    if (!newName.isEmpty() && currentTicketId >= 0) {
        tickets[currentTicketId].name = newName;
        nameLabel->setText("Название: " + newName);

        QListWidgetItem *item = ticketsListWidget->item(currentTicketId);
        item->setText(newName);
        
        nameEdit->clear();
    }
}