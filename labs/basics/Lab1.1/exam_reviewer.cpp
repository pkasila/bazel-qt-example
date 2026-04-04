#include "exam_reviewer.h"

ExamReviewer::ExamReviewer(QWidget *parent)
    : QMainWindow(parent), currentTicketIndex(-1) {
    
    setupUI();
    
    // Инициализация с 10 билетами по умолчанию
    countSpinBox->setValue(10);
    onTicketCountChanged(10);
}

ExamReviewer::~ExamReviewer() {}

void ExamReviewer::setupUI() {
    centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);
    
    mainLayout = new QHBoxLayout(centralWidget);
    leftLayout = new QVBoxLayout();
    rightLayout = new QVBoxLayout();
    
    // Левая панель - список билетов
    countSpinBox = new QSpinBox();
    countSpinBox->setRange(1, 1000);
    countSpinBox->setPrefix("Количество билетов: ");
    connect(countSpinBox, QOverload<int>::of(&QSpinBox::valueChanged), 
            this, &ExamReviewer::onTicketCountChanged);
    
    ticketList = new QListWidget();
    ticketList->setSelectionMode(QAbstractItemView::SingleSelection);
    connect(ticketList, &QListWidget::itemClicked, 
            this, &ExamReviewer::onTicketClicked);
    connect(ticketList, &QListWidget::itemDoubleClicked, 
            this, &ExamReviewer::onTicketDoubleClicked);
    
    leftLayout->addWidget(countSpinBox);
    leftLayout->addWidget(ticketList);
    
    // Правая панель - информация о билете
    questionView = new QGroupBox("Информация о билете");
    questionLayout = new QVBoxLayout(questionView);
    
    numberLabel = new QLabel("Номер: -");
    nameLabel = new QLabel("Название: -");
    nameEdit = new QLineEdit();
    nameEdit->setPlaceholderText("Введите название билета и нажмите Enter");
    connect(nameEdit, &QLineEdit::returnPressed, 
            this, &ExamReviewer::onNameChanged);
    
    statusCombo = new QComboBox();
    statusCombo->addItems({"Не изучен", "Желтый", "Зеленый"});
    connect(statusCombo, QOverload<int>::of(&QComboBox::currentIndexChanged), 
            this, &ExamReviewer::onStatusChanged);
    
    questionLayout->addWidget(numberLabel);
    questionLayout->addWidget(nameLabel);
    questionLayout->addWidget(new QLabel("Изменить название:"));
    questionLayout->addWidget(nameEdit);
    questionLayout->addWidget(new QLabel("Статус:"));
    questionLayout->addWidget(statusCombo);
    
    // Кнопки навигации
    buttonLayout = new QHBoxLayout();
    nextButton = new QPushButton("Следующий билет");
    previousButton = new QPushButton("Предыдущий билет");
    
    connect(nextButton, &QPushButton::clicked, 
            this, &ExamReviewer::onNextQuestion);
    connect(previousButton, &QPushButton::clicked, 
            this, &ExamReviewer::onPreviousQuestion);
    
    buttonLayout->addWidget(previousButton);
    buttonLayout->addWidget(nextButton);
    
    // Прогресс
    progressLayout = new QHBoxLayout();
    totalProgress = new QProgressBar();
    greenProgress = new QProgressBar();
    
    totalProgress->setFormat("Общий прогресс: %p%");
    greenProgress->setFormat("Зеленые: %p%");
    
    progressLayout->addWidget(new QLabel("Общий прогресс:"));
    progressLayout->addWidget(totalProgress);
    progressLayout->addWidget(new QLabel("Зеленые:"));
    progressLayout->addWidget(greenProgress);
    
    rightLayout->addWidget(questionView);
    rightLayout->addLayout(buttonLayout);
    rightLayout->addLayout(progressLayout);
    rightLayout->addStretch();
    
    mainLayout->addLayout(leftLayout, 2);
    mainLayout->addLayout(rightLayout, 1);
    
    setWindowTitle("Повторение билетов");
    resize(800, 600);
}

void ExamReviewer::onTicketCountChanged(int count) {
    tickets.clear();
    for (int i = 1; i <= count; ++i) {
        tickets.emplace_back(i);
    }
    
    history.clear();
    currentTicketIndex = -1;
    
    updateTicketList();
    updateQuestionView();
    updateProgress();
}

void ExamReviewer::updateTicketList() {
    ticketList->clear();
    
    for (size_t i = 0; i < tickets.size(); ++i) {
        QListWidgetItem* item = new QListWidgetItem();
        item->setText(QString("%1. %2").arg(tickets[i].number).arg(tickets[i].name));
        item->setData(Qt::UserRole, static_cast<int>(i));
        
        QColor color = getStatusColor(tickets[i].status);
        item->setBackground(color);
        
        ticketList->addItem(item);
    }
}

