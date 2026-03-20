#include "calendar.h"

#include <QHBoxLayout>
#include <QMessageBox>
#include <QRandomGenerator>
#include <QVBoxLayout>

void CustomCalendar::paintCell(QPainter* painter, const QRect& rect, QDate date) const {
    painter->save();
    if (highlightedDates.contains(date)) {
        painter->fillRect(rect, QColor(230, 255, 230));
    }

    QFont font = painter->font();
    font.setBold(true);
    painter->setFont(font);

    QRect dateRect = rect.adjusted(0, 2, 0, -rect.height() / 3);
    painter->drawText(dateRect, Qt::AlignCenter, QString::number(date.day()));

    if (dataRef && dataRef->contains(date) && !(*dataRef)[date].isEmpty()) {
        const QList<Task>& tasks = (*dataRef)[date];
        int total = tasks.size();
        int uncompleted = 0;
        for (const auto& t : tasks) {
            if (!t.isDone) {
                uncompleted++;
            }
        }

        font.setBold(false);
        font.setPointSize(7);
        painter->setFont(font);
        painter->setPen(uncompleted > 0 ? QColor(200, 40, 40) : QColor(120, 120, 120));

        QRect infoRect = rect.adjusted(0, rect.height() / 3, 0, -2);
        painter->drawText(
            infoRect, Qt::AlignCenter, QString("Задачи: %1 (%2)").arg(uncompleted).arg(total));
    }
    painter->restore();
}

MainWindow::MainWindow(QWidget* parent) : QMainWindow(parent) {
    setupUi();
    applyStyles();
    loadData();
    onDateSelected();
}

MainWindow::~MainWindow() {
    saveData();
}

void MainWindow::setupUi() {
    QWidget* centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);
    QHBoxLayout* mainLayout = new QHBoxLayout(centralWidget);

    QVBoxLayout* leftLayout = new QVBoxLayout();
    calendar = new CustomCalendar(this);
    calendar->dataRef = &dataStore;
    calendar->setGridVisible(true);
    leftLayout->addWidget(calendar);

    bulkTaskInput = new QLineEdit(this);
    bulkTaskInput->setPlaceholderText("Задачи через запятую...");
    leftLayout->addWidget(bulkTaskInput);

    QHBoxLayout* dLayout = new QHBoxLayout();
    startDateEdit = new QDateEdit(QDate::currentDate(), this);
    endDateEdit = new QDateEdit(QDate::currentDate().addDays(7), this);
    startDateEdit->setCalendarPopup(true);
    endDateEdit->setCalendarPopup(true);
    dLayout->addWidget(new QLabel("С:"));
    dLayout->addWidget(startDateEdit);
    dLayout->addWidget(new QLabel("По:"));
    dLayout->addWidget(endDateEdit);
    leftLayout->addLayout(dLayout);

    QHBoxLayout* bLayout = new QHBoxLayout();
    btnDistribute = new QPushButton("Раскидать", this);
    btnUndo = new QPushButton("↩️ Отмена", this);
    btnUndo->setEnabled(false);
    bLayout->addWidget(btnDistribute);
    bLayout->addWidget(btnUndo);
    leftLayout->addLayout(bLayout);

    btnReset = new QPushButton("Сбросить всё", this);
    btnReset->setObjectName("resetBtn");
    leftLayout->addWidget(btnReset);

    QVBoxLayout* rightLayout = new QVBoxLayout();
    lblCurrentDate = new QLabel(this);
    rightLayout->addWidget(lblCurrentDate);

    taskList = new QListWidget(this);
    rightLayout->addWidget(taskList);

    btnDeleteTask = new QPushButton("🗑️ Удалить выбранное", this);
    btnDeleteTask->setObjectName("deleteBtn");
    rightLayout->addWidget(btnDeleteTask);

    QHBoxLayout* addLayout = new QHBoxLayout();
    singleTaskInput = new QLineEdit(this);
    btnAddTask = new QPushButton("Добавить", this);
    addLayout->addWidget(singleTaskInput);
    addLayout->addWidget(btnAddTask);
    rightLayout->addLayout(addLayout);

    mainLayout->addLayout(leftLayout, 6);
    mainLayout->addLayout(rightLayout, 4);

    connect(calendar, &QCalendarWidget::selectionChanged, this, &MainWindow::onDateSelected);
    connect(btnAddTask, &QPushButton::clicked, this, &MainWindow::onAddTask);
    connect(singleTaskInput, &QLineEdit::returnPressed, this, &MainWindow::onAddTask);
    connect(btnDeleteTask, &QPushButton::clicked, this, &MainWindow::onDeleteTask);
    connect(taskList, &QListWidget::itemChanged, this, &MainWindow::onTaskStateChanged);
    connect(btnDistribute, &QPushButton::clicked, this, &MainWindow::onDistributeTasks);
    connect(btnUndo, &QPushButton::clicked, this, &MainWindow::onUndoDistribution);
    connect(btnReset, &QPushButton::clicked, this, &MainWindow::onResetAll);
}

