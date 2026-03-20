// NOLINTBEGIN(cppcoreguidelines-owning-memory, readability-identifier-naming)
#include "mainwindow.h"

#include <QColor>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QListWidgetItem>
#include <QRandomGenerator>
#include <QVBoxLayout>
#include <algorithm>

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
    , countSpin(new QSpinBox)
    , ticketsList(new QListWidget)
    , nameEdit(new QLineEdit)
    , statusCombo(new QComboBox)
    , progressBar(new QProgressBar)
    , progressBar2(new QProgressBar)
    , statsLabel(new QLabel)
    , studyTimer(new QTimer(this)) {
    auto* centralWidget = new QWidget(this);
    auto* mainLayout = new QHBoxLayout(centralWidget);

    auto* leftPanel = new QWidget;
    auto* leftLayout = new QVBoxLayout(leftPanel);

    countSpin->setMinimum(0);
    countSpin->setMaximum(100);
    countSpin->setValue(0);

    leftLayout->addWidget(countSpin);
    leftLayout->addWidget(ticketsList);

    auto* rightPanel = new QGroupBox("Текущий билет");
    auto* rightLayout = new QVBoxLayout(rightPanel);

    numberLabel = new QLabel("Номер: ");
    nameLabel = new QLabel("Название: ");
    previousButton = new QPushButton("Предыдущий");
    nextButton = new QPushButton("Следующий");

    rightLayout->addWidget(numberLabel);
    rightLayout->addWidget(nameLabel);
    rightLayout->addWidget(nameEdit);
    rightLayout->addWidget(statusCombo);
    rightLayout->addWidget(previousButton);
    rightLayout->addWidget(nextButton);

    mainLayout->addWidget(leftPanel);
    mainLayout->addWidget(rightPanel);
    setCentralWidget(centralWidget);

    // Progress
    progressBar->setTextVisible(true);
    progressBar->setFormat("%p% завершено");
    progressBar->setMinimum(0);
    progressBar->setMaximum(100);
    progressBar->setStyleSheet(
        "QProgressBar {"
        "   border: 2px solid #3A3A3A;"
        "   border-radius: 5px;"
        "   text-align: center;"
        "}"
        "QProgressBar::chunk {"
        "   background-color: #4CAF50;"
        "   border-radius: 3px;"
    "}");

    progressBar2->setTextVisible(true);
    progressBar2->setFormat("%p% завершено + в процессе");
    progressBar2->setMinimum(0);
    progressBar2->setMaximum(100);

    statusCombo->addItems({"Default", "Yellow", "Green"});

    // Stats
    auto* statsGroup = new QGroupBox("Статистика");
    auto* statsLayout = new QVBoxLayout(statsGroup);

    auto* timeButtonsLayout = new QHBoxLayout;
    pauseButton = new QPushButton("⏸ Пауза");
    auto* timeButton = new QPushButton("⏱ Сбросить время");

    timeButtonsLayout->addWidget(pauseButton);
    timeButtonsLayout->addWidget(timeButton);

    statsLayout->addLayout(timeButtonsLayout);

    studyTimer->start(1000);

    statsLayout->addWidget(progressBar);
    statsLayout->addWidget(progressBar2);
    updateProgress();
    statsLayout->addWidget(statsLabel);

    totalStudyTime = QTime(0, 0);
    updateStatistics();
    rightLayout->addWidget(statsGroup);

    // Slots-Signals
    connect(studyTimer, &QTimer::timeout, [this]() {
        totalStudyTime = totalStudyTime.addSecs(1);
        updateStatistics();
    });

    connect(timeButton, &QPushButton::clicked, [this]() {
        totalStudyTime = QTime(0, 0);
        updateStatistics();
    });

    connect(pauseButton, &QPushButton::clicked, [this]() {
        isPaused = !isPaused;
        pauseButton->setText(isPaused ? "▶ Продолжить" : "⏸ Пауза");
        pauseButton->setStyleSheet(isPaused ? "background-color: #e74c3c; color: white;" : "");

        if (isPaused) {
            studyTimer->stop();
        } else {
            studyTimer->start();
        }
    });

    connect(
        countSpin, QOverload<int>::of(&QSpinBox::valueChanged), this, &MainWindow::onCountChanged);
    connect(ticketsList, &QListWidget::itemClicked, this, &MainWindow::onItemClicked);
    connect(ticketsList, &QListWidget::itemDoubleClicked, this, &MainWindow::onItemDoubleClicked);
    connect(nameEdit, &QLineEdit::returnPressed, this, &MainWindow::onNameEdited);
    connect(
        statusCombo, QOverload<int>::of(&QComboBox::currentIndexChanged), this,
        &MainWindow::onStatusChanged);
    connect(nextButton, &QPushButton::clicked, this, &MainWindow::onNextClicked);
    connect(previousButton, &QPushButton::clicked, this, &MainWindow::onPreviousClicked);
}

MainWindow::~MainWindow() = default;

void MainWindow::onCountChanged(int count) {
    tickets.clear();
    for (int i = 0; i < count; i++) {
        tickets.append(Ticket(i + 1, QString("Билет %1").arg(i + 1), TicketStatus::Default));
    }
    history.clear();
    currentIndex = -1;
    updateTicketList();
    updateStatistics();
    updateQuestionView();
    updateProgress();
}

void MainWindow::onItemClicked(QListWidgetItem* item) {
    currentIndex = item->data(Qt::UserRole).toInt();
    updateQuestionView();
    if (!history.empty() && history.last() == currentIndex) {
        return;
    }
    history.append(currentIndex);
}

