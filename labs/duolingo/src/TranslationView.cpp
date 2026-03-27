#include "TranslationView.h"

#include "data_base.h"

#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QVBoxLayout>
#include <QWidget>

TranslationView::TranslationView(QWidget* parent)
    : QWidget(parent)
    , question_label_(new QLabel(tr("Переведите текст:"), this))
    , input_field_(new QLineEdit(this))
    , submit_button_(new QPushButton(tr("Подтвердить"), this)) {
    auto* layout = new QVBoxLayout(this);
    layout->addWidget(question_label_);
    layout->addWidget(input_field_);
    layout->addStretch();
    layout->addWidget(submit_button_);

    connect(submit_button_, &QPushButton::clicked, this, &TranslationView::Submit);
    connect(input_field_, &QLineEdit::returnPressed, this, &TranslationView::Submit);
}

void TranslationView::SetTask(TaskDifficulty difficulty, const int& taskId) {
    const auto task = data_base::GetTranslationTask(difficulty, taskId);
    question_label_->setText(*task->question);
    correct_answer_ = *task->answer;
    input_field_->clear();
}

void TranslationView::Submit() {
    const QString userText = input_field_->text().trimmed();
    if (userText.isEmpty()) {
        return;
    }
    if (userText.toLower() == correct_answer_.toLower()) {
        emit Accepted();
    } else {
        emit WrongAnswer();
    }
}