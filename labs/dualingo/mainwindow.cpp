#include "mainwindow.h"
#include "taskdata.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QMessageBox>
#include <QDebug>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), currentScore(0), mistakesMade(0), tasksCompletedInSet(0), currentTaskIndex(-1),
    currentDifficulty("Medium"), currentExerciseType(ExerciseType::None),
    m_elapsedSecondsInExercise(0)
{
    setWindowTitle("EngliQuest - English Learning");
    setMinimumSize(700, 500);

    this->setStyleSheet(
        "QMainWindow { background-color: #f0f8ff; }"
        "QPushButton { background-color: #6495ED; color: white; border-radius: 5px; padding: 8px; font-size: 11pt; }"
        "QPushButton:hover { background-color: #4169E1; }"
        "QPushButton:disabled { background-color: #B0C4DE; }"
        "QLabel { font-size: 11pt; }"
        "QLineEdit { padding: 5px; border: 1px solid #B0C4DE; border-radius: 3px; font-size: 12pt; }"
        "QProgressBar { text-align: center; font-size: 10pt; } QProgressBar::chunk { background-color: #6495ED; }"
        "QGroupBox { font-weight: bold; font-size: 11pt; margin-top: 1ex; } QGroupBox::title { subcontrol-origin: margin; left: 7px; padding: 0 5px 0 5px; }"
        "QRadioButton { font-size: 10pt; margin-bottom: 5px;}"
        );

    ensureTasksCategorized();

    createMenus();
    setupCentralWidget();

    exerciseTimer = new QTimer(this);
    connect(exerciseTimer, &QTimer::timeout, this, &MainWindow::updateTimerDisplay);

    handleDifficultyChanged(currentDifficulty);

    resetToSetupScreen();
}

MainWindow::~MainWindow()
{
}

void MainWindow::createMenus() {
    QMenu *gameMenu = menuBar()->addMenu(tr("&Game"));

    difficultyAction = new QAction(tr("Set &Difficulty..."), this);
    connect(difficultyAction, &QAction::triggered, this, &MainWindow::showDifficultyDialog);
    gameMenu->addAction(difficultyAction);

    gameMenu->addSeparator();

    exitAction = new QAction(tr("E&xit"), this);
    connect(exitAction, &QAction::triggered, this, &QWidget::close);
    gameMenu->addAction(exitAction);
}

void MainWindow::setupCentralWidget() {
    QWidget *centralPage = new QWidget(this);
    QVBoxLayout *pageLayout = new QVBoxLayout(centralPage);

    stackedWidget = new QStackedWidget(this);
    setupWidget = new ExerciseSetupWidget(this);
    runnerWidget = new ExerciseRunnerWidget(this);

    stackedWidget->addWidget(setupWidget);
    stackedWidget->addWidget(runnerWidget);

    pageLayout->addWidget(stackedWidget, 1);

    QWidget *statusBarWidget = new QWidget(this);
    QHBoxLayout *statusLayout = new QHBoxLayout(statusBarWidget);
    statusBarWidget->setLayout(statusLayout);
    statusBarWidget->setFixedHeight(40);

    taskProgressBar = new QProgressBar(this);
    taskProgressBar->setRange(0, 100); 
    taskProgressBar->setValue(0);

    scoreLabel = new QLabel("Score: 0", this);
    timerLabel = new QLabel("Time: 00:00", this);
    mistakesLabel = new QLabel("Mistakes: 0/0", this);
    difficultyLabel = new QLabel("Difficulty: Medium", this); 

    statusLayout->addWidget(difficultyLabel, 1, Qt::AlignLeft);
    statusLayout->addStretch(1);
    statusLayout->addWidget(new QLabel("Progress:", this));
    statusLayout->addWidget(taskProgressBar, 2);
    statusLayout->addStretch(1);
    statusLayout->addWidget(scoreLabel,1);
    statusLayout->addWidget(timerLabel,1);
    statusLayout->addWidget(mistakesLabel,1);

    pageLayout->addWidget(statusBarWidget); 
    centralPage->setLayout(pageLayout);
    setCentralWidget(centralPage);

    connect(setupWidget, &ExerciseSetupWidget::startTranslationChallengeClicked, this, &MainWindow::startTranslationExercise);
    connect(setupWidget, &ExerciseSetupWidget::startGrammarChallengeClicked, this, &MainWindow::startGrammarExercise);
    connect(runnerWidget, &ExerciseRunnerWidget::submitClicked, this, &MainWindow::handleSubmitAnswer);
}

void MainWindow::createStatusBar() {}


void MainWindow::showDifficultyDialog() {
    DifficultyDialog dialog(currentDifficulty, this);
    if (dialog.exec() == QDialog::Accepted) {
        handleDifficultyChanged(dialog.selectedDifficulty());
    }
}

