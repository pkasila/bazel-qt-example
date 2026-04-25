#pragma once

#include <QString>
#include <QStringList>
#include <vector>

enum class QuestionType {
    Translation,
    Grammar
};

struct Question {
    QuestionType type;
    QString text;
    std::vector<QString> acceptedAnswers; // Replaced single answer with multiple possible variants
    QStringList options; // Used for Grammar questions
    QString hint;
};
