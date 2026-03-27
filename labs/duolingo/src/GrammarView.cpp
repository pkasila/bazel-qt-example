#include "GrammarView.h"

#include "TaskDifficulty.h"
#include "data_base.h"

#include <QButtonGroup>
#include <QKeySequence>
#include <QLabel>
#include <QPushButton>
#include <QShortcut>
#include <QVBoxLayout>
#include <QWidget>

GrammarView::GrammarView(QWidget* parent)
    : QWidget(parent), question_(new QLabel("question", this)), option_group_(new QButtonGroup) {
    auto* main_layout = new QVBoxLayout;
    main_layout->addWidget(question_);

    auto* buttons_layout = new QVBoxLayout;
    options_.resize(4);
    for (int i = 0; i < 4; i++) {
        options_[i] = new QRadioButton("option", this);
        option_group_->addButton(options_[i], i);
        buttons_layout->addWidget(options_[i]);
    }
    option_group_->setExclusive(true);
    main_layout->addLayout(buttons_layout);

    main_layout->addStretch(1);

    auto* submit_button = new QPushButton("Подтвердить", this);
    connect(submit_button, &QPushButton::pressed, this, &GrammarView::Submit);
    main_layout->addWidget(submit_button);

    auto* submit_shortcut = new QShortcut(QKeySequence("Enter"), this);
    connect(submit_shortcut, &QShortcut::activated, this, &GrammarView::Submit);

    setLayout(main_layout);
}

void GrammarView::SetTask(TaskDifficulty difficulty, const int& task_id) {
    const auto task = data_base::GetGrammarTask(difficulty, task_id);
    question_->setText(*task->question);
    for (int i = 0; i < 4; i++) {
        options_[i]->setText((*task->options)[i]);
    }
    answer_ = *task->answer;
}

void GrammarView::Submit() {
    const int chosen = option_group_->checkedId();
    if (chosen == -1) {
        return;
    }
    if (chosen == answer_) {
        emit Accepted();
    } else {
        emit WrongAnswer();
    }
}