void MainWindow::handleDifficultyChanged(const QString& newDifficulty) {
    currentDifficulty = newDifficulty;
    difficultyLabel->setText(QString("Difficulty: %1").arg(currentDifficulty));

    if (currentDifficulty == "Easy") {
        N_tasksPerSet = 5;
        M_maxMistakes = 3;
        T_timeLimitSeconds = 120;
    } else if (currentDifficulty == "Medium") {
        N_tasksPerSet = 7; 
        M_maxMistakes = 2;
        T_timeLimitSeconds = 90;
    } else if (currentDifficulty == "Hard") {
        N_tasksPerSet = 10; 
        M_maxMistakes = 1;
        T_timeLimitSeconds = 60;
    }
    qDebug() << "Difficulty set to" << currentDifficulty << "N:" << N_tasksPerSet << "M:" << M_maxMistakes << "T:" << T_timeLimitSeconds;
    updateStatusDisplay(); 
}


void MainWindow::startTranslationExercise() {
    currentExerciseType = ExerciseType::Translation;
    loadTasksForCurrentExercise();
    beginExercise();
}

void MainWindow::startGrammarExercise() {
    currentExerciseType = ExerciseType::Grammar;
    loadTasksForCurrentExercise();
    beginExercise();
}

void MainWindow::loadTasksForCurrentExercise() {
    currentTasks = getSampleTasks(currentExerciseType, currentDifficulty, N_tasksPerSet);

    QString exerciseTypeStringForLog;
    switch (currentExerciseType) {
    case ExerciseType::Translation: exerciseTypeStringForLog = "Translation/Vocabulary"; break;
    case ExerciseType::Grammar:     exerciseTypeStringForLog = "Grammar";     break;
    case ExerciseType::None:        exerciseTypeStringForLog = "None";        break;
    default:                        exerciseTypeStringForLog = "Unknown";     break;
    }

    if (currentTasks.isEmpty()) {
        QMessageBox::critical(this, "Error",
                              QString("No tasks loaded for type '%1' and difficulty '%2'!\n"
                                      "This might be an internal issue with question data.")
                                  .arg(exerciseTypeStringForLog).arg(currentDifficulty));
        resetToSetupScreen();
        return;
    }

    if (currentTasks.size() < N_tasksPerSet) {
        qWarning() << "Warning: Loaded fewer tasks (" << currentTasks.size()
        << ") than requested (" << N_tasksPerSet
        << ") for type" << exerciseTypeStringForLog << "and difficulty" << currentDifficulty;
    }

    qInfo() << "Loaded" << currentTasks.size() << "tasks for" << exerciseTypeStringForLog << "exercise, difficulty" << currentDifficulty;
}

void MainWindow::beginExercise() {
    if (currentTasks.isEmpty()) {
        QMessageBox::warning(this, "No Tasks", "Could not start exercise: No tasks available.");
        resetToSetupScreen();
        return;
    }
    exerciseActive = true;
    currentScore = 0;
    mistakesMade = 0;
    tasksCompletedInSet = 0;
    currentTaskIndex = -1;
    m_elapsedSecondsInExercise = 0; 

    taskProgressBar->setRange(0, N_tasksPerSet);
    taskProgressBar->setValue(0);

    updateStatusDisplay(); 

    
    int initialMinutes = T_timeLimitSeconds / 60;
    int initialSeconds = T_timeLimitSeconds % 60;
    timerLabel->setText(QString("Time: %1:%2")
                            .arg(initialMinutes, 2, 10, QChar('0'))
                            .arg(initialSeconds, 2, 10, QChar('0')));

    exerciseTimer->start(1000); 
    stackedWidget->setCurrentWidget(runnerWidget);
    advanceToNextTaskOrEnd(); 
}

void MainWindow::displayCurrentTask() {
    if (currentTaskIndex < 0 || currentTaskIndex >= currentTasks.size()) {
        qCritical() << "Invalid currentTaskIndex:" << currentTaskIndex;
        endExercise("Internal error: Task index out of bounds.", false);
        return;
    }
    runnerWidget->setupForTask(currentTasks[currentTaskIndex]);
}

void MainWindow::advanceToNextTaskOrEnd() {
    currentTaskIndex++;
    tasksCompletedInSet++; 

    if (currentTaskIndex < N_tasksPerSet && currentTaskIndex < currentTasks.size()) {
        displayCurrentTask();
    } else {
        
        if (mistakesMade < M_maxMistakes) {
            endExercise(QString("Congratulations! You completed all %1 tasks!").arg(N_tasksPerSet), true);
        } else {
            endExercise(QString("Exercise finished, but with too many mistakes (%1/%2).").arg(mistakesMade).arg(M_maxMistakes), false);
        }
    }
}


void MainWindow::handleSubmitAnswer() {
    if (!exerciseActive || currentTaskIndex < 0 || currentTaskIndex >= currentTasks.size()) return;

    const TaskData& task = currentTasks[currentTaskIndex];
    bool correct = false;

    if (task.type == ExerciseType::Translation) {
        QString userAnswer = runnerWidget->getTranslationAnswer();
        correct = (userAnswer.compare(task.answer, Qt::CaseInsensitive) == 0);
    } else if (task.type == ExerciseType::Grammar) {
        int selectedIndex = runnerWidget->getGrammarAnswerIndex();
        correct = (selectedIndex == task.correctAnswerIndex);
    }

    processAnswer(correct);
}

