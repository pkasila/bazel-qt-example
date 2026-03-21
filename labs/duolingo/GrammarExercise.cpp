//
// Created by Admin on 16.05.2025.
//

#include "GrammarExercise.h"

#include <QLabel>
#include <QCheckBox>
#include <QVBoxLayout>
#include <QButtonGroup>
#include <QRandomGenerator>
#include <random>

GrammarExercise::GrammarExercise(QWidget *parent, QString q, QString ans, QString v1, QString v2, QString v3) : Exercise(parent) {
    QWidget *w = new QWidget(this);
    auto vb = new QVBoxLayout(w);

    auto l = new QLabel(q, w);

    _bg = new QButtonGroup(w);
    _bg->setExclusive(true);
    _bg->addButton(new QCheckBox(ans, w), 0);
    _bg->addButton(new QCheckBox(v1, w), 1);
    _bg->addButton(new QCheckBox(v2, w), 2);
    _bg->addButton(new QCheckBox(v3, w), 3);

    vb->addWidget(l);
    std::vector<int> ord = {0,1,2,3};
    std::shuffle(ord.begin(), ord.end(), std::mt19937(QRandomGenerator::global()->generate()));
    for (int i : ord) vb->addWidget(_bg->button(i));

    w->setLayout(vb);
}


QString GrammarExercise::Result () {
    return _bg->button(0)->isChecked() ? QString("Correct!") : QString("Wrong, correct was \n %1").arg(_bg->button(0)->text());
}

void GrammarExercise::Submit() {
    if (_bg->button(0)->isChecked()) {
        _score = 1;
    }
}

