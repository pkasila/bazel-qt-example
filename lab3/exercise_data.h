#pragma once

#include <QList>
#include <QPair>
#include <QString>
#include <QStringList>

struct Question {
    QString prompt;
    QString correctAnswer;
    QStringList options;
    int correctIndex;
    QString hint;
    bool isGrammar;
    bool reverseTranslation;
};

namespace ExerciseGenerator {
Question makeTranslation(
    const QString& prompt, const QString& answer, const QString& hint, bool reverse = false);
Question makeGrammar(
    const QString& prompt, const QStringList& opts, int correctIdx, const QString& hint);
QList<Question> generate(int count, bool isGrammar);
}  // namespace ExerciseGenerator