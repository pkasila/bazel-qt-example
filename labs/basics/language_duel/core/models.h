#pragma once

#include <QString>
#include <QStringList>
#include <vector>

enum class Difficulty {
    Easy,
    Medium,
    Hard,
};

enum class ExerciseMode {
    Translation,
    Grammar,
};

struct TranslationTask {
    QString prompt;
    QString expected;
    QString help;
};

struct GrammarTask {
    QString sentence;
    QStringList options;
    int correctIndex = 0;
    QString help;
};

struct SessionConfig {
    Difficulty difficulty = Difficulty::Easy;
    ExerciseMode mode = ExerciseMode::Translation;
    int taskCount = 5;
    int timeLimitSeconds = 90;
    int maxMistakes = 2;
};

struct SessionStats {
    int completed = 0;
    int total = 0;
    int mistakes = 0;
    int maxMistakes = 0;
    int scoreAwarded = 0;
    int streak = 0;
    bool finishedSuccessfully = false;
    bool failedByTime = false;
    bool failedByMistakes = false;
    QString title;
};

struct SessionData {
    SessionConfig config;
    std::vector<TranslationTask> translationTasks;
    std::vector<GrammarTask> grammarTasks;
};
