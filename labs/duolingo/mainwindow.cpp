#include "mainwindow.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QMenuBar>
#include <QInputDialog>
#include <QMessageBox>
#include <QKeyEvent>
#include <QRandomGenerator>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    resize(1000, 600);

    score = 0;
    mistakes = 0;
    currentTask = 0;

    totalTasks = 5;
    maxMistakes = 3;

    difficulty = "Easy";

    translationTasks =
        {
            {"Hello", "Привет"},
            {"Good morning", "Доброе утро"},
            {"Thank you", "Спасибо"},
            {"I love programming", "Я люблю программирование"},
            {"How are you?", "Как дела?"}
        };

    grammarTasks =
        {
            {"He ___ to school every day.",
             "go",
             "goes",
             "going",
             2},

            {"They ___ football yesterday.",
             "played",
             "play",
             "playing",
             1},

            {"She ___ coffee now.",
             "drink",
             "drinks",
             "is drinking",
             3}
        };

    setupUI();
    setupMenu();

    setStyleSheet(
        "QMainWindow { background-color: #f4f6f8; }"

        "QPushButton {"
        "background-color: #58cc02;"
        "color: white;"
        "font-size: 16px;"
        "padding: 10px;"
        "border-radius: 10px;"
        "}"

        "QPushButton:hover {"
        "background-color: #46a302;"
        "}"

        "QLabel {"
        "font-size: 18px;"
        "}"

        "QTextEdit {"
        "font-size: 16px;"
        "border: 2px solid gray;"
        "border-radius: 10px;"
        "}"

        "QProgressBar {"
        "height: 25px;"
        "font-size: 14px;"
        "}"
        );
}

void MainWindow::setupUI()
{
    centralWidget = new QWidget;
    setCentralWidget(centralWidget);

    QHBoxLayout *mainLayout = new QHBoxLayout(centralWidget);

    // left menu
    QWidget *leftPanel = new QWidget;
    QVBoxLayout *leftLayout = new QVBoxLayout(leftPanel);

    translationBtn = new QPushButton("Translation");
    grammarBtn = new QPushButton("Grammar");

    leftLayout->addWidget(translationBtn);
    leftLayout->addWidget(grammarBtn);
    leftLayout->addStretch();

    // stacked widget
    stack = new QStackedWidget;

    // translation page
    translationPage = new QWidget;
    QVBoxLayout *translationLayout = new QVBoxLayout(translationPage);

    translationQuestion = new QLabel;
    translationInput = new QTextEdit;
    translationSubmit = new QPushButton("Submit");

    translationLayout->addWidget(translationQuestion);
    translationLayout->addWidget(translationInput);
    translationLayout->addWidget(translationSubmit);

    // grammar page
    grammarPage = new QWidget;
    QVBoxLayout *grammarLayout = new QVBoxLayout(grammarPage);

    grammarQuestion = new QLabel;

    r1 = new QRadioButton;
    r2 = new QRadioButton;
    r3 = new QRadioButton;

    group = new QButtonGroup(this);

    group->addButton(r1);
    group->addButton(r2);
    group->addButton(r3);

    grammarSubmit = new QPushButton("Submit");

    grammarLayout->addWidget(grammarQuestion);
    grammarLayout->addWidget(r1);
    grammarLayout->addWidget(r2);
    grammarLayout->addWidget(r3);
    grammarLayout->addWidget(grammarSubmit);

    stack->addWidget(translationPage);
    stack->addWidget(grammarPage);

    // bottom info
    QWidget *infoWidget = new QWidget;
    QHBoxLayout *infoLayout = new QHBoxLayout(infoWidget);

    scoreLabel = new QLabel("Score: 0");

    timerLabel = new QLabel("Time: 60");

    progressBar = new QProgressBar;
    progressBar->setRange(0, totalTasks);

    infoLayout->addWidget(scoreLabel);
    infoLayout->addWidget(timerLabel);
    infoLayout->addWidget(progressBar);

    // right side
    QVBoxLayout *rightLayout = new QVBoxLayout;

    rightLayout->addWidget(stack);
    rightLayout->addWidget(infoWidget);

    mainLayout->addWidget(leftPanel, 1);
    mainLayout->addLayout(rightLayout, 4);

    // timer
    timer = new QTimer(this);

    connect(timer,
            &QTimer::timeout,
            this,
            &MainWindow::updateTimer);

    connect(translationBtn,
            &QPushButton::clicked,
            this,
            &MainWindow::startTranslation);

    connect(grammarBtn,
            &QPushButton::clicked,
            this,
            &MainWindow::startGrammar);

    connect(translationSubmit,
            &QPushButton::clicked,
            this,
            &MainWindow::submitTranslation);

    connect(grammarSubmit,
            &QPushButton::clicked,
            this,
            &MainWindow::submitGrammar);
}

