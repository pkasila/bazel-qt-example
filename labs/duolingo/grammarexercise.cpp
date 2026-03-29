#include "grammarexercise.h"
#include <QVBoxLayout>
#include <QButtonGroup>

GrammarExerciseWidget::GrammarExerciseWidget(QWidget *parent) : QWidget(parent) {
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setSpacing(10);

    instructionLabel = new QLabel(tr("Choose the correct option:"));
    QFont instructionFont = instructionLabel->font();
    instructionFont.setPointSize(instructionFont.pointSize() + 1);
    instructionLabel->setFont(instructionFont);

    questionLabel = new QLabel();
    questionLabel->setWordWrap(true);
    QFont boldFont = questionLabel->font();
    boldFont.setBold(true);
    boldFont.setPointSize(boldFont.pointSize() + 2);
    questionLabel->setFont(boldFont);
    questionLabel->setStyleSheet("color: #2c3e50;");

    optionsGroup = new QGroupBox(tr("Options"));
    optionsLayout = new QVBoxLayout();
    optionsGroup->setLayout(optionsLayout);
    optionsGroup->setStyleSheet("QGroupBox { border: 1px solid gray; border-radius: 5px; margin-top: 1ex; } "
                                "QGroupBox::title { subcontrol-origin: margin; subcontrol-position: top center; padding: 0 3px; }");


    submitButton = new QPushButton(tr("Submit Choice"));
    submitButton->setStyleSheet("QPushButton { background-color: #3498db; color: white; border-radius: 5px; padding: 8px; font-weight: bold; }"
                                "QPushButton:hover { background-color: #2980b9; }"
                                "QPushButton:disabled { background-color: #aed6f1; }");
    submitButton->setCursor(Qt::PointingHandCursor);

    feedbackLabel = new QLabel();
    feedbackLabel->setWordWrap(true);
    feedbackLabel->setAlignment(Qt::AlignCenter);

    mainLayout->addWidget(instructionLabel);
    mainLayout->addWidget(questionLabel);
    mainLayout->addWidget(optionsGroup);
    mainLayout->addSpacing(10);
    mainLayout->addWidget(submitButton, 0, Qt::AlignHCenter);
    mainLayout->addWidget(feedbackLabel);
    mainLayout->addStretch();

    connect(submitButton, &QPushButton::clicked, this, &GrammarExerciseWidget::submitClicked);

    setLayout(mainLayout);
}

void GrammarExerciseWidget::setTask(const GrammarTask& task) {
    questionLabel->setText(task.question);
    feedbackLabel->clear();
    feedbackLabel->setStyleSheet("");

    qDeleteAll(radioButtons);
    radioButtons.clear();
    QLayoutItem *child;
    while ((child = optionsLayout->takeAt(0)) != nullptr) {
        delete child->widget();
        delete child;
    }

    QButtonGroup *buttonGroup = new QButtonGroup(this);

    for (const QString& optionText : task.options) {
        QRadioButton *rb = new QRadioButton(optionText);
        rb->setStyleSheet("QRadioButton { spacing: 5px; } QRadioButton::indicator { width: 15px; height: 15px; }");
        radioButtons.append(rb);
        optionsLayout->addWidget(rb);
        buttonGroup->addButton(rb);
    }

    if (!radioButtons.isEmpty()) {
        radioButtons.first()->setFocus();
    }
}

int GrammarExerciseWidget::getSelectedOption() const {
    for (int i = 0; i < radioButtons.size(); ++i) {
        if (radioButtons[i]->isChecked()) {
            return i;
        }
    }
    return -1;
}

void GrammarExerciseWidget::showCorrectAnswer(int correctIndex) {
    if (correctIndex >= 0 && correctIndex < radioButtons.size()) {
        feedbackLabel->setText(tr("The correct option was: %1").arg(radioButtons[correctIndex]->text()));
        feedbackLabel->setStyleSheet("color: blue;");
    }
}
