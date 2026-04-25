#ifndef EXERCISE_DATA_H
#define EXERCISE_DATA_H

#include <QString>
#include <QStringList>
#include <vector>

enum class Difficulty { Easy, Medium, Hard };

struct DifficultyParams {
  int task_count;
  int timer_seconds;
  int max_wrong;
  int points_on_success;
};

DifficultyParams GetDifficultyParams(Difficulty difficulty);

struct TranslationTask {
  QString english;
  QString russian;
  QString hint;
};

struct GrammarTask {
  QString sentence;
  QStringList options;
  int correct_index;
  QString hint;
};

std::vector<TranslationTask> GetTranslationPool(Difficulty difficulty);
std::vector<GrammarTask> GetGrammarPool(Difficulty difficulty);

std::vector<TranslationTask> SampleTranslationTasks(Difficulty difficulty);
std::vector<GrammarTask> SampleGrammarTasks(Difficulty difficulty);

bool CheckTranslation(const QString& user_input, const QString& expected);

#endif
