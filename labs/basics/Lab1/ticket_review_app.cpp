#include "ticket_review_app.h"
#include <QListWidgetItem>
#include <QHeaderView>
#include <QMessageBox>
#include <QApplication>
#include <random>
#include <algorithm>

TicketReviewApp::TicketReviewApp(QWidget *parent)
    : QMainWindow(parent)
    , currentTicketIndex(-1)
{
    setupUI();
}

TicketReviewApp::~TicketReviewApp()
{
}

void TicketReviewApp::setupUI()
{
    QWidget *centralWidget = new QWidget(this);
    QHBoxLayout *mainLayout = new QHBoxLayout(centralWidget);
    
    QWidget *leftPanel = new QWidget();
    QVBoxLayout *leftLayout = new QVBoxLayout(leftPanel);
    
    QHBoxLayout *countLayout = new QHBoxLayout();
    QLabel *countLabel = new QLabel("Total tickets:");
    countSpinBox = new QSpinBox();
    countSpinBox->setRange(0, 1000);
    countSpinBox->setValue(10);
    countLayout->addWidget(countLabel);
    countLayout->addWidget(countSpinBox);
    countLayout->addStretch();
    leftLayout->addLayout(countLayout);
    
    viewListWidget = new QListWidget();

    viewListWidget->setStyleSheet(
        "QListWidget {"
        "    color: #2c3e50;"
        "    font-size: 14px;" 
        "}"
    );

    viewListWidget->setSelectionMode(QAbstractItemView::SingleSelection);
    viewListWidget->setSpacing(2);
    leftLayout->addWidget(viewListWidget);
    
    QGroupBox *progressGroup = new QGroupBox("Progress");
    QVBoxLayout *progressLayout = new QVBoxLayout(progressGroup);
    
    QLabel *totalProgressLabel = new QLabel("Total progress:");
    totalProgressBar = new QProgressBar();
    totalProgressBar->setRange(0, 100);
    totalProgressBar->setTextVisible(true);
    
    QLabel *greenProgressLabel = new QLabel("Green tickets:");
    greenProgressBar = new QProgressBar();
    greenProgressBar->setRange(0, 100);
    greenProgressBar->setTextVisible(true);
    
    progressLayout->addWidget(totalProgressLabel);
    progressLayout->addWidget(totalProgressBar);
    progressLayout->addWidget(greenProgressLabel);
    progressLayout->addWidget(greenProgressBar);
    leftLayout->addWidget(progressGroup);
    
    questionViewGroupBox = new QGroupBox("Ticket Information");
    QVBoxLayout *questionLayout = new QVBoxLayout(questionViewGroupBox);
    
    QHBoxLayout *numberNameLayout = new QHBoxLayout();
    numberLabel = new QLabel("Ticket #");
    numberLabel->setStyleSheet("font-weight: bold; font-size: 14px;");
    nameLabel = new QLabel("Ticket name");
    nameLabel->setStyleSheet("font-size: 14px;");
    numberNameLayout->addWidget(numberLabel);
    numberNameLayout->addWidget(nameLabel);
    numberNameLayout->addStretch();
    questionLayout->addLayout(numberNameLayout);

    QHBoxLayout *editLayout = new QHBoxLayout();
    QLabel *editLabel = new QLabel("Edit name:");
    nameEdit = new QLineEdit();
    nameEdit->setPlaceholderText("Enter new name and press Enter");
    editLayout->addWidget(editLabel);
    editLayout->addWidget(nameEdit);
    questionLayout->addLayout(editLayout);
    
    QHBoxLayout *statusLayout = new QHBoxLayout();
    QLabel *statusLabel = new QLabel("Status:");
    statusComboBox = new QComboBox();
    statusComboBox->addItem("Not reviewed");
    statusComboBox->addItem("Need review");
    statusComboBox->addItem("Completed");
    statusLayout->addWidget(statusLabel);
    statusLayout->addWidget(statusComboBox);
    statusLayout->addStretch();
    questionLayout->addLayout(statusLayout);
    
    QHBoxLayout *buttonLayout = new QHBoxLayout();
    previousQuestionButton = new QPushButton("← Previous");
    nextQuestionButton = new QPushButton("Next →");
    previousQuestionButton->setEnabled(false);
    buttonLayout->addWidget(previousQuestionButton);
    buttonLayout->addWidget(nextQuestionButton);
    questionLayout->addLayout(buttonLayout);
    
    questionLayout->addStretch();
    
    mainLayout->addWidget(leftPanel, 1);
    mainLayout->addWidget(questionViewGroupBox, 1);
    
    mainLayout->setStretch(0, 2);
    mainLayout->setStretch(1, 1);
    
    setCentralWidget(centralWidget);
    
    setWindowTitle("Ticket Review App");
    resize(900, 600);
    
    connect(countSpinBox, QOverload<int>::of(&QSpinBox::valueChanged), 
            this, &TicketReviewApp::onTicketCountChanged);
    connect(viewListWidget, &QListWidget::itemClicked, 
            this, &TicketReviewApp::onViewItemClicked);
    connect(viewListWidget, &QListWidget::itemDoubleClicked, 
            this, &TicketReviewApp::onViewItemDoubleClicked);
    connect(nameEdit, &QLineEdit::returnPressed, 
            this, &TicketReviewApp::onNameEditReturnPressed);
    connect(statusComboBox, QOverload<int>::of(&QComboBox::currentIndexChanged), 
            this, &TicketReviewApp::onStatusChanged);
    connect(nextQuestionButton, &QPushButton::clicked, 
            this, &TicketReviewApp::onNextQuestionClicked);
    connect(previousQuestionButton, &QPushButton::clicked, 
            this, &TicketReviewApp::onPreviousQuestionClicked);
    
    onTicketCountChanged(10);
}

