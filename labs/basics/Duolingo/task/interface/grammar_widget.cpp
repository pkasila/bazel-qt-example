#include "grammar_widget.h"

#include "styles.h"

GrammarWidget::GrammarWidget(QWidget* parent) : QWidget(parent), correct_index_(-1) {
    auto* layout = new QVBoxLayout(this);
    layout->setContentsMargins(40, 40, 40, 40);
    layout->setSpacing(20);

    auto* prompt = new QLabel("Select the correct option:", this);
    prompt->setStyleSheet(Styles::SUB_TEXT);
    layout->addWidget(prompt);

    question_label_ = new QLabel(this);
    question_label_->setStyleSheet(Styles::QUESTION_TEXT);
    layout->addWidget(question_label_);

    options_layout_ = new QVBoxLayout();
    layout->addLayout(options_layout_);

    button_group_ = new QButtonGroup(this);

    layout->addStretch();

    submit_btn_ = new QPushButton("SUBMIT", this);
    submit_btn_->setStyleSheet(Styles::CHECK_BUTTON);
    connect(submit_btn_, &QPushButton::clicked, this, &GrammarWidget::onSubmit);
    layout->addWidget(submit_btn_, 0, Qt::AlignCenter);
}

void GrammarWidget::setQuestion(
    const QString& question, const QStringList& options, int correct_index) {
    question_label_->setText(question);
    correct_index_ = correct_index;

    QLayoutItem* child;
    while ((child = options_layout_->takeAt(0)) != nullptr) {
        delete child->widget();
        delete child;
    }

    for (int i = 0; i < options.size(); ++i) {
        auto* rb = new QRadioButton(options[i], this);
        rb->setStyleSheet(Styles::RADIO_BUTTON);
        options_layout_->addWidget(rb);
        button_group_->addButton(rb, i);
    }
}

void GrammarWidget::onSubmit() {
    int id = button_group_->checkedId();
    if (id == -1) {
        return;  // Nothing selected
    }
    emit answerSubmitted(id == correct_index_);
}
