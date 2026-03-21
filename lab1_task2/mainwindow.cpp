#include "mainwindow.h"

#include <QWidget>
#include <QLineEdit>
#include <QPushButton>
#include <QCheckBox>
#include <QComboBox>
#include <QListWidget>
#include <QListWidgetItem>
#include <QLabel>
#include <QProgressBar>
#include <QDateEdit>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGroupBox>
#include <QKeyEvent>
#include <QFont>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), currentDate(QDate::currentDate())
{
    setupUi();
    connectUi();

    setCurrentDate(QDate::currentDate());
    clearHabitDetails();
    refreshHabitList();
}

void MainWindow::setupUi()
{
    resize(920, 580);
    setWindowTitle("Habit Tracker Calendar");

    QWidget *central = new QWidget(this);
    setCentralWidget(central);

    auto *mainLayout = new QVBoxLayout(central);
    mainLayout->setContentsMargins(12, 12, 12, 12);
    mainLayout->setSpacing(10);

    auto *dateRow = new QHBoxLayout;
    dateRow->setSpacing(8);

    prevDayButton = new QPushButton("<", central);
    nextDayButton = new QPushButton(">", central);
    todayButton = new QPushButton("Today", central);

    dateLabel = new QLabel(central);
    QFont dateFont = dateLabel->font();
    dateFont.setPointSize(dateFont.pointSize() + 2);
    dateFont.setBold(true);
    dateLabel->setFont(dateFont);

    dateEdit = new QDateEdit(currentDate, central);
    dateEdit->setCalendarPopup(true);
    dateEdit->setDisplayFormat("dd.MM.yyyy");

    dateRow->addWidget(prevDayButton);
    dateRow->addWidget(nextDayButton);
    dateRow->addWidget(todayButton);
    dateRow->addSpacing(10);
    dateRow->addWidget(dateLabel);
    dateRow->addStretch();
    dateRow->addWidget(new QLabel("Jump to date:", central));
    dateRow->addWidget(dateEdit);

    mainLayout->addLayout(dateRow);

    auto *topRow = new QHBoxLayout;
    topRow->setSpacing(8);

    nameEdit = new QLineEdit(central);
    nameEdit->setPlaceholderText("Enter habit name");

    categoryBox = new QComboBox(central);
    categoryBox->addItems({"Sport", "Study", "Sleep", "Nutrition", "Other"});

    addButton = new QPushButton("Add habit", central);

    filterBox = new QComboBox(central);
    filterBox->addItems({"All", "Sport", "Study", "Sleep", "Nutrition", "Other"});

    topRow->addWidget(nameEdit, 3);
    topRow->addWidget(categoryBox, 1);
    topRow->addWidget(addButton, 1);
    topRow->addSpacing(8);
    topRow->addWidget(new QLabel("Filter:", central));
    topRow->addWidget(filterBox, 1);

    mainLayout->addLayout(topRow);

    auto *contentRow = new QHBoxLayout;
    contentRow->setSpacing(12);

    habitList = new QListWidget(central);
    habitList->setMinimumWidth(320);
    contentRow->addWidget(habitList, 1);

    auto *detailsBox = new QGroupBox("Habit details", central);
    auto *detailsLayout = new QVBoxLayout(detailsBox);

    nameLabel = new QLabel("Name: -", detailsBox);
    categoryLabel = new QLabel("Category: -", detailsBox);
    startDateLabel = new QLabel("Start date: -", detailsBox);
    statusHintLabel = new QLabel("Select a habit to see details", detailsBox);

    doneCheckBox = new QCheckBox("Done on selected day", detailsBox);
    toggleDoneButton = new QPushButton("Toggle status", detailsBox);
    archiveButton = new QPushButton("Archive habit", detailsBox);

    detailsLayout->addWidget(nameLabel);
    detailsLayout->addWidget(categoryLabel);
    detailsLayout->addWidget(startDateLabel);
    detailsLayout->addWidget(statusHintLabel);
    detailsLayout->addSpacing(8);
    detailsLayout->addWidget(doneCheckBox);
    detailsLayout->addWidget(toggleDoneButton);
    detailsLayout->addWidget(archiveButton);
    detailsLayout->addStretch();

    contentRow->addWidget(detailsBox, 1);

    mainLayout->addLayout(contentRow);

    progressLabel = new QLabel("Daily progress", central);
    progressBar = new QProgressBar(central);
    progressBar->setRange(0, 100);
    progressBar->setValue(0);

    mainLayout->addWidget(progressLabel);
    mainLayout->addWidget(progressBar);

    doneCheckBox->setEnabled(false);
    toggleDoneButton->setEnabled(false);
    archiveButton->setEnabled(false);

    updateDateLabel();
}

