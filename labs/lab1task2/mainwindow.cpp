#include "mainwindow.h"

#include <qboxlayout.h>
#include <qgroupbox.h>
#include <qlabel.h>
#include <qlineedit.h>
#include <qlistwidget.h>
#include <QPushButton>
#include <qresource.h>
#include <qstatusbar.h>

MainWindow::MainWindow() : QMainWindow(nullptr)
{

    setWindowTitle("123");
    setMinimumSize(QSize(800, 600));
    QWidget *widget = new QWidget();
    setCentralWidget(widget);


    line_edit = new QLineEdit();
    label = new QLabel();

    table = new QTableWidget();
    table->setRowCount(10);
    table->setColumnCount(10);
    table->setShowGrid(true);
    table->setSelectionMode(QAbstractItemView::NoSelection);

    listWidget1 = new QListWidget();

    for (int i = 0; i < 10; i++) {
        for (int j = 0; j < 10; j++) {
            table->setItem(i, j, new QTableWidgetItem());
        }
    }

    button = new QPushButton("Clear");

    check1 = new QCheckBox("Enable vibe");
    check2 = new QCheckBox("Enable super vibe");
    check2->setEnabled(false);

    connect(table, &QTableWidget::cellDoubleClicked, this, &MainWindow::clicked);
    connect(line_edit, &QLineEdit::editingFinished, this, &MainWindow::edited);
    connect(check1, &QCheckBox::checkStateChanged, check2, &QCheckBox::setEnabled);
    connect(button, &QPushButton::clicked, this, &MainWindow::clear);
    // connect(listWidget1, &QListWidget::currentRowChanged, listWidget1, &QListWidget::takeItem);
    // connect(check2, &QCheckBox::checkStateChanged);


    QVBoxLayout *layout = new QVBoxLayout();

    layout->addWidget(line_edit);
    layout->addWidget(table);
    layout->addWidget(label);
    layout->addWidget(check1);
    layout->addWidget(check2);
    layout->addWidget(button);
    layout->addWidget(listWidget1);

    widget->setLayout(layout);
}

void MainWindow::clicked() {
    table->currentItem()->setText(line_edit->text());
    if (check1->isChecked()) {
        table->currentItem()->setBackground(QBrush(Qt::red));
        if (check2->isChecked()) {
            auto* b = new QPushButton(line_edit->text());
            connect(b, &QPushButton::clicked, this, &MainWindow::buttonpress);
            table->setCellWidget(table->currentRow(), table->currentColumn(), b);
        }
    }
}

void MainWindow::edited() {
    label->setText(line_edit->text());
}

void MainWindow::buttonpress() {
    for (int i = 0; i < 10; i++) {
        table->setItem(i, table->currentColumn(), new QTableWidgetItem(std::to_string(i).data()));
    }
    for (int i = 0; i < 10; i++) {
        table->setItem( table->currentRow(), i, new QTableWidgetItem(std::to_string(i).data()));
    }
}

void MainWindow::clear() {
    for (int i = 0; i<10; i++) {
        for (int j = 0; j<10; j++) {
            table->setItem(i, j, new QTableWidgetItem());
        }
    }
    listWidget1->addItem(line_edit->text());
}
