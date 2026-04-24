#ifndef EXERCISE_DATA_H
#define EXERCISE_DATA_H

#include <QString>
#include <QStringList>

enum class Difficulty { Easy, Medium, Hard };

struct TranslationTask {
    QString question;
    QString answer;
    QString hint;
};

struct GrammarTask {
    QString question;
    QStringList options;
    int correctIndex;
    QString hint;
};

#endif