#include "mainwindow.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QMenuBar>
#include <QAction>
#include <QMessageBox>
#include <QInputDialog>
#include <QDebug>
#include <QCoreApplication> 
#include <QUrl> 


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
      stackedWidget(new QStackedWidget(this)),
      startMenuWidget(new StartMenuWidget(this)),
      translationExerciseWidget(new TranslationExerciseWidget(this)),
      grammarExerciseWidget(new GrammarExerciseWidget(this)),
      progressBar(new QProgressBar(this)),
      scoreLabel(new QLabel("Счет: 0", this)),
      timerLabel(new QLabel("Время: --:--", this)),
      exerciseTimer(new QTimer(this)),
      timeLeft(0),
      totalTimerDuration(0),
      currentExerciseIndex(0),
      totalExercisesInSet(0),
      currentScore(0),
      incorrectAttempts(0),
      maxIncorrectAttempts(3),
      currentExerciseType(-1),
      currentDifficulty(DifficultyDialog::Easy) 
{
    setupUi();
    setupMenu();
    setupData(); 

    
    connect(startMenuWidget, &StartMenuWidget::translationExerciseRequested, this, &MainWindow::showTranslationExercise);
    connect(startMenuWidget, &StartMenuWidget::grammarExerciseRequested, this, &MainWindow::showGrammarExercise);

    connect(translationExerciseWidget, &TranslationExerciseWidget::answerSubmitted, this, &MainWindow::handleTranslationSubmit);
    connect(grammarExerciseWidget, &GrammarExerciseWidget::answerSubmitted, this, &MainWindow::handleGrammarSubmit);

    connect(exerciseTimer, &QTimer::timeout, this, &MainWindow::updateTimer);
   
}

MainWindow::~MainWindow()
{}

void MainWindow::setupUi()
{
 
    QWidget *centralWidget = new QWidget(this);
    QVBoxLayout *mainLayout = new QVBoxLayout(centralWidget);
    setCentralWidget(centralWidget);

    QHBoxLayout *statusBarLayout = new QHBoxLayout();
    progressBar->setRange(0, 100);
    progressBar->setValue(0);
    progressBar->setTextVisible(true);
    progressBar->hide();
    scoreLabel->hide(); 
    timerLabel->hide(); 

    statusBarLayout->addWidget(scoreLabel);
    statusBarLayout->addStretch();
    statusBarLayout->addWidget(timerLabel);
    statusBarLayout->addStretch();
    statusBarLayout->addWidget(progressBar);


    mainLayout->addLayout(statusBarLayout);

    stackedWidget->addWidget(startMenuWidget);        
    stackedWidget->addWidget(translationExerciseWidget);
    stackedWidget->addWidget(grammarExerciseWidget); 

    mainLayout->addWidget(stackedWidget);

    stackedWidget->setCurrentIndex(Page_StartMenu);

    setWindowTitle("Duolingo");
    resize(800, 600);
}

void MainWindow::setupMenu()
{
    QMenu *fileMenu = menuBar()->addMenu("Файл");
    QAction *difficultyAction = new QAction("Изменить сложность...", this);
    fileMenu->addAction(difficultyAction);
    connect(difficultyAction, &QAction::triggered, this, &MainWindow::changeDifficulty);

    QMenu *helpMenu = menuBar()->addMenu("Справка");
    QAction *aboutAction = new QAction("О программе", this);
    helpMenu->addAction(aboutAction);
    connect(aboutAction, &QAction::triggered, this, [this](){
        QMessageBox::about(this, "О программе", "Duolingo - приложение для изучения языка.\n");
    });
}

void MainWindow::setupData()
{
    translationExercises.append({"Hello", "Привет", "Basic greeting.", ""});
    translationExercises.append({"Goodbye", "Пока", "Basic farewell.", ""});
    grammarExercises.append({"I __ (to be) a student", {"am", "are", "were"}, "был", "Masculine singular present tense of 'to be'.", ""});

    // средний и сложный уровни потом добавлю
}

