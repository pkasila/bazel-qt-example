#include "multicorrectgrammarexercise.h"

#include <QVBoxLayout>

MultiCorrectGrammarExerciseWidget::MultiCorrectGrammarExerciseWidget(QWidget* parent)
    : QWidget(parent) {
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->setSpacing(10);

    instructionLabel =
        new QLabel(tr("Select all correct options to complete/answer the statement:"));
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
    optionsGroup->setStyleSheet(
        "QGroupBox { border: 1px solid gray; border-radius: 5px; margin-top: 1ex; } "
        "QGroupBox::title { subcontrol-origin: margin; subcontrol-position: top center; padding: 0 "
        "3px; }");

    submitButton = new QPushButton(tr("Submit Answer"));
    submitButton->setStyleSheet(
        "QPushButton { background-color: #3498db; color: white; border-radius: 5px; padding: 8px; }"
        "QPushButton:hover { background-color: #2980b9; }");
    submitButton->setCursor(Qt::PointingHandCursor);

    feedbackLabel = new QLabel();
    feedbackLabel->setWordWrap(true);
    feedbackLabel->setAlignment(Qt::AlignCenter);

    mainLayout->addWidget(instructionLabel);
    mainLayout->addWidget(questionLabel);
    mainLayout->addWidget(optionsGroup);
    mainLayout->addWidget(submitButton, 0, Qt::AlignHCenter);
    mainLayout->addWidget(feedbackLabel);
    mainLayout->addStretch();

    connect(
        submitButton, &QPushButton::clicked, this,
        &MultiCorrectGrammarExerciseWidget::submitClicked);

    setLayout(mainLayout);
}

void MultiCorrectGrammarExerciseWidget::setTask(const MultiCorrectGrammarTask& task) {
    questionLabel->setText(task.question);
    feedbackLabel->clear();
    feedbackLabel->setStyleSheet("");

    qDeleteAll(checkBoxes);
    checkBoxes.clear();
    QLayoutItem* child;
    while ((child = optionsLayout->takeAt(0)) != nullptr) {
        delete child->widget();
        delete child;
    }

    for (const QString& optionText : task.options) {
        QCheckBox* cb = new QCheckBox(optionText);
        cb->setStyleSheet(
            "QCheckBox { spacing: 5px; } QCheckBox::indicator { width: 15px; height: 15px; }");
        checkBoxes.append(cb);
        optionsLayout->addWidget(cb);
    }
    if (!checkBoxes.isEmpty()) {
        checkBoxes.first()->setFocus();
    }
}

QSet<int> MultiCorrectGrammarExerciseWidget::getSelectedIndices() const {
    QSet<int> selected;
    for (int i = 0; i < checkBoxes.size(); ++i) {
        if (checkBoxes[i]->isChecked()) {
            selected.insert(i);
        }
    }
    return selected;
}

void MultiCorrectGrammarExerciseWidget::showCorrectAnswers(
    const QSet<int>& correctIndices, const QStringList& options) {
    QString correctText = tr("Correct options were: ");
    bool first = true;
    for (int index : correctIndices) {
        if (index >= 0 && index < options.size()) {
            if (!first) {
                correctText += ", ";
            }
            correctText += "\"" + options[index] + "\"";
            first = false;
        }
    }
    feedbackLabel->setText(correctText);
    feedbackLabel->setStyleSheet("color: #2980b9; font-weight: bold;");
}

void MultiCorrectGrammarExerciseWidget::clearSelection() {
    for (QCheckBox* cb : checkBoxes) {
        cb->setChecked(false);
    }
    feedbackLabel->clear();
}