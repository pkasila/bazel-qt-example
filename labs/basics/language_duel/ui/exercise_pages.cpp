#include "ui/exercise_pages.h"

#include <QAbstractButton>
#include <QApplication>
#include <QButtonGroup>
#include <QFrame>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QRadioButton>
#include <QSizePolicy>
#include <QStyle>
#include <QTextEdit>
#include <QVBoxLayout>

namespace {
QFrame* makeCard(QWidget* parent = nullptr) {
    auto* frame = new QFrame(parent);
    frame->setObjectName("Card");
    frame->setFrameShape(QFrame::StyledPanel);
    frame->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    return frame;
}

QFrame* makeMiniCard(const QString& title, const QString& value, QWidget* parent = nullptr) {
    auto* frame = new QFrame(parent);
    frame->setObjectName("MiniCard");
    auto* layout = new QVBoxLayout(frame);
    layout->setContentsMargins(12, 10, 12, 10);
    layout->setSpacing(4);

    auto* titleLabel = new QLabel(title);
    titleLabel->setObjectName("SecondaryLabel");
    auto* valueLabel = new QLabel(value);
    valueLabel->setWordWrap(true);

    layout->addWidget(titleLabel);
    layout->addWidget(valueLabel);
    return frame;
}
}  // namespace

StartPage::StartPage(QWidget* parent)
    : QWidget(parent) {
    auto* root = new QVBoxLayout(this);
    root->setContentsMargins(34, 32, 34, 32);
    root->setSpacing(18);

    auto* header = new QLabel("Language Duel");
    header->setObjectName("HeroTitle");

    auto* subtitle = new QLabel(
        "A tiny language arena with strict grammar, forgiving translation checks and a very serious cat professor.");
    subtitle->setWordWrap(true);

    auto* card = makeCard(this);
    auto* cardLayout = new QVBoxLayout(card);
    cardLayout->setContentsMargins(18, 18, 18, 18);
    cardLayout->setSpacing(12);

    difficultyLabel_ = new QLabel("Difficulty: Easy");
    difficultyLabel_->setObjectName("TinyBadge");

    auto* translationButton = new QPushButton(
        QApplication::style()->standardIcon(QStyle::SP_FileDialogDetailedView),
        "Translation");
    auto* grammarButton = new QPushButton(
        QApplication::style()->standardIcon(QStyle::SP_DialogApplyButton),
        "Grammar");

    translationButton->setMinimumHeight(44);
    grammarButton->setMinimumHeight(44);
    translationButton->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    grammarButton->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

    connect(translationButton, &QPushButton::clicked, this, &StartPage::translationRequested);
    connect(grammarButton, &QPushButton::clicked, this, &StartPage::grammarRequested);

    cardLayout->addWidget(difficultyLabel_);
    cardLayout->addSpacing(8);
    cardLayout->addWidget(translationButton);
    cardLayout->addWidget(grammarButton);

    auto* bonusRow = new QHBoxLayout();
    bonusRow->setSpacing(12);
    bonusRow->addWidget(makeMiniCard("Coach", "Professor Cat reacts to every attempt.", this));
    bonusRow->addWidget(makeMiniCard("Matcher", "Typos and word-order shifts are handled gently.", this));
    bonusRow->addWidget(makeMiniCard("Rewards", "Streaks, speed and clean runs unlock badges.", this));

    root->addWidget(header);
    root->addWidget(subtitle);
    root->addWidget(card);
    root->addLayout(bonusRow);
    root->addStretch();
}

void StartPage::setDifficultyText(const QString& text) {
    difficultyLabel_->setText("Difficulty: " + text);
}

TranslationPage::TranslationPage(QWidget* parent)
    : QWidget(parent) {
    auto* root = new QVBoxLayout(this);
    root->setContentsMargins(34, 32, 34, 32);
    root->setSpacing(18);

    promptLabel_ = new QLabel("Prompt");
    promptLabel_->setWordWrap(true);
    promptLabel_->setObjectName("TaskLabel");

    answerEdit_ = new QTextEdit(this);
    answerEdit_->setPlaceholderText("Type your translation here...");
    answerEdit_->setMinimumHeight(180);
    answerEdit_->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    auto* submitButton = new QPushButton(
        QApplication::style()->standardIcon(QStyle::SP_DialogApplyButton),
        "Submit");
    submitButton->setMinimumHeight(44);
    submitButton->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

    connect(submitButton, &QPushButton::clicked, this, &TranslationPage::submitRequested);

    root->addWidget(promptLabel_);
    root->addWidget(answerEdit_);
    root->addWidget(submitButton);
    root->addStretch();
}

void TranslationPage::setTask(const TranslationTask& task) {
    promptLabel_->setText(task.prompt);
    helpText_ = task.help;
    clearInput();
}

QString TranslationPage::currentAnswer() const {
    return answerEdit_->toPlainText().trimmed();
}

QString TranslationPage::helpText() const {
    return helpText_;
}

void TranslationPage::clearInput() {
    answerEdit_->clear();
}

void TranslationPage::focusInput() {
    answerEdit_->setFocus();
}

GrammarPage::GrammarPage(QWidget* parent)
    : QWidget(parent) {
    auto* root = new QVBoxLayout(this);
    root->setContentsMargins(34, 32, 34, 32);
    root->setSpacing(18);

    sentenceLabel_ = new QLabel("Sentence");
    sentenceLabel_->setWordWrap(true);
    sentenceLabel_->setObjectName("TaskLabel");

    auto* optionsHost = new QWidget(this);
    optionsLayout_ = new QVBoxLayout(optionsHost);
    optionsLayout_->setContentsMargins(0, 0, 0, 0);
    optionsLayout_->setSpacing(10);

    buttonGroup_ = new QButtonGroup(this);

    auto* submitButton = new QPushButton(
        QApplication::style()->standardIcon(QStyle::SP_DialogApplyButton),
        "Submit");
    submitButton->setMinimumHeight(44);
    submitButton->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    connect(submitButton, &QPushButton::clicked, this, &GrammarPage::submitRequested);

    root->addWidget(sentenceLabel_);
    root->addWidget(optionsHost);
    root->addWidget(submitButton);
    root->addStretch();
}

void GrammarPage::setTask(const GrammarTask& task) {
    sentenceLabel_->setText(task.sentence);
    helpText_ = task.help;

    while (QLayoutItem* item = optionsLayout_->takeAt(0)) {
        if (item->widget()) {
            buttonGroup_->removeButton(qobject_cast<QAbstractButton*>(item->widget()));
            delete item->widget();
        }
        delete item;
    }

    for (int i = 0; i < task.options.size(); ++i) {
        auto* button = new QRadioButton(task.options[i], this);
        button->setMinimumHeight(32);
        optionsLayout_->addWidget(button);
        buttonGroup_->addButton(button, i);
    }
}

int GrammarPage::selectedIndex() const {
    return buttonGroup_->checkedId();
}

QString GrammarPage::helpText() const {
    return helpText_;
}

void GrammarPage::clearSelection() {
    if (auto* checked = buttonGroup_->checkedButton()) {
        const bool wasExclusive = buttonGroup_->exclusive();
        buttonGroup_->setExclusive(false);
        checked->setChecked(false);
        buttonGroup_->setExclusive(wasExclusive);
    }
}
