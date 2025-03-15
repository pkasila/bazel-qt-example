#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include <vector>
#include <QMessageBox>
#include <QDebug>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void showMessage(QString header, QString text) {
    QMessageBox::information(nullptr, header, text);
}


std::vector<std::vector<int>> field(8, std::vector<int>(8));
std::pair<int, int> current = {-2, -2};
int red_counter = 12, blue_counter = 12;
auto first_color = QBrush(Qt::red);
auto second_color = QBrush(Qt::blue);

void MainWindow::UpDateProgress(){
    ui->BlueWin->setValue(static_cast<double>((12 - red_counter)) / 12 * 100);
    ui->RedWin->setValue(static_cast<double>((12 - blue_counter)) / 12 * 100);
}

void Deb(){
    for (const auto& row : field) {
        QString line;
        for (int value : row) {
            line += QString::number(value) + " ";
        }
        qDebug() << line;
    }
}

void SetField(){
    for(int i = 0; i != 3; ++i){
        int first_num, second_num;
        if(i % 2 != 0){
            first_num = 1;
            second_num = 0;
        } else{
            first_num = 0;
            second_num = 1;
        }
        for(int j = 0; j != 8; ++j){
            if(j % 2 == 0){
                field[i][j] = first_num;
            } else{
                field[i][j] = second_num;
            }
        }
    }
    for(int i = 3; i != 5; ++i){
        int first_num, second_num;
        if(i % 2 == 0){
            first_num = 0;
            second_num = -1;
        } else{
            first_num = -1;
            second_num = 0;
        }
        for(int j = 0; j != 8; ++j){
            if(j % 2 == 0){
                field[i][j] = first_num;
            } else{
                field[i][j] = second_num;
            }
        }
    }
    for(int i = 5; i != 8; ++i){
        int first_num, second_num;
        if(i % 2 != 0){
            first_num = 2;
            second_num = 0;
        } else{
            first_num = 0;
            second_num = 2;
        }
        for(int j = 0; j != 8; ++j){
            if(j % 2 == 0){
                field[i][j] = first_num;
            } else{
                field[i][j] = second_num;
            }
        }
    }
}

void MainWindow::SetColor(QTableWidgetItem* item, int val){
    switch(val){
        case 1:
        item->setBackground(first_color);
        case 2:
            item->setBackground(second_color);
    }
}




void MainWindow::SetColorInField(){
    for(int i = 0; i != 8; ++i){
        for(int j = 0; j != 8; ++j){
            auto new_item = new QTableWidgetItem();
            if(field[i][j] == 0){
                new_item->setBackground(QBrush(Qt::gray));
            } else if(field[i][j] == -1){
                new_item->setBackground(QBrush(Qt::black));
            } else if(field[i][j] == 1){
                new_item->setBackground(first_color);
            } else if(field[i][j] == 2){
                new_item->setBackground(second_color);
            }
            ui->tableWidget->setItem(i, j, new_item);
        }
    }
}

void MainWindow::RenewField(){
    for(int i = 0; i != field.size(); ++i){
        for(int j = 0; j != field[0].size(); ++j){
            auto new_item = new QTableWidgetItem(QString::number(field[i][j]));
            new_item->setBackground(second_color);
            ui->tableWidget->setItem(i, j, new_item);
        }
    }
}

void MainWindow::UpdateCounters(){
    ui->Bcounter->setText(" " + QString::number(blue_counter));
    ui->Rcounter->setText(" " + QString::number(red_counter));
}

void MainWindow::RestartGame(){
    SetField();
    SetColorInField();
    red_counter = 12;
    blue_counter = 12;
    ui->Bcounter->setText(" " + QString::number(red_counter));
    ui->Rcounter->setText(" " + QString::number(blue_counter));
    ui->Bcounter->setText(" " + QString::number(blue_counter));
    ui->Rcounter->setText(" " + QString::number(red_counter));
}

void MainWindow::MakeResultMessage(QString text){
    QMessageBox msgBox;
    msgBox.setWindowTitle(text);
    msgBox.setText("Хотите начать заново?");
    msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
    msgBox.setDefaultButton(QMessageBox::No);

    // Обрабатываем нажатие кнопки
    int result = msgBox.exec();
    if (result == QMessageBox::Yes) {
        RestartGame();
    } else {
        QApplication::quit();
    }
}

