#include "mainwindow.h"
//#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    QWidget *centralWidget = new QWidget();
    QVBoxLayout *layout = new QVBoxLayout(centralWidget);

    spinBox = new QSpinBox(this);
    listWidget = new QListWidget(this);

    connect(spinBox, QOverload<int>::of(&QSpinBox::valueChanged), this, &MainWindow::valueChanged);

    connect(listWidget, &QListWidget::itemDoubleClicked, this, &MainWindow::changeItemColor);

    groupBox = new QGroupBox("Selected Item", this);
    groupBox1 = new QGroupBox("Buttons & progress", this);
    groupBox2 = new QGroupBox();
    label2 = new QLabel("Number of item", groupBox);
    label = new QLabel("Select an item", groupBox);
    lineEdit = new QLineEdit("Edit selected item name", groupBox);
    comboBox = new QComboBox(groupBox);
    comboBox->addItem("Choose a status");
    comboBox->addItem("Default");
    comboBox->addItem("Studied, but should be repeated");
    comboBox->addItem("Studied");
    pushButton = new QPushButton("Next ticket", this);
    pushButton1 = new QPushButton("Previous ticket", this);
    progressBar = new QProgressBar(this);
    progressBar->setRange(0, 100);
    label3 = new QLabel("Total progress");
    greenProgressBar = new QProgressBar(this);
    greenProgressBar->setRange(0, 100);
    label4 = new QLabel("Green progress");

    QVBoxLayout *groupBoxLayout = new QVBoxLayout(groupBox);
    groupBoxLayout->addWidget(label2);
    groupBoxLayout->addWidget(label);
    groupBoxLayout->addWidget(lineEdit);
    groupBoxLayout->addWidget(comboBox);
    groupBox->setLayout(groupBoxLayout);

    QVBoxLayout *groupBoxLayout1 = new QVBoxLayout(groupBox1);
    groupBoxLayout1->addWidget(pushButton1);
    groupBoxLayout1->addWidget(pushButton);
    groupBoxLayout1->addWidget(label3);
    groupBoxLayout1->addWidget(progressBar);
    groupBoxLayout1->addWidget(label4);
    groupBoxLayout1->addWidget(greenProgressBar);
    groupBox1->setLayout(groupBoxLayout1);

    QHBoxLayout *hLayout = new QHBoxLayout();
    hLayout->addWidget(groupBox);
    hLayout->addWidget(groupBox1);
    groupBox2->setLayout(hLayout);

    connect(listWidget, &QListWidget::itemClicked, this, &MainWindow::chosenItem);
    connect(listWidget, &QListWidget::itemClicked, this, &MainWindow::chosenItemName);
    connect(lineEdit, &QLineEdit::returnPressed, this, &MainWindow::nameChange);
    connect(comboBox, static_cast<void(QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this, &MainWindow::statusChange);
    connect(pushButton, &QPushButton::clicked, this, &MainWindow::nextTicket);
    connect(pushButton1, &QPushButton::clicked, this, &MainWindow::previousTicket);
    connect(listWidget, &QListWidget::itemSelectionChanged, this, &MainWindow::stackFill);
//    connect(pushButton, &QPushButton::clicked, this, &MainWindow::stackFill);
    connect(listWidget, &QListWidget::itemChanged, this, &MainWindow::progressChange);
    connect(listWidget, &QListWidget::itemChanged, this, &MainWindow::totalProgressChange);
    connect(spinBox, QOverload<int>::of(&QSpinBox::valueChanged), this, &MainWindow::nullProgress);

    layout->addWidget(groupBox2);
    layout->addWidget(spinBox);
    layout->addWidget(listWidget);

    centralWidget->setLayout(layout);

    this->setCentralWidget(centralWidget);
}

MainWindow::~MainWindow() {}