void TicketReviewApp::onTicketCountChanged(int count)
{
    tickets.clear();
    tickets.resize(count);
    
    for (int i = 0; i < count; ++i) {
        tickets[i].name = QString("Ticket %1").arg(i + 1);
        tickets[i].status = 0;
    }
    
    clearHistory();
    currentTicketIndex = -1;
    
    updateView();
    
    numberLabel->setText("Ticket #");
    nameLabel->setText("No ticket selected");
    nameEdit->clear();
    statusComboBox->setCurrentIndex(0);
    previousQuestionButton->setEnabled(false);
    
    updateProgressBars();
}

void TicketReviewApp::updateView()
{
    viewListWidget->clear();
    
    for (size_t i = 0; i < tickets.size(); ++i) {
        QString displayText = tickets[i].name;
        QListWidgetItem *item = new QListWidgetItem(displayText);
        
        QString color = getStatusColor(tickets[i].status);
        item->setBackground(QColor(color));
        
        item->setData(Qt::UserRole, static_cast<int>(i));
        
        viewListWidget->addItem(item);
    }
}

void TicketReviewApp::updateViewItemColor(int ticketIndex)
{
    if (ticketIndex >= 0 && ticketIndex < static_cast<int>(tickets.size())) {
        QListWidgetItem *item = viewListWidget->item(ticketIndex);
        if (item) {
            QString color = getStatusColor(tickets[ticketIndex].status);
            item->setBackground(QColor(color));
        }
    }
}

QString TicketReviewApp::getStatusColor(int status) const
{
    switch (status) {
        case 0: return "#E0E0E0"; // Gray
        case 1: return "#FFF59D"; // Light yellow
        case 2: return "#A5D6A5"; // Light green
        default: return "#FFFFFF";
    }
}

void TicketReviewApp::updateQuestionView(int ticketIndex)
{
    if (ticketIndex < 0 || ticketIndex >= static_cast<int>(tickets.size())) {
        numberLabel->setText("Ticket #");
        nameLabel->setText("No ticket selected");
        nameEdit->clear();
        statusComboBox->setCurrentIndex(0);
        return;
    }
    
    currentTicketIndex = ticketIndex;
    
    numberLabel->setText(QString("Ticket #%1").arg(ticketIndex + 1));
    nameLabel->setText(tickets[ticketIndex].name);
    
    nameEdit->setText(tickets[ticketIndex].name);
    
    statusComboBox->blockSignals(true);
    statusComboBox->setCurrentIndex(tickets[ticketIndex].status);
    statusComboBox->blockSignals(false);
    
    viewListWidget->setCurrentRow(ticketIndex);
}

void TicketReviewApp::onViewItemClicked(QListWidgetItem *item)
{
    if (item) {
        int ticketIndex = item->data(Qt::UserRole).toInt();
        updateQuestionView(ticketIndex);
    }
}

void TicketReviewApp::onViewItemDoubleClicked(QListWidgetItem *item)
{
    if (item) {
        int ticketIndex = item->data(Qt::UserRole).toInt();
        int currentStatus = tickets[ticketIndex].status;
        int newStatus;
        
        if (currentStatus == 2) {
            newStatus = 1;
        } else if (currentStatus == 0 || currentStatus == 1) {
            newStatus = 2;
        } else {
            return;
        }
        
        setTicketStatus(ticketIndex, newStatus);
        
        if (currentTicketIndex == ticketIndex) {
            updateQuestionView(ticketIndex);
        }
    }
}

