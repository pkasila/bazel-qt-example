#include "ticket_app.h"

#include <QFile>
#include <QFileDialog>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QtWidgets>
#include <ctime>
#include <random>

TicketApp::TicketApp(QWidget* parent) : QWidget(parent) {
    std::srand(std::time(nullptr));
    setupUI();
    onCountChanged(m_countSpin->value());
    setWindowTitle("Exam Helper Pro");
    resize(1000, 650);
}

void TicketApp::setupUI() {
    QHBoxLayout* mainLayout = new QHBoxLayout(this);

    QVBoxLayout* leftLayout = new QVBoxLayout();

    QHBoxLayout* fileBox = new QHBoxLayout();
    m_saveBtn = new QPushButton("Сохранить");
    m_loadBtn = new QPushButton("Загрузить");
    fileBox->addWidget(m_saveBtn);
    fileBox->addWidget(m_loadBtn);
    leftLayout->addLayout(fileBox);

    m_countSpin = new QSpinBox();
    m_countSpin->setRange(1, 1000);
    m_countSpin->setValue(10);
    leftLayout->addWidget(new QLabel("Количество билетов:"));
    leftLayout->addWidget(m_countSpin);

    m_viewList = new QListWidget();
    leftLayout->addWidget(new QLabel("Список билетов:"));
    leftLayout->addWidget(m_viewList);

    m_totalProgress = new QProgressBar();
    m_totalProgress->setFormat("Общий прогресс: %p%");
    m_greenProgress = new QProgressBar();
    m_greenProgress->setFormat("Выучено: %p%");
    m_greenProgress->setStyleSheet("QProgressBar::chunk { background-color: #27ae60; }");

    leftLayout->addWidget(m_totalProgress);
    leftLayout->addWidget(m_greenProgress);

    m_questionGroup = new QGroupBox("Детали билета");
    QVBoxLayout* rightLayout = new QVBoxLayout(m_questionGroup);

    m_numberLabel = new QLabel("-");
    m_nameLabel = new QLabel("-");
    m_nameLabel->setWordWrap(true);
    m_nameLabel->setStyleSheet("font-weight: bold; font-size: 16px; color: #2c3e50;");

    m_nameEdit = new QLineEdit();
    m_nameEdit->setPlaceholderText("Новое имя (Enter)...");

    m_statusCombo = new QComboBox();
    m_statusCombo->addItems({"Дефолт", "Повторить", "Выучено"});

    m_hintEdit = new QTextEdit();
    m_hintEdit->setPlaceholderText("Твои заметки здесь...");

    rightLayout->addWidget(new QLabel("Номер:"));
    rightLayout->addWidget(m_numberLabel);
    rightLayout->addWidget(new QLabel("Название:"));
    rightLayout->addWidget(m_nameLabel);
    rightLayout->addWidget(m_nameEdit);
    rightLayout->addWidget(new QLabel("Статус:"));
    rightLayout->addWidget(m_statusCombo);
    rightLayout->addWidget(new QLabel("Подсказка:"));
    rightLayout->addWidget(m_hintEdit);

    QHBoxLayout* navLayout = new QHBoxLayout();
    m_prevBtn = new QPushButton("⬅ Назад");
    m_nextBtn = new QPushButton("Рандом ➡");
    navLayout->addWidget(m_prevBtn);
    navLayout->addWidget(m_nextBtn);
    rightLayout->addLayout(navLayout);

    mainLayout->addLayout(leftLayout, 1);
    mainLayout->addWidget(m_questionGroup, 2);

    connect(
        m_countSpin, QOverload<int>::of(&QSpinBox::valueChanged), this, &TicketApp::onCountChanged);
    connect(m_viewList, &QListWidget::itemClicked, this, &TicketApp::onTicketSelected);
    connect(m_viewList, &QListWidget::itemDoubleClicked, this, &TicketApp::onTicketDoubleClicked);
    connect(m_nameEdit, &QLineEdit::returnPressed, this, &TicketApp::onNameEdited);
    connect(
        m_statusCombo, QOverload<int>::of(&QComboBox::currentIndexChanged), this,
        &TicketApp::onStatusComboChanged);
    connect(m_hintEdit, &QTextEdit::textChanged, this, &TicketApp::onHintChanged);
    connect(m_nextBtn, &QPushButton::clicked, this, &TicketApp::nextRandomTicket);
    connect(m_prevBtn, &QPushButton::clicked, this, &TicketApp::previousTicket);
    connect(m_saveBtn, &QPushButton::clicked, this, &TicketApp::saveToFile);
    connect(m_loadBtn, &QPushButton::clicked, this, &TicketApp::loadFromFile);
}

void TicketApp::onCountChanged(int count) {
    m_tickets.clear();
    m_viewList->clear();
    m_history.clear();
    m_currentIndex = -1;
    for (int i = 0; i < count; ++i) {
        Ticket t;
        t.name = QString("Билет %1").arg(i + 1);
        m_tickets.append(t);
        QListWidgetItem* item = new QListWidgetItem(t.name);
        setItemColor(item, t.status);
        m_viewList->addItem(item);
    }
    updateProgress();
}

