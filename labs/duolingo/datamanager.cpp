#include "datamanager.h"

#include <QCoreApplication>
#include <QCryptographicHash>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QMap>
#include <QMetaType>
#include <QRandomGenerator>
#include <QSqlError>
#include <QSqlQuery>
#include <QVariant>

DataManager& DataManager::Instance() {
    static DataManager dmInstance;
    return dmInstance;
}

DataManager::DataManager(QObject* parent)
    : QObject(parent), currentUserId_(-1) {}

DataManager::~DataManager() { CloseDb(); }

QString DataManager::GetLastErrorString() const { return lastErrorString_; }

bool DataManager::OpenDb(const QString& dbPath) {
    if (db_.isOpen() && db_.databaseName() == dbPath) {
        return true;
    }
    if (db_.isOpen()) {
        CloseDb();
    }

    QFileInfo dbFileInfo(dbPath);
    QDir dbDir(dbFileInfo.absolutePath());
    if (!dbDir.exists()) {
        if (!dbDir.mkpath(".")) {
            lastErrorString_ = "Не удалось создать директорию для базы данных: " +
                               dbDir.absolutePath();
            emit DatabaseError(lastErrorString_);
            return false;
        }
    }

    if (!QSqlDatabase::isDriverAvailable("QSQLITE")) {
        lastErrorString_ = "Драйвер SQLite (QSQLITE) недоступен.";
        emit DatabaseError(lastErrorString_);
        return false;
    }

    const QString connectionName = QStringLiteral("DuolingoAppConnection_%1")
                                     .arg(QDateTime::currentMSecsSinceEpoch());
    if (QSqlDatabase::contains(connectionName)) {
        db_ = QSqlDatabase::database(connectionName);
    } else {
        db_ = QSqlDatabase::addDatabase("QSQLITE", connectionName);
    }
    db_.setDatabaseName(dbPath);

    if (!db_.open()) {
        const QSqlError err = db_.lastError();
        QString errorText = err.text().trimmed();
        if (errorText.isEmpty() && err.type() != QSqlError::NoError) {
            errorText =
                QString("SQL Error Type %1 (driver text: %2, native code: %3)")
                    .arg(err.type())
                    .arg(err.driverText().trimmed())
                    .arg(err.nativeErrorCode().trimmed());
        } else if (errorText.isEmpty()) {
            errorText = "Неизвестная ошибка при открытии базы данных.";
        }
        lastErrorString_ =
            QString("Не удалось открыть базу данных '%1': %2")
                .arg(dbPath, errorText);
        emit DatabaseError(lastErrorString_);
        return false;
    }

    QSqlQuery pragmaQuery(db_);
    if (!pragmaQuery.exec("PRAGMA foreign_keys = ON;")) {
        lastErrorString_ =
            "Не удалось включить внешние ключи: " + pragmaQuery.lastError().text();
        emit DatabaseError(lastErrorString_);
    }

    if (!InitSchema()) {
        CloseDb();
        return false;
    }

    QString populatedFlagPath = QCoreApplication::applicationDirPath() +
                                QDir::separator() + dbFileInfo.baseName() +
                                "-populated.flag";
    if (!QFile(populatedFlagPath).exists()) {
        if (PopulateInitialData()) {
            QFile populatedFlag(populatedFlagPath);
            if (populatedFlag.open(QIODevice::WriteOnly)) {
                populatedFlag.write("populated");
                populatedFlag.close();
            }
        }
    }
    return true;
}

void DataManager::CloseDb() const {
    if (db_.isOpen()) {
        QString connectionName = db_.connectionName();
        db_.close();
        QSqlDatabase::removeDatabase(connectionName);
    }
}

bool DataManager::IsOpen() const { return db_.isOpen(); }

