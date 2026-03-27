#ifndef SETTINGS_H
#define SETTINGS_H

#include "ExerciseTypes.h"
#include "TaskDifficulty.h"

#include <QObject>
#include <QSettings>

class Settings : public QObject {
    Q_OBJECT
   public:
    void SetTaskDifficulty(const TaskDifficulty&);
    [[nodiscard]] TaskDifficulty GetTaskDifficulty() const;

    void SetScore(const int&);
    [[nodiscard]] int GetScore() const;
    void AddScore(const int&);

    void MarkTaskDone(const int& type, const int& diff, const QString& id);
    [[nodiscard]] QStringList GetDoneTasks(const int& type, const int& diff) const;
    void ClearDoneTasks();

    [[nodiscard]]std::vector<int> GetUnusedTasks(int type, int diff) const;

   signals:
    void DifficultyChanged(TaskDifficulty difficulty);
    void ScoreChanged(int score);

   private:
    QSettings settings_{};
};

#endif  // SETTINGS_H
