#include "exercisegrammar.h"
#include "constants.h"
#include <QLabel>
#include <QRadioButton>
#include <QButtonGroup>
#include <QVBoxLayout>
#include <QMessageBox>

ExerciseGrammar::ExerciseGrammar(QWidget *parent)
    : QWidget(parent), currentQuestion(0), attemptsLeft(Constants::MAX_ATTEMPTS)
{
    layout = new QVBoxLayout(this);

    questionLabel = new QLabel(this);
    questionLabel->setStyleSheet("font-size: 16px; color: black;");
    layout->addWidget(questionLabel);

    optionsGroup = new QButtonGroup(this);

    for (int i = 0; i < 4; ++i) {
        QRadioButton *radio = new QRadioButton(this);
        radio->setStyleSheet("font-size: 14px; color: black;");
        optionButtons.append(radio);
        optionsGroup->addButton(radio, i);
        layout->addWidget(radio);
    }

    setLayout(layout);
}

void ExerciseGrammar::generateQuestions(const QString &difficulty)
{
    questions.clear();
    correctAnswers.clear();

    if (difficulty == Constants::EASY_DIFFICULTY) {
        questions << "Choose correct form:\n\nI ___ a student."
                  << "Select right option:\n\nThey ___ happy.";

        correctAnswers << 0 << 2;

        optionButtons[0]->setText("am");
        optionButtons[1]->setText("is");
        optionButtons[2]->setText("are");
        optionButtons[3]->setText("be");
    }
    else if (difficulty == Constants::MEDIUM_DIFFICULTY) {
        questions << "Choose correct past form:\n\nShe ___ to the store yesterday."
                  << "Select right option:\n\nWe ___ dinner at 7pm yesterday.";

        correctAnswers << 2 << 2;

        optionButtons[0]->setText("go");
        optionButtons[1]->setText("goed");
        optionButtons[2]->setText("went");
        optionButtons[3]->setText("gone");
    }
    else { // HARD_DIFFICULTY
        questions << "Choose correct subjunctive:\n\nIf I ___ you, I would reconsider."
                  << "Select right tense:\n\nBy next year, I ___ here for five years.";

        correctAnswers << 2 << 1;

        optionButtons[0]->setText("am");
        optionButtons[1]->setText("was");
        optionButtons[2]->setText("were");
        optionButtons[3]->setText("would be");
    }

    showNextQuestion();
}

bool ExerciseGrammar::checkAnswer()
{
    if (currentQuestion < 0 || currentQuestion >= correctAnswers.size()) {
        qWarning("Invalid question index!");
        return false;
    }

    int selectedId = optionsGroup->checkedId();
    if (selectedId == -1) {
        QMessageBox::warning(this, "Error", "Please select an answer!");
        return false;
    }

    bool isCorrect = (selectedId == correctAnswers[currentQuestion]);

    if (isCorrect) {
        currentQuestion++;
        if (currentQuestion < questions.size()) {
            showNextQuestion();
        }
        return true;
    } else {
        attemptsLeft--;
        if (attemptsLeft < 0) {
            QMessageBox::warning(this, "Attempts exceeded", "No more attempts left!");
            return false;
        }
        QMessageBox::warning(this, "Incorrect", "Wrong answer, try again.");
        return false;
    }
}

void ExerciseGrammar::showNextQuestion()
{
    if (currentQuestion >= 0 && currentQuestion < questions.size()) {
        questionLabel->setText(questions[currentQuestion]);

        optionsGroup->setExclusive(false);
        for (auto button : optionButtons) {
            button->setChecked(false);
        }
        optionsGroup->setExclusive(true);
    }
}

void ExerciseGrammar::startExercise(const QString &difficulty)
{
    currentQuestion = 0;
    attemptsLeft = Constants::MAX_ATTEMPTS;
    generateQuestions(difficulty);
}
