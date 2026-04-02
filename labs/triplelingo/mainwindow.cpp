#include "mainwindow.h"
#include "difficultydialog.h"
#include "translationexercisewidget.h"
#include "grammarexercisewidget.h"
#include "exercisestructures.h"

#include <QApplication>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QStackedWidget>
#include <QProgressBar>
#include <QMenuBar>
#include <QMenu>
#include <QAction>
#include <QTimer>
#include <QMessageBox>
#include <QKeyEvent>
#include <QRandomGenerator>
#include <QDebug>
#include <algorithm>
#include <QSizePolicy>


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
    currentDifficulty("Easy"),
    currentScore(0),
    activeExerciseType(ExerciseType::None),
    currentTaskIndex(0),
    incorrectAttempts(0),
    timeLeftSeconds(0),
    currentExerciseWidgetPtr(nullptr)
{
    exerciseTimer = new QTimer(this);
    setupUi();
    setupMenu();
    setupConnections();

    updateScoreDisplay();
    updateTimerDisplay();
    showWelcomeMessage();

    setStyleSheet(R"(
        QMainWindow { background-color: #f0f0f0; }
        QLabel#ScoreLabel, QLabel#TimerLabel { font-size: 14px; font-weight: bold; }
        QLabel#TimerLabel { color: #337ab7; }
        QPushButton#TranslationButton { font-size: 14px; background-color: #5cb85c; color: white; border-radius: 5px; padding: 8px; }
        QPushButton#GrammarButton { font-size: 14px; background-color: #f0ad4e; color: white; border-radius: 5px; padding: 8px; }
        QPushButton#CancelExerciseButton { background-color: #d9534f; color: white; border-radius: 5px; padding: 6px; }
        QPushButton#ExerciseSubmitButton {
            font-size: 14px;
            background-color: #5BC0DE;
            color: white;
            border-radius: 5px;
            padding: 10px;
            border: 1px solid #46b8da;
        }
        QStackedWidget { border: 1px solid #ddd; border-radius: 5px; }
        QProgressBar {
            min-height: 20px;
            text-align: center;
            color: black;
        }
        QProgressBar::chunk {
             background-color: #5cb85c;
             border-radius: 3px;
        }
        QTextEdit { font-size: 14px; }
    )");
}

MainWindow::~MainWindow()
{
}

void MainWindow::setupUi() {
    setWindowTitle(tr("Language Learning App - Level: %1").arg(currentDifficulty));
    setGeometry(100, 100, 850, 650);

    QWidget *centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);
    QVBoxLayout *mainLayout = new QVBoxLayout(centralWidget);

    QWidget *topInfoPanel = new QWidget();
    QHBoxLayout *topInfoLayout = new QHBoxLayout(topInfoPanel);
    topInfoLayout->setContentsMargins(0,0,0,0);

    scoreLabel = new QLabel();
    scoreLabel->setObjectName("ScoreLabel");
    topInfoLayout->addWidget(scoreLabel);
    topInfoLayout->addStretch();
    timerLabel = new QLabel();
    timerLabel->setObjectName("TimerLabel");
    topInfoLayout->addWidget(timerLabel);
    mainLayout->addWidget(topInfoPanel);

    QWidget *contentAreaWidget = new QWidget();
    QHBoxLayout *contentLayout = new QHBoxLayout(contentAreaWidget);
    contentLayout->setContentsMargins(0,5,0,5);

    QWidget *buttonsPanel = new QWidget();
    QVBoxLayout *buttonsLayout = new QVBoxLayout(buttonsPanel);
    buttonsLayout->setSpacing(15);
    // buttonsLayout->setAlignment(Qt::AlignTop);

    translationButton = new QPushButton(tr("📚 Translation Exercise"));
    translationButton->setObjectName("TranslationButton");
    translationButton->setMinimumHeight(40);
    translationButton->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    buttonsLayout->addWidget(translationButton);

    grammarButton = new QPushButton(tr("✍️ Grammar Exercise"));
    grammarButton->setObjectName("GrammarButton");
    grammarButton->setMinimumHeight(40);
    grammarButton->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    buttonsLayout->addWidget(grammarButton);

    buttonsLayout->addSpacing(20);

    cancelExerciseButton = new QPushButton(tr("🚪 Quit Current Exercise"));
    cancelExerciseButton->setObjectName("CancelExerciseButton");
    cancelExerciseButton->setMinimumHeight(35);
    cancelExerciseButton->setEnabled(false);
    cancelExerciseButton->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    buttonsLayout->addWidget(cancelExerciseButton);

    contentLayout->addWidget(buttonsPanel, 1);

    exerciseStackWidget = new QStackedWidget();

    welcomeWidget = new QWidget();
    QVBoxLayout* welcomeLayout = new QVBoxLayout(welcomeWidget);
    QLabel* welcomeLabel = new QLabel(tr("<h2>Welcome to Language Learner!</h2>"
                                         "<p>Select an exercise type from the left to begin.</p>"
                                         "<p>Press 'H' for help on the current exercise once started.</p>"));
    welcomeLabel->setAlignment(Qt::AlignCenter);
    welcomeLabel->setWordWrap(true);
    welcomeLayout->addWidget(welcomeLabel);
    exerciseStackWidget->addWidget(welcomeWidget);

    translationExerciseWidget = new TranslationExerciseWidget();
    QPushButton* transSubmitButton = translationExerciseWidget->getSubmitButton();
    if (transSubmitButton) {
        transSubmitButton->setObjectName("ExerciseSubmitButton");
        transSubmitButton->setMinimumHeight(35);
        transSubmitButton->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    }
    exerciseStackWidget->addWidget(translationExerciseWidget);

    grammarExerciseWidget = new GrammarExerciseWidget();
    QPushButton* grammarSubmitButton = grammarExerciseWidget->getSubmitButton();
    if (grammarSubmitButton) {
        grammarSubmitButton->setObjectName("ExerciseSubmitButton");
        grammarSubmitButton->setMinimumHeight(35);
        grammarSubmitButton->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    }
    exerciseStackWidget->addWidget(grammarExerciseWidget);

    contentLayout->addWidget(exerciseStackWidget, 3);
    mainLayout->addWidget(contentAreaWidget, 1);

    progressBar = new QProgressBar();
    progressBar->setValue(0);
    progressBar->setTextVisible(true);
    progressBar->setFormat(tr("Progress: %p%"));
    mainLayout->addWidget(progressBar);
}