bool DataManager::InitSchema() {
    if (!IsOpen()) {
        lastErrorString_ = "DataManager::InitSchema: База данных не открыта.";
        emit DatabaseError(lastErrorString_);
        return false;
    }
    QSqlQuery query(db_);
    QStringList createTableStatements = {
        "CREATE TABLE IF NOT EXISTS users ("
        "id INTEGER PRIMARY KEY AUTOINCREMENT, "
        "name TEXT NOT NULL UNIQUE, "
        "email TEXT UNIQUE, "
        "password_hash TEXT NOT NULL, "
        "salt TEXT NOT NULL, "
        "created_at TEXT DEFAULT CURRENT_TIMESTAMP, "
        "total_study_time_seconds INTEGER DEFAULT 0, "
        "completed_exercises_count INTEGER DEFAULT 0"
        ");",
        "CREATE TABLE IF NOT EXISTS lessons ("
        "id INTEGER PRIMARY KEY AUTOINCREMENT, "
        "name TEXT NOT NULL UNIQUE, "
        "description TEXT, "
        "difficulty INTEGER NOT NULL DEFAULT 1"
        ");",
        "CREATE TABLE IF NOT EXISTS questions ("
        "id INTEGER PRIMARY KEY AUTOINCREMENT, "
        "lesson_id INTEGER NOT NULL, "
        "type INTEGER NOT NULL, "
        "text TEXT NOT NULL, "
        "correct_answer TEXT NOT NULL, "
        "options TEXT, "
        "audio_path TEXT, "
        "hint TEXT, "
        "difficulty_order INTEGER DEFAULT 0, "
        "FOREIGN KEY (lesson_id) REFERENCES lessons (id) ON DELETE CASCADE"
        ");",
        "CREATE TABLE IF NOT EXISTS user_lesson_progress ("
        "user_id INTEGER NOT NULL, "
        "lesson_id INTEGER NOT NULL, "
        "stars_translation INTEGER DEFAULT 0, "
        "stars_grammar INTEGER DEFAULT 0, "
        "stars_audio INTEGER DEFAULT 0, "
        "PRIMARY KEY (user_id, lesson_id), "
        "FOREIGN KEY (user_id) REFERENCES users (id) ON DELETE CASCADE, "
        "FOREIGN KEY (lesson_id) REFERENCES lessons (id) ON DELETE CASCADE"
        ");"};

    if (!db_.transaction()) {
        lastErrorString_ =
            QString(
                "DataManager: Не удалось начать транзакцию для инициализации схемы: %1")
                .arg(db_.lastError().text());
        emit DatabaseError(lastErrorString_);
        return false;
    }
    bool success = true;
    for (const QString& stmt : createTableStatements) {
        if (!query.exec(stmt)) {
            lastErrorString_ =
                QString(
                    "DataManager: Не удалось выполнить оператор схемы: %1\nЗапрос: %2")
                    .arg(query.lastError().text(), stmt);
            emit DatabaseError(lastErrorString_);
            success = false;
            break;
        }
    }
    if (success) {
        if (!db_.commit()) {
            lastErrorString_ =
                QString("DataManager: Не удалось подтвердить транзакцию схемы: %1")
                    .arg(db_.lastError().text());
            emit DatabaseError(lastErrorString_);
            success = false;
        }
    }
    if (!success) {
        db_.rollback();
    }
    return success;
}

long long DataManager::AddUser(const User& userData, const QString& password) {
    if (!IsOpen()) {
        lastErrorString_ = "База данных не открыта для AddUser";
        emit DatabaseError(lastErrorString_);
        return -1;
    }
    if (userData.name_.isEmpty() || password.isEmpty()) {
        lastErrorString_ =
            "Имя пользователя или пароль не могут быть пустыми для AddUser";
        emit DatabaseError(lastErrorString_);
        return -1;
    }
    if (UserExists(userData.name_)) {
        lastErrorString_ =
            "Пользователь с таким именем уже существует: " + userData.name_;
        return -1;
    }

    QByteArray saltBytes(16, 0);
    QRandomGenerator::global()->generate(saltBytes.begin(), saltBytes.end());
    const QString saltHex = QString::fromUtf8(saltBytes.toHex());

    const QByteArray passwordBytes = password.toUtf8();
    const QByteArray saltedPassword = saltBytes + passwordBytes;
    const QByteArray hashBytes =
        QCryptographicHash::hash(saltedPassword, QCryptographicHash::Sha256);
    const QString passwordHashHex = QString::fromUtf8(hashBytes.toHex());

    QSqlQuery query(db_);
    query.prepare(
        "INSERT INTO users (name, email, password_hash, salt, created_at, "
        "total_study_time_seconds, completed_exercises_count) "
        "VALUES (:name, :email, :password_hash, :salt, :created_at, 0, 0)");
    query.bindValue(":name", userData.name_);
    query.bindValue(":email",
                    userData.email_.isEmpty() ? QVariant() : userData.email_);
    query.bindValue(":password_hash", passwordHashHex);
    query.bindValue(":salt", saltHex);
    query.bindValue(":created_at",
                    QDateTime::currentDateTime().toString(Qt::ISODate));

    if (!query.exec()) {
        lastErrorString_ =
            QString("DataManager: Не удалось добавить пользователя '%1': %2")
                .arg(userData.name_, query.lastError().text());
        emit DatabaseError(lastErrorString_);
        return -1;
    }
    return query.lastInsertId().toLongLong();
}

bool DataManager::UserExists(const QString& username) const {
    if (!IsOpen() || username.isEmpty()) return false;
    QSqlQuery query(db_);
    query.prepare("SELECT id FROM users WHERE name = :name");
    query.bindValue(":name", username);
    if (query.exec() && query.next()) return true;
    return false;
}

