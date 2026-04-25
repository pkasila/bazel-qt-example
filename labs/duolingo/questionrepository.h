#pragma once

#include "question.h"

#include <QList>
#include <QString>

class QuestionRepository {
public:
    static QList<Question> questionsFor(ExerciseType type);
    static QString titleFor(ExerciseType type);
    static QString startTextFor(ExerciseType type);
};