void MainWindow::setupMenu() {
    QMenuBar *menuBar = this->menuBar();
    QMenu *settingsMenu = menuBar->addMenu(tr("&Settings"));

    QAction *difficultyAction = new QAction(tr("&Change Difficulty..."), this);
    connect(difficultyAction, &QAction::triggered, this, &MainWindow::openDifficultyDialog);
    settingsMenu->addAction(difficultyAction);

    QAction *helpAction = new QAction(tr("Show &Help"), this);
    helpAction->setShortcut(QKeySequence(Qt::Key_H));
    connect(helpAction, &QAction::triggered, this, &MainWindow::showCurrentHelp);
    settingsMenu->addAction(helpAction);
}

void MainWindow::setupConnections() {
    connect(translationButton, &QPushButton::clicked, this, [this](){ startExercise(ExerciseType::Translation); });
    connect(grammarButton, &QPushButton::clicked, this, [this](){ startExercise(ExerciseType::Grammar); });
    connect(cancelExerciseButton, &QPushButton::clicked, this, &MainWindow::cancelCurrentExercise);

    connect(translationExerciseWidget->getSubmitButton(), &QPushButton::clicked, this, &MainWindow::handleSubmitAnswer);
    connect(grammarExerciseWidget->getSubmitButton(), &QPushButton::clicked, this, &MainWindow::handleSubmitAnswer);

    connect(translationExerciseWidget, &BaseExerciseWidget::helpRequested, this, &MainWindow::showCurrentHelp);
    connect(grammarExerciseWidget, &BaseExerciseWidget::helpRequested, this, &MainWindow::showCurrentHelp);

    connect(exerciseTimer, &QTimer::timeout, this, &MainWindow::updateTimerAndCheck);
}