void MainWindow::startExercise(int type)
{
    currentExerciseType = type;
    currentExerciseIndex = 0;
    currentScore = 0;
    incorrectAttempts = 0;

    exerciseTimer->stop();
    timeLeft = 0;
    totalTimerDuration = 60;
    timeLeft = totalTimerDuration;
    exerciseTimer->start(1000);

    QList<TranslationData> selectedTranslation;
    QList<GrammarData> selectedGrammar;

    if (currentExerciseType == 0) { 
        selectedTranslation = translationExercises;
        totalExercisesInSet = selectedTranslation.size();
        if (totalExercisesInSet == 0) {
            QMessageBox::information(this, "Ошибка", "Нет доступных упражнений для перевода на этом уровне сложности.");
            endExercise("Нет упражнений.");
            return;
        }
        translationExerciseWidget->setExerciseData(selectedTranslation[currentExerciseIndex]);
        stackedWidget->setCurrentIndex(Page_TranslationExercise);
    } else if (currentExerciseType == 1) { 
        selectedGrammar = grammarExercises;
        totalExercisesInSet = selectedGrammar.size();
         if (totalExercisesInSet == 0) {
            QMessageBox::information(this, "Ошибка", "Нет доступных грамматических упражнений на этом уровне сложности.");
            endExercise("Нет упражнений.");
            return;
        }
        grammarExerciseWidget->setExerciseData(selectedGrammar[currentExerciseIndex]);
        stackedWidget->setCurrentIndex(Page_GrammarExercise);
    } else {
        return; 
    }
    progressBar->setRange(0, totalExercisesInSet);
    progressBar->setValue(0);
    scoreLabel->setText(QString("Счет: %1").arg(currentScore));
    updateTimer();
    progressBar->show();
    scoreLabel->show();
    timerLabel->show();

    qDebug() << "Exercise started: type" << currentExerciseType << ", total" << totalExercisesInSet;
}

void MainWindow::loadNextExercise()
{
    currentExerciseIndex++;

    if (currentExerciseIndex < totalExercisesInSet) {
        qDebug() << "Loading next exercise:" << currentExerciseIndex;
        if (currentExerciseType == 0) {
            translationExerciseWidget->setExerciseData(translationExercises[currentExerciseIndex]);
        } else if (currentExerciseType == 1) {
            grammarExerciseWidget->setExerciseData(grammarExercises[currentExerciseIndex]);
        }

        
        progressBar->setValue(currentExerciseIndex);

    } else {
        currentScore = totalExercisesInSet; 
        endExercise(QString("Поздравляем! Вы выполнили все %1 заданий.").arg(totalExercisesInSet));
    }
}

void MainWindow::endExercise(const QString& message)
{
    exerciseTimer->stop();
    qDebug() << "Exercise ended. Message:" << message;


    QMessageBox::information(this, "Упражнение завершено",
                             QString("%1\nИтоговый счет: %2").arg(message).arg(currentScore));

    currentExerciseType = -1;
    currentExerciseIndex = 0;
    totalExercisesInSet = 0;
    currentScore = 0;
    incorrectAttempts = 0;


    progressBar->hide();
    scoreLabel->hide();
    timerLabel->hide();


    stackedWidget->setCurrentIndex(Page_StartMenu);
}