void MainWindow::setupMenu()
{
    QMenu *menu = menuBar()->addMenu("Settings");

    QAction *difficultyAction =
        new QAction("Change difficulty", this);

    menu->addAction(difficultyAction);

    connect(difficultyAction,
            &QAction::triggered,
            this,
            &MainWindow::changeDifficulty);
}

void MainWindow::startTranslation()
{
    translationSubmit->setEnabled(true);

    stack->setCurrentWidget(translationPage);

    currentTask = 0;
    mistakes = 0;

    progressBar->setRange(0, totalTasks);
    progressBar->setValue(0);

    timeLeft = 60;

    timer->start(1000);

    loadTranslationTask();
}

void MainWindow::startGrammar()
{
    grammarSubmit->setEnabled(true);

    stack->setCurrentWidget(grammarPage);

    currentTask = 0;
    mistakes = 0;

    progressBar->setRange(0, grammarTasks.size());
    progressBar->setValue(0);

    timeLeft = 60;

    timer->start(1000);

    loadGrammarTask();
}

void MainWindow::loadTranslationTask()
{
    if(currentTask >= totalTasks)
    {
        score += 20;

        scoreLabel->setText(
            "Score: " + QString::number(score));

        finishExercise("Translation completed!");
        return;
    }

    translationQuestion->setText(
        "Translate: " +
        translationTasks[currentTask].first);

    translationInput->clear();
}

void MainWindow::submitTranslation()
{
    QString answer =
        translationInput->toPlainText();

    QString correct =
        translationTasks[currentTask].second;

    if(compareAnswers(answer, correct))
    {
        currentTask++;

        progressBar->setValue(currentTask);

        loadTranslationTask();
    }
    else
    {
        mistakes++;

        QMessageBox::warning(
            this,
            "Wrong",
            "Incorrect translation");

        if(mistakes >= maxMistakes)
        {
            finishExercise(
                "Too many mistakes!");
        }
    }
}

void MainWindow::loadGrammarTask()
{
    if(currentTask >= grammarTasks.size())
    {
        score += 20;

        scoreLabel->setText(
            "Score: " + QString::number(score));

        finishExercise("Grammar completed!");
        return;
    }

    GrammarTask t = grammarTasks[currentTask];

    grammarQuestion->setText(t.question);

    r1->setText(t.a1);
    r2->setText(t.a2);
    r3->setText(t.a3);

    group->setExclusive(false);

    r1->setChecked(false);
    r2->setChecked(false);
    r3->setChecked(false);

    group->setExclusive(true);
}

void MainWindow::submitGrammar()
{
    GrammarTask t = grammarTasks[currentTask];

    bool correct = false;

    if(t.correct == 1 && r1->isChecked())
        correct = true;

    if(t.correct == 2 && r2->isChecked())
        correct = true;

    if(t.correct == 3 && r3->isChecked())
        correct = true;

    if(correct)
    {
        currentTask++;

        progressBar->setValue(currentTask);

        loadGrammarTask();
    }
    else
    {
        mistakes++;

        QMessageBox::warning(
            this,
            "Wrong",
            "Incorrect answer");

        if(mistakes >= maxMistakes)
        {
            finishExercise(
                "Too many mistakes!");
        }
    }
}

void MainWindow::updateTimer()
{
    timeLeft--;

    timerLabel->setText(
        "Time: " + QString::number(timeLeft));

    if(timeLeft <= 0)
    {
        finishExercise("Time is over!");
    }
}

void MainWindow::finishExercise(QString text)
{
    timer->stop();

    translationSubmit->setEnabled(false);
    grammarSubmit->setEnabled(false);

    QMessageBox::information(
        this,
        "Exercise finished",
        text);
}

void MainWindow::changeDifficulty()
{
    QStringList levels;

    levels << "Easy"
           << "Medium"
           << "Hard";

    bool ok;

    QString level =
        QInputDialog::getItem(
            this,
            "Difficulty",
            "Choose level:",
            levels,
            0,
            false,
            &ok);

    if(ok)
    {
        difficulty = level;

        if(level == "Easy")
        {
            timeLeft = 60;
            maxMistakes = 5;
        }

        if(level == "Medium")
        {
            timeLeft = 45;
            maxMistakes = 3;
        }

        if(level == "Hard")
        {
            timeLeft = 30;
            maxMistakes = 2;
        }

        QMessageBox::information(
            this,
            "Difficulty",
            "Selected: " + level);
    }
}

bool MainWindow::compareAnswers(QString a, QString b)
{
    a = a.toLower().trimmed();
    b = b.toLower().trimmed();

    return a == b;
}

void MainWindow::keyPressEvent(QKeyEvent *event)
{
    if(event->key() == Qt::Key_H)
    {
        QMessageBox::information(
            this,
            "Help",
            "Hint:\n"
            "Present Simple:\n"
            "He/She/It + verb+s");
    }

    QMainWindow::keyPressEvent(event);
}