void MainWindow::openDifficultyDialog() {
    if (exerciseTimer->isActive()) {
        QMessageBox warningMsgBox(this);
        warningMsgBox.setWindowTitle(tr("Exercise in Progress"));
        warningMsgBox.setText(tr("Cannot change difficulty while an exercise is active."));
        warningMsgBox.setIcon(QMessageBox::Warning);
        warningMsgBox.setStandardButtons(QMessageBox::Ok);
        warningMsgBox.setFixedSize(1000, 300);
        warningMsgBox.exec();
        return;
    }

    DifficultyDialog dialog(currentDifficulty, exerciseDataHandler.getDifficultyLevels(), this);
    if (dialog.exec() == QDialog::Accepted) {
        currentDifficulty = dialog.getSelectedDifficulty();
        setWindowTitle(tr("Language Learning App - Level: %1").arg(currentDifficulty));
        QMessageBox difficultyMsgBox(this);
        difficultyMsgBox.setWindowTitle(tr("Difficulty Changed"));
        difficultyMsgBox.setText(tr("Difficulty set to %1.").arg(currentDifficulty));
        difficultyMsgBox.setIcon(QMessageBox::Information);
        difficultyMsgBox.setStandardButtons(QMessageBox::Ok);
        difficultyMsgBox.setFixedSize(1000, 300);
        difficultyMsgBox.exec();
    }
}

void MainWindow::selectTasksForCurrentExercise(ExerciseType type) {
    currentTasksList.clear();
    QList<int> indices;

    if (type == ExerciseType::Translation) {
        const auto& allTasks = exerciseDataHandler.getTranslationTasks(currentDifficulty);
        if (allTasks.isEmpty()) return;
        for(int i=0; i < allTasks.size(); ++i) indices.append(i);
        std::shuffle(indices.begin(), indices.end(), *QRandomGenerator::global());

        int count = qMin(TASKS_PER_EXERCISE, allTasks.size());
        for(int i=0; i < count; ++i) {
            currentTasksList.append(QVariant::fromValue(allTasks[indices[i]]));
        }
    } else if (type == ExerciseType::Grammar) {
        const auto& allTasks = exerciseDataHandler.getGrammarTasks(currentDifficulty);
        if (allTasks.isEmpty()) return;
        for(int i=0; i < allTasks.size(); ++i) indices.append(i);
        std::shuffle(indices.begin(), indices.end(), *QRandomGenerator::global());

        int count = qMin(TASKS_PER_EXERCISE, allTasks.size());
        for(int i=0; i < count; ++i) {
            currentTasksList.append(QVariant::fromValue(allTasks[indices[i]]));
        }
    }
}


