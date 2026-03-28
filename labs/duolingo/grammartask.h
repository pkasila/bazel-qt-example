#ifndef GRAMMARTASK_H
#define GRAMMARTASK_H

#include <QSqlQuery>
#include <QString>
#include <QStringList>
#include <QVariant>

class GrammarTask {
   public:
    GrammarTask();
    GrammarTask(
        int id, const QString& question, const QStringList& options, int correct_option,
        const QString& hint = "", int difficulty = 1);

    int id() const;
    QString question() const;
    QStringList options() const;
    int correctOption() const;
    QString hint() const;
    int difficulty() const;

    static GrammarTask fromSql(const QSqlQuery& query);

   private:
    int id_;
    QString question_;
    QStringList options_;
    int corrOpt_;
    QString hint_;
    int difficulty_;
};

#endif  // GRAMMARTASK_H
