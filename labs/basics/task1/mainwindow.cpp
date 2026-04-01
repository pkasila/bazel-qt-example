#include "mainwindow.h"
#include "./ui_mainwindow.h"

#include <QVBoxLayout>
#include <QHeaderView>
#include <QHBoxLayout>
#include <QSpacerItem>
#include <QPushButton>
#include <QLineEdit>
#include <QComboBox>
#include <QRandomGenerator>
#include <QDialog>
#include <QProgressBar>
#include <QPalette>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    //, ui(new Ui::MainWindow)
{
    //ui->setupUi(this);

    box = new QSpinBox(this);
    box->setRange(1, 1000);
    box->setValue(1);
    box->setFixedSize(100, 30);

    list = new QTableWidget(this);
    list->setColumnCount(1);
    list->setRowCount(1);
    list->verticalHeader()->setVisible(false);
    list->setHorizontalHeaderLabels({"Список билетов"});

    for (int row = 0; row < list->rowCount(); ++row) {
        QTableWidgetItem *item = new QTableWidgetItem(QString("Билет номер %1").arg(row + 1));
        item->setBackground(QColor());
        list->setItem(row, 0, item);
        Ticket tic;
        tic.setSatus(0);
        tic.setName(QString("Билет номер %1").arg(row + 1));
        tic.setNum(row + 1);
        tickets.push_back(tic);
    }

    list->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    list->setEditTriggers(QAbstractItemView::NoEditTriggers);

    QPushButton* next = new QPushButton("Случайный");
    QPushButton* prev = new QPushButton("Предыдущий");

    total_bar = new QProgressBar(this);
    green_bar = new QProgressBar(this);
    updateProgressBar();

    QVBoxLayout *mainLayout = new QVBoxLayout();

    QHBoxLayout *spinBoxLayout = new QHBoxLayout();
    spinBoxLayout->addWidget(box, 0, Qt::AlignTop | Qt::AlignRight);
    spinBoxLayout->addWidget(next, Qt::AlignTop);
    spinBoxLayout->addWidget(prev, Qt::AlignTop);
    spinBoxLayout->addSpacerItem(new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum));

    mainLayout->addLayout(spinBoxLayout);
    mainLayout->addWidget(list);
    mainLayout->addWidget(total_bar, Qt::AlignBottom);
    mainLayout->addWidget(green_bar, Qt::AlignBottom);

    QWidget *centralWidget = new QWidget(this);

    centralWidget->setLayout(mainLayout);
    setCentralWidget(centralWidget);

    connect(next, &QPushButton::clicked, this, [&]() {
        std::vector<Ticket> norm_tic;
        for (auto tick : tickets) {
            if (tick.getStatus() != 2) {
                norm_tic.push_back(tick);
            }
        }

        if (norm_tic.empty()) {
            return;
        }
        int row;
        if (norm_tic.size() == 1) {
            row = 0;
        } else {
            row = QRandomGenerator::global()->bounded(0, static_cast<int>(norm_tic.size() - 1));
        }

        QWidget *ticketWidget = new QWidget();
        ticketWidget->setWindowTitle("Информация о билете");
        ticketWidget->setAttribute(Qt::WA_DeleteOnClose);

        QString originalName = tickets[row].getName();

        QLineEdit *nameEdit = new QLineEdit(originalName, ticketWidget);
        QLabel *numLabel = new QLabel("Номер билета:" + QString::number(tickets[row].getNum()), ticketWidget);

        QString about;
        if (tickets[row].getStatus() == 0) {
            about = "Билет не повторялся";
        } else if (tickets[row].getStatus() == 1) {
            about = "Билет стоит повторить";
        } else {
            about = "Билет хорошо рассказан";
        }
        QComboBox *statusLabel = new QComboBox();
        statusLabel->addItem(about);
        if (about == "Билет не повторялся") {
            statusLabel->addItem("Билет стоит повторить");
            statusLabel->addItem("Билет хорошо рассказан");
        } else if (about == "Билет стоит повторить") {
            statusLabel->addItem("Билет хорошо рассказан");
            statusLabel->addItem("Билет не повторялся");
        } else {
            statusLabel->addItem("Билет стоит повторить");
            statusLabel->addItem("Билет не повторялся");
        }

        QPushButton *saveButton = new QPushButton("Сохранить", ticketWidget);

        QVBoxLayout *layout = new QVBoxLayout(ticketWidget);
        layout->addWidget(nameEdit);
        layout->addWidget(numLabel);
        layout->addWidget(statusLabel);
        layout->addWidget(saveButton);

        connect(saveButton, &QPushButton::clicked, [this, row, ticketWidget, nameEdit, statusLabel]() {
            QString newName = nameEdit->text();
            tickets[row].setName(newName);
            list->item(row, 0)->setText(newName);
            QString selectedStatus = statusLabel->currentText();
            if (selectedStatus == "Билет не повторялся") {
                tickets[row].setSatus(0);
            } else if (selectedStatus == "Билет стоит повторить") {
                tickets[row].setSatus(1);
            } else if (selectedStatus == "Билет хорошо рассказан") {
                tickets[row].setSatus(2);
            }
            auto stat = tickets[row].getStatus();
            if (stat == 2) {
                tickets[row].setSatus(2);
                QTableWidgetItem* item = list->item(row, 0);
                item->setBackground(QColor(164, 180, 101));
            } else if (stat == 1) {
                tickets[row].setSatus(1);
                QTableWidgetItem* item = list->item(row, 0);
                item->setBackground(QColor(255, 207, 80));
            } else {
                tickets[row].setSatus(0);
                QTableWidgetItem* item = list->item(row, 0);
                item->setBackground(QColor());
                updateProgressBar();
            }
            ticketWidget->close();
            updateProgressBar();
        });
        history.push_back(tickets[row]);
        ticketWidget->show();
    });
    connect(prev, &QPushButton::clicked, this, [&]() {
        if (history.empty()) {
            return;
        }
        auto row = history.back().getNum() - 1;
        QWidget *ticketWidget = new QWidget();
        ticketWidget->setWindowTitle("Информация о билете");
        ticketWidget->setAttribute(Qt::WA_DeleteOnClose);

        QString originalName = tickets[row].getName();

        QLineEdit *nameEdit = new QLineEdit(originalName, ticketWidget);
        QLabel *numLabel = new QLabel("Номер билета: " + QString::number(tickets[row].getNum()), ticketWidget);

        QString about;
        if (tickets[row].getStatus() == 0) {
            about = "Билет не повторялся";
        } else if (tickets[row].getStatus() == 1) {
            about = "Билет стоит повторить";
        } else {
            about = "Билет хорошо рассказан";
        }
        QComboBox *statusLabel = new QComboBox();
        if (about == "Билет не повторялся") {
            statusLabel->addItem("Билет стоит повторить");
            statusLabel->addItem("Билет хорошо рассказан");
        } else if (about == "Билет стоит повторить") {
            statusLabel->addItem("Билет хорошо рассказан");
            statusLabel->addItem("Билет не повторялся");
        } else {
            statusLabel->addItem("Билет стоит повторить");
            statusLabel->addItem("Билет не повторялся");
        }

        QPushButton *saveButton = new QPushButton("Сохранить", ticketWidget);

        QVBoxLayout *layout = new QVBoxLayout(ticketWidget);
        layout->addWidget(nameEdit);
        layout->addWidget(numLabel);
        layout->addWidget(statusLabel);
        layout->addWidget(statusLabel);
        layout->addWidget(saveButton);

        connect(saveButton, &QPushButton::clicked, [this, row, ticketWidget, nameEdit, statusLabel]() {
            QString newName = nameEdit->text();
            tickets[row].setName(newName);
            list->item(row, 0)->setText(newName);
            QString selectedStatus = statusLabel->currentText();
            if (selectedStatus == "Билет не повторялся") {
                tickets[row].setSatus(0);
            } else if (selectedStatus == "Билет стоит повторить") {
                tickets[row].setSatus(1);
            } else if (selectedStatus == "Билет хорошо рассказан") {
                tickets[row].setSatus(2);
            }
            auto stat = tickets[row].getStatus();
            if (stat == 2) {
                tickets[row].setSatus(2);
                QTableWidgetItem* item = list->item(row, 0);
                item->setBackground(QColor(164, 180, 101));
            } else if (stat == 1) {
                tickets[row].setSatus(1);
                QTableWidgetItem* item = list->item(row, 0);
                item->setBackground(QColor(255, 207, 80));
            } else {
                tickets[row].setSatus(1);
                QTableWidgetItem* item = list->item(row, 0);
                item->setBackground(QColor());
            }
            updateProgressBar();
            ticketWidget->close();
        });
        history.pop_back();
        updateProgressBar();
        ticketWidget->show();

    });
    connections();
}