void MainWindow::valueChanged() {
    listWidget->clear();
    for(int i = 1; i <= spinBox->value(); ++i) {
        QListWidgetItem *item = new QListWidgetItem(QString("Bilet %1").arg(i));
        QBrush grayBrush(QColor(25, 25, 25));
        item->setBackground(grayBrush);
        listWidget->addItem(item);
    }
}

void MainWindow::changeItemColor(QListWidgetItem *item) {
    if (item->background().color() == QColor(0, 255, 0)) {
        QBrush brush(QColor(255, 255, 0));
        item->setBackground(brush);
    } else {
        QBrush brush(QColor(0, 255, 0));
        item->setBackground(brush);
    }
}

void MainWindow::chosenItem() {
    int index = listWidget->currentRow() + 1;
    label2->setText(QString("Selected Item Number: %1").arg(index));
}

void MainWindow::nameChange() {
    int currentRow = listWidget->currentRow();
    if (currentRow != -1 && lineEdit->text() != "") {
        listWidget->item(currentRow)->setText(lineEdit->text());
        label->setText(lineEdit->text());
        lineEdit->clear();
    }
}

void MainWindow::chosenItemName(QListWidgetItem *item) {
    label->setText(item->text());
}

void MainWindow::statusChange() {
    int currentRow = listWidget->currentRow();
    if (currentRow != -1) {
        QString color = comboBox->currentText();
        QColor qColor;
        if (color == "Studied, but should be repeated") {
            qColor = Qt::yellow;
        } else if (color == "Studied") {
            qColor = Qt::green;
        } else if (color == "Default") {
            qColor = QColor(25, 25, 25);
        } else {
            return;
        }
        QListWidgetItem *item = listWidget->item(currentRow);
        item->setBackground(qColor);
    }
}

void MainWindow::nextTicket() {
    int count = listWidget->count();
    if (count > 0) {
        QList<int> nonGreenIndices;
        for (int i = 0; i < count; ++i) {
            if (listWidget->item(i)->background() != Qt::green) {
                nonGreenIndices.append(i);
            }
        }
        if (!nonGreenIndices.isEmpty()) {
            int randomIndex = nonGreenIndices.at(QRandomGenerator::global()->bounded(nonGreenIndices.size()));
            listWidget->setCurrentRow(randomIndex);
            chosenItem();
            chosenItemName(listWidget->item(listWidget->currentRow()));
        }
    }
}

void MainWindow::previousTicket() {
    if (!indexStack.empty()) {
        listWidget->setCurrentRow(indexStack.top());
        indexStack.pop();
        indexStack.pop();
        chosenItem();
        chosenItemName(listWidget->item(listWidget->currentRow()));
    }
}

void MainWindow::stackFill() {
    if (previousIndex != -1) {
        indexStack.push(previousIndex);
    }
    previousIndex = listWidget->currentRow();
}

void MainWindow::progressChange() {
    int yellowCount = 0;
    int greenCount = 0;
    int totalCount = listWidget->count();

    for (int i = 0; i < totalCount; i++) {
        QColor color = listWidget->item(i)->background().color();
        if (color == Qt::yellow) {
            yellowCount++;
        } else if (color == Qt::green) {
            greenCount++;
        }
    }

    int validCount = yellowCount + greenCount;

    if (totalCount > 0) {
        int percentage = (validCount * 100) / totalCount;
        progressBar->setValue(percentage);
    } else {
        progressBar->setValue(0);
    }
};

void MainWindow::totalProgressChange () {
    int greenCount = 0;
    int totalCount = listWidget->count();

    for (int i = 0; i < totalCount; i++) {
        QColor color = listWidget->item(i)->background().color();
        if (color == Qt::green) {
            greenCount++;
        }
    }

    if (totalCount > 0) {
        int percentage = (greenCount * 100) / totalCount;
        greenProgressBar->setValue(percentage);
    } else {
        greenProgressBar->setValue(0);
    }
};

void MainWindow::nullProgress() {
    greenProgressBar->setValue(0);
    progressBar->setValue(0);
}
