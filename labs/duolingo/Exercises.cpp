#include "Exercises.h"

#include <QtCore/QList>
#include <QtWidgets/QAbstractButton>

TranslationWidget::TranslationWidget(QWidget* parent) : QWidget(parent) {
    QVBoxLayout* layout = new QVBoxLayout(this);
    taskLabel = new QLabel("Переведите предложение:", this);
    inputField = new QLineEdit(this);
    submitBtn = new QPushButton("Ответить", this);
    audioBtn = new QPushButton("Озвучить 🔊", this);

    layout->addWidget(taskLabel);
    layout->addWidget(audioBtn);
    layout->addWidget(inputField);
    layout->addWidget(submitBtn);

    connect(submitBtn, &QPushButton::clicked, this, &TranslationWidget::submitted);
    connect(inputField, &QLineEdit::returnPressed, this, &TranslationWidget::submitted);
    connect(audioBtn, &QPushButton::clicked, this, &TranslationWidget::playAudioRequested);
}

void TranslationWidget::setTask(const QString& text) {
    taskLabel->setText(text);
    inputField->clear();
}

QString TranslationWidget::getAnswer() const {
    return inputField->text().trimmed();
}

GrammarWidget::GrammarWidget(QWidget* parent) : QWidget(parent) {
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    questionLabel = new QLabel("Выберите правильный вариант:", this);
    optionsLayout = new QVBoxLayout();
    group = new QButtonGroup(this);
    submitBtn = new QPushButton("Ответить", this);

    mainLayout->addWidget(questionLabel);
    mainLayout->addLayout(optionsLayout);
    mainLayout->addWidget(submitBtn);

    connect(submitBtn, &QPushButton::clicked, this, &GrammarWidget::submitted);
}

void GrammarWidget::setTask(const QString& question, const QStringList& options) {
    questionLabel->setText(question);

    QList<QAbstractButton*> buttons = group->buttons();
    for (QAbstractButton* btn : buttons) {
        group->removeButton(btn);
        optionsLayout->removeWidget(btn);
        delete btn;
    }

    for (int i = 0; i < options.size(); ++i) {
        QRadioButton* rb = new QRadioButton(QString::number(i + 1) + ". " + options[i], this);
        group->addButton(rb, i);
        optionsLayout->addWidget(rb);
    }
}

void GrammarWidget::selectOption(int index) {
    QList<QAbstractButton*> buttons = group->buttons();
    if (index >= 0 && index < buttons.size()) {
        buttons[index]->setChecked(true);
    }
}

QString GrammarWidget::getSelected() const {
    QAbstractButton* checked = group->checkedButton();
    if (checked) {
        QString text = checked->text();
        return text.mid(text.indexOf(". ") + 2);
    }
    return "";
}

MathWidget::MathWidget(QWidget* parent) : QWidget(parent) {
    QVBoxLayout* layout = new QVBoxLayout(this);
    expressionLabel = new QLabel("2 + 2 = ?", this);
    expressionLabel->setAlignment(Qt::AlignCenter);
    expressionLabel->setStyleSheet(
        "font-size: 32px; font-weight: bold; color: #58cc02; margin: 20px;");

    answerEdit = new QLineEdit(this);
    answerEdit->setFixedHeight(50);
    answerEdit->setAlignment(Qt::AlignCenter);

    submitBtn = new QPushButton("Ответить", this);

    layout->addStretch();
    layout->addWidget(expressionLabel);
    layout->addWidget(answerEdit);
    layout->addWidget(submitBtn);
    layout->addStretch();

    connect(submitBtn, &QPushButton::clicked, this, &MathWidget::submitted);
    connect(answerEdit, &QLineEdit::returnPressed, this, &MathWidget::submitted);
}

void MathWidget::setTask(const QString& expression) {
    expressionLabel->setText(expression);
    answerEdit->clear();
}

QString MathWidget::getAnswer() const {
    return answerEdit->text().trimmed();
}