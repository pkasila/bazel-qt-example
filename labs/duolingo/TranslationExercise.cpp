//
// Created by Admin on 16.05.2025.
//

#include "TranslationExercise.h"

#include <QLabel>
#include <QVBoxLayout>
#include <QLineEdit>

TranslationExercise::TranslationExercise(QString ru, QString en, QWidget *parent) : Exercise(parent), _ru(ru){
    auto w = new QWidget(this);
    auto vl = new QVBoxLayout(w);

    auto label = new QLabel(QString("Translate: %1").arg(en), w);
    _lineEdit = new QLineEdit(w);

    vl->addWidget(label);
    vl->addWidget(_lineEdit);

    w->setLayout(vl);
}

QString TranslationExercise::Result() {
    if (_score == 0) {
        return QString("Wrong! Correct answer was: \n %1").arg(_ru.toLower());
    }
    else {
        return QString("Correct!");
    }
}

void TranslationExercise::Submit() {
    if (_lineEdit->text().toLower() == _ru.toLower()) {
        _score = 1;
    }
}

