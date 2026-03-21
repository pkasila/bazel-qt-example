//
// Created by Admin on 16.05.2025.
//

#ifndef EXERCISEVIEW_H
#define EXERCISEVIEW_H
#include <qwidget.h>

#include "Exercise.h"


class ExerciseView : public QWidget {
    Q_OBJECT
public:

    ExerciseView(std::function<Exercise *(QWidget *)> gen, int N, int M, int time, QString help, QWidget *parent = nullptr);

    ~ExerciseView();

    int getScore();

    QString getHelp ();

private:

    Exercise* _exercise;
    int _timeRemaining;
    int _exRemaining;
    bool _addScore = true;
    int _wrongs;
    QString _help;
};



#endif //EXERCISEVIEW_H
