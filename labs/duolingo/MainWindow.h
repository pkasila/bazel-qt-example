//
// Created by Admin on 16.05.2025.
//

#ifndef MAINWINDOW_H
#define MAINWINDOW_H



#include <QMainWindow>
#include <QSqlDatabase>
#include <QStackedWidget>

#include "Exercise.h"
#include "ExerciseView.h"
#include "TranslationExercise.h"


class QLabel;

class MainWindow : public QMainWindow{
    Q_OBJECT
    public:
    MainWindow(QString dbPath, QWidget* parent = nullptr);


protected:
    void keyPressEvent(QKeyEvent *event) override;

private:
    void StartExercise(std::function<Exercise *(QWidget *)> gen, QString help);
    void MainPage();
    QLabel* _score;
    QSqlDatabase _db;
    QStackedWidget* _widget;
    int _dif = 0;
    int _iscore = 0;
    ExerciseView* _ev = nullptr;
};



#endif //MAINWINDOW_H
