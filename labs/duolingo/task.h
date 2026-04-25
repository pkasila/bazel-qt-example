#ifndef TASK_H
#define TASK_H

#include <QString>
#include <QStringList>

enum class ExerciseMode {
    Translation,
    Grammar,
};

enum class DifficultyLevel {
    Beginner,
    Intermediate,
    Advanced,
};

struct Task {
    enum class Type {
        Translation,
        Grammar,
    };

    Type type = Type::Translation;
    QString prompt;
    QString answer;
    QStringList options;
    QString hint;
};

struct SessionConfig {
    ExerciseMode mode = ExerciseMode::Translation;
    DifficultyLevel level = DifficultyLevel::Beginner;
    int taskCount = 5;
    int maxWrongAttempts = 3;
    int durationSeconds = 90;
    int scorePerTask = 10;
};

#endif // TASK_H
