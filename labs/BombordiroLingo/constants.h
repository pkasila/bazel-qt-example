#ifndef CONSTANTS_H
#define CONSTANTS_H

#include <QString>

namespace Constants {
const int MAX_ATTEMPTS = 3;
const int EXERCISE_DURATION_SEC = 300;
const int QUESTIONS_PER_EXERCISE = 10;

const QString EASY_DIFFICULTY = "Easy";
const QString MEDIUM_DIFFICULTY = "Medium";
const QString HARD_DIFFICULTY = "Hard";

const int EASY_SCORE = 1;
const int MEDIUM_SCORE = 2;
const int HARD_SCORE = 3;

const int EASY_QUESTIONS_COUNT = 10;
const int MEDIUM_QUESTIONS_COUNT = 15;
const int HARD_QUESTIONS_COUNT = 20;
}

#endif // CONSTANTS_H
