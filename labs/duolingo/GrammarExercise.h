//
// Created by Admin on 16.05.2025.
//

#ifndef GRAMMAREXERCISE_H
#define GRAMMAREXERCISE_H
#include <QButtonGroup>

#include "Exercise.h"


class GrammarExercise : public Exercise{
    Q_OBJECT
    public:
    GrammarExercise(QWidget *parent, QString q, QString ans, QString v1, QString v2, QString v3);

    QString Result() override;

    void Submit() override;

private:
    QButtonGroup* _bg;

};



#endif //GRAMMAREXERCISE_H
