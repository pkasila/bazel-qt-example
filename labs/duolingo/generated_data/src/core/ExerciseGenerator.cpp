#include "ExerciseGenerator.h"
#include "db/ExerciseRepository.h"
#include <QDebug>
#include <algorithm> // Для std::shuffle, если понадобится локальная перетасовка

ExerciseGenerator::ExerciseGenerator(ExerciseRepository *repository)
    : m_repository(repository) {
  if (!m_repository) {
    qCritical("ExerciseGenerator: ExerciseRepository is null!");
  }
}

// Изменено: добавлен параметр userIdToExcludeSolved
QList<ExerciseData>
ExerciseGenerator::generateExerciseSet(ExerciseType type, int difficultyId,
                                       const QString &languageCode, int count,
                                       int userIdToExcludeSolved) {
  QList<ExerciseData> allAvailableExercises;
  if (m_repository) {
    // Передаем userIdToExcludeSolved и count (как limit) в репозиторий
    // Репозиторий должен уметь обработать limit = -1 как "без лимита", если
    // count здесь всегда > 0 Или, если SQL не может лимитировать так, как
    // нужно, получаем больше и обрезаем здесь. Текущая реализация
    // ExerciseRepository::getExercises уже принимает limit, поэтому можно
    // передать count прямо туда, если ORDER BY RANDOM() в SQL надежен для
    // получения N случайных. Если ORDER BY RANDOM() не используется или
    // неэффективен, получаем все нерешенные и выбираем count здесь. Для
    // простоты предположим, что ExerciseRepository::getExercises вернет
    // достаточное количество (или все) нерешенных заданий, а мы здесь выберем
    // `count`.
    allAvailableExercises = m_repository->getExercises(
        type, difficultyId, languageCode, userIdToExcludeSolved,
        -1); // -1 значит, что лимит по количеству будет применен ниже
  } else {
    qWarning(
        "ExerciseGenerator: Repository not available, returning empty set.");
    return {};
  }

  if (allAvailableExercises.isEmpty()) {
    qWarning() << "No exercises found (or all solved) for type:"
               << static_cast<int>(type) << "difficulty:" << difficultyId
               << "language:" << languageCode
               << "excluding solved for user:" << userIdToExcludeSolved;
    return {};
  }

  // Если ExerciseRepository::getExercises не использует ORDER BY RANDOM(), то
  // нужно перетасовать здесь std::shuffle(allAvailableExercises.begin(),
  // allAvailableExercises.end(), *QRandomGenerator::global());

  QList<ExerciseData> selectedExercises;
  for (int i = 0; i < qMin(count, allAvailableExercises.size()); ++i) {
    selectedExercises.append(
        allAvailableExercises.at(i)); // Берем первые 'count' после ORDER BY
                                      // RANDOM в SQL (или shuffle здесь)
  }

  qDebug() << "Generated" << selectedExercises.size() << "exercises.";
  return selectedExercises;
}
