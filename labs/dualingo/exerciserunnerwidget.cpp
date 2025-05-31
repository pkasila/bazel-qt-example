#include "exerciserunnerwidget.h"
#include <QTimer> // For feedback label timeout

ExerciseRunnerWidget::ExerciseRunnerWidget(QWidget *parent) : QWidget(parent), currentType(ExerciseType::None)
{
    mainLayout = new QVBoxLayout(this);

    promptLabel = new QLabel("Prompt will appear here.", this);
    promptLabel->setWordWrap(true);
    QFont promptFont = promptLabel->font();
    promptFont.setPointSize(14);
    promptLabel->setFont(promptFont);

    feedbackLabel = new QLabel("", this);
    feedbackLabel->setAlignment(Qt::AlignCenter);
    QFont feedbackFont = feedbackLabel->font();
    feedbackFont.setPointSize(12);
    feedbackFont.setBold(true);
    feedbackLabel->setFont(feedbackFont);
    feedbackLabel->setFixedHeight(30); // Reserve space

    // Translation input
    translationEdit = new QLineEdit(this);
    translationEdit->setFont(promptFont);
    translationEdit->setVisible(false);

    // Grammar input
    grammarGroup = new QGroupBox("Choose the correct option:", this);
    radioLayout = new QVBoxLayout(grammarGroup);
    grammarGroup->setLayout(radioLayout);
    grammarGroup->setVisible(false);
    grammarButtonGroup = new QButtonGroup(this);


    submitButton = new QPushButton("Submit Answer", this);
    submitButton->setMinimumHeight(40);
    QFont submitFont = submitButton->font();
    submitFont.setPointSize(12);
    submitButton->setFont(submitFont);

    mainLayout->addWidget(promptLabel, 1); // Give prompt more space
    mainLayout->addWidget(feedbackLabel);
    mainLayout->addWidget(translationEdit);
    mainLayout->addWidget(grammarGroup, 1); // Give options more space
    mainLayout->addStretch(1);
    mainLayout->addWidget(submitButton);

    setLayout(mainLayout);

    connect(submitButton, &QPushButton::clicked, this, &ExerciseRunnerWidget::submitClicked);
}

void ExerciseRunnerWidget::clearRadioButtons() {
    for (QRadioButton* button : qAsConst(radioButtons)) {
        radioLayout->removeWidget(button);
        grammarButtonGroup->removeButton(button);
        delete button;
    }
    radioButtons.clear();
}

void ExerciseRunnerWidget::setupForTask(const TaskData& task) {
    currentType = task.type;
    promptLabel->setText(task.prompt);
    feedbackLabel->setText(""); // Clear previous feedback
    clearInputs();

    if (task.type == ExerciseType::Translation) {
        translationEdit->setVisible(true);
        grammarGroup->setVisible(false);
        translationEdit->setFocus();
    } else if (task.type == ExerciseType::Grammar) {
        translationEdit->setVisible(false);
        grammarGroup->setVisible(true);
        clearRadioButtons(); // Clear previous radio buttons

        for (int i = 0; i < task.options.size(); ++i) {
            QRadioButton *rb = new QRadioButton(task.options[i], grammarGroup);
            QFont radioFont = rb->font();
            radioFont.setPointSize(11);
            rb->setFont(radioFont);
            radioLayout->addWidget(rb);
            radioButtons.append(rb);
            grammarButtonGroup->addButton(rb, i); // Use index as ID
        }
        if (!radioButtons.isEmpty()) radioButtons.first()->setFocus();
    }
}

QString ExerciseRunnerWidget::getTranslationAnswer() const {
    return translationEdit->text().trimmed();
}

int ExerciseRunnerWidget::getGrammarAnswerIndex() const {
    return grammarButtonGroup->checkedId(); // Returns -1 if none checked
}

void ExerciseRunnerWidget::clearInputs() {
    translationEdit->clear();
    if(grammarButtonGroup->checkedButton()) { // Uncheck radio button
        grammarButtonGroup->setExclusive(false); // Allow unchecking
        grammarButtonGroup->checkedButton()->setChecked(false);
        grammarButtonGroup->setExclusive(true);
    }
}

void ExerciseRunnerWidget::showFeedback(const QString& message, bool isCorrect) {
    feedbackLabel->setText(message);
    if (isCorrect) {
        feedbackLabel->setStyleSheet("QLabel { color : green; }");
    } else {
        feedbackLabel->setStyleSheet("QLabel { color : red; }");
    }
    // Optional: Clear feedback after a few seconds
    // Corrected line:
    QTimer::singleShot(2000, this, [this, message](){ if(feedbackLabel->text() == message) feedbackLabel->setText(""); });
}