void MainWindow::startExercise(ExerciseType type) {
    activeExerciseType = type;
    selectTasksForCurrentExercise(type);

    if (currentTasksList.isEmpty()) {
        QMessageBox noTasksMsgBox(this);
        noTasksMsgBox.setWindowTitle(tr("No Tasks"));
        noTasksMsgBox.setText(tr("No %1 tasks available for %2 difficulty or not enough tasks to start.")
                                  .arg(type == ExerciseType::Translation ? "translation" : "grammar")
                                  .arg(currentDifficulty));
        noTasksMsgBox.setIcon(QMessageBox::Warning);
        noTasksMsgBox.setStandardButtons(QMessageBox::Ok);
        noTasksMsgBox.setFixedSize(1000, 300);
        noTasksMsgBox.exec();
        activeExerciseType = ExerciseType::None;
        return;
    }

    currentTaskIndex = 0;
    incorrectAttempts = 0;

    progressBar->setMaximum(currentTasksList.size());
    progressBar->setValue(0);

    timeLeftSeconds = exerciseDataHandler.getTimeLimit(currentDifficulty);
    updateTimerDisplay();
    exerciseTimer->start(1000);

    if (activeExerciseType == ExerciseType::Translation) {
        exerciseStackWidget->setCurrentWidget(translationExerciseWidget);
        currentExerciseWidgetPtr = translationExerciseWidget;
    } else if (activeExerciseType == ExerciseType::Grammar) {
        exerciseStackWidget->setCurrentWidget(grammarExerciseWidget);
        currentExerciseWidgetPtr = grammarExerciseWidget;
    }
    if(currentExerciseWidgetPtr) currentExerciseWidgetPtr->setFocus();

    setExerciseButtonsEnabled(false);
    loadNextTask();
}


void MainWindow::loadNextTask() {
    if (currentTaskIndex < currentTasksList.size()) {
        QVariant taskData = currentTasksList.at(currentTaskIndex);
        if(currentExerciseWidgetPtr) {
            currentExerciseWidgetPtr->loadTask(taskData);
            currentExerciseWidgetPtr->setFocus();
        }
        progressBar->setValue(currentTaskIndex);
        progressBar->setFormat(tr("Task %1 of %2 - %p%").arg(currentTaskIndex + 1).arg(currentTasksList.size()));
    } else {
        completeExercise(true);
    }
}

void MainWindow::handleSubmitAnswer() {
    if (!currentExerciseWidgetPtr || currentTasksList.isEmpty() || activeExerciseType == ExerciseType::None) return;

    QString userAnswer = currentExerciseWidgetPtr->getUserAnswer();
    if (userAnswer.isEmpty()) {
        QMessageBox noAnswerMsgBox(this);
        noAnswerMsgBox.setWindowTitle(tr("No Answer"));
        noAnswerMsgBox.setText(tr("Please provide an answer."));
        noAnswerMsgBox.setIcon(QMessageBox::Warning);
        noAnswerMsgBox.setStandardButtons(QMessageBox::Ok);
        noAnswerMsgBox.setFixedSize(1000, 300);
        noAnswerMsgBox.exec();
        return;
    }

    QVariant currentTaskData = currentTasksList.at(currentTaskIndex);
    QString correctAnswerText;
    bool isCorrect = false;

    if (activeExerciseType == ExerciseType::Translation && currentTaskData.canConvert<TranslationTask>()) {
        correctAnswerText = currentTaskData.value<TranslationTask>().correctAnswer;
        isCorrect = userAnswer.compare(correctAnswerText, Qt::CaseInsensitive) == 0;
    } else if (activeExerciseType == ExerciseType::Grammar && currentTaskData.canConvert<GrammarTask>()) {
        correctAnswerText = currentTaskData.value<GrammarTask>().correctAnswer;
        isCorrect = (userAnswer == correctAnswerText);
    }

    if (isCorrect) {
        QMessageBox correctMsgBox(this);
        correctMsgBox.setWindowTitle(tr("Correct!"));
        correctMsgBox.setText(tr("Well done!"));
        correctMsgBox.setIcon(QMessageBox::Information);
        correctMsgBox.setStandardButtons(QMessageBox::Ok);
        correctMsgBox.setFixedSize(1000, 300);
        correctMsgBox.exec();
        currentTaskIndex++;
        progressBar->setValue(currentTaskIndex);
        currentExerciseWidgetPtr->clearFields();
        loadNextTask();
    } else {
        incorrectAttempts++;
        QMessageBox incorrectMsgBox(this);
        incorrectMsgBox.setWindowTitle(tr("Incorrect"));
        incorrectMsgBox.setText(tr("That's not correct. Attempts left: %1")
                                    .arg(MAX_INCORRECT_ATTEMPTS - incorrectAttempts));
        incorrectMsgBox.setIcon(QMessageBox::Warning);
        incorrectMsgBox.setStandardButtons(QMessageBox::Ok);
        incorrectMsgBox.setFixedSize(1000, 300);
        incorrectMsgBox.exec();
        if (incorrectAttempts >= MAX_INCORRECT_ATTEMPTS) {
            completeExercise(false, tr("Too many incorrect attempts."));
        }
    }
}