MainWindow::~MainWindow()
{
    //delete ui;
}

void MainWindow::connections() {
    timer = new QTimer(this);
    timer->setSingleShot(true);
    connect(box, &QSpinBox::valueChanged, this, &MainWindow::updateTable);
    connect(list, &QTableWidget::cellClicked, this, &MainWindow::aboutClick);
    connect(list, &QTableWidget::doubleClicked, this, &MainWindow::onCelldoubleClicked);
    connect(timer, &QTimer::timeout, this, &MainWindow::onCellClicked);
    updateProgressBar();
}

void MainWindow::aboutClick(int row) {
    last = row;
    timer->start(250);
}

void MainWindow::updateTable(int num) {
    int currentRowCount = list->rowCount();

    if (num > currentRowCount) {
        list->setRowCount(num);
        for (int row = currentRowCount; row < num; ++row) {
            QTableWidgetItem *item = new QTableWidgetItem(QString("Билет номер %1").arg(row + 1));
            item->setBackground(QColor());
            list->setItem(row, 0, item);
            Ticket tic;
            tic.setSatus(0);
            tic.setName(QString("Билет номер %1").arg(row + 1));
            tic.setNum(row + 1);
            tickets.push_back(tic);
        }
    } else if (num < currentRowCount) {
        list->setRowCount(num);
        while (currentRowCount-- != num) {
            tickets.pop_back();
        }
    }
    updateProgressBar();
}

