#include "ExerciseRepository.h"
#include "DatabaseManager.h"
#include <QSqlError>
#include <QVariant>
#include <QJsonDocument>
#include <QJsonObject>
#include <QDebug>


ExerciseRepository::ExerciseRepository(DatabaseManager& dbManager) : m_dbManager(dbManager) {}

QList<ExerciseData> ExerciseRepository::getExercises(ExerciseType type, int difficultyId, const QString& languageCode, int userIdToExcludeSolved, int limit) {
    QList<ExerciseData> exercises;
    if (!m_dbManager.isConnected()) return exercises;

    QSqlQuery query(m_dbManager.database());
    QString queryString = 
        "SELECT e.exercise_id, e.type_id, e.language_id, e.difficulty_id, "
        "e.original_text, e.correct_answer_text, e.hint_text, e.additional_data_json "
        "FROM Exercises e "
        "JOIN Languages l ON e.language_id = l.language_id "
        "JOIN DifficultyLevels dl ON e.difficulty_id = dl.difficulty_id "
        "JOIN ExerciseTypes et ON e.type_id = et.type_id "
        "WHERE et.type_name = :type_name AND dl.level_name = :difficulty_name AND l.language_code = :lang_code ";

    if (userIdToExcludeSolved != -1) {
        queryString += "AND e.exercise_id NOT IN (SELECT usex.exercise_id FROM UserSolvedExercises usex WHERE usex.user_id = :user_id) ";
    }
    queryString += "ORDER BY RANDOM() "; 
    if (limit > 0) {
        queryString += "LIMIT :limit";
    }

    query.prepare(queryString);
    
    QString typeName;
    switch(type) {
        case ExerciseType::Translation: typeName = "Translation"; break;
        case ExerciseType::Grammar: typeName = "Grammar"; break;
        default: qWarning() << "Unknown exercise type for DB query:" << static_cast<int>(type); return exercises;
    }
    query.bindValue(":type_name", typeName);

    QString difficultyName; 
    if (difficultyId == 0) difficultyName = "Easy";
    else if (difficultyId == 1) difficultyName = "Medium";
    else if (difficultyId == 2) difficultyName = "Hard";
    else { qWarning() << "Invalid difficultyId from app:" << difficultyId; return exercises; }
    query.bindValue(":difficulty_name", difficultyName);
    
    query.bindValue(":lang_code", languageCode);
    if (userIdToExcludeSolved != -1) {
        query.bindValue(":user_id", userIdToExcludeSolved);
    }
    if (limit > 0) {
        query.bindValue(":limit", limit);
    }

    if (m_dbManager.executeQuery(query)) {
        while (query.next()) {
            exercises.append(mapQueryToExerciseData(query));
        }
    } else {
        qWarning() << "getExercises query failed.";
    }
    return exercises;
}

ExerciseData ExerciseRepository::getExerciseById(int exerciseId) {
    ExerciseData exercise;
    exercise.id = -1; 
    if (!m_dbManager.isConnected()) return exercise;

    QSqlQuery query(m_dbManager.database());
    query.prepare("SELECT e.exercise_id, e.type_id, e.language_id, e.difficulty_id, "
                  "e.original_text, e.correct_answer_text, e.hint_text, e.additional_data_json "
                  "FROM Exercises e "
                  "WHERE e.exercise_id = :id");
    query.bindValue(":id", exerciseId);

    if (m_dbManager.executeQuery(query)) {
        if (query.next()) {
            exercise = mapQueryToExerciseData(query);
        }
    }
    return exercise;
}

ExerciseData ExerciseRepository::mapQueryToExerciseData(QSqlQuery& query) {
    ExerciseData data;
    data.id = query.value("exercise_id").toInt();
    
    int dbTypeId = query.value("type_id").toInt();
    if (dbTypeId == 1) data.type = ExerciseType::Translation;
    else if (dbTypeId == 2) data.type = ExerciseType::Grammar;

    int dbDifficultyId = query.value("difficulty_id").toInt();
    if (dbDifficultyId == 1) data.difficultyId = 0;      
    else if (dbDifficultyId == 2) data.difficultyId = 1; 
    else if (dbDifficultyId == 3) data.difficultyId = 2; 

    data.originalText = query.value("original_text").toString();
    data.correctAnswerText = query.value("correct_answer_text").toString();
    data.hintText = query.value("hint_text").toString();

    QString jsonString = query.value("additional_data_json").toString();
    if (!jsonString.isEmpty() && jsonString.toLower() != "null") {
        QJsonDocument doc = QJsonDocument::fromJson(jsonString.toUtf8());
        if (!doc.isNull() && doc.isObject()) {
            data.additionalData = doc.object().toVariantMap();
            if (data.type == ExerciseType::Grammar && data.additionalData.contains("options")) {
                QVariantList optionsVariantList = data.additionalData.value("options").toList();
                for(const QVariant& v : optionsVariantList) {
                    data.grammarOptions.append(v.toString());
                }
            }
        } else {
            qWarning() << "Failed to parse additional_data_json for exercise_id:" << data.id << jsonString;
        }
    }
    return data;
}

bool ExerciseRepository::markExerciseAsSolved(int userId, int exerciseId) {
    if (!m_dbManager.isConnected()) return false;
    QSqlQuery query(m_dbManager.database());
    query.prepare("INSERT OR IGNORE INTO UserSolvedExercises (user_id, exercise_id, solved_timestamp) VALUES (:user_id, :exercise_id, CURRENT_TIMESTAMP)");
    query.bindValue(":user_id", userId);
    query.bindValue(":exercise_id", exerciseId);
    return m_dbManager.executeQuery(query);
}

QList<int> ExerciseRepository::getSolvedExerciseIds(int userId) {
    QList<int> solvedIds;
    if (!m_dbManager.isConnected()) return solvedIds;

    QSqlQuery query(m_dbManager.database());
    query.prepare("SELECT exercise_id FROM UserSolvedExercises WHERE user_id = :user_id");
    query.bindValue(":user_id", userId);
    
    if (m_dbManager.executeQuery(query)) {
        while (query.next()) {
            solvedIds.append(query.value(0).toInt());
        }
    }
    return solvedIds;
}