void MainWindow::completeExercise(bool success, const QString& reason) {
    exerciseTimer->stop();
    progressBar->setValue(success ? progressBar->maximum() : currentTaskIndex);

    if (success) {
        currentScore += POINTS_PER_EXERCISE;
        updateScoreDisplay();
        QMessageBox finishedMsgBox(this);
        finishedMsgBox.setWindowTitle(tr("Exercise Finished!"));
        finishedMsgBox.setText(tr("Congratulations! You've completed all tasks correctly.\n+%1 points awarded.").arg(POINTS_PER_EXERCISE));
        finishedMsgBox.setIcon(QMessageBox::Information);
        finishedMsgBox.setStandardButtons(QMessageBox::Ok);
        finishedMsgBox.setFixedSize(1000, 300);
        finishedMsgBox.exec();
    } else {
        QMessageBox failedMsgBox(this);
        failedMsgBox.setWindowTitle(tr("Exercise Failed"));
        failedMsgBox.setText(tr("Exercise terminated. Reason: %1").arg(reason));
        failedMsgBox.setIcon(QMessageBox::Critical);
        failedMsgBox.setStandardButtons(QMessageBox::Ok);
        failedMsgBox.setFixedSize(1000, 300);
        failedMsgBox.exec();
    }
    showWelcomeMessage();
    activeExerciseType = ExerciseType::None;
    currentTasksList.clear();
    currentExerciseWidgetPtr = nullptr;
}

void MainWindow::updateTimerDisplay() {
    int minutes = timeLeftSeconds / 60;
    int seconds = timeLeftSeconds % 60;
    timerLabel->setText(tr("Time Left: %1:%2").arg(minutes, 2, 10, QChar('0')).arg(seconds, 2, 10, QChar('0')));
}

void MainWindow::updateTimerAndCheck() {
    if (timeLeftSeconds > 0) {
        timeLeftSeconds--;
        updateTimerDisplay();
    } else {
        exerciseTimer->stop();
        if(activeExerciseType != ExerciseType::None) {
            completeExercise(false, tr("Time ran out."));
        }
    }
}

void MainWindow::updateScoreDisplay() {
    scoreLabel->setText(tr("Score: %1").arg(currentScore));
}

void MainWindow::setExerciseButtonsEnabled(bool enabled) {
    translationButton->setEnabled(enabled);
    grammarButton->setEnabled(enabled);
    cancelExerciseButton->setEnabled(!enabled);
}

void MainWindow::showWelcomeMessage() {
    exerciseStackWidget->setCurrentWidget(welcomeWidget);
    progressBar->setValue(0);
    progressBar->setFormat(tr("Progress: %p%"));
    timerLabel->setText(tr("Time Left: --:--"));
    timeLeftSeconds = 0;
    setExerciseButtonsEnabled(true);
    activeExerciseType = ExerciseType::None;
    currentExerciseWidgetPtr = nullptr;
}

