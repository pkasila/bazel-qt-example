#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include <QString>
#include <QListWidget>
#include <QListWidgetItem>
#include <QRandomGenerator>
#include <QProgressBar>
#include <QHBoxLayout>
#include <QLabel>
#include <QFrame>
#include <QColor>
#include <QComboBox>
#include <QDebug>
#include <QMessageBox>

std::map<int, Question> questions;

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


void addListItemWithStatus(QListWidget *listWidget, const QString &text) {
    QWidget *itemWidget = new QWidget();
    QHBoxLayout *layout = new QHBoxLayout(itemWidget);

    QLabel *label = new QLabel(text);
    QFrame *statusBar = new QFrame();
    statusBar->setObjectName("statusBar");
    statusBar->setFixedSize(20, 20);
    statusBar->setStyleSheet(QString("background-color: %1; border: 1px solid black;").arg(QColor(Qt::gray).name()));


    layout->addWidget(label);
    layout->addWidget(statusBar);
    layout->setContentsMargins(5, 2, 5, 2); // Минимальные отступы
    itemWidget->setLayout(layout);

    QListWidgetItem *item = new QListWidgetItem(listWidget);
    item->setSizeHint(itemWidget->sizeHint());
    listWidget->addItem(item);
    listWidget->setItemWidget(item, itemWidget);  // Устанавливаем кастомный виджет
}

void SetStatus(Status s, QComboBox* box){
    if(s == Status::DEFAULT){
        box->setCurrentIndex(2);
    } else if(s == Status::LEARNED){
        box->setCurrentIndex(1);
    } else{
        box->setCurrentIndex(0);
    }
}

void MainWindow::RenewProgress(){
    double percent = static_cast<double>((questions.size() - unused.size())) / questions.size() * 100;
    ui->progressBar->setValue(percent);
    qDebug() << percent;
}

int FindNumber(QString text){
    int index = text.length() - 1;
    std::string digit;
    while(isdigit(text[index].toLatin1())){
        digit += text[index].toLatin1();
        --index;
    }
    std::reverse(digit.begin(), digit.end());
    return stoi(digit);
}

int GenerateRandomNumber(std::vector<int>& unused){
    if(unused.empty()){
        showMessage("Предупреждение", "Все билеты уже выучены");
        return -1;
    }
    int position = QRandomGenerator::global()->bounded(0, unused.size());
    qDebug() << "random: " << position;
    QStringList list;
    for (int x : unused) {
        list << QString::number(x);
    }
    qDebug() << "Unused: " << list.join(", ");
    return unused[position];
}

void MainWindow::on_pushButton_clicked()
{
    int randomNum = GenerateRandomNumber(unused);
    if(randomNum != -1){
        ui->questionText->setPlainText(questions[randomNum].GetText());
        ui->questionName->setPlainText(questions[randomNum].GetName());
        ui->questionNumber->setText(QString::number(randomNum));
        ui->listWidget->setCurrentRow(randomNum - 1);
    }

}

void MainWindow::RenewUnused(Question x, std::vector<int>& unused){
    std::sort(unused.begin(), unused.end());
    if(x.GetStatus() == Status::LEARNED){
        auto iter = std::lower_bound(unused.begin(), unused.end(), x.GetNumber());
        if(iter != unused.end() && *iter == x.GetNumber()){
            unused.erase(iter);
        }
    } else if(x.GetStatus() == Status::PROGRESS || x.GetStatus() == Status::DEFAULT){
        auto iter = std::lower_bound(unused.begin(), unused.end(), x.GetNumber());
        if(iter == unused.end() || *iter != x.GetNumber()){
            unused.push_back(x.GetNumber());
        }
    }
    RenewProgress();
}

void MainWindow::SetCurrentQuestion(int number){
    ui->questionNumber->setText(QString::number(questions[number].GetNumber()));
    ui->questionName->setText(QString(questions[number].GetName()));
    ui->questionText->setText(QString(questions[number].GetText()));
    ui->listWidget->setCurrentRow(number - 1);
    auto status = questions[number].GetStatus();
    if(status == Status::DEFAULT){
        ui->statusBox->setCurrentIndex(2);
    } else if(status == Status::PROGRESS){
        ui->statusBox->setCurrentIndex(1);
    } else if(status == Status::LEARNED){
        ui->statusBox->setCurrentIndex(0);
    }
    RenewProgress();
}


