#include "mainwindow.h"
#include "./ui_mainwindow.h"

#include <qDebug>
#include <map>
#include <vector>
#include <QMessageBox>

// std::map<QBrush, int> color_index = {
//     {QBrush(Qt::gray), 0},
//     {QBrush(Qt::yellow), 1},
//     {QBrush(Qt::green), 2}
// }

std::vector<QListWidgetItem*> previous_items;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->groupBox->setVisible(false);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_spinBox_valueChanged(int arg1)
{
    ui->listWidget->clear();
    previous_items.resize(0);
    ui->progressBar->setValue(0);
    ui->progressBar->setMaximum(arg1);
    ui->progressBar_2->setValue(0);
    ui->progressBar_2->setMaximum(arg1);
    ui->groupBox->setVisible(false);
    for (int i = 0; i < arg1; ++i) {
        ui->listWidget->addItem("Билет " + QString::number(i + 1));
        ui->listWidget->item(i)->setForeground(Qt::gray);
    }
}


void MainWindow::on_listWidget_itemDoubleClicked(QListWidgetItem *item)
{
    if (item->foreground() == Qt::gray || item->foreground() == Qt::yellow) {
        if (item->foreground() == Qt::gray) {
            ui->progressBar->setValue(ui->progressBar->value() + 1);
        }
        ui->progressBar_2->setValue(ui->progressBar->value() + 1);
        item->setForeground(Qt::green);
    } else {
        ui->progressBar_2->setValue(ui->progressBar->value() - 1);
        item->setForeground(Qt::yellow);
    }
}

void MainWindow::on_listWidget_itemClicked(QListWidgetItem *item)
{
    ui->name->setText("Билет " + QString::number(ui->listWidget->currentRow() + 1));
    ui->number->setText(QString::number(ui->listWidget->currentRow() + 1));

    if (item->foreground() == Qt::gray) {
        ui->comboBox->setCurrentIndex(0);
    }
    if (item->foreground() == Qt::yellow) {
        ui->comboBox->setCurrentIndex(1);
    }
    if (item->foreground() == Qt::green) {
        ui->comboBox->setCurrentIndex(2);
    }

    ui->groupBox->setVisible(true);

}



void MainWindow::on_comboBox_currentIndexChanged(int index)
{
    QListWidgetItem* current_item = ui->listWidget->currentItem();
    QBrush current_color = ui->listWidget->currentItem()->foreground();
    QBrush color;
    if (index == 0) {
        if (current_color == Qt::green) {
            ui->progressBar_2->setValue(ui->progressBar->value() - 1);
        }
        color = Qt::gray;
        ui->progressBar->setValue(ui->progressBar->value() - 1);
    }
    if (index == 1) {
        if (current_color == Qt::gray) {
            ui->progressBar->setValue(ui->progressBar->value() + 1);
        }
        if (current_color == Qt::green) {
            ui->progressBar_2->setValue(ui->progressBar->value() - 1);
        }
        color = Qt::yellow;
    }
    if (index == 2) {
        if (current_color == Qt::gray) {
            ui->progressBar->setValue(ui->progressBar->value() + 1);
        }
        ui->progressBar_2->setValue(ui->progressBar->value() + 1);
        color = Qt::green;
    }
    current_item->setForeground(color);
}


void MainWindow::on_lineEdit_editingFinished()
{
    if (ui->lineEdit->text().length() != 0) {
        ui->name->setText(ui->lineEdit->text());
        ui->listWidget->currentItem()->setText(ui->name->text());
    }
}


void MainWindow::on_pushButton_clicked()
{int n = ui->listWidget->count();
    int count_no_green = 0;

    for (int i = 0; i < n; ++i) {
        QListWidgetItem* item = ui->listWidget->item(i);
        if (item->foreground().color() != Qt::green) {
            count_no_green++;
        }
    }

    if (count_no_green == 0) {
        QMessageBox::information(this, "", "Все билеты выучены!!");
        return;
    }

    int rand_next = std::rand() % count_no_green;

    QListWidgetItem* item = nullptr;
    int p = 0;
    for (int i = 0; i < n; ++i) {
        QListWidgetItem* currentItem = ui->listWidget->item(i);
        if (currentItem->foreground().color() != Qt::green) {
            if (p == rand_next) {
                item = currentItem;
                break;
            }
            ++p;
        }
    }

    previous_items.push_back(ui->listWidget->currentItem());
    ui->listWidget->setCurrentItem(item);
    on_listWidget_itemClicked(item);
}

void MainWindow::on_pushButton_2_clicked()
{
    if (previous_items.size() == 0) {
        QMessageBox::information(this, "", "Нет предыдущих билетов");
        return;
    }
    QListWidgetItem* item = previous_items[previous_items.size() - 1];
    ui->listWidget->setCurrentItem(item);
    on_listWidget_itemClicked(item);
    previous_items.erase(previous_items.end() - 1);
}