void MainWindow::connectUi()
{
    connect(addButton, &QPushButton::clicked, this, [this]() {
        addHabit();
    });

    connect(nameEdit, &QLineEdit::returnPressed, this, [this]() {
        addHabit();
    });

    connect(prevDayButton, &QPushButton::clicked, this, [this]() {
        setCurrentDate(currentDate.addDays(-1));
    });

    connect(nextDayButton, &QPushButton::clicked, this, [this]() {
        setCurrentDate(currentDate.addDays(1));
    });

    connect(todayButton, &QPushButton::clicked, this, [this]() {
        setCurrentDate(QDate::currentDate());
    });

    connect(dateEdit, &QDateEdit::dateChanged, this, [this](const QDate &date) {
        if (date != currentDate) {
            setCurrentDate(date);
        }
    });

    connect(filterBox, &QComboBox::currentTextChanged, this, [this](const QString &) {
        refreshHabitList();
    });

    connect(habitList, &QListWidget::itemClicked, this, [this](QListWidgetItem *item) {
        if (!item) return;
        int habitId = item->data(Qt::UserRole).toInt();
        showHabit(habitId);
    });

    connect(toggleDoneButton, &QPushButton::clicked, this, [this]() {
        if (currentHabitId < 0) return;
        bool newValue = !isHabitDoneOnCurrentDate(currentHabitId);
        setHabitDoneOnCurrentDate(currentHabitId, newValue);
        doneCheckBox->setChecked(newValue);
        refreshHabitList();
        showHabit(currentHabitId);
    });

    connect(doneCheckBox, &QCheckBox::toggled, this, [this](bool checked) {
        if (currentHabitId < 0) return;
        setHabitDoneOnCurrentDate(currentHabitId, checked);
        refreshHabitList();
        showHabit(currentHabitId);
    });

    connect(archiveButton, &QPushButton::clicked, this, [this]() {
        archiveCurrentHabit();
    });
}

void MainWindow::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_Left) {
        setCurrentDate(currentDate.addDays(-1));
        return;
    }

    if (event->key() == Qt::Key_Right) {
        setCurrentDate(currentDate.addDays(1));
        return;
    }

    if (event->key() == Qt::Key_T) {
        setCurrentDate(QDate::currentDate());
        return;
    }

    if (event->key() == Qt::Key_Delete) {
        archiveCurrentHabit();
        return;
    }

    QMainWindow::keyPressEvent(event);
}

void MainWindow::addHabit()
{
    QString name = nameEdit->text().trimmed();
    if (name.isEmpty()) {
        return;
    }

    Habit h;
    h.id = nextHabitId++;
    h.name = name;
    h.category = categoryBox->currentText();
    h.startDate = currentDate;
    h.active = true;

    habits.push_back(h);

    history[currentDate][h.id] = false;

    nameEdit->clear();

    refreshHabitList();

    for (int i = 0; i < habitList->count(); ++i) {
        QListWidgetItem *item = habitList->item(i);
        if (item && item->data(Qt::UserRole).toInt() == h.id) {
            habitList->setCurrentItem(item);
            showHabit(h.id);
            break;
        }
    }
}

void MainWindow::archiveCurrentHabit()
{
    if (currentHabitId < 0) return;

    Habit *habit = findHabitById(currentHabitId);
    if (!habit) return;

    habit->active = false;
    currentHabitId = -1;

    refreshHabitList();
    clearHabitDetails();
}

void MainWindow::setCurrentDate(const QDate &date)
{
    currentDate = date;
    dateEdit->blockSignals(true);
    dateEdit->setDate(currentDate);
    dateEdit->blockSignals(false);

    updateDateLabel();
    currentHabitId = -1;
    refreshHabitList();
    clearHabitDetails();
}

void MainWindow::updateDateLabel()
{
    QString mode = (currentDate == QDate::currentDate()) ? "Today" : "History";
    dateLabel->setText(QString("%1 — %2").arg(currentDate.toString("dd.MM.yyyy"), mode));
}

