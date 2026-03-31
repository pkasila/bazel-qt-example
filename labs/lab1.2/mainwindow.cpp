#include "mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    QWidget *centralWidget = new QWidget();
    QVBoxLayout *layout = new QVBoxLayout(centralWidget);
    setStyleSheet("background-color: #f7f7e1;");

    mainLabel = new QLabel("Egg boil timer");
    mainLabel->setAlignment(Qt::AlignCenter);
    mainLabel->setStyleSheet("font-size: 36px; font-weight: bold; color: #2e2e2d;");
    questionLabel = new QLabel("Какое яйцо хочешь сварить?");
    questionLabel->setStyleSheet("color: #2e2e2d; bold;");
    literLabel = new QLabel("Выберите количество яиц");
    literLabel->setStyleSheet("color: #2e2e2d; bold;");
    literLabel1 = new QLabel("Вам понадобится 1 литр");
    literLabel1->setStyleSheet("color: #2e2e2d; bold;");
    timerLabel = new QLabel("5 : 00");
    timerLabel->setAlignment(Qt::AlignCenter);
    timerLabel->setStyleSheet("font-size: 80px; font-weight: bold; color: #ffff00; padding: 10px;");

    typeOfEggs = new QComboBox;
    typeOfEggs->addItem("Тип яйца");
    typeOfEggs->addItem("Всмятку");
    typeOfEggs->addItem("В мешочек");
    typeOfEggs->addItem("Вкрутую");
    typeOfEggs->setItemData(1, "Отличный перекус. Можно вымакивать желток хлебушком", Qt::ToolTipRole);
    typeOfEggs->setItemData(2, "Хлеб, ветчина, соус, яйцо в мешочек -- идеальный завтрак", Qt::ToolTipRole);
    typeOfEggs->setItemData(3, "Подходит для салатов и закусок", Qt::ToolTipRole);
    typeOfEggs->setStyleSheet("color: #2e2e2d; background-color: #ffffff;");


    numberOfEggs = new QSpinBox(this);
    numberOfEggs->setRange(1, 10);
    numberOfEggs->setStyleSheet("color: #2e2e2d; background-color: #ffffff;");

    startButton = new QPushButton("Да начнётся приготовление!", this);
    startButton->setStyleSheet("background-color: #ffff00; color: #2e2e2d;");
    stopButton = new QPushButton("Стоп", this);
    stopButton->setStyleSheet("background-color: #ffff00; color: #2e2e2d;");
    nullButton = new QPushButton("Сброс", this);
    nullButton->setStyleSheet("background-color: #ffff00; color: #2e2e2d;");

    groupBox1 = new QGroupBox("Выберите тип яйца", this);
    groupBox2 = new QGroupBox("Дополнительно: количество воды", this);
    groupBox3= new QGroupBox();
    groupBox4= new QGroupBox();
    groupBox1->setStyleSheet("color: #2e2e2d;");
    groupBox2->setStyleSheet("color: #2e2e2d;");

    timer = new QTimer();

    QVBoxLayout *groupBoxLayout1 = new QVBoxLayout();
    groupBoxLayout1->addWidget(questionLabel);
    groupBoxLayout1->addWidget(typeOfEggs);
    groupBox1->setLayout(groupBoxLayout1);

    QVBoxLayout *groupBoxLayout2 = new QVBoxLayout();
    groupBoxLayout2->addWidget(literLabel);
    groupBoxLayout2->addWidget(numberOfEggs);
    groupBoxLayout2->addWidget(literLabel1);
    groupBox2->setLayout(groupBoxLayout2);

    QHBoxLayout *groupBoxLayout3 = new QHBoxLayout();
    groupBoxLayout3->addWidget(groupBox1);
    groupBoxLayout3->addWidget(groupBox2);

    QHBoxLayout *hbox = new QHBoxLayout();
    hbox->addWidget(stopButton);
    hbox->addWidget(nullButton);

    connect(numberOfEggs, QOverload<int>::of(&QSpinBox::valueChanged), this, &MainWindow::literChange);
    connect(typeOfEggs, static_cast<void(QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this, &MainWindow::setSeconds);
    connect(typeOfEggs, QOverload<int>::of(&QComboBox::highlighted), this, &MainWindow::showToolTip);
    connect(startButton, &QPushButton::clicked, this, &MainWindow::startTimer);
    connect(stopButton, &QPushButton::clicked, this, &MainWindow::stopTimer);
    connect(nullButton, &QPushButton::clicked, this, &MainWindow::nullTimer);
    connect(timer, &QTimer::timeout, this, &MainWindow::onTimeout);

    layout->addWidget(mainLabel);
    layout->addWidget(timerLabel);
    layout->addWidget(startButton);
    layout->addLayout(hbox);
    layout->addLayout(groupBoxLayout3);
    // layout->addWidget(typeOfEggs);
    // layout->addWidget(groupBox1);

    centralWidget->setLayout(layout);

    this->setCentralWidget(centralWidget);
}

MainWindow::~MainWindow() {}

void MainWindow::literChange() {
    if (numberOfEggs->value() < 5) {
        literLabel1->setText("Вам понадобится 1 литра");
    } else if (numberOfEggs->value() < 8 && numberOfEggs->value() > 4) {
        literLabel1->setText("Вам понадобится 1.5 литра");
    } else if (numberOfEggs->value() < 11 && numberOfEggs->value() > 7) {
        literLabel1->setText("Вам понадобится 2 литра");
    }
}

void MainWindow::setSeconds() {
    if (timer->isActive()) {
        timer->stop();
    }
    QString type = typeOfEggs->currentText();
    if (type == "Всмятку") {
        timeLeft = 179;
        timerLabel->setText("3 : 00");
    } else if (type == "В мешочек") {
        timeLeft = 359;
        timerLabel->setText("6 : 00");
    } else if (type == "Вкрутую") {
        timeLeft = 539;
        timerLabel->setText("9 : 00");
    }
}

void MainWindow::showToolTip(int index)
{
    if (index >= 0) {
        QString toolTipText = typeOfEggs->itemData(index, Qt::ToolTipRole).toString();
        QToolTip::showText(QCursor::pos(), toolTipText);
    }
}

void MainWindow::startTimer() {
    timer->start(1000);
}

void MainWindow::onTimeout() {
    if (timeLeft > 0) {
        int minutes = timeLeft / 60;
        int seconds = timeLeft % 60;
        timerLabel->setText(QString::number(minutes) + " : " + QString::number(seconds).rightJustified(2, '0'));
        timeLeft--;
    } else {
        timer->stop();
        timerLabel->setText("Время вышло!");
    }
}

void MainWindow::stopTimer() {
    timer->stop();
}

void MainWindow::nullTimer() {
    timerLabel->setText("0 : 00");
    timeLeft = 0;
}
