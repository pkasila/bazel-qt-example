#include "mainwindow.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QListWidget>
#include <QSpinBox>
#include <QLabel>
#include <QComboBox>
#include <QPushButton>
#include <QProgressBar>
#include <QGroupBox>
#include <QLineEdit>
#include <QMessageBox>

std::vector<QListWidgetItem*> previous_items;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);

    listWidget = new QListWidget(this);
    spinBox = new QSpinBox(this);
    progressBar = new QProgressBar(this);
    progressBar_2 = new QProgressBar(this);
    groupBox = new QGroupBox("Информация о билете", this);
    name = new QLabel("Билет: ", this);
    number = new QLabel("Номер: ", this);
    comboBox = new QComboBox(this);
    lineEdit = new QLineEdit(this);
    QPushButton *randomButton = new QPushButton("Выбрать случайный", this);
    QPushButton *prevButton = new QPushButton("Предыдущий билет", this);

    comboBox->addItems({"Серый", "Желтый", "Зеленый"});
    groupBox->setVisible(false);

    // Компоновка
    QVBoxLayout *mainLayout = new QVBoxLayout(centralWidget);
    mainLayout->addWidget(spinBox);
    mainLayout->addWidget(progressBar);
    mainLayout->addWidget(progressBar_2);
    mainLayout->addWidget(listWidget);
    mainLayout->addWidget(groupBox);
    mainLayout->addWidget(randomButton);
    mainLayout->addWidget(prevButton);

    QVBoxLayout *groupBoxLayout = new QVBoxLayout(groupBox);
    groupBoxLayout->addWidget(name);
    groupBoxLayout->addWidget(number);
    groupBoxLayout->addWidget(comboBox);
    groupBoxLayout->addWidget(lineEdit);

    // Подключение сигналов и слотов (новый синтаксис)
    connect(spinBox, &QSpinBox::valueChanged, this, &MainWindow::on_spinBox_valueChanged);
    connect(listWidget, &QListWidget::itemDoubleClicked, this, &MainWindow::on_listWidget_itemDoubleClicked);
    connect(listWidget, &QListWidget::itemClicked, this, &MainWindow::on_listWidget_itemClicked);
    connect(comboBox, &QComboBox::currentIndexChanged, this, &MainWindow::on_comboBox_currentIndexChanged);
    connect(lineEdit, &QLineEdit::editingFinished, this, &MainWindow::on_lineEdit_editingFinished);
    connect(randomButton, &QPushButton::clicked, this, &MainWindow::on_pushButton_clicked);
    connect(prevButton, &QPushButton::clicked, this, &MainWindow::on_pushButton_2_clicked);
}

MainWindow::~MainWindow() {}

void MainWindow::on_spinBox_valueChanged(int arg1)
{
    listWidget->clear();
    previous_items.clear();
    progressBar->setValue(0);
    progressBar->setMaximum(arg1);
    progressBar_2->setValue(0);
    progressBar_2->setMaximum(arg1);
    groupBox->setVisible(false);
    for (int i = 0; i < arg1; ++i) {
        QListWidgetItem *item = new QListWidgetItem("Билет " + QString::number(i + 1));
        item->setForeground(Qt::gray);
        listWidget->addItem(item);
    }
}


void MainWindow::on_listWidget_itemDoubleClicked(QListWidgetItem *item)
{
    if (item->foreground() == Qt::gray) {
        progressBar->setValue(progressBar->value() + 1);
        progressBar_2->setValue(progressBar_2->value() + 1);
        item->setForeground(Qt::green);
    } else if (item->foreground() == Qt::yellow) {
        progressBar_2->setValue(progressBar_2->value() + 1);
        item->setForeground(Qt::green);
    } else {
        progressBar_2->setValue(progressBar_2->value() - 1);
        item->setForeground(Qt::yellow);
    }
}

void MainWindow::on_listWidget_itemClicked(QListWidgetItem *item)
{
    name->setText(item->text());
    number->setText(QString::number(listWidget->row(item) + 1));
    groupBox->setVisible(true);
}

void MainWindow::on_comboBox_currentIndexChanged(int index)
{
    QListWidgetItem *currentItem = listWidget->currentItem();
    if (!currentItem) return;

    QBrush newColor;
    if (index == 0) {
        if (currentItem->foreground() != Qt::gray) {
            progressBar->setValue(progressBar->value() - 1);
        }
        newColor = Qt::gray;
    } else if (index == 1) {
        newColor = Qt::yellow;
    } else {
        if (currentItem->foreground() != Qt::green) {
            progressBar_2->setValue(progressBar_2->value() + 1);
        }
        newColor = Qt::green;
    }
    currentItem->setForeground(newColor);
}


void MainWindow::on_lineEdit_editingFinished()
{
    if (lineEdit->text().length() != 0) {
        name->setText(lineEdit->text());
        listWidget->currentItem()->setText(name->text());
    }
}


void MainWindow::on_pushButton_clicked()
{
    std::vector<QListWidgetItem*> available;
    for (int i = 0; i < listWidget->count(); ++i) {
        QListWidgetItem *item = listWidget->item(i);
        if (item->foreground() != Qt::green) {
            available.push_back(item);
        }
    }

    if (available.empty()) {
        QMessageBox::information(this, "", "Все билеты выучены!!");
        return;
    }

    QListWidgetItem *randomItem = available[rand() % available.size()];
    previous_items.push_back(listWidget->currentItem());
    listWidget->setCurrentItem(randomItem);
    on_listWidget_itemClicked(randomItem);
}

void MainWindow::on_pushButton_2_clicked()
{
    if (previous_items.empty()) {
        QMessageBox::information(this, "", "Нет предыдущих билетов");
        return;
    }

    QListWidgetItem *item = previous_items.back();
    previous_items.pop_back();
    listWidget->setCurrentItem(item);
    on_listWidget_itemClicked(item);
}