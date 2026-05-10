#ifndef TRANSLATION_WIDGET_H
#define TRANSLATION_WIDGET_H

#include "../languagetool/answer_evaluator.h"

#include <QLabel>
#include <QProgressBar>
#include <QPushButton>
#include <QTextEdit>
#include <QVBoxLayout>
#include <QWidget>

class TranslationWidget : public QWidget {
    Q_OBJECT
   public:
    explicit TranslationWidget(QWidget* parent = nullptr);
    void reset();
    void setQuestion(const QString& question, const QString& answer);

   signals:
    void answerSubmitted(bool correct);

   private slots:
    void onSubmit();
    void onEvaluationFinished(const AnswerEvaluation& eval);

   private:
    void showFeedback(const QString& text, const QString& color);

    QLabel* question_label_;
    QTextEdit* input_field_;
    QPushButton* submit_btn_;
    QLabel* feedback_label_;
    QString correct_answer_;
    AnswerEvaluator* evaluator_;
};

#endif  // TRANSLATION_WIDGET_H