User DataManager::GetUserByUsername(const QString& username) const {
    User user;
    if (!IsOpen() || username.isEmpty()) return user;
    QSqlQuery query(db_);
    query.prepare(
        "SELECT id, name, email, password_hash, salt, created_at, "
        "total_study_time_seconds, completed_exercises_count FROM users WHERE "
        "name = :name");
    query.bindValue(":name", username);
    if (query.exec() && query.next()) {
        user.id_ = query.value("id").toLongLong();
        user.name_ = query.value("name").toString();
        user.email_ = query.value("email").toString();
        user.passwordHash_ = query.value("password_hash").toString();
        user.salt_ = query.value("salt").toString();
        user.createdAt_ =
            QDateTime::fromString(query.value("created_at").toString(), Qt::ISODate);
        user.totalStudyTimeSeconds_ =
            query.value("total_study_time_seconds").toLongLong();
        user.completedExercisesCount_ =
            query.value("completed_exercises_count").toInt();
    }
    return user;
}

User DataManager::GetUserById(long long userId) const {
    User user;
    if (!IsOpen() || userId <= 0) return user;
    QSqlQuery query(db_);
    query.prepare(
        "SELECT id, name, email, password_hash, salt, created_at, "
        "total_study_time_seconds, completed_exercises_count FROM users WHERE id "
        "= :id");
    query.bindValue(":id", userId);
    if (query.exec() && query.next()) {
        user.id_ = query.value("id").toLongLong();
        user.name_ = query.value("name").toString();
        user.email_ = query.value("email").toString();
        user.passwordHash_ = query.value("password_hash").toString();
        user.salt_ = query.value("salt").toString();
        user.createdAt_ =
            QDateTime::fromString(query.value("created_at").toString(), Qt::ISODate);
        user.totalStudyTimeSeconds_ =
            query.value("total_study_time_seconds").toLongLong();
        user.completedExercisesCount_ =
            query.value("completed_exercises_count").toInt();
    }
    return user;
}

bool DataManager::CheckPassword(long long userId, const QString& password) {
    if (!IsOpen() || userId <= 0 || password.isEmpty()) return false;
    User user = GetUserById(userId);
    if (!user.IsValid() || user.salt_.isEmpty() ||
        user.passwordHash_.isEmpty())
        return false;

    QByteArray saltBytes = QByteArray::fromHex(user.salt_.toUtf8());
    QByteArray passwordBytes = password.toUtf8();
    QByteArray saltedPassword = saltBytes + passwordBytes;
    const QByteArray currentHashBytes =
        QCryptographicHash::hash(saltedPassword, QCryptographicHash::Sha256);
    QString currentHashHex = QString::fromUtf8(currentHashBytes.toHex());

    return (currentHashHex == user.passwordHash_);
}

bool DataManager::ChangePassword(long long userId,
                                 const QString& newPassword) {
    if (!IsOpen() || userId <= 0 || newPassword.isEmpty()) {
        lastErrorString_ = "Неверные параметры для смены пароля.";
        emit DatabaseError(lastErrorString_);
        return false;
    }

    QByteArray saltBytes(16, 0);
    QRandomGenerator::global()->generate(saltBytes.begin(), saltBytes.end());
    const QString saltHex = QString::fromUtf8(saltBytes.toHex());

    const QByteArray passwordBytes = newPassword.toUtf8();
    const QByteArray saltedPassword = saltBytes + passwordBytes;
    const QByteArray hashBytes =
        QCryptographicHash::hash(saltedPassword, QCryptographicHash::Sha256);
    const QString passwordHashHex = QString::fromUtf8(hashBytes.toHex());

    QSqlQuery query(db_);
    query.prepare(
        "UPDATE users SET password_hash = :password_hash, salt = :salt WHERE id "
        "= :id");
    query.bindValue(":password_hash", passwordHashHex);
    query.bindValue(":salt", saltHex);
    query.bindValue(":id", userId);

    if (!query.exec()) {
        lastErrorString_ =
            QString("Не удалось сменить пароль для пользователя ID %1: %2")
                .arg(userId)
                .arg(query.lastError().text());
        emit DatabaseError(lastErrorString_);
        return false;
    }
    return query.numRowsAffected() > 0;
}

void DataManager::SetCurrentUserId(long long userId) {
    if (currentUserId_ != userId) {
        currentUserId_ = userId;
        emit CurrentUserChanged(currentUserId_);
    }
}

long long DataManager::GetCurrentUserId() const { return currentUserId_; }

User DataManager::GetCurrentUser() const {
    if (currentUserId_ > 0) return GetUserById(currentUserId_);
    return User();
}

QList<Lesson> DataManager::GetAllLessonsSorted() {
    QList<Lesson> lessons;
    if (!IsOpen()) return lessons;
    QSqlQuery query(
        "SELECT id, name, description, difficulty FROM lessons ORDER BY "
        "difficulty ASC, name ASC",
        db_);
    if (query.exec()) {
        while (query.next()) {
            Lesson l;
            l.id = query.value("id").toLongLong();
            l.name = query.value("name").toString();
            l.description = query.value("description").toString();
            l.difficulty = query.value("difficulty").toInt();
            lessons.append(l);
        }
    } else {
        lastErrorString_ =
            QString("DataManager: Не удалось получить все уроки: %1")
                .arg(query.lastError().text());
        emit DatabaseError(lastErrorString_);
    }
    return lessons;
}