void MainWindow::showCurrentHelp() {
    if (activeExerciseType == ExerciseType::None || !currentExerciseWidgetPtr || currentTasksList.isEmpty() || currentTaskIndex >= currentTasksList.size()) {
        QMessageBox helpNoActiveMsgBox(this);
        helpNoActiveMsgBox.setWindowTitle(tr("Help"));
        helpNoActiveMsgBox.setText(tr("No active exercise or task to show help for.\nStart an exercise first. Global help: Press 'H' key during an exercise for task-specific help."));
        helpNoActiveMsgBox.setIcon(QMessageBox::Information);
        helpNoActiveMsgBox.setStandardButtons(QMessageBox::Ok);
        helpNoActiveMsgBox.setFixedSize(1000, 300);
        helpNoActiveMsgBox.exec();
        return;
    }

    QString helpText = currentExerciseWidgetPtr->getCurrentTaskHelpText();
    if (helpText.isEmpty()){
        if (activeExerciseType == ExerciseType::Translation) {
            helpText = "Try to translate the given text. Focus on meaning and basic grammar.";
        } else {
            helpText = "Choose the grammatically correct option.";
        }
    }

    QMessageBox helpTaskMsgBox(this);
    helpTaskMsgBox.setWindowTitle(tr("Help for Current Task"));
    helpTaskMsgBox.setText(helpText);
    helpTaskMsgBox.setIcon(QMessageBox::Information);
    helpTaskMsgBox.setStandardButtons(QMessageBox::Ok);
    helpTaskMsgBox.setFixedSize(1000, 300);
    helpTaskMsgBox.exec();
}

void MainWindow::cancelCurrentExercise() {
    if (activeExerciseType == ExerciseType::None && !exerciseTimer->isActive()) {
        return;
    }

    exerciseTimer->stop();

    QMessageBox quitQueryMsgBox(this);
    quitQueryMsgBox.setWindowTitle(tr("Quit Exercise"));
    quitQueryMsgBox.setText(tr("Are you sure you want to quit the current exercise? Your progress in this exercise will be lost."));
    quitQueryMsgBox.setIcon(QMessageBox::Question);
    quitQueryMsgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
    quitQueryMsgBox.setDefaultButton(QMessageBox::No);
    quitQueryMsgBox.setFixedSize(1000, 300);
    QMessageBox::StandardButton reply = static_cast<QMessageBox::StandardButton>(quitQueryMsgBox.exec());

    if (reply == QMessageBox::Yes) {
        QMessageBox stoppedMsgBox(this);
        stoppedMsgBox.setWindowTitle(tr("Exercise Stopped"));
        stoppedMsgBox.setText(tr("The current exercise has been stopped."));
        stoppedMsgBox.setIcon(QMessageBox::Information);
        stoppedMsgBox.setStandardButtons(QMessageBox::Ok);
        stoppedMsgBox.setFixedSize(1000, 300);
        stoppedMsgBox.exec();
        showWelcomeMessage();
    } else {
        if(timeLeftSeconds > 0) {
            exerciseTimer->start(1000);
        } else if (activeExerciseType != ExerciseType::None) {
            completeExercise(false, tr("Time ran out while confirming exit."));
        }
    }
}


void MainWindow::keyPressEvent(QKeyEvent *event) {
    if (event->key() == Qt::Key_H) {
        showCurrentHelp();
    } else {
        QMainWindow::keyPressEvent(event);
    }
}

void MainWindow::closeEvent(QCloseEvent *event) {
    if (exerciseTimer->isActive()) {
        QMessageBox queryCloseMsgBox(this);
        queryCloseMsgBox.setWindowTitle(tr("Exercise in Progress"));
        queryCloseMsgBox.setText(tr("An exercise is currently active. Are you sure you want to quit?"));
        queryCloseMsgBox.setIcon(QMessageBox::Question);
        queryCloseMsgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
        queryCloseMsgBox.setDefaultButton(QMessageBox::No);
        queryCloseMsgBox.setFixedSize(1000, 300);
        QMessageBox::StandardButton reply = static_cast<QMessageBox::StandardButton>(queryCloseMsgBox.exec());

        if (reply == QMessageBox::Yes) {
            event->accept();
        } else {
            event->ignore();
        }
    } else {
        event->accept();
    }
}
