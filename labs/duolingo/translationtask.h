#ifndef TRANSLATIONTASK_H
#define TRANSLATIONTASK_H

#include <QString>
#include <QtSql/QSqlQuery>

class TranslationTask {
   public:
    TranslationTask();
    TranslationTask(
        int id, const QString& native_text, const QString& target_text, const QString& hint = "",
        int difficulty = 1);
    int id() const;
    QString nativeText() const;
    QString targetText() const;
    QString hint() const;
    int difficulty() const;
    bool checkAnswer(const QString& usrAns) const;
    static TranslationTask fromSql(const QSqlQuery& query);

   private:
    int id_;
    QString nativeTxt_;
    QString targetTxt_;
    QString hint_;
    int difficulty_;
};

#endif  // TRANSLATIONTASK_H
