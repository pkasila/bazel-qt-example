#ifndef STATISTICSREPOSITORY_H
#define STATISTICSREPOSITORY_H

#include <QVariantMap> // For generic stats data
#include <QList>
#include <QString> // For UserSessionRecord members

class DatabaseManager;
struct ExerciseData; // For session details

// Represents a completed session
struct UserSessionRecord {
    int sessionId = -1;
    int userId = 0; // Default
    int difficultyId = 0; // App's 0-indexed difficulty
    QString sessionStartTime; // ISO Format String
    QString sessionEndTime;   // ISO Format String
    int totalTasksInSession = 0;
    int tasksCorrectlyCompleted = 0;
    int errorsMade = 0;
    int finalScore = 0;
    bool wasSuccessful = false;
    int timeSpentSeconds = 0;
};


class StatisticsRepository {
public:
    StatisticsRepository(DatabaseManager& dbManager);

    bool saveUserSession(const UserSessionRecord& session);
    // bool saveSessionDetail(int sessionId, int exerciseId, const QString& userAnswer, bool wasCorrect); // Example
    
    QList<UserSessionRecord> getAllUserSessions(int userId = 0); // Default user

private:
    DatabaseManager& m_dbManager;
};

#endif // STATISTICSREPOSITORY_H