#include "Settings.h"

#include "ExerciseTypes.h"
#include "TaskDifficulty.h"
#include "data_base.h"

#include <random>

void Settings::SetTaskDifficulty(const TaskDifficulty& difficulty) {
    settings_.setValue("TaskDifficulty", static_cast<int>(difficulty));
    emit DifficultyChanged(difficulty);
}

TaskDifficulty Settings::GetTaskDifficulty() const {
    const auto& difficulty = settings_.value("TaskDifficulty", 0).toInt();
    return static_cast<TaskDifficulty>(difficulty);
}

void Settings::SetScore(const int& score) {
    settings_.setValue("Score", score);
    emit ScoreChanged(score);
}

int Settings::GetScore() const {
    return settings_.value("Score", 0).toInt();
}

void Settings::AddScore(const int& delta) {
    const int score = GetScore();
    SetScore(score + delta);
}

void Settings::MarkTaskDone(const int& type, const int& diff, const QString& id) {
    const std::string path = "DoneTasks/" + std::to_string(type) + "/" + std::to_string(diff);
    QStringList done = settings_.value(path).toStringList();
    if (!done.contains(id)) {
        done << id;
        settings_.setValue(path, done);
    }
}

QStringList Settings::GetDoneTasks(const int& type, const int& diff) const {
    const std::string path = "DoneTasks/" + std::to_string(type) + "/" + std::to_string(diff);
    return settings_.value(path, {}).toStringList();
}

void Settings::ClearDoneTasks() {
    for (int i = 0; i < 2; i++) {
        for (int j = 0; j < 3; j++) {
            settings_.remove("DoneTasks/" + std::to_string(i) + "/" + std::to_string(j));
        }
    }
}

std::vector<int> Settings::GetUnusedTasks(const int type, const int diff) const {
    QStringList done = GetDoneTasks(type, diff);

    std::vector<int> unused;
    unused.reserve(data_base::kQuestions);
    for (int i = 0; i < data_base::kQuestions; i++) {
        if (!done.contains(QString::number(i))) {
            unused.push_back(i);
        }
    }

    std::random_device rd;
    std::mt19937 g(rd());
    std::ranges::shuffle(unused, g);

    while (unused.size() > 5) {
        unused.pop_back();
    }

    return std::move(unused);
}
