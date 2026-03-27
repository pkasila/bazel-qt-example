#ifndef EXERCISEREPOSITORY_H
#define EXERCISEREPOSITORY_H

#include "models/ExerciseData.h"
#include <QList>
#include <QString>
#include <QSqlQuery> // For mapQueryToExerciseData

class DatabaseManager; // Forward declaration

class ExerciseRepository {
public:
    ExerciseRepository(DatabaseManager& dbManager);

    // Fetches exercises matching criteria, optionally excluding solved ones
    QList<ExerciseData> getExercises(ExerciseType type, int difficultyId, const QString& languageCode, int userIdToExcludeSolved = -1, int limit = -1);
    ExerciseData getExerciseById(int exerciseId);
    
    bool markExerciseAsSolved(int userId, int exerciseId);
    QList<int> getSolvedExerciseIds(int userId);

private:
    DatabaseManager& m_dbManager;
    ExerciseData mapQueryToExerciseData(QSqlQuery& query);
};

#endif // EXERCISEREPOSITORY_H