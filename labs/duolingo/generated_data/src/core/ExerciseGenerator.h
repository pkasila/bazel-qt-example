#ifndef EXERCISEGENERATOR_H
#define EXERCISEGENERATOR_H

#include "models/ExerciseData.h"
#include <QList>
#include <QRandomGenerator>

class ExerciseRepository;

class ExerciseGenerator {
public:
  ExerciseGenerator(ExerciseRepository *repository);

  // Изменено: добавлен параметр userIdToExcludeSolved
  QList<ExerciseData> generateExerciseSet(ExerciseType type, int difficultyId,
                                          const QString &languageCode,
                                          int count = 5,
                                          int userIdToExcludeSolved = -1);

private:
  ExerciseRepository *m_repository;
};

#endif // EXERCISEGENERATOR_H
