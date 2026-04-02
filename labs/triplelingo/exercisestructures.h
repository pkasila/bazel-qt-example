#ifndef EXERCISESTRUCTURES_H
#define EXERCISESTRUCTURES_H

#include <QString>
#include <QStringList>
#include <QVariant>

struct BaseTask {
    int id;
    QString helpText;

    BaseTask(int i = 0, QString h = "") : id(i), helpText(h) {}
    virtual ~BaseTask() = default;
};

struct TranslationTask : public BaseTask {
    QString textToTranslate;
    QString correctAnswer;

    TranslationTask(int i = 0, const QString& text = "", const QString& answer = "", const QString& help = "")
        : BaseTask(i, help), textToTranslate(text), correctAnswer(answer) {}
};

struct GrammarTask : public BaseTask {
    QString sentence;
    QStringList options;
    QString correctAnswer;

    GrammarTask(int i = 0, const QString& s = "", const QStringList& opts = {}, const QString& answer = "", const QString& help = "")
        : BaseTask(i, help), sentence(s), options(opts), correctAnswer(answer) {}
};

Q_DECLARE_METATYPE(TranslationTask)
Q_DECLARE_METATYPE(GrammarTask)

const int TASKS_PER_EXERCISE = 5;
const int MAX_INCORRECT_ATTEMPTS = 3;
const int POINTS_PER_EXERCISE = 10;

enum class ExerciseType {
    None,
    Translation,
    Grammar
};

#endif // EXERCISESTRUCTURES_H
