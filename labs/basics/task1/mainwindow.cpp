#include "mainwindow.h"

#include <QColor>
#include <QComboBox>
#include <QFormLayout>
#include <QFrame>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QListWidget>
#include <QListWidgetItem>
#include <QMessageBox>
#include <QProgressBar>
#include <QPushButton>
#include <QRandomGenerator>
#include <QSpinBox>
#include <QSplitter>
#include <QVBoxLayout>
#include <QWidget>

MainWindow::MainWindow()
{
    setupUi();
    applyStyle();
    setupConnections();
    rebuildTickets(countSpinBox_->value());
}

void MainWindow::setupUi()
{
    setWindowTitle("Повторение билетов");
    resize(980, 620);

    QWidget *central = new QWidget(this);
    setCentralWidget(central);

    QVBoxLayout *rootLayout = new QVBoxLayout(central);
    rootLayout->setContentsMargins(14, 14, 14, 14);
    rootLayout->setSpacing(12);

    QSplitter *splitter = new QSplitter(Qt::Horizontal, central);
    splitter->setChildrenCollapsible(false);

    QWidget *leftPanel = new QWidget(splitter);
    QVBoxLayout *leftLayout = new QVBoxLayout(leftPanel);
    leftLayout->setContentsMargins(0, 0, 0, 0);
    leftLayout->setSpacing(10);

    QLabel *countLabel = new QLabel("Количество билетов:", leftPanel);
    countSpinBox_ = new QSpinBox(leftPanel);
    countSpinBox_->setRange(1, 300);
    countSpinBox_->setValue(20);

    viewList_ = new QListWidget(leftPanel);
    viewList_->setSelectionMode(QAbstractItemView::SingleSelection);

    leftLayout->addWidget(countLabel);
    leftLayout->addWidget(countSpinBox_);
    leftLayout->addWidget(viewList_, 1);

    QWidget *rightPanel = new QWidget(splitter);
    QVBoxLayout *rightLayout = new QVBoxLayout(rightPanel);
    rightLayout->setContentsMargins(0, 0, 0, 0);
    rightLayout->setSpacing(10);

    questionBox_ = new QGroupBox("Текущий билет", rightPanel);
    QVBoxLayout *questionLayout = new QVBoxLayout(questionBox_);
    questionLayout->setSpacing(10);

    numberLabel_ = new QLabel(questionBox_);
    nameLabel_ = new QLabel(questionBox_);

    QFormLayout *formLayout = new QFormLayout;
    formLayout->setHorizontalSpacing(10);
    formLayout->setVerticalSpacing(8);

    nameEdit_ = new QLineEdit(questionBox_);
    nameEdit_->setPlaceholderText("Введите новое название и нажмите Enter");

    statusCombo_ = new QComboBox(questionBox_);
    statusCombo_->addItem(statusText(TicketStatus::Default));
    statusCombo_->addItem(statusText(TicketStatus::Yellow));
    statusCombo_->addItem(statusText(TicketStatus::Green));

    formLayout->addRow("Название:", nameEdit_);
    formLayout->addRow("Статус:", statusCombo_);

    QHBoxLayout *buttonsLayout = new QHBoxLayout;
    previousButton_ = new QPushButton("Предыдущий", questionBox_);
    nextButton_ = new QPushButton("Случайный следующий", questionBox_);
    buttonsLayout->addWidget(previousButton_);
    buttonsLayout->addWidget(nextButton_);

    QLabel *totalProgressLabel = new QLabel("Общий прогресс", questionBox_);
    totalProgressBar_ = new QProgressBar(questionBox_);

    QLabel *greenProgressLabel = new QLabel("Пройденный прогресс", questionBox_);
    greenProgressBar_ = new QProgressBar(questionBox_);

    questionLayout->addWidget(numberLabel_);
    questionLayout->addWidget(nameLabel_);
    questionLayout->addLayout(formLayout);
    questionLayout->addLayout(buttonsLayout);
    questionLayout->addWidget(totalProgressLabel);
    questionLayout->addWidget(totalProgressBar_);
    questionLayout->addWidget(greenProgressLabel);
    questionLayout->addWidget(greenProgressBar_);
    questionLayout->addStretch(1);

    rightLayout->addWidget(questionBox_);

    splitter->addWidget(leftPanel);
    splitter->addWidget(rightPanel);
    splitter->setStretchFactor(0, 3);
    splitter->setStretchFactor(1, 4);

    rootLayout->addWidget(splitter, 1);
}

