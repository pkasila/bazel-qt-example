#include "mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent) {
    setupUI();
    resize(900,600);
}

MainWindow::~MainWindow() {}

void MainWindow::setupUI() {
    QWidget *centralWidget = new QWidget(this);
    QVBoxLayout *mainLayout = new QVBoxLayout(centralWidget);

    spinBox = new QSpinBox(this);
    spinBox->setRange(1, 100);
    spinBox->setValue(1);
    connect(spinBox, QOverload<int>::of(&QSpinBox::valueChanged), this, &MainWindow::updateView);


    listWidget = new QListWidget(this);
    connect(listWidget, &QListWidget::itemClicked, this, &MainWindow::handleItemClick);
    connect(listWidget, &QListWidget::itemDoubleClicked, this, &MainWindow::handleItemDoubleClick);

    numberLabel = new QLabel(this);
    nameLabel = new QLabel(this);
    nameEdit = new QLineEdit(this);
    connect(nameEdit, &QLineEdit::returnPressed, this, &MainWindow::updateTicketName);

    statusComboBox = new QComboBox(this);
    statusComboBox->addItems({"Неповторённый", "Хочу повторить ещё", "Повторённый"});
    connect(statusComboBox, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &MainWindow::updateStatus);

    nextButton = new QPushButton("Следующий билет", this);
    connect(nextButton, &QPushButton::clicked, this, &MainWindow::nextQuestion);

    previousButton = new QPushButton("Предыдущий билет", this);
    connect(previousButton, &QPushButton::clicked, this, &MainWindow::previousQuestion);

    totalProgress = new QProgressBar(this);
    greenProgress = new QProgressBar(this);

    resetStatusButton = new QPushButton("Сбросить статус", this);
    connect(resetStatusButton, &QPushButton::clicked, this, &MainWindow::resetStatusToDefault);

    QLabel *hintLabel = new QLabel("Подсказка:", this);
    hintEdit = new HintTextEdit(this);  // Используем HintTextEdit
    hintEdit->setPlaceholderText("Введите подсказку для билета");
    connect(hintEdit, &HintTextEdit::enterPressed, this, &MainWindow::saveHint);

    QHBoxLayout *buttonLayout = new QHBoxLayout();
    buttonLayout->addWidget(previousButton);
    buttonLayout->addWidget(nextButton);

    mainLayout->addWidget(spinBox);
    mainLayout->addWidget(listWidget);
    mainLayout->addWidget(numberLabel);
    mainLayout->addWidget(nameLabel);
    mainLayout->addWidget(nameEdit);
    mainLayout->addWidget(statusComboBox);
    mainLayout->addLayout(buttonLayout);
    mainLayout->addWidget(hintLabel);
    mainLayout->addWidget(hintEdit);
    mainLayout->addWidget(totalProgress);
    mainLayout->addWidget(greenProgress);
    buttonLayout->addWidget(resetStatusButton);

    setCentralWidget(centralWidget);
}

void MainWindow::resetStatusToDefault() {
    QListWidgetItem *item = listWidget->currentItem();
    if (item) {
        item->setBackground(Qt::gray);
        statusComboBox->setCurrentIndex(0);
        updateProgress();
    }
}

void MainWindow::saveHint() {
    QString hint = hintEdit->toPlainText();
    QListWidgetItem *item = listWidget->currentItem();
    if (item) {
        if (hint.isEmpty()) {
            item->setData(Qt::UserRole + 1, QVariant());
            QMessageBox::information(this, "Подсказка удалена", "Подсказка успешно удалена!");
        } else {
            item->setData(Qt::UserRole + 1, hint);
            QMessageBox::information(this, "Подсказка сохранена", "Подсказка успешно добавлена!");
        }
    }
}

void MainWindow::updateView(int count) {
    listWidget->clear();
    for (int i = 1; i < count+1; ++i) {
        QListWidgetItem *item = new QListWidgetItem(QString("Билет %1").arg(i), listWidget);
        item->setBackground(Qt::gray);
        item->setData(Qt::UserRole, i);
        listWidget->addItem(item);
    }
    updateProgress();
}

void MainWindow::handleItemClick(QListWidgetItem *item) {
    int ticketNumber = item->data(Qt::UserRole).toInt();
    currentTicket = ticketNumber;
    ticketHistory.push(ticketNumber);
    updateQuestionView(ticketNumber);
}

void MainWindow::handleItemDoubleClick(QListWidgetItem *item) {
    if (item->background() == Qt::green) {
        item->setBackground(Qt::yellow);
    } else {
        item->setBackground(Qt::green);
    }
    updateProgress();
}

void MainWindow::updateQuestionView(int ticketNumber) {
    numberLabel->setText(QString::number(ticketNumber));
    nameLabel->setText(QString("Билет %1").arg(ticketNumber));
    statusComboBox->setCurrentIndex(0);

    QListWidgetItem *item = listWidget->currentItem();
    if (item) {
        QString hint = item->data(Qt::UserRole + 1).toString();
        hintEdit->setPlainText(hint);
    } else {
        hintEdit->clear();
    }
}

void MainWindow::updateTicketName() {
    QString newName = nameEdit->text();
    if (!newName.isEmpty()) {
        nameLabel->setText(newName);
        QListWidgetItem *item = listWidget->currentItem();
        if (item) {
            item->setText(newName);
        }
    }
}

void MainWindow::updateStatus(int index) {
    QListWidgetItem *item = listWidget->currentItem();
    if (item) {
        if (index == 0) {
            item->setBackground(Qt::gray);
        } else if (index == 1) {
            item->setBackground(Qt::yellow);
        } else if (index == 2) {
            item->setBackground(Qt::green);
        }
        updateProgress();
    }
}

void MainWindow::nextQuestion() {
    int count = spinBox->value();
    if (count == 0) return;

    int nextTicket = QRandomGenerator::global()->bounded(1, count + 1);
    QListWidgetItem *item = listWidget->item(nextTicket - 1);
    if (item && (item->background() == Qt::gray || item->background() == Qt::yellow)) {
        listWidget->setCurrentItem(item);
        handleItemClick(item);
    }
}

void MainWindow::previousQuestion() {
    if (!ticketHistory.isEmpty()) {
        ticketHistory.pop();
        if (!ticketHistory.isEmpty()) {
            int previousTicket = ticketHistory.top();
            QListWidgetItem *item = listWidget->item(previousTicket - 1);
            listWidget->setCurrentItem(item);
            handleItemClick(item);
        }
    }
}

void MainWindow::updateProgress() {
    int total = listWidget->count();
    int completed = 0;
    int green = 0;

    for (int i = 0; i < total; ++i) {
        QListWidgetItem *item = listWidget->item(i);
        if (item->background() == Qt::yellow || item->background() == Qt::green) {
            completed++;
        }
        if (item->background() == Qt::green) {
            green++;
        }
    }

    totalProgress->setValue(completed * 100 / total);
    greenProgress->setValue(green * 100 / total);

    greenProgress->setStyleSheet(
        "QProgressBar {"
        "   text-align: center;"
        "}"
        "QProgressBar::chunk {"
        "   background-color: green;"
        "}"
        );
}