void MainWindow::on_listWidget_itemClicked(QListWidgetItem *item)
{
    QWidget *itemWidget = ui->listWidget->itemWidget(item);  // Получаем кастомный виджет
    if (!itemWidget) return;  // Проверяем, существует ли он

    QLabel *label = itemWidget->findChild<QLabel*>();  // Ищем QLabel внутри
    if (!label) return;  // Проверяем, найден ли QLabel

    int questionNum = FindNumber(label->text());
    ui->questionText->setPlainText(questions[questionNum].GetText());
    ui->questionName->setPlainText(questions[questionNum].GetName());
    ui->questionNumber->setText(QString::number(questionNum));

    RenewProgress();
}

void SetStatusColor(QWidget *itemWidget){
    QLabel *label = itemWidget->findChild<QLabel*>();  // Ищем QLabel внутри
    if (!label) return;

    QFrame *frame = itemWidget->findChild<QFrame*>("statusBar");
    if(!frame){
        return;
    }
    int questionNum = FindNumber(label->text());
    if(questions[questionNum].GetStatus() == Status::DEFAULT){
        frame->setStyleSheet(QString("background-color: %1; border: 1px solid black;").arg(QColor(Qt::gray).name()));
    } else if(questions[questionNum].GetStatus() == Status::PROGRESS){
        frame->setStyleSheet(QString("background-color: %1; border: 1px solid black;").arg(QColor(Qt::yellow).name()));
    } else if(questions[questionNum].GetStatus() == Status::LEARNED){
        frame->setStyleSheet(QString("background-color: %1; border: 1px solid black;").arg(QColor(Qt::green).name()));
    }

}



void MainWindow::on_listWidget_itemDoubleClicked(QListWidgetItem *item)
{
    QWidget *itemWidget = ui->listWidget->itemWidget(item);  // Получаем кастомный виджет
    if (!itemWidget) return;  // Проверяем, существует ли он

    QLabel *label = itemWidget->findChild<QLabel*>();  // Ищем QLabel внутри
    if (!label) return;
    QFrame *frame = itemWidget->findChild<QFrame*>("statusBar");
    if(!frame){
        return;
    }
    int questionNum = FindNumber(label->text());
    if(questions[questionNum].GetStatus() == Status::DEFAULT || questions[questionNum].GetStatus() == Status::PROGRESS){
        questions[questionNum].SetStatus(Status::LEARNED);
    } else if(questions[questionNum].GetStatus() == Status::LEARNED){
        questions[questionNum].SetStatus(Status::PROGRESS);
    }
    SetStatusColor(itemWidget);
    //
    RenewUnused(questions[questionNum], unused);
    QStringList list;
    for (int x : unused) {
        list << QString::number(x);
    }
    qDebug() << "Unused: " << list.join(", ");
    //

    RenewProgress();
}


void MainWindow::on_spinBox_valueChanged(int arg1)
{
    unused.clear();
    ui->listWidget->clear();
    for(int i = 1; i <= arg1; ++i){
        QString name = "Билет " + QString::number(i);
        QString text = "text " + QString::number(i);
        auto new_question = new Question(i, text, name);
        questions[i] = *new_question;
        addListItemWithStatus(ui->listWidget, name);
        unused.push_back(i);
    }

    RenewProgress();
}


void MainWindow::on_statusBox_currentIndexChanged(int index)
{
    if(index == 0){
        questions[ui->questionNumber->text().toInt()].SetStatus(Status::LEARNED);
    } else if(index == 1){
        questions[ui->questionNumber->text().toInt()].SetStatus(Status::PROGRESS);
    } else if(index == 2){
        questions[ui->questionNumber->text().toInt()].SetStatus(Status::DEFAULT);
    }
    auto item = ui->listWidget->item(ui->questionNumber->text().toInt() - 1);
    QWidget *itemWidget = ui->listWidget->itemWidget(item);
    //if (!itemWidget) return;

    RenewUnused(questions[ui->questionNumber->text().toInt()], unused);
    SetStatusColor(itemWidget);
    RenewProgress();
}


void MainWindow::on_NextButton_clicked()
{
    int number = ui->questionNumber->text().toInt();
    ++number;
    SetCurrentQuestion(number);

}


void MainWindow::on_PreviousButton_clicked()
{
    int number = ui->questionNumber->text().toInt();
    --number;
    SetCurrentQuestion(number);
}



void MainWindow::on_SaveButton_clicked()
{
    int number = ui->questionNumber->text().toInt();
    QString text = ui->questionText->toPlainText();
    questions[number].SetText(text);
}