void MainWindow::checkAnswer(const QString& userAnswer)
{
    QString correctAnswer;
    if (currentExerciseType == 0) {
        correctAnswer = translationExercises[currentExerciseIndex].correctAnswer;
    } else if (currentExerciseType == 1) { 
        correctAnswer = grammarExercises[currentExerciseIndex].correctAnswer;
    } else {
        return; 
    }


    QString cleanUserAnswer = userAnswer.trimmed().toLower();
    QString cleanCorrectAnswer = correctAnswer.trimmed().toLower();

    bool isCorrect = (cleanUserAnswer == cleanCorrectAnswer);

    if (isCorrect) {
        qDebug() << "Correct answer!";
        if (currentExerciseType == 0) translationExerciseWidget->showFeedback("Верно!", true);
        else grammarExerciseWidget->showFeedback("Верно!", true);
        loadNextExercise();
    } else {
        qDebug() << "Incorrect answer. User:" << userAnswer << ", Correct:" << correctAnswer;
        incorrectAttempts++;
        if (currentExerciseType == 0) translationExerciseWidget->showFeedback(QString("Неверно. Правильно: \"%1\"").arg(correctAnswer), false);
        else grammarExerciseWidget->showFeedback(QString("Неверно. Правильно: \"%1\"").arg(correctAnswer), false);

        if (incorrectAttempts >= maxIncorrectAttempts) {
            endExercise(QString("Превышено количество неверных попыток (%1).").arg(maxIncorrectAttempts));
        } else {
             QMessageBox::warning(this, "Неверный ответ",
                                 QString("Неверный ответ. Осталось попыток: %1")
                                 .arg(maxIncorrectAttempts - incorrectAttempts));
            loadNextExercise();
        }
    }
    scoreLabel->setText(QString("Счет: %1").arg(currentScore));
}

void MainWindow::handleTranslationSubmit(const QString& userAnswer)
{
    checkAnswer(userAnswer);
}

void MainWindow::handleGrammarSubmit(const QString& userAnswer)
{
    checkAnswer(userAnswer);
}

void MainWindow::updateTimer()
{
    timeLeft--;
    if (timeLeft >= 0) {
        int minutes = timeLeft / 60;
        int seconds = timeLeft % 60;
        timerLabel->setText(QString("Время: %1:%2").arg(minutes, 2, 10, QChar('0')).arg(seconds, 2, 10, QChar('0')));
        progressBar->setValue(totalExercisesInSet - (totalExercisesInSet * timeLeft / totalTimerDuration)); 
    } else {
        exerciseTimer->stop();
        endExercise("Время вышло!");
    }
}

void MainWindow::timerFinished()
{
}

void MainWindow::changeDifficulty()
{
    DifficultyDialog dialog(currentDifficulty, this);
    if (dialog.exec() == QDialog::Accepted) {
        currentDifficulty = dialog.getSelectedDifficulty();
        qDebug() << "Difficulty changed to:" << currentDifficulty;
        QMessageBox::information(this, "Сложность изменена", QString("Новый уровень сложности: %1")
                                 .arg(dialog.getDifficultyString(currentDifficulty)));
    }
}

void MainWindow::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_H) {
        if (currentExerciseType != -1 && currentExerciseIndex < totalExercisesInSet) {
            QString helpText = getCurrentExerciseHelpText();
            if (!helpText.isEmpty()) {
                HelpDialog dialog(helpText, this);
                dialog.exec();
            } else {
                QMessageBox::information(this, "Справка", "Для этого упражнения справка недоступна.");
            }
        } else {
             QMessageBox::information(this, "Справка", "Справка доступна только во время активного упражнения.");
        }
    } else {
        QMainWindow::keyPressEvent(event);
    }
}

QString MainWindow::getCurrentExerciseHelpText() const
{
    if (currentExerciseType == 0 && currentExerciseIndex < translationExercises.size()) {
        return translationExercises[currentExerciseIndex].helpText;
    } else if (currentExerciseType == 1 && currentExerciseIndex < grammarExercises.size()) {
        return grammarExercises[currentExerciseIndex].helpText;
    }
    return "";
}

QString MainWindow::getCurrentExerciseAudioFile() const
{
    if (currentExerciseType == 0 && currentExerciseIndex < translationExercises.size()) {
        return translationExercises[currentExerciseIndex].audioFile;
    } else if (currentExerciseType == 1 && currentExerciseIndex < grammarExercises.size()) {
        return grammarExercises[currentExerciseIndex].audioFile;
    }
    return "";
}
void MainWindow::showTranslationExercise()
{
    qDebug() << "Starting Translation Exercise...";
    startExercise(0); 
}

void MainWindow::showGrammarExercise()
{
    qDebug() << "Starting Grammar Exercise...";
    startExercise(1); 
}