Lesson DataManager::GetLessonById(long long lessonId) {
    Lesson lesson;
    if (!IsOpen() || lessonId <= 0) return lesson;
    QSqlQuery query(db_);
    query.prepare(
        "SELECT id, name, description, difficulty FROM lessons WHERE id = :id");
    query.bindValue(":id", lessonId);
    if (query.exec() && query.next()) {
        lesson.id = query.value("id").toLongLong();
        lesson.name = query.value("name").toString();
        lesson.description = query.value("description").toString();
        lesson.difficulty = query.value("difficulty").toInt();
    } else if (query.lastError().isValid()) {
        lastErrorString_ =
            QString("DataManager: Не удалось получить урок по ID %1: %2")
                .arg(lessonId)
                .arg(query.lastError().text());
        emit DatabaseError(lastErrorString_);
    }
    return lesson;
}

QList<Question> DataManager::GetQuestionsForLesson(long long lessonId,
                                                  ExerciseType type,
                                                  int count) {
    QList<Question> questions;
    if (!IsOpen() || lessonId <= 0) return questions;

    QSqlQuery query(db_);
    QString queryString =
        "SELECT id, lesson_id, type, text, correct_answer, options, "
        "audio_path, hint, difficulty_order "
        "FROM questions WHERE lesson_id = :lesson_id ";
    if (type != ExerciseType::None) {
        queryString += "AND type = :type ";
    }
    queryString += "ORDER BY difficulty_order ASC, RANDOM() ";
    if (count > 0) {
        queryString += "LIMIT :count";
    }

    query.prepare(queryString);
    query.bindValue(":lesson_id", lessonId);
    if (type != ExerciseType::None) {
        query.bindValue(":type", static_cast<int>(type));
    }
    if (count > 0) {
        query.bindValue(":count", count);
    }

    if (query.exec()) {
        while (query.next()) {
            Question q;
            q.id = query.value("id").toLongLong();
            q.lessonId = query.value("lesson_id").toLongLong();
            q.type = static_cast<ExerciseType>(query.value("type").toInt());
            q.text = query.value("text").toString();
            q.correctAnswer = query.value("correct_answer").toString();

            QVariant optionsVariant = query.value("options");
            if (!optionsVariant.isNull()) {
                QString optionsJson = optionsVariant.toString();
                if (!optionsJson.isEmpty() && optionsJson.startsWith('[') &&
                    optionsJson.endsWith(']')) {
                    optionsJson = optionsJson.mid(1, optionsJson.length() - 2);
                    if (!optionsJson.isEmpty()) {
                        QStringList opts =
                            optionsJson.split("\",\"", Qt::SkipEmptyParts);
                        for (const QString& opt : opts) {
                            QString cleanedOpt = opt;
                            if (cleanedOpt.startsWith('\"')) cleanedOpt.remove(0, 1);
                            if (cleanedOpt.endsWith('\"')) cleanedOpt.chop(1);
                            q.options.append(cleanedOpt);
                        }
                    }
                }
            }
            q.audioPath = query.value("audio_path").toString();
            q.hint = query.value("hint").toString();
            q.difficultyOrder = query.value("difficulty_order").toInt();
            questions.append(q);
        }
    } else {
        lastErrorString_ =
            QString("DataManager: Не удалось получить вопросы для урока %1: %2")
                .arg(lessonId)
                .arg(query.lastError().text());
        emit DatabaseError(lastErrorString_);
    }
    return questions;
}

UserLessonProgress DataManager::GetUserProgressForLesson(long long userId,
                                                         long long lessonId) {
    UserLessonProgress progress;
    progress.userId = userId;
    progress.lessonId = lessonId;
    if (!IsOpen() || userId <= 0 || lessonId <= 0) return progress;

    QSqlQuery query(db_);
    query.prepare(
        "SELECT stars_translation, stars_grammar, stars_audio FROM "
        "user_lesson_progress "
        "WHERE user_id = :user_id AND lesson_id = :lesson_id");
    query.bindValue(":user_id", userId);
    query.bindValue(":lesson_id", lessonId);

    if (query.exec() && query.next()) {
        progress.starsTranslation = query.value("stars_translation").toInt();
        progress.starsGrammar = query.value("stars_grammar").toInt();
        progress.starsAudio = query.value("stars_audio").toInt();
    }
    return progress;
}

