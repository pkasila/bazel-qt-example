#ifndef DATABASE_MANAGER_H
#define DATABASE_MANAGER_H

#include <QJsonArray>
#include <QJsonDocument>
#include <QList>
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include <QString>
#include <QVariant>

struct TranslationQuestion {
    int id;
    QString prompt;
    QString answer;
};

struct GrammarQuestion {
    int id;
    QString prompt;
    QStringList options;
    int correct_index;
};

class DatabaseManager {
   public:
    static DatabaseManager& instance();

    bool init(const QString& path = "duolingo.sqlite");

    bool addTranslationQuestion(const QString& prompt, const QString& answer);
    bool deleteTranslationQuestion(int id);
    QList<TranslationQuestion> getTranslationQuestions(int limit = 5);

    bool addGrammarQuestion(const QString& prompt, const QStringList& options, int correctIndex);
    bool deleteGrammarQuestion(int id);
    QList<GrammarQuestion> getGrammarQuestions(int limit = 5);

   private:
    DatabaseManager();
    ~DatabaseManager();

    QSqlDatabase db_;
};

#endif  // DATABASE_MANAGER_H
