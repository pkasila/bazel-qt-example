#include "mainwindow.h"

#include <QGroupBox>
#include <QHBoxLayout>
#include <QSplitter>
#include <QVBoxLayout>
#include <QWidget>
#include <QRandomGenerator>

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
{
    auto* central = new QWidget(this);
    auto* mainLayout = new QVBoxLayout(central);

    auto* topLayout = new QHBoxLayout();
    auto* labelCount = new QLabel("Количество билетов:", central);
    spin = new QSpinBox(central);
    spin->setRange(1, 200);
    spin->setValue(10);

    topLayout->addWidget(labelCount);
    topLayout->addWidget(spin);
    topLayout->addStretch();

    mainLayout->addLayout(topLayout);

    auto* splitter = new QSplitter(Qt::Horizontal, central);

    list = new QListWidget(splitter);
    list->setSelectionMode(QAbstractItemView::SingleSelection);

    auto* right = new QWidget(splitter);
    auto* rightLayout = new QVBoxLayout(right);

    auto* group = new QGroupBox("Текущий билет", right);
    auto* groupLayout = new QVBoxLayout(group);

    labelNumber = new QLabel("#", group);
    labelName = new QLabel("Имя", group);
    editName = new QLineEdit(group);

    comboStatus = new QComboBox(group);
    comboStatus->addItems({"Дефолт", "Желтый", "Зеленый"});

    groupLayout->addWidget(new QLabel("Номер:", group));
    groupLayout->addWidget(labelNumber);
    groupLayout->addWidget(new QLabel("Имя:", group));
    groupLayout->addWidget(labelName);
    groupLayout->addWidget(new QLabel("Изменить имя:", group));
    groupLayout->addWidget(editName);
    groupLayout->addWidget(new QLabel("Статус:", group));
    groupLayout->addWidget(comboStatus);

    auto* controlLayout = new QHBoxLayout();
    buttonNext = new QPushButton("Следующий билет", right);
    buttonPrev = new QPushButton("Предыдущий билет", right);

    controlLayout->addWidget(buttonPrev);
    controlLayout->addWidget(buttonNext);

    progressTotal = new QProgressBar(right);
    progressGreen = new QProgressBar(right);

    progressTotal->setRange(0, 100);
    progressGreen->setRange(0, 100);

    rightLayout->addWidget(group);
    rightLayout->addLayout(controlLayout);
    rightLayout->addWidget(new QLabel("Общий прогресс (желтые+зеленые):", right));
    rightLayout->addWidget(progressTotal);
    rightLayout->addWidget(new QLabel("Зеленый прогресс (только зеленые):", right));
    rightLayout->addWidget(progressGreen);
    rightLayout->addStretch();

    splitter->addWidget(list);
    splitter->addWidget(right);
    splitter->setStretchFactor(0, 1);
    splitter->setStretchFactor(1, 2);

    mainLayout->addWidget(splitter);

    setCentralWidget(central);
    setWindowTitle("Ticket Repeater");
    resize(1000, 600);

    init(spin->value());

    connect(spin, qOverload<int>(&QSpinBox::valueChanged), this, [this](int value) {
        init(value);
    });

    connect(list, &QListWidget::currentRowChanged, this, [this](int row) {
        if (row >= 0) {
            setCurrent(row, true);
        }
    });

    connect(list, &QListWidget::itemDoubleClicked, this, [this](QListWidgetItem* item) {
        int index = list->row(item);
        if (index < 0 || index >= tickets.size()) return;

        auto st = tickets[index].status;
        Status next = (st == Status::Green ? Status::Yellow : Status::Green);

        applyStatus(index, next);
        setCurrent(index, true);
    });

    connect(editName, &QLineEdit::returnPressed, this, [this]() {
        if (!editName->hasFocus()) return;

        QString text = editName->text().trimmed();
        if (text.isEmpty()) return;
        if (currentIndex < 0 || currentIndex >= tickets.size()) return;

        tickets[currentIndex].name = text;
        labelName->setText(text);

        auto* item = list->item(currentIndex);
        if (item) item->setText(text);
    });

    connect(comboStatus, qOverload<int>(&QComboBox::currentIndexChanged), this, [this](int index) {
        if (currentIndex < 0 || currentIndex >= tickets.size()) return;

        Status st = static_cast<Status>(index);
        applyStatus(currentIndex, st);
    });

    connect(buttonNext, &QPushButton::clicked, this, [this]() {
        QVector<int> candidates;

        for (int i = 0; i < tickets.size(); i++) {
            if (tickets[i].status != Status::Green) {
                candidates.append(i);
            }
        }

        if (candidates.isEmpty()) return;

        int rnd = QRandomGenerator::global()->bounded(candidates.size());
        int nextIndex = candidates[rnd];

        setCurrent(nextIndex, true);
    });

    connect(buttonPrev, &QPushButton::clicked, this, [this]() {
        if (history.isEmpty()) return;

        int previous = history.takeLast();
        setCurrent(previous, false);
    });
}