void TicketApp::setItemColor(QListWidgetItem* item, TicketStatus status) {
    if (status == TicketStatus::Green) {
        item->setBackground(Qt::green);
    } else if (status == TicketStatus::Yellow) {
        item->setBackground(Qt::yellow);
    } else {
        item->setBackground(Qt::lightGray);
    }
}

void TicketApp::onTicketSelected(QListWidgetItem* item) {
    updateTicketDisplay(m_viewList->row(item));
}

void TicketApp::updateTicketDisplay(int index) {
    if (index < 0 || index >= m_tickets.size()) {
        return;
    }
    m_currentIndex = index;
    Ticket& t = m_tickets[index];

    m_numberLabel->setText(QString::number(index + 1));
    m_nameLabel->setText(t.name);

    m_statusCombo->blockSignals(true);
    m_statusCombo->setCurrentIndex(static_cast<int>(t.status));
    m_statusCombo->blockSignals(false);

    m_hintEdit->blockSignals(true);
    m_hintEdit->setPlainText(t.hint);
    m_hintEdit->blockSignals(false);

    m_viewList->setCurrentRow(index);
}

void TicketApp::onTicketDoubleClicked(QListWidgetItem* item) {
    int index = m_viewList->row(item);
    Ticket& t = m_tickets[index];
    if (t.status == TicketStatus::Green) {
        t.status = TicketStatus::Yellow;
    } else {
        t.status = TicketStatus::Green;
    }
    setItemColor(item, t.status);
    updateTicketDisplay(index);
    updateProgress();
}

void TicketApp::onNameEdited() {
    if (m_currentIndex == -1 || m_nameEdit->text().trimmed().isEmpty()) {
        return;
    }
    m_tickets[m_currentIndex].name = m_nameEdit->text();
    m_nameLabel->setText(m_nameEdit->text());
    m_viewList->item(m_currentIndex)->setText(m_nameEdit->text());
    m_nameEdit->clear();
}

void TicketApp::onStatusComboChanged(int index) {
    if (m_currentIndex == -1) {
        return;
    }
    m_tickets[m_currentIndex].status = static_cast<TicketStatus>(index);
    setItemColor(m_viewList->item(m_currentIndex), m_tickets[m_currentIndex].status);
    updateProgress();
}

void TicketApp::onHintChanged() {
    if (m_currentIndex != -1) {
        m_tickets[m_currentIndex].hint = m_hintEdit->toPlainText();
    }
}

void TicketApp::updateProgress() {
    int total = m_tickets.size();
    if (total == 0) {
        return;
    }
    int yellowGreen = 0, green = 0;
    for (const auto& t : m_tickets) {
        if (t.status == TicketStatus::Green) {
            green++;
            yellowGreen++;
        } else if (t.status == TicketStatus::Yellow) {
            yellowGreen++;
        }
    }
    m_totalProgress->setValue((yellowGreen * 100) / total);
    m_greenProgress->setValue((green * 100) / total);
}

void TicketApp::nextRandomTicket() {
    QVector<int> avail;
    for (int i = 0; i < m_tickets.size(); ++i) {
        if (m_tickets[i].status != TicketStatus::Green) {
            avail.append(i);
        }
    }
    if (avail.isEmpty()) {
        QMessageBox::information(this, "Готово", "Все билеты выучены!");
        return;
    }
    int nextIdx = avail[std::rand() % avail.size()];
    if (m_currentIndex != -1) {
        m_history.push(m_currentIndex);
    }
    updateTicketDisplay(nextIdx);
}

void TicketApp::previousTicket() {
    if (!m_history.isEmpty()) {
        updateTicketDisplay(m_history.pop());
    }
}

void TicketApp::saveToFile() {
    QString path = QFileDialog::getSaveFileName(this, "Save", "", "JSON (*.json)");
    if (path.isEmpty()) {
        return;
    }
    QJsonArray arr;
    for (const auto& t : m_tickets) {
        QJsonObject o;
        o["name"] = t.name;
        o["hint"] = t.hint;
        o["status"] = (int)t.status;
        arr.append(o);
    }
    QFile f(path);
    if (f.open(QIODevice::WriteOnly)) {
        f.write(QJsonDocument(arr).toJson());
        f.close();
    }
}

void TicketApp::loadFromFile() {
    QString path = QFileDialog::getOpenFileName(this, "Load", "", "JSON (*.json)");
    if (path.isEmpty()) {
        return;
    }
    QFile f(path);
    if (!f.open(QIODevice::ReadOnly)) {
        return;
    }
    QJsonArray arr = QJsonDocument::fromJson(f.readAll()).array();
    m_countSpin->blockSignals(true);
    m_countSpin->setValue(arr.size());
    m_countSpin->blockSignals(false);
    onCountChanged(arr.size());
    for (int i = 0; i < arr.size(); ++i) {
        QJsonObject o = arr[i].toObject();
        m_tickets[i].name = o["name"].toString();
        m_tickets[i].hint = o["hint"].toString();
        m_tickets[i].status = (TicketStatus)o["status"].toInt();
        m_viewList->item(i)->setText(m_tickets[i].name);
        setItemColor(m_viewList->item(i), m_tickets[i].status);
    }
    updateProgress();
}