bool DataManager::UpdateUserProgressForLesson(
    const UserLessonProgress& progress) {
    if (!IsOpen() || progress.userId <= 0 || progress.lessonId <= 0)
        return false;

    QSqlQuery query(db_);
    query.prepare(
        "INSERT OR REPLACE INTO user_lesson_progress (user_id, lesson_id, "
        "stars_translation, stars_grammar, stars_audio) "
        "VALUES (:user_id, :lesson_id, :stars_translation, :stars_grammar, "
        ":stars_audio)");
    query.bindValue(":user_id", progress.userId);
    query.bindValue(":lesson_id", progress.lessonId);
    query.bindValue(":stars_translation",
                    qBound(0, progress.starsTranslation, 1));
    query.bindValue(":stars_grammar", qBound(0, progress.starsGrammar, 1));
    query.bindValue(":stars_audio", qBound(0, progress.starsAudio, 1));

    if (!query.exec()) {
        lastErrorString_ = QString(
            "DataManager: Не удалось обновить прогресс пользователя для урока %1: %2")
                               .arg(progress.lessonId)
                               .arg(query.lastError().text());
        emit DatabaseError(lastErrorString_);
        return false;
    }
    return true;
}

QMap<long long, UserLessonProgress> DataManager::GetAllUserProgress(
    long long userId) {
    QMap<long long, UserLessonProgress> allProgress;
    if (!IsOpen() || userId <= 0) return allProgress;

    QSqlQuery query(db_);
    query.prepare(
        "SELECT lesson_id, stars_translation, stars_grammar, stars_audio FROM "
        "user_lesson_progress WHERE user_id = :user_id");
    query.bindValue(":user_id", userId);

    if (query.exec()) {
        while (query.next()) {
            UserLessonProgress p;
            p.userId = userId;
            p.lessonId = query.value("lesson_id").toLongLong();
            p.starsTranslation = query.value("stars_translation").toInt();
            p.starsGrammar = query.value("stars_grammar").toInt();
            p.starsAudio = query.value("stars_audio").toInt();
            allProgress.insert(p.lessonId, p);
        }
    } else {
        lastErrorString_ = QString(
            "DataManager: Не удалось получить весь прогресс пользователя %1: %2")
                               .arg(userId)
                               .arg(query.lastError().text());
        emit DatabaseError(lastErrorString_);
    }
    return allProgress;
}

bool DataManager::UpdateUserStudyTime(long long userId,
                                      long long additionalTimeSeconds) {
    if (!IsOpen() || userId <= 0 || additionalTimeSeconds < 0) return false;
    if (additionalTimeSeconds == 0) return true;

    QSqlQuery query(db_);
    query.prepare(
        "UPDATE users SET total_study_time_seconds = "
        "total_study_time_seconds + :time WHERE id = :id");
    query.bindValue(":time", additionalTimeSeconds);
    query.bindValue(":id", userId);

    if (!query.exec()) {
        lastErrorString_ =
            QString(
                "DataManager: Не удалось обновить время обучения для пользователя %1: %2")
                .arg(userId)
                .arg(query.lastError().text());
        emit DatabaseError(lastErrorString_);
        return false;
    }
    return true;
}

bool DataManager::IncrementUserCompletedExercises(long long userId) {
    if (!IsOpen() || userId <= 0) return false;
    QSqlQuery query(db_);
    query.prepare(
        "UPDATE users SET completed_exercises_count = "
        "completed_exercises_count + 1 WHERE id = :id");
    query.bindValue(":id", userId);
    if (!query.exec()) {
        lastErrorString_ = QString(
            "DataManager: Не удалось увеличить счетчик выполненных упражнений для "
            "пользователя %1: %2")
                               .arg(userId)
                               .arg(query.lastError().text());
        emit DatabaseError(lastErrorString_);
        return false;
    }
    return true;
}

long long DataManager::addLessonInternal(const QString& name,
                                         const QString& description,
                                         int difficulty) {
    QSqlQuery query(db_);
    query.prepare(
        "INSERT INTO lessons (name, description, difficulty) VALUES (:name, "
        ":desc, :diff)");
    query.bindValue(":name", name);
    query.bindValue(":desc", description.isEmpty() ? QVariant() : description);
    query.bindValue(":diff", difficulty);
    if (!query.exec()) {
        return -1;
    }
    return query.lastInsertId().toLongLong();
}

bool DataManager::addQuestionInternal(long long lessonId, ExerciseType type,
                                      const QString& text,
                                      const QString& correctAnswer,
                                      const QList<QString>& options,
                                      const QString& audioPath,
                                      const QString& hint,
                                      int difficultyOrder) {
    QSqlQuery query(db_);
    query.prepare(
        "INSERT INTO questions (lesson_id, type, text, correct_answer, "
        "options, audio_path, hint, difficulty_order) "
        "VALUES (:lesson_id, :type, :text, :correct_answer, :options, "
        ":audio_path, :hint, :diff_order)");
    query.bindValue(":lesson_id", lessonId);
    query.bindValue(":type", static_cast<int>(type));
    query.bindValue(":text", text);
    query.bindValue(":correct_answer", correctAnswer);

    QString optionsJsonString = "[]";
    if (!options.isEmpty()) {
        optionsJsonString = "[";
        for (int i = 0; i < options.size(); ++i) {
            QString currentOption = options[i];
            optionsJsonString += "\"" + currentOption.replace("\"", "\\\"") + "\"";
            if (i < options.size() - 1) optionsJsonString += ",";
        }
        optionsJsonString += "]";
    }
    query.bindValue(":options", optionsJsonString);
    query.bindValue(":audio_path", audioPath.isEmpty() ? QVariant() : audioPath);
    query.bindValue(":hint", hint.isEmpty() ? QVariant() : hint);
    query.bindValue(":diff_order", difficultyOrder);

    if (!query.exec()) {
        return false;
    }
    return true;
}