void MainWindow::init(int count)
{
    tickets.clear();
    list->clear();
    history.clear();

    currentIndex = -1;

    for (int i = 0; i < count; i++) {
        Ticket t;
        t.name = QString("Билет %1").arg(i + 1);
        t.status = Status::Default;

        tickets.append(t);

        auto* item = new QListWidgetItem(t.name);
        item->setBackground(color(t.status));
        list->addItem(item);
    }

    if (count > 0) {
        setCurrent(0, false);
    }

    updateProgress();
}

void MainWindow::updateAll()
{
    for (int i = 0; i < tickets.size(); i++) {
        updateOne(i);
    }
}

void MainWindow::updateOne(int index)
{
    if (index < 0 || index >= tickets.size()) return;

    auto* item = list->item(index);
    if (!item) return;

    item->setText(tickets[index].name);
    item->setBackground(color(tickets[index].status));
}

void MainWindow::setCurrent(int index, bool useHistory)
{
    if (index < 0 || index >= tickets.size()) return;
    if (currentIndex == index) return;

    if (useHistory && currentIndex >= 0) {
        history.append(currentIndex);
    }

    currentIndex = index;
    list->setCurrentRow(index);

    auto& t = tickets[index];

    labelNumber->setText(QString::number(index + 1));
    labelName->setText(t.name);
    editName->setText(t.name);

    comboStatus->setCurrentIndex(static_cast<int>(t.status));
}

void MainWindow::applyStatus(int index, Status status)
{
    if (index < 0 || index >= tickets.size()) return;
    if (tickets[index].status == status) return;

    tickets[index].status = status;

    updateOne(index);
    updateProgress();

    if (index == currentIndex) {
        comboStatus->blockSignals(true);
        comboStatus->setCurrentIndex(static_cast<int>(status));
        comboStatus->blockSignals(false);
    }
}

void MainWindow::updateProgress()
{
    int total = tickets.size();

    if (total == 0) {
        progressTotal->setValue(0);
        progressGreen->setValue(0);
        return;
    }

    int done = 0;
    int green = 0;

    for (int i = 0; i < tickets.size(); i++) {
        if (tickets[i].status != Status::Default) {
            done++;
        }
        if (tickets[i].status == Status::Green) {
            green++;
        }
    }

    int totalPercent = (done * 100) / total;
    int greenPercent = (green * 100) / total;

    progressTotal->setValue(totalPercent);
    progressGreen->setValue(greenPercent);
}

QColor MainWindow::color(Status status)
{
    switch (status) {
    case Status::Default:
        return QColor(Qt::lightGray);
    case Status::Yellow:
        return QColor(Qt::yellow);
    case Status::Green:
        return QColor(Qt::green);
    }

    return QColor(Qt::lightGray);
}