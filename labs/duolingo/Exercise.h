//
// Created by Admin on 16.05.2025.
//

#ifndef EXERCISE_H
#define EXERCISE_H
#include <qwidget.h>


class Exercise : public QWidget{
    Q_OBJECT
    public:
    Exercise(QWidget* parent = nullptr);
    ~Exercise();

    virtual void Submit();
    virtual QString Result();
    int GetScore();

protected:
    int _score = 0;
};



#endif //EXERCISE_H
