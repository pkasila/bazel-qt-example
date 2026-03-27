#include "StatisticsRepository.h"
#include "DatabaseManager.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QVariant>
#include <QDebug> // For logging

StatisticsRepository::StatisticsRepository(DatabaseManager& dbManager) : m_dbManager(dbManager) {}

bool StatisticsRepository::saveUserSession(const UserSessionRecord& session) {
    if (!m_dbManager.isConnected()) return false;

    QSqlQuery query(m_dbManager.database());
    query.prepare("INSERT INTO UserExerciseSessions "
                  "(user_id, difficulty_id, session_start_time, session_end_time, "
                  "total_tasks_in_session, tasks_correctly_completed, errors_made, "
                  "final_score, was_successful, time_spent_seconds) "
                  "VALUES (:user_id, :db_difficulty_id, :start_time, :end_time, " // Use db_difficulty_id
                  ":total_tasks, :correct_tasks, :errors, :score, :successful, :time_spent)");
    
    query.bindValue(":user_id", session.userId);
    
    // Convert app's 0-indexed difficulty to DB's 1-indexed difficulty_id
    // This requires knowing the mapping or querying DifficultyLevels table by level_name
    // For simplicity, assume 0->1, 1->2, 2->3 for now
    int dbDifficultyId = session.difficultyId + 1; 
    query.bindValue(":db_difficulty_id", dbDifficultyId);

    query.bindValue(":start_time", session.sessionStartTime);
    query.bindValue(":end_time", session.sessionEndTime);
    query.bindValue(":total_tasks", session.totalTasksInSession);
    query.bindValue(":correct_tasks", session.tasksCorrectlyCompleted);
    query.bindValue(":errors", session.errorsMade);
    query.bindValue(":score", session.finalScore);
    query.bindValue(":successful", session.wasSuccessful);
    query.bindValue(":time_spent", session.timeSpentSeconds);

    if (!m_dbManager.executeQuery(query)) {
        qWarning() << "Failed to save user session.";
        return false;
    }
    // int sessionId = query.lastInsertId().toInt(); // To save details if needed
    qInfo() << "User session saved successfully. LastInsertId:" << query.lastInsertId().toString();
    return true;
}


QList<UserSessionRecord> StatisticsRepository::getAllUserSessions(int userId) {
    QList<UserSessionRecord> sessions;
    if (!m_dbManager.isConnected()) return sessions;

    QSqlQuery query(m_dbManager.database());
    query.prepare("SELECT session_id, user_id, difficulty_id, session_start_time, session_end_time, "
                  "total_tasks_in_session, tasks_correctly_completed, errors_made, "
                  "final_score, was_successful, time_spent_seconds "
                  "FROM UserExerciseSessions WHERE user_id = :user_id ORDER BY session_start_time DESC");
    query.bindValue(":user_id", userId);

    if (m_dbManager.executeQuery(query)) {
        while (query.next()) {
            UserSessionRecord s;
            s.sessionId = query.value("session_id").toInt();
            s.userId = query.value("user_id").toInt();
            
            // Convert DB's 1-indexed difficulty_id back to app's 0-indexed
            int dbDifficulty = query.value("difficulty_id").toInt();
            s.difficultyId = dbDifficulty - 1; 

            s.sessionStartTime = query.value("session_start_time").toString();
            s.sessionEndTime = query.value("session_end_time").toString();
            s.totalTasksInSession = query.value("total_tasks_in_session").toInt();
            s.tasksCorrectlyCompleted = query.value("tasks_correctly_completed").toInt();
            s.errorsMade = query.value("errors_made").toInt();
            s.finalScore = query.value("final_score").toInt();
            s.wasSuccessful = query.value("was_successful").toBool();
            s.timeSpentSeconds = query.value("time_spent_seconds").toInt();
            sessions.append(s);
        }
    } else {
        qWarning() << "Failed to fetch user sessions.";
    }
    return sessions;
}