void MainWindow::processAnswer(bool isCorrect) {
    if (!exerciseActive) return; 

    if (isCorrect) {
        currentScore += 10;
        runnerWidget->showFeedback("Correct!", true);
    } else {
        mistakesMade++;
        runnerWidget->showFeedback("Incorrect.", false);
    }

    updateStatusDisplay(); 

    if (mistakesMade >= M_maxMistakes) {
        endExercise(QString("Too many mistakes! (%1/%2)").arg(mistakesMade).arg(M_maxMistakes), false);
        return;
    }

    
    QTimer::singleShot(1200, this, &MainWindow::advanceToNextTaskOrEnd);
}


void MainWindow::updateTimerDisplay() {
    if (!exerciseActive) {
        
        
        int minutes = T_timeLimitSeconds / 60;
        int seconds = T_timeLimitSeconds % 60;
        timerLabel->setText(QString("Time: %1:%2")
                                .arg(minutes, 2, 10, QChar('0'))
                                .arg(seconds, 2, 10, QChar('0')));
        if (exerciseTimer->isActive()) exerciseTimer->stop(); 
        return;
    }

    m_elapsedSecondsInExercise++; 

    int remainingSeconds = T_timeLimitSeconds - m_elapsedSecondsInExercise;

    if (remainingSeconds < 0) {
        remainingSeconds = 0;
    }

    int minutes = remainingSeconds / 60;
    int seconds = remainingSeconds % 60;
    timerLabel->setText(QString("Time: %1:%2")
                            .arg(minutes, 2, 10, QChar('0'))
                            .arg(seconds, 2, 10, QChar('0')));

    if (remainingSeconds <= 0) {
        if (exerciseTimer->isActive()) { 
            exerciseTimer->stop();
        }
        handleExerciseTimeout(); 
    }
}

void MainWindow::handleExerciseTimeout() {
    if (!exerciseActive) return; 

    
    endExercise("Time's up!", false);
}

void MainWindow::endExercise(const QString& reasonMessage, bool awardedPoints) {
    if (!exerciseActive) return; 

    exerciseActive = false;
    if (exerciseTimer->isActive()) { 
        exerciseTimer->stop();
    }

    QString finalMessage = reasonMessage;
    if (awardedPoints) {
        finalMessage += QString("\nYour Score: %1").arg(currentScore);
    } else {
        currentScore = 0;
        finalMessage += "\nYour Score: 0";
    }

    QMessageBox::information(this, "Exercise Over", finalMessage);
    resetToSetupScreen(); 
}


void MainWindow::resetToSetupScreen() {
    exerciseActive = false;
    if(exerciseTimer->isActive()) {
        exerciseTimer->stop();
    }

    currentExerciseType = ExerciseType::None;
    currentTasks.clear();
    currentScore = 0;
    mistakesMade = 0;
    tasksCompletedInSet = 0;
    currentTaskIndex = -1;
    m_elapsedSecondsInExercise = 0; 

    
    taskProgressBar->setValue(0);
    updateStatusDisplay(); 

    stackedWidget->setCurrentWidget(setupWidget);
}


void MainWindow::updateStatusDisplay() {
    scoreLabel->setText(QString("Score: %1").arg(currentScore));
    mistakesLabel->setText(QString("Mistakes: %1/%2").arg(mistakesMade).arg(M_maxMistakes));
    if (exerciseActive) {
        taskProgressBar->setValue(currentTaskIndex +1);
        taskProgressBar->setFormat(QString("%1 / %2 Tasks").arg(qMax(0,currentTaskIndex+1)).arg(N_tasksPerSet));
    } else {
        taskProgressBar->setValue(0);
        taskProgressBar->setFormat("0 / 0 Tasks");
        
        int initialMinutes = T_timeLimitSeconds / 60;
        int initialSeconds = T_timeLimitSeconds % 60;
        timerLabel->setText(QString("Time: %1:%2")
                                .arg(initialMinutes, 2, 10, QChar('0'))
                                .arg(initialSeconds, 2, 10, QChar('0')));
    }
}


void MainWindow::keyPressEvent(QKeyEvent *event) {
    if (event->key() == Qt::Key_H) {
        if (exerciseActive && currentTaskIndex >= 0 && currentTaskIndex < currentTasks.size()) {
            const TaskData& task = currentTasks[currentTaskIndex];
            QMessageBox::information(this, "Hint", task.hint.isEmpty() ? "No hint available for this task." : task.hint);
        } else {
            QMessageBox::information(this, "Hint", "No active exercise or task to get a hint for.");
        }
    } else {
        QMainWindow::keyPressEvent(event);
    }
}