void MainWindow::CheckWinner(){
    if(red_counter <= 0){
        MakeResultMessage("Синие выиграли");
    } else if(blue_counter <= 0){
        MakeResultMessage("Красные выиграли");
    }
}

void MainWindow::on_pushButton_clicked()
{
    ui->tableWidget->setColumnCount(8);
    ui->tableWidget->setRowCount(8);
    for(int i = 0; i != 8; ++i){
        ui->tableWidget->setColumnWidth(i, 62);
        ui->tableWidget->setRowHeight(i, 62);
    }
    ui->tableWidget->setHorizontalHeaderLabels({"1", "2", "3", "4", "5", "6", "7", "8"});
    ui->tableWidget->setVerticalHeaderLabels({"1", "2", "3", "4", "5", "6", "7", "8"});

    for(int i = 0; i != 2; ++i){
        for(int j = 0; j != field[0].size(); ++j){
            field[i][j] = 1;
        }
    }
    for(int i = 2; i != field.size() - 3; ++i){
        for(int j = 0; j != field[0].size(); ++j){
            field[i][j] = -1;
        }
    }
    for(int i = field.size() - 3; i != field.size(); ++i){
        for(int j = 0; j != field[0].size(); ++j){
            field[i][j] = 2;
        }
    }
    //RenewField();
    SetField();
    SetColorInField();
    //SetTokensColor();
}


void MainWindow::SwapTokens(int row, int column, int curr_row, int curr_column){
    field[row][column] = field[curr_row][curr_column];
    if(curr_row % 2 == 0 && curr_column % 2 == 0){
        field[curr_row][curr_column] = 0;
    } else if(curr_row % 2 == 0 && curr_column % 2 != 0){
        field[curr_row][curr_column] = -1;
    } else if(curr_row % 2 != 0 && curr_column % 2 == 0){
        field[curr_row][curr_column] = -1;
    } else if(curr_row % 2 != 0 && curr_column % 2 != 0){
        field[curr_row][curr_column] = 0;
    }
}


void MainWindow::on_tableWidget_cellClicked(int row, int column)
{
    if(current.first == -2 && current.second == -2){
        if(field[row][column] != -1 && field[row][column] != 0){
            current = {row, column};
        }
    } else if(row > current.first + 1 || row < current.first - 1 || column > current.second + 1 || column < current.second - 1){
        showMessage("Предупрждение", "Так ходить нельзя");
        current = {-2, -2};
    }
    else if(field[current.first][current.second] == field[row][column]){
        if((field[current.first][current.second] == 1 || field[current.first][current.second] == 2)){
            showMessage("Предупрждение", "Так ходить нельзя");
        }
        current = {-2, -2};
    }
    else {
        if(field[current.first][current.second] == 1 && field[row][column] == 2){
            --blue_counter;
        } else if(field[current.first][current.second] == 2 && field[row][column] == 1){
            --red_counter;
        }
        SwapTokens(row, column, current.first, current.second);
        UpDateProgress();
        Deb();
        UpdateCounters();
        SetColorInField();
        current = {-2, -2};
        CheckWinner();
    }
}


void MainWindow::on_Colors1_currentTextChanged(const QString &arg1)
{
    if(arg1 == "Красный"){
        first_color = QBrush(Qt::red);
    } else if(arg1 == "Синий"){
        first_color = QBrush(Qt::blue);
    } else if(arg1 == "Желтый"){
        first_color = QBrush(Qt::yellow);
    } else if(arg1 == "Зеленый"){
        first_color = QBrush(Qt::green);
    }
    SetColorInField();
}


void MainWindow::on_Colors2_currentTextChanged(const QString &arg1)
{
    if(arg1 == "Красный"){
        second_color = QBrush(Qt::red);
    } else if(arg1 == "Синий"){
        second_color = QBrush(Qt::blue);
    } else if(arg1 == "Желтый"){
        second_color = QBrush(Qt::yellow);
    } else if(arg1 == "Зеленый"){
        second_color = QBrush(Qt::green);
    }
    SetColorInField();
}

