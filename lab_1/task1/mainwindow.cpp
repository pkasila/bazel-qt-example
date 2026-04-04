#include "mainwindow.h"
#include <QMessageBox>
#include <QColor>
#include <QPalette>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , m_countBox(nullptr)
    , m_ticketView(nullptr)
    , m_questionGroupBox(nullptr)
    , m_numberLabel(nullptr)
    , m_nameLabel(nullptr)
    , m_nameEdit(nullptr)
    , m_statusCombo(nullptr)
    , m_nextButton(nullptr)
    , m_prevButton(nullptr)
    , m_totalProgress(nullptr)
    , m_greenProgress(nullptr)
    , m_manager(new TicketManager(this))
    , m_currentIndex(-1)
{
    setupUI();
    connect(m_manager, &TicketManager::ticketsChanged, this, &MainWindow::onManagerTicketsChanged);

    m_countBox->setValue(5);
    onTicketCountChanged();
}

MainWindow::~MainWindow()
{
}

void MainWindow::setupUI()
{
    QWidget *centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);

    QVBoxLayout *mainLayout = new QVBoxLayout(centralWidget);

    // Верхняя часть - спинбокс
    QHBoxLayout *topLayout = new QHBoxLayout();
    topLayout->addWidget(new QLabel(tr("Количество билетов:")));
    m_countBox = new QSpinBox();
    m_countBox->setRange(1, 1000);
    connect(m_countBox, QOverload<int>::of(&QSpinBox::valueChanged), this, &MainWindow::onTicketCountChanged);
    topLayout->addWidget(m_countBox);
    topLayout->addStretch();
    mainLayout->addLayout(topLayout);

    // Прогресс бары
    QHBoxLayout *progressLayout = new QHBoxLayout();
    m_totalProgress = new QProgressBar();
    m_totalProgress->setFormat(tr("Общий прогресс: %p%"));
    progressLayout->addWidget(new QLabel(tr("Общий прогресс:")));
    progressLayout->addWidget(m_totalProgress);

    m_greenProgress = new QProgressBar();
    m_greenProgress->setFormat(tr("Готовые: %p%"));
    progressLayout->addWidget(new QLabel(tr("Готовые:")));
    progressLayout->addWidget(m_greenProgress);
    mainLayout->addLayout(progressLayout);

    // Основная часть - список и детали
    QHBoxLayout *contentLayout = new QHBoxLayout();
    
    // Левая часть - список билетов
    m_ticketView = new QListWidget();
    connect(m_ticketView, &QListWidget::itemClicked, this, &MainWindow::onTicketSelected);
    connect(m_ticketView, &QListWidget::itemDoubleClicked, this, &MainWindow::onTicketDoubleClicked);
    contentLayout->addWidget(m_ticketView, 1);

    // Правая часть - детали
    m_questionGroupBox = new QGroupBox(tr("Детали билета"));
    QVBoxLayout *detailsLayout = new QVBoxLayout(m_questionGroupBox);

    // Форма для деталей
    QFormLayout *formLayout = new QFormLayout();

    m_numberLabel = new QLabel();
    formLayout->addRow(tr("Номер:"), m_numberLabel);

    m_nameLabel = new QLabel();
    formLayout->addRow(tr("Имя:"), m_nameLabel);

    m_nameEdit = new QLineEdit();
    connect(m_nameEdit, &QLineEdit::returnPressed, this, &MainWindow::onNameEdited);
    formLayout->addRow(tr("Изменить имя:"), m_nameEdit);

    m_statusCombo = new QComboBox();
    m_statusCombo->addItem(tr("Не повторен"), TicketManager::Default);
    m_statusCombo->addItem(tr("Повторяем"), TicketManager::Yellow);
    m_statusCombo->addItem(tr("Готов"), TicketManager::Green);
    connect(m_statusCombo, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &MainWindow::onStatusChanged);
    formLayout->addRow(tr("Статус:"), m_statusCombo);

    detailsLayout->addLayout(formLayout);

    // Кнопки навигации
    QHBoxLayout *navLayout = new QHBoxLayout();
    m_prevButton = new QPushButton(tr("Предыдущий"));
    connect(m_prevButton, &QPushButton::clicked, this, &MainWindow::onPreviousQuestionClicked);
    navLayout->addWidget(m_prevButton);

    m_nextButton = new QPushButton(tr("Следующий"));
    connect(m_nextButton, &QPushButton::clicked, this, &MainWindow::onNextQuestionClicked);
    navLayout->addWidget(m_nextButton);

    detailsLayout->addLayout(navLayout);
    detailsLayout->addStretch();

    contentLayout->addWidget(m_questionGroupBox, 2);
    mainLayout->addLayout(contentLayout);

    setWindowTitle(tr("Прокрастинация - Повторение билетов"));
    resize(800, 600);
}

