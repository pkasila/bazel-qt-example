//
// Created by Admin on 16.05.2025.
//

#ifndef TRANSLATIONEXERCISE_H
#define TRANSLATIONEXERCISE_H
#include <QApplication>
#include <QLineEdit>

#include "Exercise.h"


class TranslationExercise : public Exercise{
    Q_OBJECT
    public:
    TranslationExercise(QString ru, QString en, QWidget* parent);

    QString Result() override;

    void Submit() override;

private:
    QLineEdit* _lineEdit;
    QString _ru;
};



#endif //TRANSLATIONEXERCISE_H
