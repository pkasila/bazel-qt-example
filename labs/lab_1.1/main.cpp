#include "Header.h"
#include <QtWidgets/QApplication>

ExamApp::ExamApp(QWidget *parent) : QMainWindow(parent), currentTicketIndex(-1), historyIndex(-1) {
    setupUI();

    ticketCount->setValue(10);
    onTicketCountChanged(10);
}

ExamApp::~ExamApp() {}

void ExamApp::setupUI() {
    centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);
    
    mainLayout = new QVBoxLayout(centralWidget);

    QHBoxLayout *topLayout = new QHBoxLayout();
    topLayout->addWidget(new QLabel("Количество билетов:"));
    ticketCount = new QSpinBox();
    ticketCount->setMinimum(1);
    ticketCount->setMaximum(1000);
    connect(ticketCount, QOverload<int>::of(&QSpinBox::valueChanged), [this](int value) { onTicketCountChanged(value); });
    topLayout->addWidget(ticketCount);
    topLayout->addStretch();
    mainLayout->addLayout(topLayout);

    contentLayout = new QHBoxLayout();

    QVBoxLayout *leftLayout = new QVBoxLayout();
    ticketView = new QTableWidget();
    ticketView->setColumnCount(1);
    ticketView->horizontalHeader()->hide();
    ticketView->verticalHeader()->hide();
    ticketView->horizontalHeader()->setStretchLastSection(true);
    connect(ticketView, &QTableWidget::cellClicked, [this](int row, int column) { onTicketClicked(row, column); });
    connect(ticketView, &QTableWidget::cellDoubleClicked, [this](int row, int column) { onTicketDoubleClicked(row, column); });
    leftLayout->addWidget(ticketView);
    contentLayout->addLayout(leftLayout);

    questionGroup = new QGroupBox("Информация о билете");
    questionLayout = new QVBoxLayout(questionGroup);
    
    ticketNumber = new QLabel("Номер: -");
    questionLayout->addWidget(ticketNumber);
    
    ticketName = new QLabel("Название: -");
    questionLayout->addWidget(ticketName);
    
    nameEdit = new QLineEdit();
    nameEdit->setPlaceholderText("Введите новое название и нажмите Enter");
    connect(nameEdit, &QLineEdit::returnPressed, [this]() { onNameChanged(); });
    questionLayout->addWidget(nameEdit);
    
    questionLayout->addWidget(new QLabel("Статус:"));
    statusCombo = new QComboBox();
    statusCombo->addItem("Не изучен", static_cast<int>(TicketStatus::Default));
    statusCombo->addItem("В процессе", static_cast<int>(TicketStatus::Yellow));
    statusCombo->addItem("Изучен", static_cast<int>(TicketStatus::Green));
    connect(statusCombo, QOverload<int>::of(&QComboBox::currentIndexChanged), [this](int index) { onStatusChanged(); });
    questionLayout->addWidget(statusCombo);
    
    buttonLayout = new QHBoxLayout();
    nextQuestion = new QPushButton("Следующий вопрос");
    connect(nextQuestion, &QPushButton::clicked, [this]() { onNextQuestion(); });
    buttonLayout->addWidget(nextQuestion);
    
    previousQuestion = new QPushButton("Предыдущий вопрос");
    connect(previousQuestion, &QPushButton::clicked, [this]() { onPreviousQuestion(); });
    buttonLayout->addWidget(previousQuestion);
    
    questionLayout->addLayout(buttonLayout);
    questionLayout->addStretch();
    
    contentLayout->addWidget(questionGroup);
    mainLayout->addLayout(contentLayout);

    QVBoxLayout *progressLayout = new QVBoxLayout();
    
    progressLayout->addWidget(new QLabel("Общий прогресс:"));
    totalProgress = new QProgressBar();
    progressLayout->addWidget(totalProgress);
    
    progressLayout->addWidget(new QLabel("Прогресс изученных:"));
    greenProgress = new QProgressBar();
    progressLayout->addWidget(greenProgress);
    
    mainLayout->addLayout(progressLayout);
    
    setWindowTitle("Подготовка к экзамену");
    resize(800, 600);
}

void ExamApp::onTicketCountChanged(int count) {
    tickets.clear();
    for (int i = 1; i <= count; ++i) {
        tickets.append(Ticket(i));
    }
    
    currentTicketIndex = -1;
    history.clear();
    historyIndex = -1;
    
    updateTicketView();
    updateQuestionView();
    updateProgressBars();
}

