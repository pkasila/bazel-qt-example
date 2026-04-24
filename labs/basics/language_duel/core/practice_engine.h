#pragma once

#include "core/models.h"

class PracticeEngine {
public:
    PracticeEngine();

    void setDifficulty(Difficulty difficulty);
    Difficulty difficulty() const;

    SessionData createSession(ExerciseMode mode) const;

    static QString difficultyToString(Difficulty difficulty);

private:
    Difficulty difficulty_ = Difficulty::Easy;

    SessionConfig makeConfig(ExerciseMode mode) const;
};