void MainWindow::applyStyle()
{
    setStyleSheet(
        "QMainWindow { background-color: #f4f7fb; }"
        "QGroupBox {"
        "  border: 1px solid #d5deea;"
        "  border-radius: 10px;"
        "  margin-top: 8px;"
        "  padding-top: 12px;"
        "  font-weight: 600;"
        "  color: #0f172a;"
        "  background-color: #ffffff;"
        "}"
        "QGroupBox::title {"
        "  subcontrol-origin: margin;"
        "  left: 10px;"
        "  padding: 0 6px;"
        "  color: #0f172a;"
        "  background-color: #f4f7fb;"
        "}"
        "QLabel { color: #1f2937; }"
        "QListWidget {"
        "  border: 1px solid #d5deea;"
        "  border-radius: 10px;"
        "  background-color: #ffffff;"
        "  padding: 4px;"
        "  outline: 0;"
        "  selection-background-color: transparent;"
        "  selection-color: #111827;"
        "}"
        "QListWidget::item {"
        "  margin: 3px 2px;"
        "  padding: 8px;"
        "  color: #111827;"
        "  outline: 0;"
        "}"
        "QListWidget::item:selected {"
        "  border: 2px solid #2563eb;"
        "  color: #111827;"
        "  background-color: transparent;"
        "  outline: 0;"
        "}"
        "QListWidget::item:focus {"
        "  outline: 0;"
        "}"
        "QLineEdit, QSpinBox, QComboBox {"
        "  border: 1px solid #c7d2e3;"
        "  border-radius: 8px;"
        "  padding: 6px 8px;"
        "  background-color: #ffffff;"
        "  color: #111827;"
        "}"
        "QComboBox:focus {"
        "  border: 1px solid #c7d2e3;"
        "  outline: 0;"
        "}"
        "QComboBox::drop-down {"
        "  border: none;"
        "  background-color: #edf2fb;"
        "  width: 26px;"
        "}"
        "QComboBox QAbstractItemView {"
        "  background-color: #ffffff;"
        "  color: #111827;"
        "  border: none;"
        "  border-top: 0px;"
        "  border-bottom: 0px;"
        "  padding-top: 2px;"
        "  padding-bottom: 2px;"
        "  selection-background-color: #dbeafe;"
        "  selection-color: #111827;"
        "  outline: 0;"
        "}"
        "QComboBox QAbstractItemView::item {"
        "  border: none;"
        "  padding: 6px 8px;"
        "  outline: 0;"
        "}"
        "QComboBox QAbstractItemView::item:selected {"
        "  border: none;"
        "  outline: 0;"
        "  background-color: #dbeafe;"
        "  color: #111827;"
        "}"
        "QSpinBox::up-button, QSpinBox::down-button {"
        "  background-color: #edf2fb;"
        "  border-left: 1px solid #c7d2e3;"
        "  width: 22px;"
        "  color: #1f2937;"
        "}"
        "QSpinBox::up-button:hover, QSpinBox::down-button:hover {"
        "  background-color: #e2e9f7;"
        "}"
        "QScrollBar:vertical {"
        "  background: #eef3fb;"
        "  width: 12px;"
        "  margin: 2px;"
        "  border-radius: 6px;"
        "}"
        "QScrollBar::handle:vertical {"
        "  background: #9bb0d4;"
        "  min-height: 28px;"
        "  border-radius: 6px;"
        "}"
        "QScrollBar::handle:vertical:hover {"
        "  background: #7f98c6;"
        "}"
        "QScrollBar:horizontal {"
        "  background: #eef3fb;"
        "  height: 12px;"
        "  margin: 2px;"
        "  border-radius: 6px;"
        "}"
        "QScrollBar::handle:horizontal {"
        "  background: #9bb0d4;"
        "  min-width: 28px;"
        "  border-radius: 6px;"
        "}"
        "QScrollBar::handle:horizontal:hover {"
        "  background: #7f98c6;"
        "}"
        "QScrollBar::add-line, QScrollBar::sub-line, QScrollBar::add-page, QScrollBar::sub-page {"
        "  background: transparent;"
        "  border: none;"
        "}"
        "QPushButton {"
        "  border: none;"
        "  border-radius: 8px;"
        "  padding: 8px 12px;"
        "  background-color: #2563eb;"
        "  color: #ffffff;"
        "  font-weight: 600;"
        "}"
        "QPushButton:disabled {"
        "  background-color: #93a8ce;"
        "}"
        "QProgressBar {"
        "  border: 1px solid #c7d2e3;"
        "  border-radius: 8px;"
        "  text-align: center;"
        "  color: #111827;"
        "  font-weight: 600;"
        "  background-color: #e7edf8;"
        "  min-height: 22px;"
        "}"
        "QProgressBar::chunk {"
        "  border-radius: 7px;"
        "  background-color: #2f9e44;"
        "}"
    );
}