void ExamApp::updateTicketView() {
    ticketView->setRowCount(tickets.size());
    
    for (int i = 0; i < tickets.size(); ++i) {
        QTableWidgetItem *item = new QTableWidgetItem(tickets[i].name);
        item->setBackground(getTicketColor(tickets[i].status));
        ticketView->setItem(i, 0, item);
    }
}

void ExamApp::onTicketClicked(int row, int column) {
    Q_UNUSED(column);
    selectTicket(row);
}

void ExamApp::onTicketDoubleClicked(int row, int column) {
    Q_UNUSED(column);
    
    if (row >= 0 && row < tickets.size()) {
        switch (tickets[row].status) {
            case TicketStatus::Default:
            case TicketStatus::Yellow:
                tickets[row].status = TicketStatus::Green;
                break;
            case TicketStatus::Green:
                tickets[row].status = TicketStatus::Yellow;
                break;
        }
        updateTicketView();
        updateProgressBars();
        
        if (currentTicketIndex == row) {
            updateQuestionView();
        }
    }
}

void ExamApp::selectTicket(int index) {
    if (index >= 0 && index < tickets.size()) {
        currentTicketIndex = index;

        if (historyIndex < history.size() - 1) {
            history = history.mid(0, historyIndex + 1);
        }
        history.append(index);
        historyIndex = history.size() - 1;
        
        ticketView->selectRow(index);
        updateQuestionView();
    }
}

void ExamApp::updateQuestionView() {
    if (currentTicketIndex >= 0 && currentTicketIndex < tickets.size()) {
        const Ticket &ticket = tickets[currentTicketIndex];
        ticketNumber->setText(QString("Номер: %1").arg(ticket.number));
        ticketName->setText(QString("Название: %1").arg(ticket.name));
        nameEdit->setText(ticket.name);
        statusCombo->setCurrentIndex(static_cast<int>(ticket.status));
    } else {
        ticketNumber->setText("Номер: -");
        ticketName->setText("Название: -");
        nameEdit->setText("");
        statusCombo->setCurrentIndex(0);
    }
}

void ExamApp::onNameChanged() {
    if (currentTicketIndex >= 0 && currentTicketIndex < tickets.size()) {
        QString newName = nameEdit->text().trimmed();
        if (!newName.isEmpty()) {
            tickets[currentTicketIndex].name = newName;
            updateTicketView();
            updateQuestionView();
        }
    }
}

void ExamApp::onStatusChanged() {
    if (currentTicketIndex >= 0 && currentTicketIndex < tickets.size()) {
        tickets[currentTicketIndex].status = static_cast<TicketStatus>(statusCombo->currentData().toInt());
        updateTicketView();
        updateProgressBars();
    }
}

void ExamApp::onNextQuestion() {
    QVector<int> available = getAvailableTickets();
    if (available.isEmpty()) {
        return;
    }
    
    int randomIndex = QRandomGenerator::global()->bounded(available.size());
    selectTicket(available[randomIndex]);
}

void ExamApp::onPreviousQuestion() {
    if (historyIndex > 0) {
        historyIndex--;
        selectTicket(history[historyIndex]);
    }
}

QVector<int> ExamApp::getAvailableTickets() {
    QVector<int> available;
    for (int i = 0; i < tickets.size(); ++i) {
        if (tickets[i].status == TicketStatus::Default || tickets[i].status == TicketStatus::Yellow) {
            available.append(i);
        }
    }
    return available;
}

void ExamApp::updateProgressBars() {
    if (tickets.isEmpty()) {
        totalProgress->setValue(0);
        greenProgress->setValue(0);
        return;
    }
    
    int totalCount = tickets.size();
    int nonDefaultCount = 0;
    int greenCount = 0;
    
    for (const Ticket &ticket : tickets) {
        if (ticket.status != TicketStatus::Default) {
            nonDefaultCount++;
        }
        if (ticket.status == TicketStatus::Green) {
            greenCount++;
        }
    }
    
    totalProgress->setValue((nonDefaultCount * 100) / totalCount);
    greenProgress->setValue((greenCount * 100) / totalCount);
}

QColor ExamApp::getTicketColor(TicketStatus status) {
    switch (status) {
        case TicketStatus::Default:
            return QColor(240, 240, 240);
        case TicketStatus::Yellow:
            return QColor(255, 255, 200);
        case TicketStatus::Green:
            return QColor(200, 255, 200);
    }
    return QColor(240, 240, 240);
}

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    
    ExamApp window;
    window.show();
    
    return app.exec();
}