void MainWindow::onTicketCountChanged()
{
    int count = m_countBox->value();
    m_manager->setTicketCount(count);
    m_currentIndex = -1;
    m_navigationHistory.clear();
    updateView();
}

void MainWindow::onTicketSelected(QListWidgetItem* item)
{
    int index = findItemIndex(item);
    if (index != -1) {
        m_currentIndex = index;
        updateQuestionView(index);
    }
}

void MainWindow::onTicketDoubleClicked(QListWidgetItem* item)
{
    int index = findItemIndex(item);
    if (index == -1) return;

    auto& tickets = m_manager->getTickets();
    if (tickets[index].status == TicketManager::Green) {
        m_manager->updateTicketStatus(index, TicketManager::Yellow);
    } else {
        m_manager->updateTicketStatus(index, TicketManager::Green);
    }
}

void MainWindow::onNameEdited()
{
    if (m_currentIndex != -1 && m_nameEdit->hasFocus() && !m_nameEdit->text().isEmpty()) {
        m_manager->updateTicketName(m_currentIndex, m_nameEdit->text());
    }
}

void MainWindow::onStatusChanged(int index)
{
    if (m_currentIndex != -1) {
        int statusValue = m_statusCombo->currentData().toInt();
        m_manager->updateTicketStatus(m_currentIndex, static_cast<TicketManager::Status>(statusValue));
    }
}

void MainWindow::onNextQuestionClicked()
{
    if (m_currentIndex != -1) {
        m_navigationHistory.append(m_currentIndex);
    }

    int nextIndex = m_manager->getRandomNonGreenIndex();
    if (nextIndex != -1) {
        m_currentIndex = nextIndex;
        updateQuestionView(nextIndex);
        
        // Выделить элемент в списке
        if (m_currentIndex < m_ticketView->count()) {
            m_ticketView->setCurrentRow(m_currentIndex);
        }
    } else {
        QMessageBox::information(this, tr("Информация"), tr("Все билеты уже готовы!"));
    }
}

void MainWindow::onPreviousQuestionClicked()
{
    if (!m_navigationHistory.isEmpty()) {
        int prevIndex = m_navigationHistory.takeLast();
        m_currentIndex = prevIndex;
        updateQuestionView(prevIndex);
        
        // Выделить элемент в списке
        if (m_currentIndex < m_ticketView->count()) {
            m_ticketView->setCurrentRow(m_currentIndex);
        }
    } else {
        QMessageBox::information(this, tr("Информация"), tr("Нет предыдущего билета в истории."));
    }
}

void MainWindow::onManagerTicketsChanged()
{
    updateView();
    updateProgressBars();
}

void MainWindow::updateView()
{
    m_ticketView->clear();
    const auto& tickets = m_manager->getTickets();

    for (const auto& ticket : tickets) {
        QListWidgetItem *item = new QListWidgetItem(ticket.name);
        
        switch (ticket.status) {
        case TicketManager::Default:
            item->setBackground(Qt::lightGray);
            break;
        case TicketManager::Yellow:
            item->setBackground(Qt::yellow);
            break;
        case TicketManager::Green:
            item->setBackground(Qt::green);
            break;
        }
        m_ticketView->addItem(item);
    }

    updateProgressBars();
}

void MainWindow::updateQuestionView(int index)
{
    if (index < 0 || index >= m_manager->getTicketCount()) {
        // Сброс полей
        m_numberLabel->setText("");
        m_nameLabel->setText("");
        m_nameEdit->setText("");
        m_statusCombo->setCurrentIndex(0);
        return;
    }

    const auto& ticket = m_manager->getTickets()[index];

    m_numberLabel->setText(QString::number(ticket.number));
    m_nameLabel->setText(ticket.name);
    m_nameEdit->setText(ticket.name);

    int comboIndex = 0;
    if (ticket.status == TicketManager::Yellow) comboIndex = 1;
    else if (ticket.status == TicketManager::Green) comboIndex = 2;
    m_statusCombo->setCurrentIndex(comboIndex);
}

void MainWindow::updateProgressBars()
{
    int totalTickets = m_manager->getTicketCount();
    if (totalTickets == 0) {
        m_totalProgress->setValue(0);
        m_greenProgress->setValue(0);
        return;
    }

    int totalProgress = m_manager->getTotalProgress();
    int greenProgress = m_manager->getGreenProgress();

    m_totalProgress->setMaximum(totalTickets);
    m_totalProgress->setValue(totalProgress);

    m_greenProgress->setMaximum(totalTickets);
    m_greenProgress->setValue(greenProgress);
}

int MainWindow::findItemIndex(QListWidgetItem* item) const
{
    for (int i = 0; i < m_ticketView->count(); ++i) {
        if (m_ticketView->item(i) == item) {
            return i;
        }
    }
    return -1;
}