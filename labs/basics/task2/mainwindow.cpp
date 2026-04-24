#include "mainwindow.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGroupBox>
#include <QDate>
#include <QMessageBox> 

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {
    setupUI();
    setWindowTitle("Stress Tracker");
    resize(900, 500);
}

MainWindow::~MainWindow() {}

void MainWindow::setupUI() {
    auto *centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);
    auto *mainLayout = new QHBoxLayout(centralWidget);

    auto *leftLayout = new QVBoxLayout();
    
    m_taskEdit = new QLineEdit();
    m_taskEdit->setPlaceholderText("Subject...");
    
    m_difficultySlider = new QSlider(Qt::Horizontal);
    m_difficultySlider->setRange(1, 10);
    m_difficultySlider->setValue(5);
    
    m_urgentBox = new QCheckBox("Urgent (x2)");
    
    m_taskList = new QListWidget();
    m_taskList->setMaximumHeight(150); 
    m_taskList->setStyleSheet("QListWidget { background-color: #374448; border: 1px solid #ddd; }");

    m_addBtn = new QPushButton("Add task");
    m_addBtn->setStyleSheet("background-color: #087c77; font-weight: bold; height: 40px;");

    leftLayout->addWidget(new QLabel("Task:"));
    leftLayout->addWidget(m_taskEdit);
    leftLayout->addWidget(new QLabel("Difficulty (1-10):"));
    leftLayout->addWidget(m_difficultySlider);
    leftLayout->addWidget(m_urgentBox);
    leftLayout->addStretch();
    leftLayout->addWidget(m_addBtn);

    leftLayout->addWidget(new QLabel("Задачи на выбранную неделю:"));
    leftLayout->addWidget(m_taskList);
    
    leftLayout->addStretch();
    leftLayout->addWidget(m_addBtn);

    auto *rightLayout = new QVBoxLayout();
    
    m_calendar = new QCalendarWidget();

    m_calendar->setStyleSheet(
        "QCalendarWidget QToolButton { color: #c6e6f0; }" 
        "QCalendarWidget QWidget#qt_calendar_navigationbar { background-color: #333; }"
    );
    
    m_lcdDisplay = new QLCDNumber();
    m_lcdDisplay->setSegmentStyle(QLCDNumber::Flat);
    m_lcdDisplay->setStyleSheet("color: #70b3da; background-color: black;");
    m_lcdDisplay->setMinimumHeight(100);
    
    m_statusLabel = new QLabel("Choose date and subject");
    m_statusLabel->setWordWrap(true);

    rightLayout->addWidget(new QLabel("Deadline calendar"));
    rightLayout->addWidget(m_calendar);
    rightLayout->addWidget(new QLabel("Stress points on current week: "));
    rightLayout->addWidget(m_lcdDisplay);
    rightLayout->addWidget(m_statusLabel);

    mainLayout->addLayout(leftLayout, 1);
    mainLayout->addLayout(rightLayout, 2);

    connect(m_calendar, &QCalendarWidget::selectionChanged, this, [this](){
        QDate selected = m_calendar->selectedDate();
        checkDateUrgency(selected); 
        int week = selected.weekNumber();
        int stressForWeek = m_weeklyStress.value(week, 0);
        
        m_lcdDisplay->display(stressForWeek); 
        refreshTaskList(week);
        m_statusLabel->setText("Current week: " + QString::number(week));
    });

    connect(m_addBtn, &QPushButton::clicked, this, [this](){
        addTaskToPlan();
    });

    connect(m_difficultySlider, &QSlider::valueChanged, this, [this](int val){
        m_lcdDisplay->display(val);
    });
}

void MainWindow::checkDateUrgency(const QDate &date) {
    qint64 daysTo = QDate::currentDate().daysTo(date);
    if (daysTo >= 0 && daysTo <= 3) {
        m_urgentBox->setChecked(true);
    } else {
        m_urgentBox->setChecked(false);
    }
}

void MainWindow::refreshTaskList(int week) {
    m_taskList->clear();
    m_taskList->addItems(m_weeklyTaskNames.value(week));
}

void MainWindow::addTaskToPlan() {
    if (m_taskEdit->text().isEmpty()) return;

    int taskScore = m_difficultySlider->value();
    if (m_urgentBox->isChecked()) taskScore *= 2;

    int week = m_calendar->selectedDate().weekNumber();
    int currentTotal = m_weeklyStress.value(week, 0);

    if (currentTotal + taskScore > 20) {
        QMessageBox::warning(this, "SOOOOS!", 
            "You can't add more subjects. Stress level on this week is too high!!");
        return;
    }
    m_weeklyStress[week] += taskScore;
    m_weeklyTaskNames[week].append(m_taskEdit->text() + " (" + QString::number(taskScore) + " pts)");

    m_lcdDisplay->display(m_weeklyStress[week]);
    refreshTaskList(week);

    if (m_weeklyStress[week] > 15) {
        m_lcdDisplay->setStyleSheet("color: orange; background-color: black;");
    }

    m_statusLabel->setText("Added: " + m_taskEdit->text());
    m_taskEdit->clear();
}