void MainWindow::onCellClicked() {
    if (last == -1) return;
    int row = last;
    QWidget *ticketWidget = new QWidget();
    ticketWidget->setWindowTitle("Информация о билете");
    ticketWidget->setAttribute(Qt::WA_DeleteOnClose);

    QString originalName = tickets[row].getName();

    QLineEdit *nameEdit = new QLineEdit(originalName, ticketWidget);
    QLabel *numLabel = new QLabel("Номер билета: " + QString::number(tickets[row].getNum()), ticketWidget);

    QString about;
    if (tickets[row].getStatus() == 0) {
        about = "Билет не повторялся";
    } else if (tickets[row].getStatus() == 1) {
        about = "Билет стоит повторить";
    } else {
        about = "Билет хорошо рассказан";
    }
    QComboBox *statusLabel = new QComboBox();
    statusLabel->addItem(about);
    if (about == "Билет не повторялся") {
        statusLabel->addItem("Билет стоит повторить");
        statusLabel->addItem("Билет хорошо рассказан");
    } else if (about == "Билет стоит повторить") {
        statusLabel->addItem("Билет хорошо рассказан");
        statusLabel->addItem("Билет не повторялся");
    } else {
        statusLabel->addItem("Билет стоит повторить");
        statusLabel->addItem("Билет не повторялся");
    }

    QPushButton *saveButton = new QPushButton("Сохранить", ticketWidget);

    QVBoxLayout *layout = new QVBoxLayout(ticketWidget);
    layout->addWidget(nameEdit);
    layout->addWidget(numLabel);
    layout->addWidget(statusLabel);
    layout->addWidget(statusLabel);
    layout->addWidget(saveButton);

    connect(saveButton, &QPushButton::clicked, [this, row, ticketWidget, nameEdit, statusLabel]() {
        QString newName = nameEdit->text();
        tickets[row].setName(newName);
        list->item(row, 0)->setText(newName);
        QString selectedStatus = statusLabel->currentText();
        if (selectedStatus == "Билет не повторялся") {
            tickets[row].setSatus(0);
        } else if (selectedStatus == "Билет стоит повторить") {
            tickets[row].setSatus(1);
        } else if (selectedStatus == "Билет хорошо рассказан") {
            tickets[row].setSatus(2);
        }
        auto stat = tickets[row].getStatus();
        if (stat == 2) {
            tickets[row].setSatus(2);
            QTableWidgetItem* item = list->item(row, 0);
            item->setBackground(QColor(164, 180, 101));
        } else if (stat == 1) {
            tickets[row].setSatus(1);
            QTableWidgetItem* item = list->item(row, 0);
            item->setBackground(QColor(255, 207, 80));
        } else {
            tickets[row].setSatus(1);
            QTableWidgetItem* item = list->item(row, 0);
            item->setBackground(QColor());
        }
        updateProgressBar();
        ticketWidget->close();
    });
    updateProgressBar();
    history.push_back(tickets[row]);
    ticketWidget->show();
}

void MainWindow::onCelldoubleClicked(QModelIndex ind) {
    timer->stop();

    int row = ind.row();
    auto stat = tickets[row].getStatus();
    if (stat == 0 || stat == 1) {
        tickets[row].setSatus(2);
        QTableWidgetItem* item = list->item(row, 0);
        item->setBackground(QColor(164, 180, 101));
    } else {
        tickets[row].setSatus(1);
        QTableWidgetItem* item = list->item(row, 0);
        item->setBackground(QColor(255, 207, 80));
    }
    history.push_back(tickets[row]);
    updateProgressBar();
}

void MainWindow::updateProgressBar() {
    int size = tickets.size();
    int count_green = 0, count_yellow = 0;
    for (auto el : tickets) {
        if (el.getStatus() == 1) {
            count_yellow++;
        }
        if (el.getStatus() == 2) {
            count_green++;
        }
    }
    total_bar->setRange(0, size);
    total_bar->setValue(count_green + count_yellow);
    QPalette palette = total_bar->palette();
    palette.setColor(QPalette::Highlight, QColor(255, 207, 80));
    total_bar->setPalette(palette);

    green_bar->setRange(0, size);
    green_bar->setValue(count_green);
    QPalette palette2 = green_bar->palette();
    palette2.setColor(QPalette::Highlight, QColor(164, 180, 101));
    green_bar->setPalette(palette2);
}
