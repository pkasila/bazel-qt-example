#pragma once

#include <QString>
#include <QStringList>

enum class ExerciseType {
    Translation,
    Grammar
};

struct Question {
    QString text;
    QString correctAnswer;
    QStringList options;
    QString hint;
};
