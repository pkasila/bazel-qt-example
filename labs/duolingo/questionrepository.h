#pragma once

#include <vector>
#include "question.h"

class QuestionRepository {
public:
    static std::vector<Question> getQuestions(int difficultyLevel, int count, int filterType = 0);
};
