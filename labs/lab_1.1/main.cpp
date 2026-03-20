#include "Header.h"
#include <QApplication>
#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QSpinBox>
#include <QListWidget>
#include <QGroupBox>
#include <QLabel>
#include <QLineEdit>
#include <QComboBox>
#include <QPushButton>
#include <QProgressBar>
#include <QMessageBox>
#include <random>
#include <algorithm>

TicketApp::TicketApp() {
    setupUI();
    updateTicketCount(countSpinBox->value());
}

void TicketApp::updateTicketCount(int count) {
    tickets.clear();
    history.clear();
    listWidget->clear();
    for (int i = 0; i < count; ++i) {
        Ticket t;
        t.id = i + 1;
        t.name = QString("Білет %1").arg(t.id);
        tickets.push_back(t);

        auto item = new QListWidgetItem(t.name, listWidget);
        item->setBackground(Qt::lightGray);
    }
    updateProgress();
    clearQuestionView();
}

void TicketApp::onTicketSelected(QListWidgetItem* item) {
    if (!item) return;
    int index = listWidget->row(item);
    displayTicket(index);
}

void TicketApp::onTicketDoubleClicked(QListWidgetItem* item) {
    int index = listWidget->row(item);
    if (tickets[index].status == Status::Green) {
        updateStatus(index, Status::Yellow);
    } else {
        updateStatus(index, Status::Green);
    }
}

void TicketApp::onNameEdited() {
    if (currentIndex == -1 || nameEdit->text().isEmpty()) return;
    tickets[currentIndex].name = nameEdit->text();
    nameLabel->setText(tickets[currentIndex].name);
    listWidget->item(currentIndex)->setText(tickets[currentIndex].name);
}

void TicketApp::onStatusComboChanged(int comboIndex) {
    if (currentIndex == -1) return;
    Status newStatus = static_cast<Status>(comboIndex);
    
    if (tickets[currentIndex].status == newStatus) return;
    
    updateStatus(currentIndex, newStatus);
}

void TicketApp::nextRandomQuestion() {
    std::vector<int> available;
    for (int i = 0; i < (int)tickets.size(); ++i) {
        if (tickets[i].status != Status::Green) {
            available.push_back(i);
        }
    }

    if (available.empty()) {
        QMessageBox::information(this, "Гатова!", "Усе білеты вывучаныя!");
        return;
    }

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, available.size() - 1);
    
    int nextIndex = available[dis(gen)];
    if (currentIndex != -1) history.push_back(currentIndex);
    displayTicket(nextIndex);
}

void TicketApp::previousQuestion() {
    if (history.empty()) return;
    int prevIndex = history.back();
    history.pop_back();
    displayTicket(prevIndex);
}

void TicketApp::setupUI() {
    auto central = new QWidget();
    auto mainLayout = new QHBoxLayout(central);

    auto leftPanel = new QVBoxLayout();
    countSpinBox = new QSpinBox();
    countSpinBox->setRange(1, 100);
    countSpinBox->setValue(10);
    leftPanel->addWidget(new QLabel("Колькасць білетаў:"));
    leftPanel->addWidget(countSpinBox);

    listWidget = new QListWidget();
    leftPanel->addWidget(listWidget);

    auto rightPanel = new QVBoxLayout();
    questionGroup = new QGroupBox("Інфармацыя пра білет");
    auto qLayout = new QFormLayout(questionGroup);
    
    numLabel = new QLabel("-");
    nameLabel = new QLabel("-");
    nameEdit = new QLineEdit();
    statusCombo = new QComboBox();
    statusCombo->addItems({"Не пачата", "Паўтарыць", "Вывучана"});

    qLayout->addRow("Нумар:", numLabel);
    qLayout->addRow("Назва:", nameLabel);
    qLayout->addRow("Рэдагаванне назвы:", nameEdit);
    qLayout->addRow("Статус:", statusCombo);

    auto navLayout = new QHBoxLayout();
    prevBtn = new QPushButton("Папярэдні");
    nextBtn = new QPushButton("Выпадковы наступны");
    navLayout->addWidget(prevBtn);
    navLayout->addWidget(nextBtn);
    rightPanel->addWidget(questionGroup);
    rightPanel->addLayout(navLayout);

    totalProgress = new QProgressBar();
    greenProgress = new QProgressBar();
    rightPanel->addWidget(new QLabel("Агульны прагрэс (Спрабаваў):"));
    rightPanel->addWidget(totalProgress);
    rightPanel->addWidget(new QLabel("Вывучана (Зялёныя):"));
    rightPanel->addWidget(greenProgress);

    mainLayout->addLayout(leftPanel, 1);
    mainLayout->addLayout(rightPanel, 2);

    setCentralWidget(central);
    resize(850, 500);

    connect(countSpinBox, QOverload<int>::of(&QSpinBox::valueChanged), this, &TicketApp::updateTicketCount);
    connect(listWidget, &QListWidget::itemClicked, this, &TicketApp::onTicketSelected);
    connect(listWidget, &QListWidget::itemDoubleClicked, this, &TicketApp::onTicketDoubleClicked);
    connect(nameEdit, &QLineEdit::returnPressed, this, &TicketApp::onNameEdited);
    
    // ВЫКАРЫСТОЎВАЕМ activated ЗАМЕСТ currentIndexChanged
    connect(statusCombo, QOverload<int>::of(&QComboBox::activated), this, &TicketApp::onStatusComboChanged);
    
    connect(nextBtn, &QPushButton::clicked, this, &TicketApp::nextRandomQuestion);
    connect(prevBtn, &QPushButton::clicked, this, &TicketApp::previousQuestion);
}

void TicketApp::displayTicket(int index) {
    if (index < 0 || index >= (int)tickets.size()) return;
    currentIndex = index;
    listWidget->setCurrentRow(index);
    auto& t = tickets[index];
    
    numLabel->setText(QString::number(t.id));
    nameLabel->setText(t.name);
    nameEdit->setText(t.name);
    
    statusCombo->blockSignals(true);
    statusCombo->setCurrentIndex(static_cast<int>(t.status));
    statusCombo->blockSignals(false);
}

void TicketApp::updateStatus(int index, Status s) {
    tickets[index].status = s;
    auto item = listWidget->item(index);
    if (s == Status::Default) item->setBackground(Qt::lightGray);
    else if (s == Status::Yellow) item->setBackground(Qt::yellow);
    else if (s == Status::Green) item->setBackground(Qt::green);

    if (currentIndex == index) {
        statusCombo->blockSignals(true);
        statusCombo->setCurrentIndex(static_cast<int>(s));
        statusCombo->blockSignals(false);
    }
    updateProgress();
}

void TicketApp::updateProgress() {
    int totalAttempted = 0;
    int totalGreen = 0;
    for (const auto& t : tickets) {
        if (t.status != Status::Default) totalAttempted++;
        if (t.status == Status::Green) totalGreen++;
    }
    totalProgress->setRange(0, (int)tickets.size());
    totalProgress->setValue(totalAttempted);
    greenProgress->setRange(0, (int)tickets.size());
    greenProgress->setValue(totalGreen);
}

void TicketApp::clearQuestionView() {
    currentIndex = -1;
    numLabel->setText("-");
    nameLabel->setText("-");
    nameEdit->clear();
    
    statusCombo->blockSignals(true);
    statusCombo->setCurrentIndex(0);
    statusCombo->blockSignals(false);
}

int main(int argc, char** argv) {
    QApplication a(argc, argv);
    TicketApp w;
    w.setWindowTitle("Падрыхтоўка да матану");
    w.show();
    return a.exec();
}