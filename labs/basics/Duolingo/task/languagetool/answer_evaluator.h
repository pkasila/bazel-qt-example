#ifndef ANSWER_EVALUATOR_H
#define ANSWER_EVALUATOR_H

#include "languagetool_client.h"

#include <QObject>
#include <QSet>
#include <QString>
#include <QStringList>
#include <algorithm>
#include <vector>

enum class AnswerGrade { CORRECT, SYNONYM, TYPO, ERROR };

struct AnswerEvaluation {
    AnswerGrade grade;
    QString feedback;
    QStringList typos;
    QStringList errors;
    QStringList suggestions;
    float similarity;
};

struct EvaluationConfig {
    bool strictGrammar = true;
    bool strictSyntax = true;
    bool strictTypos = false;
    bool strictPunctuation = false;
    bool strictStyle = false;
};

class AnswerEvaluator : public QObject {
    Q_OBJECT

   public:
    explicit AnswerEvaluator(QObject* parent = nullptr);

    static EvaluationConfig globalConfig;

    void evaluate(const QString& userText, const QString& expectedText);

   signals:
    void evaluationFinished(const AnswerEvaluation& result);

   private slots:
    void onLTCheckFinished(const LTCheckResult& ltResult);

   private:
    static int levenshteinDistance(const QString& s1, const QString& s2);
    static float levenshteinSimilarity(const QString& s1, const QString& s2);
    static float wordOverlap(const QString& a, const QString& b);

    LanguageToolClient* lt_client_;
    QString user_text_;
    QString expected_text_;
    int request_token_;
    int current_token_;
};

#endif  // ANSWER_EVALUATOR_H
