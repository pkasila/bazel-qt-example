#include "mainwindow.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QMessageBox>
#include <QBrush>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), currentIndex(-1)
{
    setWindowTitle("Повторение Билетов");
    resize(800, 600);
    spinCount = new QSpinBox();
    spinCount->setRange(1, 200);
    spinCount->setValue(10);
    spinCount->setPrefix("Количество билетов: ");

    listWidget = new QListWidget();
    listWidget->setSelectionMode(QAbstractItemView::SingleSelection);
    
    groupDetails = new QGroupBox("Информация о билете");
    labelNumber = new QLabel("Билет №: -");
    labelName = new QLabel("Название: -");
    editName = new QLineEdit();
    editName->setPlaceholderText("Введите имя и нажмите Enter");
    comboStatus = new QComboBox();
    comboStatus->addItems({"Не трогал (Default)", "Повторить (Yellow)", "Выучен (Green)"});
    
    btnNext = new QPushButton("Следующий случайный");
    btnPrev = new QPushButton("Предыдущий");
    btnPrev->setEnabled(false);

    barTotal = new QProgressBar();
    barTotal->setFormat("Общий прогресс: %p%");
    barGreen = new QProgressBar();
    barGreen->setFormat("Зелёные: %p%");

QVBoxLayout *detailsLayout = new QVBoxLayout();
detailsLayout->addWidget(labelNumber);
detailsLayout->addWidget(labelName);
detailsLayout->addWidget(editName);
detailsLayout->addWidget(comboStatus);
groupDetails->setLayout(detailsLayout);

QVBoxLayout *rightLayout = new QVBoxLayout();

rightLayout->addWidget(spinCount);
rightLayout->addSpacing(10); 

rightLayout->addWidget(groupDetails);
rightLayout->addWidget(btnNext);
rightLayout->addWidget(btnPrev);
rightLayout->addWidget(barTotal);
rightLayout->addWidget(barGreen);
rightLayout->addStretch();

QHBoxLayout *mainLayout = new QHBoxLayout();
mainLayout->addWidget(listWidget, 2);
mainLayout->addLayout(rightLayout, 1);

QWidget *central = new QWidget();
central->setLayout(mainLayout);
setCentralWidget(central);

    connect(spinCount, QOverload<int>::of(&QSpinBox::valueChanged), 
            this, &MainWindow::onCountChanged); 
    connect(listWidget, &QListWidget::itemClicked, this, &MainWindow::onTicketSelected);
    connect(listWidget, &QListWidget::itemDoubleClicked, this, &MainWindow::onTicketDoubleClicked);
    connect(btnNext, &QPushButton::clicked, this, &MainWindow::onNextClicked);
    connect(btnPrev, &QPushButton::clicked, this, &MainWindow::onPrevClicked);
    connect(editName, &QLineEdit::returnPressed, this, &MainWindow::onNameEditReturn);
    connect(comboStatus, QOverload<int>::of(&QComboBox::currentIndexChanged), 
            this, &MainWindow::onStatusChanged);

    onCountChanged(10); 
}

MainWindow::~MainWindow() {}
void MainWindow::onCountChanged(int count) {
    listWidget->setUpdatesEnabled(false); 
    resetTickets(count);
    listWidget->setUpdatesEnabled(true);
    listWidget->repaint(); 
}

void MainWindow::resetTickets(int count) {
    tickets.clear();
    listWidget->clear();
    history.clear();
    
    btnPrev->setEnabled(false);
    currentIndex = -1;
    labelNumber->setText("Билет №: -");
    labelName->setText("Название: -");
    editName->clear();
    comboStatus->setCurrentIndex(0);
    barTotal->setValue(0);
    barGreen->setValue(0);
    
    for (int i = 0; i < count; ++i) {
        Ticket t;
        t.id = i + 1;
        t.name = QString("Билет %1").arg(i + 1);
        t.status = Default;
        tickets.push_back(t);

        QListWidgetItem *item = new QListWidgetItem(t.name);
        item->setBackground(getStatusColor(Default));
        item->setData(Qt::UserRole, i);
        listWidget->addItem(item);
    }
}

