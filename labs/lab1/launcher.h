#ifndef LAUNCHER_H
#define LAUNCHER_H

#include <QWidget>
#include <QPushButton>
#include <QVBoxLayout>
#include <QLabel>
#include "mainwindow.h"
#include "spaceapp.h"

class Launcher : public QWidget {
    Q_OBJECT
public:
    Launcher(QWidget *parent = nullptr) : QWidget(parent) {
        setWindowTitle("Выбор задания - Лабораторная №1");
        setFixedSize(300, 200);

        QVBoxLayout *layout = new QVBoxLayout(this);
        QLabel *label = new QLabel("Выберите задание для запуска:", this);
        label->setAlignment(Qt::AlignCenter);

        QPushButton *btn1 = new QPushButton("Задание 1: Прокрастинация", this);
        QPushButton *btn2 = new QPushButton("Задание 2: Свободное плавание", this);

        layout->addWidget(label);
        layout->addWidget(btn1);
        layout->addWidget(btn2);

        connect(btn1, &QPushButton::clicked, this, &Launcher::showTask1);
        connect(btn2, &QPushButton::clicked, this, &Launcher::showTask2);
    }

private slots:
    void showTask1() {
        MainWindow *task1 = new MainWindow(); 
        task1->setAttribute(Qt::WA_DeleteOnClose);
        task1->show();
        this->close();
    }

    void showTask2() {
        SpaceApp *task2 = new SpaceApp();
        task2->setAttribute(Qt::WA_DeleteOnClose);
        task2->show();
        this->close();
    }
};

#endif