void ExamReviewer::updateQuestionView() {
    if (currentTicketIndex >= 0 && currentTicketIndex < static_cast<int>(tickets.size())) {
        const Ticket& ticket = tickets[currentTicketIndex];
        
        numberLabel->setText(QString("Номер: %1").arg(ticket.number));
        nameLabel->setText(QString("Название: %1").arg(ticket.name));
        nameEdit->setText(ticket.name);
        
        int statusIndex = static_cast<int>(ticket.status);
        statusCombo->setCurrentIndex(statusIndex);
        
        // Выделяем текущий билет в списке
        for (int i = 0; i < ticketList->count(); ++i) {
            QListWidgetItem* item = ticketList->item(i);
            if (item->data(Qt::UserRole).toInt() == currentTicketIndex) {
                ticketList->setCurrentItem(item);
                break;
            }
        }
    } else {
        numberLabel->setText("Номер: -");
        nameLabel->setText("Название: -");
        nameEdit->clear();
        statusCombo->setCurrentIndex(0);
        ticketList->clearSelection();
    }
}

void ExamReviewer::onTicketClicked(QListWidgetItem* item) {
    int index = item->data(Qt::UserRole).toInt();
    currentTicketIndex = index;
    updateQuestionView();
}

void ExamReviewer::onTicketDoubleClicked(QListWidgetItem* item) {
    int index = item->data(Qt::UserRole).toInt();
    if (index >= 0 && index < static_cast<int>(tickets.size())) {
        Ticket& ticket = tickets[index];
        
        if (ticket.status == TicketStatus::Default || ticket.status == TicketStatus::Yellow) {
            ticket.status = TicketStatus::Green;
        } else {
            ticket.status = TicketStatus::Yellow;
        }
        
        updateTicketItem(index);
        updateProgress();
        
        if (currentTicketIndex == index) {
            updateQuestionView();
        }
    }
}

void ExamReviewer::onNameChanged() {
    if (currentTicketIndex >= 0 && currentTicketIndex < static_cast<int>(tickets.size())) {
        QString newName = nameEdit->text().trimmed();
        if (!newName.isEmpty() && nameEdit->hasFocus()) {
            tickets[currentTicketIndex].name = newName;
            updateTicketItem(currentTicketIndex);
            updateQuestionView();
        }
    }
}

void ExamReviewer::onStatusChanged() {
    if (currentTicketIndex >= 0 && currentTicketIndex < static_cast<int>(tickets.size())) {
        TicketStatus newStatus = getTicketStatus(statusCombo->currentText());
        tickets[currentTicketIndex].status = newStatus;
        updateTicketItem(currentTicketIndex);
        updateProgress();
    }
}

void ExamReviewer::onNextQuestion() {
    int nextIndex = selectRandomUncompletedTicket();
    if (nextIndex != -1) {
        if (currentTicketIndex != -1) {
            history.push_back(currentTicketIndex);
        }
        currentTicketIndex = nextIndex;
        updateQuestionView();
    }
}

void ExamReviewer::onPreviousQuestion() {
    if (!history.empty()) {
        currentTicketIndex = history.back();
        history.pop_back();
        updateQuestionView();
    }
}

void ExamReviewer::updateProgress() {
    int total = tickets.size();
    int completed = 0;
    int green = 0;
    
    for (const auto& ticket : tickets) {
        if (ticket.status == TicketStatus::Yellow || ticket.status == TicketStatus::Green) {
            completed++;
        }
        if (ticket.status == TicketStatus::Green) {
            green++;
        }
    }
    
    if (total > 0) {
        totalProgress->setValue((completed * 100) / total);
        greenProgress->setValue((green * 100) / total);
    } else {
        totalProgress->setValue(0);
        greenProgress->setValue(0);
    }
}

void ExamReviewer::updateTicketItem(int index) {
    if (index >= 0 && index < static_cast<int>(tickets.size())) {
        for (int i = 0; i < ticketList->count(); ++i) {
            QListWidgetItem* item = ticketList->item(i);
            if (item->data(Qt::UserRole).toInt() == index) {
                item->setText(QString("%1. %2").arg(tickets[index].number).arg(tickets[index].name));
                QColor color = getStatusColor(tickets[index].status);
                item->setBackground(color);
                break;
            }
        }
    }
}

TicketStatus ExamReviewer::getTicketStatus(const QString& statusText) const {
    if (statusText == "Желтый") return TicketStatus::Yellow;
    if (statusText == "Зеленый") return TicketStatus::Green;
    return TicketStatus::Default;
}

QString ExamReviewer::getStatusText(TicketStatus status) const {
    switch (status) {
        case TicketStatus::Default: return "Не изучен";
        case TicketStatus::Yellow: return "Желтый";
        case TicketStatus::Green: return "Зеленый";
    }
    return "Не изучен";
}

QColor ExamReviewer::getStatusColor(TicketStatus status) const {
    switch (status) {
        case TicketStatus::Default: return QColor(200, 200, 200); // Серый
        case TicketStatus::Yellow: return QColor(255, 255, 200); // Желтый
        case TicketStatus::Green: return QColor(200, 255, 200); // Зеленый
    }
    return QColor(200, 200, 200);
}

int ExamReviewer::selectRandomUncompletedTicket() const {
    std::vector<int> available;
    
    for (size_t i = 0; i < tickets.size(); ++i) {
        if (tickets[i].status == TicketStatus::Default || tickets[i].status == TicketStatus::Yellow) {
            available.push_back(static_cast<int>(i));
        }
    }
    
    if (available.empty()) {
        return -1;
    }
    
    std::srand(static_cast<unsigned>(std::time(nullptr)));
    int randomIndex = std::rand() % available.size();
    return available[randomIndex];
}
