#include "grammartask.h"

GrammarTask::GrammarTask() : id_(-1), corrOpt_(-1), difficulty_(1) {
}

GrammarTask::GrammarTask(
    int id, const QString& q, const QStringList& opts, int co, const QString& h, int diff)
    : id_(id), question_(q), options_(opts), corrOpt_(co), hint_(h), difficulty_(diff) {
}

int GrammarTask::id() const {
    return id_;
}

QString GrammarTask::question() const {
    return question_;
}

QStringList GrammarTask::options() const {
    return options_;
}

int GrammarTask::correctOption() const {
    return corrOpt_;
}

QString GrammarTask::hint() const {
    return hint_;
}

int GrammarTask::difficulty() const {
    return difficulty_;
}

GrammarTask GrammarTask::fromSql(const QSqlQuery& qry) {
    QStringList opts;
    opts << qry.value("option1").toString() << qry.value("option2").toString()
         << qry.value("option3").toString() << qry.value("option4").toString();

    return GrammarTask(
        qry.value("id").toInt(), qry.value("question").toString(), opts,
        qry.value("correct_option").toInt(), qry.value("hint").toString(),
        qry.value("difficulty").toInt());
}
