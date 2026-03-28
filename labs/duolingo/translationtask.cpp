#include "translationtask.h"

#include <QRegularExpression>

TranslationTask::TranslationTask() : id_(-1), difficulty_(1) {
}

TranslationTask::TranslationTask(
    int id, const QString& nt, const QString& tt, const QString& h, int diff)
    : id_(id), nativeTxt_(nt), targetTxt_(tt), hint_(h), difficulty_(diff) {
}

int TranslationTask::id() const {
    return id_;
}

QString TranslationTask::nativeText() const {
    return nativeTxt_;
}

QString TranslationTask::targetText() const {
    return targetTxt_;
}

QString TranslationTask::hint() const {
    return hint_;
}

int TranslationTask::difficulty() const {
    return difficulty_;
}

bool TranslationTask::checkAnswer(const QString& ua) const {
    auto norm = [](const QString& s) {
        return s.toLower().replace(QRegularExpression("[^a-zа-яё0-9 ]"), "").simplified();
    };
    return norm(ua) == norm(targetTxt_);
}

TranslationTask TranslationTask::fromSql(const QSqlQuery& qry) {
    return TranslationTask(
        qry.value("id").toInt(), qry.value("native_text").toString(),
        qry.value("target_text").toString(), qry.value("hint").toString(),
        qry.value("difficulty").toInt());
}
