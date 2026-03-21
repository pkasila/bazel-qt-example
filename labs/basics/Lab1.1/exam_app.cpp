#include "exam_app.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGroupBox>
#include <random>

ExamApp::ExamApp(QWidget *parent) : QWidget(parent) {
    setupUi();
    onCountChanged(countSpinBox->value());
}

void ExamApp::setupUi() {
    auto *mainLayout = new QHBoxLayout(this);
    auto *leftLayout = new QVBoxLayout();
    
    // Левая часть: Настройки и список
    countSpinBox = new QSpinBox();
    countSpinBox->setRange(1, 100);
    leftLayout->addWidget(new QLabel("Количество билетов:"));
    leftLayout->addWidget(countSpinBox);

    viewWidget = new QListWidget();
    leftLayout->addWidget(viewWidget);
    
    totalProgress = new QProgressBar();
    greenProgress = new QProgressBar();
    leftLayout->addWidget(new QLabel("Общий прогресс:"));
    leftLayout->addWidget(totalProgress);
    leftLayout->addWidget(new QLabel("Только выученные:"));
    leftLayout->addWidget(greenProgress);

    // Правая часть: Редактирование билета
    auto *rightLayout = new QVBoxLayout();
    auto *groupBox = new QGroupBox("Инфо о билете");
    auto *groupLayout = new QVBoxLayout(groupBox);

    numberLabel = new QLabel("Выберите билет");
    nameLabel = new QLabel("-");
    nameEdit = new QLineEdit();
    nameEdit->setPlaceholderText("Введите новое имя...");

    statusCombo = new QComboBox();
    statusCombo->addItems({"Default (Grey)", "Yellow", "Green"});

    auto *btnLayout = new QHBoxLayout();
    auto *prevBtn = new QPushButton("Назад");
    auto *nextBtn = new QPushButton("Рандомный следующий");

    groupLayout->addWidget(new QLabel("Номер:"));
    groupLayout->addWidget(numberLabel);
    groupLayout->addWidget(new QLabel("Тема:"));
    groupLayout->addWidget(nameLabel);
    groupLayout->addWidget(nameEdit);
    groupLayout->addWidget(new QLabel("Статус:"));
    groupLayout->addWidget(statusCombo);
    btnLayout->addWidget(prevBtn);
    btnLayout->addWidget(nextBtn);
    groupLayout->addLayout(btnLayout);

    rightLayout->addWidget(groupBox);
    rightLayout->addStretch();

    mainLayout->addLayout(leftLayout, 1);
    mainLayout->addLayout(rightLayout, 1);

    // Signals
    connect(countSpinBox, QOverload<int>::of(&QSpinBox::valueChanged), this, &ExamApp::onCountChanged);
    connect(viewWidget, &QListWidget::itemClicked, this, &ExamApp::onTicketSelected);
    connect(viewWidget, &QListWidget::itemDoubleClicked, this, &ExamApp::onTicketDoubleClicked);
    connect(nameEdit, &QLineEdit::returnPressed, this, &ExamApp::updateTicketName);
    connect(statusCombo, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &ExamApp::onStatusChanged);
    connect(nextBtn, &QPushButton::clicked, this, &ExamApp::nextRandomTicket);
    connect(prevBtn, &QPushButton::clicked, this, &ExamApp::previousTicket);
}

void ExamApp::onCountChanged(int count) {
    tickets.clear();
    viewWidget->clear();
    history.clear();
    for (int i = 0; i < count; ++i) {
        tickets.push_back({QString("Билет %1").arg(i + 1), TicketStatus::Default});
        auto *item = new QListWidgetItem(tickets[i].name);
        item->setBackground(Qt::lightGray);
        viewWidget->addItem(item);
    }
    updateProgress();
}

void ExamApp::updateItemVisuals(int index) {
    auto *item = viewWidget->item(index);
    item->setText(tickets[index].name);
    switch (tickets[index].status) {
        case TicketStatus::Default: item->setBackground(Qt::lightGray); break;
        case TicketStatus::Yellow:  item->setBackground(Qt::yellow); break;
        case TicketStatus::Green:   item->setBackground(Qt::green); break;
    }
}

void ExamApp::onTicketSelected(QListWidgetItem *item) {
    currentIndex = viewWidget->row(item);
    updateUIForTicket(currentIndex);
}

void ExamApp::updateUIForTicket(int index) {
    if (index < 0) return;
    numberLabel->setText(QString::number(index + 1));
    nameLabel->setText(tickets[index].name);
    
    statusCombo->blockSignals(true);
    statusCombo->setCurrentIndex((int)tickets[index].status);
    statusCombo->blockSignals(false);
    
    viewWidget->setCurrentRow(index);
}
void ExamApp::updateTicketName() {
    if (currentIndex >= 0 && !nameEdit->text().isEmpty()) {
        tickets[currentIndex].name = nameEdit->text();
        updateItemVisuals(currentIndex);
        nameLabel->setText(nameEdit->text());
        nameEdit->clear();
    }
}

void ExamApp::onStatusChanged(int index) {
    if (currentIndex >= 0) {
        tickets[currentIndex].status = static_cast<TicketStatus>(index);
        updateItemVisuals(currentIndex);
        updateProgress();
    }
}

void ExamApp::onTicketDoubleClicked(QListWidgetItem *item) {
    int row = viewWidget->row(item);
    if (tickets[row].status == TicketStatus::Green) 
        tickets[row].status = TicketStatus::Yellow;
    else 
        tickets[row].status = TicketStatus::Green;
    
    updateItemVisuals(row);
    updateUIForTicket(row);
    updateProgress();
}

void ExamApp::nextRandomTicket() {
    QVector<int> candidates;
    for (int i = 0; i < tickets.size(); ++i) {
        if (tickets[i].status != TicketStatus::Green) candidates.append(i);
    }

    if (candidates.isEmpty()) return;

    int nextIdx = candidates[rand() % candidates.size()];
    if (currentIndex != -1) history.push(currentIndex);
    currentIndex = nextIdx;
    updateUIForTicket(currentIndex);
}

void ExamApp::previousTicket() {
    if (!history.isEmpty()) {
        currentIndex = history.pop();
        updateUIForTicket(currentIndex);
    }
}

void ExamApp::updateProgress() {
    int totalCount = tickets.size();
    int nonDefault = 0;
    int greenCount = 0;
    for (const auto& t : tickets) {
        if (t.status != TicketStatus::Default) nonDefault++;
        if (t.status == TicketStatus::Green) greenCount++;
    }
    totalProgress->setValue((nonDefault * 100) / totalCount);
    greenProgress->setValue((greenCount * 100) / totalCount);
}
