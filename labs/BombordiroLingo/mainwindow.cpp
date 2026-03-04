#include "mainwindow.h"
#include "difficultydialog.h"
#include "helpdialog.h"
#include <QTimer>
#include <QMessageBox>
#include <QMenuBar>
#include <QKeyEvent>
#include <QLayoutItem>
#include "constants.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), totalScore(0), timeLeft(300)
{
    exerciseTimer = new QTimer(this);
    connect(exerciseTimer, &QTimer::timeout, this, &MainWindow::updateTimer);

    setupUI();
    setupMenu();
    updateScoreDisplay();
}

MainWindow::~MainWindow(){};

void MainWindow::setupUI()
{
    buttonLayout = new QHBoxLayout();
    pauseButton = new QPushButton("Pause", this);
    connect(pauseButton, &QPushButton::clicked, this, &MainWindow::togglePause);
    buttonLayout->addWidget(pauseButton);

    QWidget *centralWidget = new QWidget(this);
    QVBoxLayout *mainLayout = new QVBoxLayout(centralWidget);

    QHBoxLayout *topLayout = new QHBoxLayout();

    translationBtn = new QPushButton("Translation", this);
    grammarBtn = new QPushButton("Grammar", this);
    submitBtn = new QPushButton("Submit", this);

    scoreLabel = new QLabel("Score: 0", this);
    timerLabel = new QLabel("Time left: 05:00", this);

    topLayout->addWidget(translationBtn);
    topLayout->addWidget(grammarBtn);
    topLayout->addStretch();
    topLayout->addWidget(scoreLabel);
    topLayout->addWidget(timerLabel);

    progressBar = new QProgressBar(this);
    progressBar->setRange(0, Constants::QUESTIONS_PER_EXERCISE);
    progressBar->setValue(0);

    stackedWidget = new QStackedWidget(this);
    translationExercise = new ExerciseTranslation(this);
    grammarExercise = new ExerciseGrammar(this);
    stackedWidget->addWidget(translationExercise);
    stackedWidget->addWidget(grammarExercise);

    mainLayout->addLayout(topLayout);
    mainLayout->addWidget(progressBar);
    mainLayout->addWidget(stackedWidget);
    mainLayout->addWidget(submitBtn);

    setCentralWidget(centralWidget);

    connect(translationBtn, &QPushButton::clicked, this, &MainWindow::onTranslationClicked);
    connect(grammarBtn, &QPushButton::clicked, this, &MainWindow::onGrammarClicked);
    connect(submitBtn, &QPushButton::clicked, this, &MainWindow::onSubmitClicked);
}

void MainWindow::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_H) {
        HelpDialog helpDialog(this);

        if (stackedWidget->currentWidget() == translationExercise) {
            helpDialog.setHelpText("Translation Help:\n\nEnter the correct translation of the given text.");
        } else {
            helpDialog.setHelpText("Grammar Help:\n\nSelect the correct grammatical option for the sentence.");
        }

        helpDialog.exec();
    }
    QMainWindow::keyPressEvent(event);
}

void MainWindow::togglePause() {
    isPaused = !isPaused;
    if (isPaused) {
        exerciseTimer->stop();
        pauseButton->setText("Resume");
        submitBtn->setEnabled(false);
    } else {
        exerciseTimer->start(1000);
        pauseButton->setText("Pause");
        submitBtn->setEnabled(true);
    }
}

void MainWindow::updateTimer()
{
    timeLeft--;
    if(timeLeft <= 0) {
        exerciseTimer->stop();
        QMessageBox::information(this, "Time's up", "Exercise time has expired!");
        resetExercise();
        return;
    }

    int minutes = timeLeft / 60;
    int seconds = timeLeft % 60;
    timerLabel->setText(QString("Time left: %1:%2")
                            .arg(minutes, 2, 10, QLatin1Char('0'))
                            .arg(seconds, 2, 10, QLatin1Char('0')));
}

void MainWindow::updateScoreDisplay()
{
    scoreLabel->setText(QString("Score: %1").arg(totalScore));
}

void MainWindow::startExerciseTimer()
{
    timeLeft = Constants::EXERCISE_DURATION_SEC;
    updateTimer();
    exerciseTimer->start(1000);
}

void MainWindow::onTranslationClicked()
{
    stackedWidget->setCurrentWidget(translationExercise);
    translationExercise->startExercise(currentDifficulty);
    progressBar->setValue(0);
    startExerciseTimer();
}

void MainWindow::onGrammarClicked()
{
    stackedWidget->setCurrentWidget(grammarExercise);
    grammarExercise->startExercise(currentDifficulty);
    progressBar->setValue(0);
    startExerciseTimer();
}

void MainWindow::onSubmitClicked()
{
    bool correct = false;

    if(stackedWidget->currentWidget() == translationExercise) {
        correct = translationExercise->checkAnswer();
    }
    else if(stackedWidget->currentWidget() == grammarExercise) {
        correct = grammarExercise->checkAnswer();
    }

    if(correct) {
        progressBar->setValue(progressBar->value() + 1);

        if(progressBar->value() == progressBar->maximum()) {
            int scoreToAdd = 0;
            if(currentDifficulty == Constants::EASY_DIFFICULTY) scoreToAdd = Constants::EASY_SCORE;
            else if(currentDifficulty == Constants::MEDIUM_DIFFICULTY) scoreToAdd = Constants::MEDIUM_SCORE;
            else scoreToAdd = Constants::HARD_SCORE;

            totalScore += scoreToAdd * Constants::QUESTIONS_PER_EXERCISE;
            updateScoreDisplay();

            QMessageBox::information(this, "Success", "Exercise completed successfully!");
            resetExercise();
        }
    } else {
        QMessageBox::warning(this, "Incorrect", "Wrong answer, try again.");
    }
}

void MainWindow::resetExercise()
{
    exerciseTimer->stop();
    progressBar->setValue(0);
    timeLeft = Constants::EXERCISE_DURATION_SEC;
    updateTimer();
}

void MainWindow::setupMenu()
{
    QMenuBar *menuBar = new QMenuBar(this);

    QMenu *settingsMenu = menuBar->addMenu("Settings");
    QAction *difficultyAction = settingsMenu->addAction("Change Difficulty");
    connect(difficultyAction, &QAction::triggered, this, [this]() {
        DifficultyDialog dialog(this);
        dialog.setDifficulty(currentDifficulty);
        if (dialog.exec() == QDialog::Accepted) {
            currentDifficulty = dialog.getDifficulty();
        }
    });

    QMenu *helpMenu = menuBar->addMenu("Help");
    QAction *helpAction = helpMenu->addAction("Show Help");
    connect(helpAction, &QAction::triggered, this, [this]() {
        HelpDialog helpDialog(this);
        if (stackedWidget->currentWidget() == translationExercise) {
            helpDialog.setHelpText("Translation Help:\n\nEnter the correct translation.");
        } else {
            helpDialog.setHelpText("Grammar Help:\n\nSelect the correct grammatical option.");
        }
        helpDialog.exec();
    });

    setMenuBar(menuBar);
}
