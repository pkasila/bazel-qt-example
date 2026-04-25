#pragma once

#include <QString>

class AnswerChecker {
public:
    static bool isTranslationCorrect(const QString& userAnswer, const QString& expectedAnswer);
    static int levenshteinDistance(const QString& left, const QString& right);

private:
    static QString normalized(QString value);
};
