#include "database_manager.h"

#include <QDebug>

DatabaseManager& DatabaseManager::instance() {
    static DatabaseManager instance;
    return instance;
}

DatabaseManager::DatabaseManager() {
}

DatabaseManager::~DatabaseManager() {
    if (db_.isOpen()) {
        db_.close();
    }
}

bool DatabaseManager::init(const QString& path) {
    db_ = QSqlDatabase::addDatabase("QSQLITE");
    db_.setDatabaseName(path);

    if (!db_.open()) {
        qWarning() << "Failed to open database:" << db_.lastError().text();
        return false;
    }

    QSqlQuery query;
    if (!query.exec(
            "CREATE TABLE IF NOT EXISTS translations ("
            "id INTEGER PRIMARY KEY AUTOINCREMENT, "
            "prompt TEXT NOT NULL, "
            "answer TEXT NOT NULL)")) {
        qWarning() << "Failed to create translations table:" << query.lastError().text();
        return false;
    }

    // Create grammar table
    if (!query.exec(
            "CREATE TABLE IF NOT EXISTS grammar ("
            "id INTEGER PRIMARY KEY AUTOINCREMENT, "
            "prompt TEXT NOT NULL, "
            "options TEXT NOT NULL, "
            "correct_index INTEGER NOT NULL)")) {
        qWarning() << "Failed to create grammar table:" << query.lastError().text();
        return false;
    }

    QSqlQuery check_query("SELECT COUNT(*) FROM translations");
    if (check_query.next() && check_query.value(0).toInt() == 0) {
        addTranslationQuestion("The cat is on the table", "Кот на столе");
        addTranslationQuestion("I am learning Qt", "Я изучаю Qt");
        addTranslationQuestion("Hello world", "Привет мир");
        addTranslationQuestion("Where is the library?", "Где находится библиотека?");
        addTranslationQuestion("Thank you very much", "Большое спасибо");
    }

    check_query.exec("SELECT COUNT(*) FROM grammar");
    if (check_query.next() && check_query.value(0).toInt() == 0) {
        addGrammarQuestion("She ___ to the store yesterday.", {"go", "goes", "went", "going"}, 2);
        addGrammarQuestion(
            "He is ___ than his brother.", {"tall", "taller", "tallest", "more tall"}, 1);
        addGrammarQuestion("I ___ never been to Paris.", {"has", "have", "had", "having"}, 1);
        addGrammarQuestion("They ___ playing football right now.", {"is", "are", "am", "be"}, 1);
        addGrammarQuestion("The dog ___ barking loudly.", {"is", "are", "am", "be"}, 0);
    }

    return true;
}

bool DatabaseManager::addTranslationQuestion(const QString& prompt, const QString& answer) {
    QSqlQuery query;
    query.prepare("INSERT INTO translations (prompt, answer) VALUES (?, ?)");
    query.addBindValue(prompt);
    query.addBindValue(answer);
    return query.exec();
}

bool DatabaseManager::deleteTranslationQuestion(int id) {
    QSqlQuery query;
    query.prepare("DELETE FROM translations WHERE id = ?");
    query.addBindValue(id);
    return query.exec();
}

QList<TranslationQuestion> DatabaseManager::getTranslationQuestions(int limit) {
    QList<TranslationQuestion> questions;
    QSqlQuery query;
    query.prepare("SELECT id, prompt, answer FROM translations ORDER BY RANDOM() LIMIT ?");
    query.addBindValue(limit);

    if (query.exec()) {
        while (query.next()) {
            TranslationQuestion q;
            q.id = query.value(0).toInt();
            q.prompt = query.value(1).toString();
            q.answer = query.value(2).toString();
            questions.append(q);
        }
    }
    return questions;
}

bool DatabaseManager::addGrammarQuestion(
    const QString& prompt, const QStringList& options, int correct_index) {
    QJsonArray options_array;
    for (const QString& opt : options) {
        options_array.append(opt);
    }
    QJsonDocument doc(options_array);
    QString options_str = doc.toJson(QJsonDocument::Compact);

    QSqlQuery query;
    query.prepare("INSERT INTO grammar (prompt, options, correct_index) VALUES (?, ?, ?)");
    query.addBindValue(prompt);
    query.addBindValue(options_str);
    query.addBindValue(correct_index);
    return query.exec();
}

bool DatabaseManager::deleteGrammarQuestion(int id) {
    QSqlQuery query;
    query.prepare("DELETE FROM grammar WHERE id = ?");
    query.addBindValue(id);
    return query.exec();
}

QList<GrammarQuestion> DatabaseManager::getGrammarQuestions(int limit) {
    QList<GrammarQuestion> questions;
    QSqlQuery query;
    query.prepare(
        "SELECT id, prompt, options, correct_index FROM grammar ORDER BY RANDOM() LIMIT ?");
    query.addBindValue(limit);

    if (query.exec()) {
        while (query.next()) {
            GrammarQuestion q;
            q.id = query.value(0).toInt();
            q.prompt = query.value(1).toString();

            QString options_str = query.value(2).toString();
            QJsonDocument doc = QJsonDocument::fromJson(options_str.toUtf8());
            QJsonArray arr = doc.array();
            for (int i = 0; i < arr.size(); ++i) {
                q.options.append(arr[i].toString());
            }

            q.correct_index = query.value(3).toInt();
            questions.append(q);
        }
    }
    return questions;
}