void MainWindow::setupConnections()
{
    connect(countSpinBox_, qOverload<int>(&QSpinBox::valueChanged), this, [this](int value) {
        rebuildTickets(value);
    });

    connect(viewList_, &QListWidget::itemClicked, this, [this](QListWidgetItem *item) {
        const int index = viewList_->row(item);
        setCurrentTicket(index, true);
    });

    connect(viewList_, &QListWidget::itemDoubleClicked, this, [this](QListWidgetItem *item) {
        const int index = viewList_->row(item);
        if (index < 0 || index >= tickets_.size()) {
            return;
        }

        TicketStatus nextStatus = TicketStatus::Yellow;
        if (tickets_[index].status == TicketStatus::Default || tickets_[index].status == TicketStatus::Yellow) {
            nextStatus = TicketStatus::Green;
        }
        setTicketStatus(index, nextStatus);
        setCurrentTicket(index, true);
    });

    connect(statusCombo_, qOverload<int>(&QComboBox::currentIndexChanged), this, [this](int comboIndex) {
        if (suppressStatusComboSignal_) {
            return;
        }
        applyStatusToCurrentFromCombo(comboIndex);
    });

    connect(nameEdit_, &QLineEdit::returnPressed, this, [this]() {
        commitCurrentNameFromEditor();
    });

    connect(nextButton_, &QPushButton::clicked, this, [this]() {
        moveToNextRandomTicket();
    });

    connect(previousButton_, &QPushButton::clicked, this, [this]() {
        moveToPreviousTicket();
    });
}

void MainWindow::rebuildTickets(int count)
{
    tickets_.clear();
    tickets_.reserve(count);
    for (int i = 0; i < count; ++i) {
        Ticket ticket;
        ticket.number = i + 1;
        ticket.name = QString("Билет %1").arg(ticket.number);
        ticket.status = TicketStatus::Default;
        tickets_.push_back(ticket);
    }

    history_.clear();
    historyPosition_ = -1;
    previousButton_->setEnabled(false);

    viewList_->clear();
    for (int i = 0; i < tickets_.size(); ++i) {
        QListWidgetItem *item = new QListWidgetItem(ticketDisplayText(tickets_[i]));
        viewList_->addItem(item);
        updateTicketItem(i);
    }

    totalProgressBar_->setRange(0, count);
    greenProgressBar_->setRange(0, count);

    if (!tickets_.isEmpty()) {
        setCurrentTicket(0, true);
    } else {
        currentTicketIndex_ = -1;
        updateQuestionView();
    }

    updateProgressBars();
}

void MainWindow::setCurrentTicket(int index, bool addToHistory)
{
    if (index < 0 || index >= tickets_.size()) {
        return;
    }

    currentTicketIndex_ = index;
    viewList_->setCurrentRow(index);
    if (addToHistory) {
        pushHistory(index);
    }

    updateQuestionView();
}

void MainWindow::pushHistory(int index)
{
    if (historyPosition_ + 1 < history_.size()) {
        history_.resize(historyPosition_ + 1);
    }

    if (!history_.isEmpty() && history_.last() == index) {
        historyPosition_ = history_.size() - 1;
        previousButton_->setEnabled(historyPosition_ > 0);
        return;
    }

    history_.push_back(index);
    historyPosition_ = history_.size() - 1;
    previousButton_->setEnabled(historyPosition_ > 0);
}

void MainWindow::updateQuestionView()
{
    const bool hasCurrent = currentTicketIndex_ >= 0 && currentTicketIndex_ < tickets_.size();
    questionBox_->setEnabled(hasCurrent);
    if (!hasCurrent) {
        numberLabel_->setText("Номер: -");
        nameLabel_->setText("Имя: -");
        nameEdit_->clear();
        suppressStatusComboSignal_ = true;
        statusCombo_->setCurrentIndex(0);
        suppressStatusComboSignal_ = false;
        return;
    }

    const Ticket &ticket = tickets_[currentTicketIndex_];
    numberLabel_->setText(QString("Номер: %1").arg(ticket.number));
    nameLabel_->setText(QString("Имя: %1").arg(ticket.name));
    nameEdit_->setText(ticket.name);

    suppressStatusComboSignal_ = true;
    statusCombo_->setCurrentIndex(toComboIndex(ticket.status));
    suppressStatusComboSignal_ = false;
}