void MainWindow::applyStyles() {
    this->setMinimumSize(950, 550);
    this->setStyleSheet(
        "QMainWindow { background-color: #f8f9fa; }"
        "QPushButton { background: white; border: 1px solid #ccc; padding: 5px; border-radius: "
        "4px; font-weight: bold; }"
        "QPushButton#deleteBtn { color: #dc3545; }"
        "QPushButton#resetBtn { color: white; background-color: #bb2d3b; border: none; margin-top: "
        "10px; }"
        "QPushButton#resetBtn:hover { background-color: #a52834; }");
}

void MainWindow::saveData() {
    QFile file("tasks.dat");
    if (file.open(QIODevice::WriteOnly)) {
        QDataStream out(&file);
        out << dataStore;
        file.close();
    }
}

void MainWindow::loadData() {
    QFile file("tasks.dat");
    if (file.open(QIODevice::ReadOnly)) {
        QDataStream in(&file);
        in >> dataStore;
        file.close();
    }
}

void MainWindow::refreshTaskList() {
    taskList->blockSignals(true);
    taskList->clear();
    QDate date = calendar->selectedDate();

    if (dataStore.contains(date)) {
        for (const Task& t : dataStore[date]) {
            QListWidgetItem* item = new QListWidgetItem(t.name, taskList);
            item->setFlags(item->flags() | Qt::ItemIsUserCheckable);
            item->setCheckState(t.isDone ? Qt::Checked : Qt::Unchecked);
            if (t.isDone) {
                item->setForeground(QColor(160, 160, 160));
                QFont f = item->font();
                f.setStrikeOut(true);
                item->setFont(f);
            }
        }
    }
    taskList->blockSignals(false);
    saveData();

    calendar->forceUpdate();
}

void MainWindow::onResetAll() {
    auto res = QMessageBox::question(this, "Подтверждение", "Удалить все задачи и историю?");
    if (res == QMessageBox::Yes) {
        dataStore.clear();
        calendar->highlightedDates.clear();
        lastDistribution.clear();
        QFile::remove("tasks.dat");
        refreshTaskList();
    }
}

void MainWindow::onDateSelected() {
    lblCurrentDate->setText(
        "<b>Задачи на " + calendar->selectedDate().toString("dd.MM.yyyy") + "</b>");
    refreshTaskList();
}

void MainWindow::onAddTask() {
    QString name = singleTaskInput->text().trimmed();
    if (name.isEmpty()) {
        return;
    }
    dataStore[calendar->selectedDate()].append({name, false});
    singleTaskInput->clear();
    refreshTaskList();
}

void MainWindow::onDeleteTask() {
    QListWidgetItem* item = taskList->currentItem();
    if (!item) {
        return;
    }
    dataStore[calendar->selectedDate()].removeAt(taskList->row(item));
    refreshTaskList();
}

void MainWindow::onTaskStateChanged(QListWidgetItem*) {
    QDate date = calendar->selectedDate();
    for (int i = 0; i < taskList->count(); ++i) {
        dataStore[date][i].isDone = (taskList->item(i)->checkState() == Qt::Checked);
    }
    refreshTaskList();
}

void MainWindow::onDistributeTasks() {
    QString text = bulkTaskInput->text().trimmed();
    if (text.isEmpty()) {
        return;
    }

    QStringList tasks = text.split(",", Qt::SkipEmptyParts);
    QDate start = startDateEdit->date();
    int range = start.daysTo(endDateEdit->date());
    if (range < 0) {
        return;
    }

    lastDistribution.clear();
    calendar->highlightedDates.clear();

    for (const QString& t : tasks) {
        QDate target = start.addDays(QRandomGenerator::global()->bounded(range + 1));
        dataStore[target].append({t.trimmed(), false});
        lastDistribution.append({target, t.trimmed()});
        calendar->highlightedDates.insert(target);
    }
    bulkTaskInput->clear();
    btnUndo->setEnabled(true);
    refreshTaskList();
}

void MainWindow::onUndoDistribution() {
    for (const auto& pair : lastDistribution) {
        auto& list = dataStore[pair.first];
        for (int i = 0; i < list.size(); ++i) {
            if (list[i].name == pair.second && !list[i].isDone) {
                list.removeAt(i);
                break;
            }
        }
    }
    lastDistribution.clear();
    calendar->highlightedDates.clear();
    btnUndo->setEnabled(false);
    refreshTaskList();
}