void TicketReviewApp::setTicketStatus(int ticketIndex, int newStatus)
{
    if (ticketIndex < 0 || ticketIndex >= static_cast<int>(tickets.size())) {
        return;
    }
    
    if (tickets[ticketIndex].status != newStatus) {
        tickets[ticketIndex].status = newStatus;
        updateViewItemColor(ticketIndex);
        updateProgressBars();
    }
}

void TicketReviewApp::onNameEditReturnPressed()
{
    if (currentTicketIndex >= 0 && currentTicketIndex < static_cast<int>(tickets.size())) {
        QString newName = nameEdit->text().trimmed();
        if (!newName.isEmpty()) {
            tickets[currentTicketIndex].name = newName;
            nameLabel->setText(newName);
            
            QListWidgetItem *item = viewListWidget->item(currentTicketIndex);
            if (item) {
                item->setText(newName);
            }
        } else {
            nameEdit->setText(tickets[currentTicketIndex].name);
        }
    }
}

void TicketReviewApp::onStatusChanged(int index)
{
    if (currentTicketIndex >= 0 && currentTicketIndex < static_cast<int>(tickets.size())) {
        if (tickets[currentTicketIndex].status != index) {
            tickets[currentTicketIndex].status = index;
            updateViewItemColor(currentTicketIndex);
            updateProgressBars();

            nameLabel->setText(tickets[currentTicketIndex].name);
        }
    }
}

int TicketReviewApp::getRandomUnfinishedTicket() const
{
    std::vector<int> unfinished;
    for (size_t i = 0; i < tickets.size(); ++i) {
        if (tickets[i].status == 0 || tickets[i].status == 1) {
            unfinished.push_back(static_cast<int>(i));
        }
    }
    
    if (unfinished.empty()) {
        return -1;
    }
    
    static std::random_device rd;
    static std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, unfinished.size() - 1);
    
    return unfinished[dis(gen)];
}

void TicketReviewApp::addToHistory(int ticketIndex)
{
    if (ticketIndex >= 0) {
        historyStack.push(ticketIndex);
    }
}

void TicketReviewApp::clearHistory()
{
    while (!historyStack.empty()) {
        historyStack.pop();
    }
}

void TicketReviewApp::onNextQuestionClicked()
{
    int nextTicket = getRandomUnfinishedTicket();
    
    if (nextTicket >= 0) {
        if (currentTicketIndex >= 0) {
            addToHistory(currentTicketIndex);
        }
        
        updateQuestionView(nextTicket);
        previousQuestionButton->setEnabled(!historyStack.empty());
    } else {
        QMessageBox::information(this, "Complete", 
                                 "Congratulations! All tickets have been reviewed!");
    }
}

void TicketReviewApp::onPreviousQuestionClicked()
{
    if (!historyStack.empty()) {
        int previousTicket = historyStack.top();
        historyStack.pop();
        
        updateQuestionView(previousTicket);
        previousQuestionButton->setEnabled(!historyStack.empty());
    }
}

void TicketReviewApp::updateProgressBars()
{
    int total = static_cast<int>(tickets.size());
    if (total == 0) {
        totalProgressBar->setValue(0);
        greenProgressBar->setValue(0);
        return;
    }
    
    int totalProgress = calculateTotalProgress();
    int greenProgress = calculateGreenProgress();
    
    totalProgressBar->setValue(totalProgress);
    greenProgressBar->setValue(greenProgress);
}

int TicketReviewApp::calculateTotalProgress() const
{
    int total = static_cast<int>(tickets.size());
    if (total == 0) return 0;
    
    int weightedSum = 0;
    for (const auto& ticket : tickets) {
        switch (ticket.status) {
            case 0: weightedSum += 0; break;   // Not reviewed
            case 1: weightedSum += 50; break;  // Need review (half progress)
            case 2: weightedSum += 100; break; // Completed
        }
    }
    
    return weightedSum / total;
}

int TicketReviewApp::calculateGreenProgress() const
{
    int total = static_cast<int>(tickets.size());
    if (total == 0) return 0;
    
    int greenCount = 0;
    for (const auto& ticket : tickets) {
        if (ticket.status == 2) {
            greenCount++;
        }
    }
    
    return (greenCount * 100) / total;
}