void MainWindow::refreshHabitList()
{
    habitList->clear();

    QVector<Habit*> visible = visibleHabitsForCurrentDate();

    for (Habit *habit : visible) {
        auto *item = new QListWidgetItem(habit->name);
        item->setData(Qt::UserRole, habit->id);
        habitList->addItem(item);
        updateHabitItem(item, habit->id);
    }

    updateProgress();
}

void MainWindow::updateHabitItem(QListWidgetItem *item, int habitId)
{
    if (!item) return;

    const Habit *habit = findHabitById(habitId);
    if (!habit) return;

    bool done = isHabitDoneOnCurrentDate(habitId);

    item->setText(QString("%1 [%2]").arg(habit->name, habit->category));

    if (done) {
        item->setBackground(QColor("#7bd389"));
        item->setForeground(QColor("#0f3d1e"));
    } else {
        item->setBackground(QColor("#e2e3e5"));
        item->setForeground(QColor("#383d41"));
    }
}

void MainWindow::showHabit(int habitId)
{
    const Habit *habit = findHabitById(habitId);
    if (!habit) return;

    currentHabitId = habitId;

    nameLabel->setText(QString("Name: %1").arg(habit->name));
    categoryLabel->setText(QString("Category: %1").arg(habit->category));
    startDateLabel->setText(QString("Start date: %1").arg(habit->startDate.toString("dd.MM.yyyy")));

    bool done = isHabitDoneOnCurrentDate(habitId);
    statusHintLabel->setText(done ? "Status: completed on selected day" : "Status: not completed on selected day");

    doneCheckBox->blockSignals(true);
    doneCheckBox->setChecked(done);
    doneCheckBox->blockSignals(false);

    doneCheckBox->setEnabled(true);
    toggleDoneButton->setEnabled(true);
    archiveButton->setEnabled(true);

    updateProgress();
}

void MainWindow::clearHabitDetails()
{
    nameLabel->setText("Name: -");
    categoryLabel->setText("Category: -");
    startDateLabel->setText("Start date: -");
    statusHintLabel->setText("Select a habit to see details");

    doneCheckBox->blockSignals(true);
    doneCheckBox->setChecked(false);
    doneCheckBox->blockSignals(false);

    doneCheckBox->setEnabled(false);
    toggleDoneButton->setEnabled(false);
    archiveButton->setEnabled(false);
}

void MainWindow::updateProgress()
{
    int total = 0;
    int done = 0;

    QVector<Habit*> visible = visibleHabitsForCurrentDate();
    for (Habit *habit : visible) {
        ++total;
        if (isHabitDoneOnCurrentDate(habit->id)) {
            ++done;
        }
    }

    int percent = (total == 0) ? 0 : (done * 100 / total);
    progressBar->setValue(percent);
    progressLabel->setText(
        QString("Progress for %1 (%2/%3)")
            .arg(currentDate.toString("dd.MM.yyyy"))
            .arg(done)
            .arg(total)
    );
}

QVector<Habit*> MainWindow::visibleHabitsForCurrentDate()
{
    QVector<Habit*> result;

    for (Habit &habit : habits) {
        if (!isHabitVisibleOnDate(habit, currentDate)) continue;
        if (!passesCategoryFilter(habit)) continue;
        result.push_back(&habit);
    }

    return result;
}

const Habit* MainWindow::findHabitById(int id) const
{
    for (const Habit &habit : habits) {
        if (habit.id == id) return &habit;
    }
    return nullptr;
}

Habit* MainWindow::findHabitById(int id)
{
    for (Habit &habit : habits) {
        if (habit.id == id) return &habit;
    }
    return nullptr;
}

bool MainWindow::isHabitVisibleOnDate(const Habit &habit, const QDate &date) const
{
    return habit.active && habit.startDate <= date;
}

bool MainWindow::passesCategoryFilter(const Habit &habit) const
{
    QString selected = filterBox->currentText();
    return selected == "All" || habit.category == selected;
}

bool MainWindow::isHabitDoneOnCurrentDate(int habitId) const
{
    return history.value(currentDate).value(habitId, false);
}

void MainWindow::setHabitDoneOnCurrentDate(int habitId, bool done)
{
    history[currentDate][habitId] = done;
}