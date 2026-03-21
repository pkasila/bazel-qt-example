//
// Created by Admin on 16.05.2025.
//

#include "Exercise.h"

#include <QLabel>
#include <QVBoxLayout>

Exercise::Exercise(QWidget *parent) : QWidget(parent){
}

Exercise::~Exercise () {}

void Exercise::Submit() {
    _score = 0;
}

QString Exercise::Result() {
    return "";
}


int Exercise::GetScore() {
    return _score;
}