void MainWindow::updateTicketItem(int index)
{
    if (index < 0 || index >= tickets_.size()) {
        return;
    }

    QListWidgetItem *item = viewList_->item(index);
    if (item == nullptr) {
        return;
    }

    item->setText(ticketDisplayText(tickets_[index]));
    item->setData(Qt::BackgroundRole, statusColor(tickets_[index].status));
    item->setData(Qt::ForegroundRole, QColor(17, 24, 39));
}

void MainWindow::updateProgressBars()
{
    int totalDone = 0;
    int greenDone = 0;

    for (const Ticket &ticket : tickets_) {
        if (ticket.status != TicketStatus::Default) {
            ++totalDone;
        }
        if (ticket.status == TicketStatus::Green) {
            ++greenDone;
        }
    }

    totalProgressBar_->setValue(totalDone);
    greenProgressBar_->setValue(greenDone);
}

void MainWindow::setTicketStatus(int index, TicketStatus status)
{
    if (index < 0 || index >= tickets_.size()) {
        return;
    }

    tickets_[index].status = status;
    updateTicketItem(index);
    updateProgressBars();

    if (index == currentTicketIndex_) {
        suppressStatusComboSignal_ = true;
        statusCombo_->setCurrentIndex(toComboIndex(status));
        suppressStatusComboSignal_ = false;
    }
}

void MainWindow::applyStatusToCurrentFromCombo(int comboIndex)
{
    if (currentTicketIndex_ < 0 || currentTicketIndex_ >= tickets_.size()) {
        return;
    }

    setTicketStatus(currentTicketIndex_, fromComboIndex(comboIndex));
}

void MainWindow::commitCurrentNameFromEditor()
{
    if (currentTicketIndex_ < 0 || currentTicketIndex_ >= tickets_.size()) {
        return;
    }

    if (!nameEdit_->hasFocus()) {
        return;
    }

    const QString newName = nameEdit_->text().trimmed();
    if (newName.isEmpty()) {
        return;
    }

    tickets_[currentTicketIndex_].name = newName;
    nameLabel_->setText(QString("Имя: %1").arg(newName));
    updateTicketItem(currentTicketIndex_);
}

void MainWindow::moveToNextRandomTicket()
{
    QVector<int> candidates;
    candidates.reserve(tickets_.size());

    for (int i = 0; i < tickets_.size(); ++i) {
        const TicketStatus status = tickets_[i].status;
        if (status == TicketStatus::Default || status == TicketStatus::Yellow) {
            candidates.push_back(i);
        }
    }

    if (candidates.isEmpty()) {
        QMessageBox::information(this, "Повторение завершено", "Нет билетов со статусом default или yellow.");
        return;
    }

    const int randomIndex = QRandomGenerator::global()->bounded(candidates.size());
    setCurrentTicket(candidates[randomIndex], true);
}

void MainWindow::moveToPreviousTicket()
{
    if (historyPosition_ <= 0 || history_.isEmpty()) {
        previousButton_->setEnabled(false);
        return;
    }

    --historyPosition_;
    previousButton_->setEnabled(historyPosition_ > 0);
    const int index = history_[historyPosition_];
    setCurrentTicket(index, false);
}

int MainWindow::toComboIndex(TicketStatus status) const
{
    switch (status) {
        case TicketStatus::Default:
            return 0;
        case TicketStatus::Yellow:
            return 1;
        case TicketStatus::Green:
            return 2;
    }

    return 0;
}

MainWindow::TicketStatus MainWindow::fromComboIndex(int comboIndex) const
{
    switch (comboIndex) {
        case 0:
            return TicketStatus::Default;
        case 1:
            return TicketStatus::Yellow;
        case 2:
            return TicketStatus::Green;
        default:
            return TicketStatus::Default;
    }
}

QString MainWindow::statusText(TicketStatus status) const
{
    switch (status) {
        case TicketStatus::Default:
            return "Default";
        case TicketStatus::Yellow:
            return "Yellow";
        case TicketStatus::Green:
            return "Green";
    }

    return "Default";
}

QColor MainWindow::statusColor(TicketStatus status) const
{
    switch (status) {
        case TicketStatus::Default:
            return QColor(212, 217, 224);
        case TicketStatus::Yellow:
            return QColor(255, 236, 142);
        case TicketStatus::Green:
            return QColor(163, 230, 172);
    }

    return QColor(212, 217, 224);
}

QString MainWindow::ticketDisplayText(const Ticket &ticket) const
{
    return QString("%1. %2").arg(ticket.number).arg(ticket.name);
}