void MainWindow::onItemDoubleClicked(QListWidgetItem* item) {
    int idx = item->data(Qt::UserRole).toInt();
    Ticket& t = tickets[idx];
    t.status = (t.status == TicketStatus::Green) ? TicketStatus::Yellow : TicketStatus::Green;
    currentIndex = idx;
    updateTicketList();
    updateQuestionView();
    updateProgress();
    updateStatistics();
    if (!history.empty() && history.last() == currentIndex) {
        return;
    }
    history.append(currentIndex);
}

void MainWindow::onNameEdited() {
    if (currentIndex == -1) {
        return;
    }
    QString newName = nameEdit->text().trimmed();
    if (!newName.isEmpty()) {
        tickets[currentIndex].name = newName;
        nameLabel->setText("Название: " + newName);
        ticketsList->item(currentIndex)->setText(newName);
    }
}

void MainWindow::onStatusChanged(int index) {
    if (currentIndex == -1) {
        return;
    }
    tickets[currentIndex].status = intToStatus(index);
    updateCurrentItem();
    updateProgress();
    updateStatistics();
}

void MainWindow::onNextClicked() {
    QVector<int> available = getAvailableTickets();
    if (available.isEmpty()) {
        return;
    }

    int randomIdx = static_cast<int>(QRandomGenerator::global()->bounded(available.size()));
    history.append(currentIndex);
    currentIndex = available[randomIdx];
    updateQuestionView();
    ticketsList->setCurrentRow(currentIndex);
}

void MainWindow::onPreviousClicked() {
    if (history.isEmpty()) {
        return;
    }
    currentIndex = history.takeLast();
    updateQuestionView();
    ticketsList->setCurrentRow(currentIndex);
}

void MainWindow::updateTicketList() {
    ticketsList->clear();
    for (const Ticket& t : tickets) {
        auto* item = new QListWidgetItem(t.name);
        item->setData(Qt::UserRole, t.number - 1);
        item->setBackground(statusColor(t.status));
        ticketsList->addItem(item);
    }
}

void MainWindow::updateQuestionView() {
    if (currentIndex == -1 || currentIndex >= tickets.size()) {
        numberLabel->setText("Номер: ");
        nameLabel->setText("Название: ");
        nameEdit->setText("");
        statusCombo->setCurrentIndex(statusToInt(TicketStatus::Default));
        return;
    }

    const Ticket& t = tickets[currentIndex];
    numberLabel->setText("Номер: " + QString::number(t.number));
    nameLabel->setText("Название: " + t.name);
    nameEdit->setText(t.name);
    statusCombo->setCurrentIndex(statusToInt(t.status));
}

void MainWindow::updateCurrentItem() {
    if (currentIndex == -1) {
        return;
    }
    QListWidgetItem* item = ticketsList->item(currentIndex);
    item->setBackground(statusColor(tickets[currentIndex].status));
}

void MainWindow::updateProgress() {
    int completed = 0;
    int inProgress = 0;
    for (const Ticket& ticket : tickets) {
        if (ticket.status == TicketStatus::Green) {
            completed++;
        }
        if (ticket.status == TicketStatus::Yellow) {
            inProgress++;
        }
    }

    if (!tickets.isEmpty()) {
        int progress = static_cast<int>(
            (static_cast<double>(completed) / static_cast<double>(tickets.size())) * 100);
        int progress2 = static_cast<int>(
            (static_cast<double>(completed + inProgress) / static_cast<double>(tickets.size())) *
            100);
        progressBar->setValue(progress);
        progressBar2->setValue(progress2);
    } else {
        progressBar->setValue(0);
        progressBar2->setValue(0);
    }
}

void MainWindow::updateStatistics() {
    qsizetype total = tickets.size();
    int completed = std::count_if(tickets.begin(), tickets.end(), [](const Ticket& t) {
        return t.status == TicketStatus::Green;
    });

    QString statsText =
        QString(
            "📊 Прогресс:\n"
            "✅ Завершено: %1/%2\n"
            "🔄 Требует повтора: %3\n"
            "⏱ Время изучения: %4")
            .arg(completed)
            .arg(total)
            .arg(
                total - completed -
                std::count_if(
                    tickets.begin(), tickets.end(),
                    [](const Ticket& t) { return t.status == TicketStatus::Default; }))
            .arg(totalStudyTime.toString("hh:mm:ss"));

    statsLabel->setText(statsText);
    QString timeText = isPaused ? totalStudyTime.toString("hh:mm:ss") + " (пауза)"
                                : totalStudyTime.toString("hh:mm:ss");

    statsText = statsText.arg(timeText);
}

QColor MainWindow::statusColor(TicketStatus status) {
    switch (status) {
        case TicketStatus::Default:
            return Qt::gray;
        case TicketStatus::Yellow:
            return Qt::yellow;
        case TicketStatus::Green:
            return Qt::green;
    }
    return Qt::white;
}

TicketStatus MainWindow::intToStatus(int index) {
    return static_cast<TicketStatus>(index);
}

int MainWindow::statusToInt(TicketStatus status) {
    return static_cast<int>(status);
}

QVector<int> MainWindow::getAvailableTickets() {
    QVector<int> available;
    for (int i = 0; i < tickets.size(); i++) {
        if (tickets[i].status != TicketStatus::Green) {
            available.append(i);
        }
    }
    return available;
}

// NOLINTEND(cppcoreguidelines-owning-memory, readability-identifier-naming)