void MainWindow::onTicketSelected(QListWidgetItem *item) {
    int idx = item->data(Qt::UserRole).toInt();
    updateTicketView(idx);
}

void MainWindow::onTicketDoubleClicked(QListWidgetItem *item) {
    int idx = item->data(Qt::UserRole).toInt();
    if (tickets[idx].status == Green) {
        tickets[idx].status = Yellow;
    } else {
        tickets[idx].status = Green;
    }
    
    refreshListColors();
    updateTicketView(idx);
    updateProgressBars();
}

void MainWindow::onNextClicked() {
    QVector<int> available;
    for (int i = 0; i < tickets.size(); ++i) {
        if (tickets[i].status == Default || tickets[i].status == Yellow) {
            available.push_back(i);
        }
    }

    if (available.isEmpty()) {
        QMessageBox::information(this, "Готово!", 
            "Все билеты выучены (зелёные)! 🎉\nМожете сбросить прогресс, изменив количество билетов.");
        return;
    }
    int randomIdx = available[QRandomGenerator::global()->bounded(available.size())];
    if (currentIndex != -1) {
        history.push(currentIndex);
        btnPrev->setEnabled(true);
    }

    updateTicketView(randomIdx);
    listWidget->setCurrentRow(randomIdx);
}

void MainWindow::onPrevClicked() {
    if (history.isEmpty()) return;
    
    int prevIdx = history.pop();
    if (history.isEmpty()) {
        btnPrev->setEnabled(false);
    }
    
    updateTicketView(prevIdx);
    listWidget->setCurrentRow(prevIdx);
}

void MainWindow::onNameEditReturn() {
    if (currentIndex == -1) return;
    if (!editName->hasFocus()) return;
    if (editName->text().trimmed().isEmpty()) return;
    tickets[currentIndex].name = editName->text().trimmed();
    labelName->setText("Название: " + tickets[currentIndex].name);
    QListWidgetItem *item = listWidget->item(currentIndex);
    if (item) {
        item->setText(tickets[currentIndex].name);
    }
}

void MainWindow::onStatusChanged(int index) {
    if (currentIndex == -1) return;
    
    tickets[currentIndex].status = static_cast<Status>(index);
    refreshListColors();
    updateProgressBars();
}

void MainWindow::updateTicketView(int idx) {
    if (idx < 0 || idx >= tickets.size()) return;
    
    currentIndex = idx;
    labelNumber->setText(QString("Билет №: %1").arg(tickets[idx].id));
    labelName->setText("Название: " + tickets[idx].name);
    editName->setText(tickets[idx].name);
    comboStatus->setCurrentIndex(static_cast<int>(tickets[idx].status));
}

void MainWindow::refreshListColors() {
    for (int i = 0; i < tickets.size(); ++i) {
        QListWidgetItem *item = listWidget->item(i);
        if (item) {
            item->setBackground(getStatusColor(tickets[i].status));
        }
    }
}

QColor MainWindow::getStatusColor(Status s) {
    switch (s) {
        case Default: return QColor(200, 200, 200);
        case Yellow:  return QColor(255, 255, 150);
        case Green:   return QColor(150, 255, 150);
        default:      return Qt::lightGray;
    }
}

void MainWindow::updateProgressBars() {
    if (tickets.isEmpty()) {
        barTotal->setValue(0);
        barGreen->setValue(0);
        return;
    }
    
    int total = tickets.size();
    int done = 0;
    int green = 0;
    
    for (const auto &t : tickets) {
        if (t.status != Default) done++;
        if (t.status == Green) green++;
    }
    
    barTotal->setValue((done * 100) / total);
    barGreen->setValue((green * 100) / total);
}

