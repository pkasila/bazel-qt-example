#ifndef EXERCISE_SESSION_H
#define EXERCISE_SESSION_H

#include "task.h"

#include <QVector>

class ExerciseSession {
public:
    ExerciseSession() = default;

    void start(ExerciseMode mode, DifficultyLevel level);
    void reset();

    bool active() const { return m_active; }
    bool finished() const { return m_finished; }
    bool succeeded() const { return m_succeeded; }
    bool timedOut() const { return m_timedOut; }

    const SessionConfig &config() const { return m_config; }
    const Task &currentTask() const { return m_tasks[m_index]; }
    int currentIndex() const { return m_index; }
    int totalTasks() const { return m_tasks.size(); }
    int wrongAttempts() const { return m_wrongAttempts; }
    int score() const { return m_score; }

    enum class SubmitResult {
        Correct,
        Wrong,
        Finished,
        Failed,
        NotStarted,
    };

    SubmitResult submitTranslationAnswer(const QString &answer);
    SubmitResult submitGrammarAnswer(const QString &answer);

    void markTimedOut();

private:
    QVector<Task> makeTranslationTasks(DifficultyLevel level, int count) const;
    QVector<Task> makeGrammarTasks(DifficultyLevel level, int count) const;

private:
    SessionConfig m_config;
    QVector<Task> m_tasks;
    int m_index = 0;
    int m_wrongAttempts = 0;
    int m_score = 0;
    bool m_active = false;
    bool m_finished = false;
    bool m_succeeded = false;
    bool m_timedOut = false;
};

#endif // EXERCISE_SESSION_H
