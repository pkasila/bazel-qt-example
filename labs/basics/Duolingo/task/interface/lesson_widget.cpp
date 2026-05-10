#include "lesson_widget.h"

#include "styles.h"

#include <QStyle>

LessonWidget::LessonWidget(QWidget* parent)
    : QWidget(parent), current_progress_(30), selected_button_(nullptr) {
    setupUi();
}

void LessonWidget::setupUi() {
    auto* main_layout = new QVBoxLayout(this);
    main_layout->setContentsMargins(40, 20, 40, 40);
    main_layout->setSpacing(30);

    auto* top_bar = new QHBoxLayout();
    auto* close_button = new QPushButton("✕", this);
    close_button->setFixedSize(30, 30);
    close_button->setStyleSheet("border: none; font-size: 20px; color: #afafaf; font-weight: bold;");

    progress_bar_ = new QProgressBar(this);
    progress_bar_->setRange(0, 100);
    progress_bar_->setValue(current_progress_);
    progress_bar_->setTextVisible(false);
    progress_bar_->setStyleSheet(Styles::PROGRESS_BAR);

    top_bar->addWidget(close_button);
    top_bar->addWidget(progress_bar_);
    main_layout->addLayout(top_bar);

    auto* question_container = new QVBoxLayout();
    auto* prompt_label = new QLabel("Translate this sentence", this);
    prompt_label->setStyleSheet(Styles::SUB_TEXT);

    question_label_ = new QLabel("The green owl is teaching me C++", this);
    question_label_->setStyleSheet(Styles::QUESTION_TEXT);
    question_label_->setWordWrap(true);

    question_container->addWidget(prompt_label);
    question_container->addWidget(question_label_);
    main_layout->addLayout(question_container);

    auto* options_layout = new QVBoxLayout();
    QStringList option_texts = {
      "Зеленая сова учит меня C++", "Кот ест яблоко", "Я люблю программировать",
      "Где находится библиотека?"};

    for (const QString& text : option_texts) {
        auto* btn = new QPushButton(text, this);
        btn->setStyleSheet(Styles::OPTION_BUTTON);
        btn->setCursor(Qt::PointingHandCursor);
        connect(btn, &QPushButton::clicked, this, &LessonWidget::onOptionSelected);
        options_layout->addWidget(btn);
        options_.append(btn);
    }
    main_layout->addLayout(options_layout);

    main_layout->addStretch();

    auto* bottom_layout = new QHBoxLayout();
    check_button_ = new QPushButton("CHECK", this);
    check_button_->setStyleSheet(Styles::CHECK_BUTTON);
    check_button_->setEnabled(false);
    connect(check_button_, &QPushButton::clicked, this, &LessonWidget::onCheckClicked);

    bottom_layout->addStretch();
    bottom_layout->addWidget(check_button_);
    bottom_layout->addStretch();
    main_layout->addLayout(bottom_layout);
}

void LessonWidget::onOptionSelected() {
    auto* clicked_btn = qobject_cast<QPushButton*>(sender());
    if (!clicked_btn) {
        return;
    }

    if (selected_button_) {
        selected_button_->setProperty("selected", false);
        selected_button_->style()->unpolish(selected_button_);
        selected_button_->style()->polish(selected_button_);
    }

    selected_button_ = clicked_btn;
    selected_button_->setProperty("selected", true);
    selected_button_->style()->unpolish(selected_button_);
    selected_button_->style()->polish(selected_button_);

    check_button_->setEnabled(true);
}

void LessonWidget::onCheckClicked() {
    if (current_progress_ < 100) {
        current_progress_ += 10;
        progress_bar_->setValue(current_progress_);
    }

    if (selected_button_) {
        selected_button_->setProperty("selected", false);
        selected_button_->style()->unpolish(selected_button_);
        selected_button_->style()->polish(selected_button_);
        selected_button_ = nullptr;
    }
    check_button_->setEnabled(false);
}