bool DataManager::PopulateInitialData() {
    if (!IsOpen()) {
        lastErrorString_ = "PopulateInitialData: База данных не открыта.";
        emit DatabaseError(lastErrorString_);
        return false;
    }
    if (!db_.transaction()) {
        lastErrorString_ = "PopulateInitialData: Не удалось начать транзакцию.";
        emit DatabaseError(lastErrorString_);
        return false;
    }
    bool success = true;
    auto createLesson =
        [&](const QString& name, const QString& desc, int diff,
            const QList<QList<QVariant>>& questionsData) {
            if (!success) return;
            long long lessonId = addLessonInternal(name, desc, diff);
            if (lessonId <= 0) {
                success = false;
                lastErrorString_ =
                    "PopulateInitialData: Не удалось создать урок '" + name +
                    "'. Ошибка БД: " + db_.lastError().text();
                emit DatabaseError(lastErrorString_);
                return;
            }
            for (const auto& qData : questionsData) {
                if (!success) break;
                if (qData.size() < 7) {
                    continue;
                }
                QStringList currentOptions;
                if (qData[3].canConvert<QStringList>()) {
                    currentOptions = qData[3].toStringList();
                } else if (qData[3].typeId() == QMetaType::QString) {
                    if (!qData[3].toString().isEmpty()) {
                        currentOptions.append(qData[3].toString());
                    }
                }

                success &= addQuestionInternal(
                    lessonId, static_cast<ExerciseType>(qData[0].toInt()),
                    qData[1].toString(), qData[2].toString(), currentOptions,
                    qData[4].toString(), qData[5].toString(), qData[6].toInt());
                if (!success) {
                    lastErrorString_ =
                        "PopulateInitialData: Не удалось добавить вопрос к уроку '" +
                        name + "'. Ошибка БД: " + db_.lastError().text();
                    emit DatabaseError(lastErrorString_);
                }
            }
    };

    // Урок 1
  createLesson("Базовые слова 1 (En)", "Приветствия, простые слова.", 1,
               QList<QList<QVariant>>{
                   {static_cast<int>(ExerciseType::Translation), "Собака", "Dog", QStringList{}, QString("qrc:/audio/dog.mp3"), "Лает.", 1},
                   {static_cast<int>(ExerciseType::Translation), "Кошка", "Cat", QStringList{}, QString("qrc:/audio/cat.mp3"), "Мяукает.", 2},
                   {static_cast<int>(ExerciseType::Translation), "Дом", "House", QStringList{}, QString(), "Здание для жилья.", 3},
                   {static_cast<int>(ExerciseType::Grammar), "This ___ an apple.", "is", QStringList{"is", "are", "am"}, QString(), "Форма 'to be'.", 4},
                   {static_cast<int>(ExerciseType::Audio), "", "Man", QStringList{}, QString("qrc:/audio/man.mp3"), "Прослушайте и напечатайте.", 5}
               });
  // Урок 2
  createLesson("Базовые слова 2 (En)", "Еда, напитки.", 1,
               QList<QList<QVariant>>{
                   {static_cast<int>(ExerciseType::Translation), "Книга", "Book", QStringList{}, QString(), "Для чтения.", 1},
                   {static_cast<int>(ExerciseType::Translation), "Вода", "Water", QStringList{}, QString("qrc:/audio/water.mp3"), "Жидкость.", 2},
                   {static_cast<int>(ExerciseType::Grammar), "She ___ milk.", "drinks", QStringList{"drink", "drinks", "drinking"}, QString(), "Она (действие).", 3},
                   {static_cast<int>(ExerciseType::Audio), "", "Woman", QStringList{}, QString("qrc:/audio/woman.mp3"), "Прослушайте и напечатайте.", 4},
                   {static_cast<int>(ExerciseType::Translation), "Яблоко", "Apple", QStringList{}, QString(), "Фрукт.", 5}
               });
  // Урок 3
  createLesson("Цвета (En)", "Основные цвета.", 2,
               QList<QList<QVariant>>{
                   {static_cast<int>(ExerciseType::Translation), "Красный", "Red", QStringList{}, QString(), "Цвет крови.", 1},
                   {static_cast<int>(ExerciseType::Translation), "Синий", "Blue", QStringList{}, QString("qrc:/audio/blue.mp3"), "Цвет неба.", 2},
                   {static_cast<int>(ExerciseType::Grammar), "The grass is ___.", "green", QStringList{"green", "blue", "red"}, QString(), "Цвет травы.", 3},
                   {static_cast<int>(ExerciseType::Audio), "", "Dog", QStringList{}, QString("qrc:/audio/dog.mp3"), "Друг человека.", 4}
               });
  // Урок 4
  createLesson("Семья (En)", "Члены семьи.", 2,
               QList<QList<QVariant>>{
                   {static_cast<int>(ExerciseType::Translation), "Мама", "Mother", QStringList{}, QString("qrc:/audio/mother.mp3"), "", 1},
                   {static_cast<int>(ExerciseType::Translation), "Папа", "Father", QStringList{}, QString(), "", 2},
                   {static_cast<int>(ExerciseType::Grammar), "He is my ___.", "father", QStringList{"mother", "sister", "father"}, QString(), "Мужской член семьи.", 3},
                   {static_cast<int>(ExerciseType::Audio), "", "Sister", QStringList{}, QString("qrc:/audio/sister.mp3"), "Прослушайте и напечатайте.", 4}
               });
  // Урок 5
  createLesson("Еда и напитки 1 (En)", "Больше еды.", 3,
               QList<QList<QVariant>>{
                   {static_cast<int>(ExerciseType::Translation), "Хлеб", "Bread", QStringList{}, QString(), "", 1},
                   {static_cast<int>(ExerciseType::Translation), "Молоко", "Milk", QStringList{}, QString("qrc:/audio/milk.mp3"), "", 2},
                   {static_cast<int>(ExerciseType::Grammar), "I ___ bread and milk.", "eat", QStringList{"eat", "drinks", "eats"}, QString(), "Я (действие).", 3},
                   {static_cast<int>(ExerciseType::Audio), "", "Cheese", QStringList{}, QString("qrc:/audio/cheese.mp3"), "Молочный продукт.", 4}
               });
  // Урок 6
  createLesson("Животные 1 (En)", "Домашние животные.", 3,
               QList<QList<QVariant>>{
                   {static_cast<int>(ExerciseType::Translation), "Лошадь", "Horse", QStringList{}, QString(), "", 1},
                   {static_cast<int>(ExerciseType::Translation), "Курица", "Chicken", QStringList{}, QString(), "", 2},
                   {static_cast<int>(ExerciseType::Grammar), "The ___ says 'oink'.", "pig", QStringList{"dog", "cat", "pig"}, QString(), "Хрюкает.", 3},
                   {static_cast<int>(ExerciseType::Audio), "", "Cow", QStringList{}, QString("qrc:/audio/cow.mp3"), "Дает молоко.", 4}
               });
  // Урок 7
  createLesson("Числа 1-5 (En)", "Считаем до пяти.", 4,
               QList<QList<QVariant>>{
                   {static_cast<int>(ExerciseType::Translation), "Один", "One", QStringList{}, QString("qrc:/audio/one.mp3"), "", 1},
                   {static_cast<int>(ExerciseType::Translation), "Три", "Three", QStringList{}, QString(), "", 2},
                   {static_cast<int>(ExerciseType::Grammar), "There are ___ fingers on one hand.", "five", QStringList{"three", "four", "five"}, QString(), "Сколько пальцев?", 3},
                   {static_cast<int>(ExerciseType::Audio), "", "Five", QStringList{}, QString("qrc:/audio/five.mp3"), "Число.", 4}
               });
  // Урок 8
  createLesson("Простые действия (En)", "Глаголы.", 4,
               QList<QList<QVariant>>{
                   {static_cast<int>(ExerciseType::Translation), "Читать", "Read", QStringList{}, QString(), "", 1},
                   {static_cast<int>(ExerciseType::Translation), "Писать", "Write", QStringList{}, QString(), "", 2},
                   {static_cast<int>(ExerciseType::Grammar), "They ___ (to play) football.", "play", QStringList{"play", "plays", "playing"}, QString(), "Они (действие).", 3},
                   {static_cast<int>(ExerciseType::Audio), "", "Run", QStringList{}, QString("qrc:/audio/run.mp3"), "Быстрое движение.", 4}
               });
  // Урок 9
  createLesson("Предметы в комнате (En)", "Мебель и вещи.", 5,
               QList<QList<QVariant>>{
                   {static_cast<int>(ExerciseType::Translation), "Стул", "Chair", QStringList{}, QString(), "", 1},
                   {static_cast<int>(ExerciseType::Translation), "Окно", "Window", QStringList{}, QString(), "", 2},
                   {static_cast<int>(ExerciseType::Grammar), "The book is ___ the table.", "on", QStringList{"in", "on", "under"}, QString(), "Предлог места.", 3},
                   {static_cast<int>(ExerciseType::Audio), "", "Pencil", QStringList{}, QString("qrc:/audio/pencil.mp3"), "Для письма.", 4}
               });
  // Урок 10
  createLesson("Дни недели (En)", "Порядок дней.", 5,
               QList<QList<QVariant>>{
                   {static_cast<int>(ExerciseType::Translation), "Понедельник", "Monday", QStringList{}, QString("qrc:/audio/monday.mp3"), "", 1},
                   {static_cast<int>(ExerciseType::Translation), "Пятница", "Friday", QStringList{}, QString(), "", 2},
                   {static_cast<int>(ExerciseType::Grammar), "Today is ___.", "Sunday", QStringList{"Monday", "Saturday", "Sunday"}, QString(), "Последний день недели.", 3},
                   {static_cast<int>(ExerciseType::Audio), "", "Sunday", QStringList{}, QString("qrc:/audio/sunday.mp3"), "Выходной.", 4}
               });
  // Урок 11
  createLesson("Месяцы (En)", "Календарь.", 6,
               QList<QList<QVariant>>{
                   {static_cast<int>(ExerciseType::Translation), "Январь", "January", QStringList{}, QString(), "", 1},
                   {static_cast<int>(ExerciseType::Translation), "Июль", "July", QStringList{}, QString(), "", 2},
                   {static_cast<int>(ExerciseType::Grammar), "My birthday is ___ May.", "in", QStringList{"on", "in", "at"}, QString(), "Предлог с месяцем.", 3},
                   {static_cast<int>(ExerciseType::Audio), "", "February", QStringList{}, QString("qrc:/audio/february.mp3"), "Короткий месяц.", 4}
               });
  // Урок 12
  createLesson("Профессии (En)", "Кем работать.", 6,
               QList<QList<QVariant>>{
                   {static_cast<int>(ExerciseType::Translation), "Учитель", "Teacher", QStringList{}, QString(), "", 1},
                   {static_cast<int>(ExerciseType::Translation), "Врач", "Doctor", QStringList{}, QString("qrc:/audio/doctor.mp3"), "", 2},
                   {static_cast<int>(ExerciseType::Grammar), "She works ___ a hospital.", "in", QStringList{"at", "in", "on"}, QString(), "Место работы.", 3},
                   {static_cast<int>(ExerciseType::Audio), "", "Mother", QStringList{}, QString("qrc:/audio/mother.mp3"), "Родственник.", 4} // Повторное использование аудио
               });
  // Урок 13
  createLesson("Погода (En)", "Описание погоды.", 7,
               QList<QList<QVariant>>{
                   {static_cast<int>(ExerciseType::Translation), "Солнечно", "Sunny", QStringList{}, QString("qrc:/audio/sunny.mp3"), "", 1},
                   {static_cast<int>(ExerciseType::Translation), "Дождливо", "Rainy", QStringList{}, QString(), "", 2},
                   {static_cast<int>(ExerciseType::Grammar), "It is ___ today.", "cold", QStringList{"cold", "hot", "warm"}, QString(), "Ощущение температуры.", 3},
                   {static_cast<int>(ExerciseType::Audio), "", "Blue", QStringList{}, QString("qrc:/audio/blue.mp3"), "Цвет.", 4} // Повторное использование
               });
  // Урок 14
  createLesson("Одежда (En)", "Предметы одежды.", 7,
               QList<QList<QVariant>>{
                   {static_cast<int>(ExerciseType::Translation), "Рубашка", "Shirt", QStringList{}, QString(), "", 1},
                   {static_cast<int>(ExerciseType::Translation), "Брюки", "Trousers", QStringList{}, QString("qrc:/audio/trousers.mp3"), "", 2},
                   {static_cast<int>(ExerciseType::Grammar), "These ___ my shoes.", "are", QStringList{"is", "are", "am"}, QString(), "Множественное число.", 3},
                   {static_cast<int>(ExerciseType::Audio), "", "Cat", QStringList{}, QString("qrc:/audio/cat.mp3"), "Домашнее животное.", 4} // Повторное
               });
  // Урок 15
  createLesson("Путешествия (фразы) (En)", "Фразы для путешествий.", 8,
               QList<QList<QVariant>>{
                   {static_cast<int>(ExerciseType::Translation), "Где находится ...?", "Where is ...?", QStringList{}, QString(), "", 1},
                   {static_cast<int>(ExerciseType::Translation), "Сколько это стоит?", "How much is it?", QStringList{}, QString("qrc:/audio/how_much.mp3"), "", 2},
                   {static_cast<int>(ExerciseType::Grammar), "Can you ___ me?", "help", QStringList{"help", "helps", "helping"}, QString(), "Просьба о помощи.", 3},
                   {static_cast<int>(ExerciseType::Audio), "", "Milk", QStringList{}, QString("qrc:/audio/milk.mp3"), "Напиток.", 4} // Повторное
               });
  if (success) {
    if (!db_.commit()) success = false;
  }
  if (!success) {
    db_.rollback();
  }
  return success;
}