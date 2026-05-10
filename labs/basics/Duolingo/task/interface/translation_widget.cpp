#include "translation_widget.h"

#include "styles.h"

TranslationWidget::TranslationWidget(QWidget* parent) : QWidget(parent) {
    auto* layout = new QVBoxLayout(this);
    layout->setContentsMargins(40, 40, 40, 40);
    layout->setSpacing(20);

    auto* prompt = new QLabel("Translate the text below:", this);
    prompt->setStyleSheet(Styles::SUB_TEXT);
    layout->addWidget(prompt);

    question_label_ = new QLabel(this);
    question_label_->setStyleSheet(Styles::QUESTION_TEXT);
    question_label_->setWordWrap(true);
    layout->addWidget(question_label_);

    input_field_ = new QTextEdit(this);
    input_field_->setStyleSheet(Styles::TEXT_EDIT);
    input_field_->setPlaceholderText("Type your translation here...");
    layout->addWidget(input_field_);

    feedback_label_ = new QLabel(this);
    feedback_label_->setWordWrap(true);
    feedback_label_->hide();
    layout->addWidget(feedback_label_);

    evaluator_ = new AnswerEvaluator(this);
    connect(
        evaluator_, &AnswerEvaluator::evaluationFinished, this,
        &TranslationWidget::onEvaluationFinished);

    layout->addStretch();

    submit_btn_ = new QPushButton("SUBMIT", this);
    submit_btn_->setStyleSheet(Styles::CHECK_BUTTON);
    connect(submit_btn_, &QPushButton::clicked, this, &TranslationWidget::onSubmit);
    layout->addWidget(submit_btn_, 0, Qt::AlignCenter);
}

void TranslationWidget::setQuestion(const QString& question, const QString& answer) {
    question_label_->setText(question);
    correct_answer_ = answer;
    input_field_->clear();
    feedback_label_->hide();
    submit_btn_->setEnabled(true);
}

void TranslationWidget::onSubmit() {
    QString user_text = input_field_->toPlainText().trimmed();
    if (user_text.isEmpty()) {
        return;
    }

    submit_btn_->setEnabled(false);
    feedback_label_->hide();
    evaluator_->evaluate(user_text, correct_answer_);
}

void TranslationWidget::onEvaluationFinished(const AnswerEvaluation& eval) {
    submit_btn_->setEnabled(true);

    switch (eval.grade) {
        case AnswerGrade::CORRECT:
            showFeedback(eval.feedback, "#00ff88");
            emit answerSubmitted(true);
            break;

        case AnswerGrade::SYNONYM:
            showFeedback(eval.feedback, "#00f2ff");
            emit answerSubmitted(true);
            break;

        case AnswerGrade::TYPO:
            showFeedback(eval.feedback, "#ff9900");
            break;

        case AnswerGrade::ERROR:
            showFeedback(eval.feedback, "#ff3366");
            emit answerSubmitted(false);
            break;
    }
}

void TranslationWidget::showFeedback(const QString& text, const QString& color) {
    feedback_label_->setStyleSheet(
        QString("font-size: 15px; color: %1; padding: 12px; "
                "background-color: rgba(255, 255, 255, 0.05); "
                "border: 1px solid %1; border-radius: 10px;")
            .arg(color));
    feedback_label_->setText(text);
    feedback_label_->show();
}

void TranslationWidget::reset() {
    input_field_->clear();
    feedback_label_->hide();
}
