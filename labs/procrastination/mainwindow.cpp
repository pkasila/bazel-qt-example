#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QListWidgetItem>
#include <QRandomGenerator>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    ui->comboBox->addItem("Не повторял");
    ui->comboBox->addItem("Повторял");
    ui->comboBox->addItem("Выучил");

    connect(ui->spinBox,
            &QSpinBox::valueChanged,
            this,
            &MainWindow::updateTickets);

    connect(ui->listWidgetItem,
            &QListWidget::itemClicked,
            this,
            &MainWindow::showTicket);

    connect(ui->lineEdit,
            &QLineEdit::returnPressed,
            this,
            &MainWindow::renameTicket);

    connect(ui->comboBox,
            &QComboBox::currentIndexChanged,
            this,
            &MainWindow::changeStatus);

    connect(ui->listWidgetItem,
            &QListWidget::itemDoubleClicked,
            this,
            &MainWindow::itemDoubleClicked);

    connect(ui->pushButton,
            &QPushButton::clicked,
            this,
            &MainWindow::nextQuestion);

    connect(ui->pushButton_2,
            &QPushButton::clicked,
            this,
            &MainWindow::previousQuestion);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::updateTickets(int count)
{
    ui->listWidgetItem->clear();

    statuses.clear();

    for (int i = 1; i <= count; i++)
    {
        ui->listWidgetItem->addItem(
            "Билет " + QString::number(i)
            );

        statuses.push_back(Default);
    }

    updateColors();
    updateProgress();
}

void MainWindow::showTicket()
{
    currentTicket = ui->listWidgetItem->currentRow();
    ui->label_2->setText(QString::number(currentTicket + 1));
    QString text =
        ui->listWidgetItem->item(currentTicket)->text();

    ui->label->setText(text);

    ui->lineEdit->setText(text);

    ui->comboBox->setCurrentIndex(
        statuses[currentTicket]
        );
}

void MainWindow::renameTicket()
{
    if (currentTicket == -1)
        return;

    QString text = ui->lineEdit->text();

    if (text.isEmpty())
        return;

    ui->listWidgetItem
        ->item(currentTicket)
        ->setText(text);

    ui->label->setText(text);
}

void MainWindow::changeStatus()
{
    if (currentTicket == -1)
        return;

    statuses[currentTicket] =
        static_cast<Status>(
            ui->comboBox->currentIndex()
            );

    updateColors();
    updateProgress();
}

void MainWindow::itemDoubleClicked(QListWidgetItem *item)
{
    int row = ui->listWidgetItem->row(item);

    if (statuses[row] == Green)
    {
        statuses[row] = Yellow;
    }
    else
    {
        statuses[row] = Green;
    }

    updateColors();
    updateProgress();

    if (row == currentTicket)
    {
        ui->comboBox->setCurrentIndex(
            statuses[row]
            );
    }
}

void MainWindow::nextQuestion()
{
    QVector<int> available;

    for (int i = 0; i < statuses.size(); i++)
    {
        if (statuses[i] != Green)
        {
            available.push_back(i);
        }
    }

    if (available.isEmpty())
        return;

    previousTicketIndex = currentTicket;

    int randomIndex =
        QRandomGenerator::global()->bounded(
            available.size()
            );

    currentTicket = available[randomIndex];

    ui->listWidgetItem->setCurrentRow(currentTicket);

    showTicket();
}

void MainWindow::previousQuestion()
{
    if (previousTicketIndex == -1)
        return;

    currentTicket = previousTicketIndex;

    ui->listWidgetItem->setCurrentRow(currentTicket);

    showTicket();
}

void MainWindow::updateColors()
{
    for (int i = 0; i < statuses.size(); i++)
    {
        QListWidgetItem *item =
            ui->listWidgetItem->item(i);

        if (statuses[i] == Default)
        {
            item->setBackground(Qt::lightGray);
        }
        else if (statuses[i] == Yellow)
        {
            item->setBackground(Qt::yellow);
        }
        else
        {
            item->setBackground(Qt::green);
        }
    }
}

void MainWindow::updateProgress()
{
    int total = statuses.size();

    if (total == 0)
        return;

    int repeated = 0;
    int green = 0;

    for (Status s : statuses)
    {
        if (s != Default)
            repeated++;

        if (s == Green)
            green++;
    }

    ui->progressBar->setMaximum(total);
    ui->progressBar->setValue(repeated);

    ui->progressBar_2->setMaximum(total);
    ui->progressBar_2->setValue(green);
}
