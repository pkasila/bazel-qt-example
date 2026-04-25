#include "mainwindow.h"
#include "translation_widget.h"
#include "grammar_widget.h"
#include "help_dialog.h"

#include <QApplication>
#include <QStyleFactory>
#include <QPalette>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , stackedWidget(nullptr)
    , mainMenuWidget(nullptr)
    , translationWidget(nullptr)
    , grammarWidget(nullptr)
    , progressBar(nullptr)
    , scoreLabel(nullptr)
    , timerLabel(nullptr)
    , titleLabel(nullptr)
    , difficultyLabel(nullptr)
    , translationButton(nullptr)
    , grammarButton(nullptr)
    , helpButton(nullptr)
    , exerciseTimer(nullptr)
    , currentScore(0)
    , currentDifficulty(1)
    , timeLimit(300)
    , exerciseInProgress(false)
    , currentExerciseIndex(0)
    , totalExercises(5)
    , mistakesCount(0)
    , maxMistakes(3)
{
    setupUI();
    applyTheme();
}

MainWindow::~MainWindow()
{
}

void MainWindow::setupUI()
{
    setWindowTitle("French Princess Learning");
    setMinimumSize(800, 600);
    
    setupMenuBar();
    setupMainLayout();
    
    exerciseTimer = new QTimer(this);
    connect(exerciseTimer, &QTimer::timeout, this, &MainWindow::updateTimer);
}

void MainWindow::setupMenuBar()
{
    QMenuBar* menuBar = this->menuBar();
    
    QMenu* settingsMenu = menuBar->addMenu("Settings");
    
    QAction* difficultyAction = settingsMenu->addAction("Change Difficulty");
    connect(difficultyAction, &QAction::triggered, this, &MainWindow::showDifficultyDialog);
    
    QAction* helpAction = settingsMenu->addAction("Help (H)");
    connect(helpAction, &QAction::triggered, this, &MainWindow::showHelp);
    
    QAction* exitAction = settingsMenu->addAction("Exit");
    connect(exitAction, &QAction::triggered, this, &QWidget::close);
}

void MainWindow::setupMainLayout()
{
    QWidget* centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);
    
    QVBoxLayout* mainLayout = new QVBoxLayout(centralWidget);
    
    // Title
    titleLabel = new QLabel("French Princess Learning", this);
    titleLabel->setAlignment(Qt::AlignCenter);
    titleLabel->setStyleSheet("font-size: 24px; font-weight: bold; color: #8B4789; margin: 10px;");
    mainLayout->addWidget(titleLabel);
    
    // Difficulty label
    difficultyLabel = new QLabel(QString("Difficulty: %1").arg(currentDifficulty), this);
    difficultyLabel->setAlignment(Qt::AlignCenter);
    difficultyLabel->setStyleSheet("font-size: 14px; color: #8B4789;");
    mainLayout->addWidget(difficultyLabel);
    
    // Score and timer
    QHBoxLayout* infoLayout = new QHBoxLayout();
    
    scoreLabel = new QLabel(QString("Score: %1").arg(currentScore), this);
    scoreLabel->setStyleSheet("font-size: 16px; font-weight: bold; color: #FF69B4;");
    infoLayout->addWidget(scoreLabel);
    
    infoLayout->addStretch();
    
    timerLabel = new QLabel("Time: 5:00", this);
    timerLabel->setStyleSheet("font-size: 16px; font-weight: bold; color: #FF69B4;");
    infoLayout->addWidget(timerLabel);
    
    mainLayout->addLayout(infoLayout);
    
    // Progress bar
    progressBar = new QProgressBar(this);
    progressBar->setRange(0, totalExercises);
    progressBar->setValue(0);
    progressBar->setStyleSheet(
        "QProgressBar {"
        "border: 2px solid #8B4789;"
        "border-radius: 8px;"
        "text-align: center;"
        "color: white;"
        "font-weight: bold;"
        "}"
        "QProgressBar::chunk {"
        "background: qlineargradient(x1:0, y1:0, x2:1, y2:0, "
        "stop:0 #FF69B4, stop:1 #DA70D6);"
        "border-radius: 6px;"
        "}"
    );
    mainLayout->addWidget(progressBar);
    
    // Stacked widget for different screens
    stackedWidget = new QStackedWidget(this);
    
    // Main menu widget
    mainMenuWidget = new QWidget(this);
    QVBoxLayout* menuLayout = new QVBoxLayout(mainMenuWidget);
    
    QLabel* welcomeLabel = new QLabel("Welcome to French Princess Learning!\nChoose your exercise:", this);
    welcomeLabel->setAlignment(Qt::AlignCenter);
    welcomeLabel->setStyleSheet("font-size: 18px; color: #8B4789; margin: 20px;");
    menuLayout->addWidget(welcomeLabel);
    
    translationButton = new QPushButton("Translation Exercise", this);
    translationButton->setStyleSheet(
        "QPushButton {"
        "background: qlineargradient(x1:0, y1:0, x2:0, y2:1, "
        "stop:0 #FF69B4, stop:1 #FF1493);"
        "color: white;"
        "border: none;"
        "border-radius: 15px;"
        "padding: 15px 30px;"
        "font-size: 16px;"
        "font-weight: bold;"
        "}"
        "QPushButton:hover {"
        "background: qlineargradient(x1:0, y1:0, x2:0, y2:1, "
        "stop:0 #FF1493, stop:1 #C71585);"
        "}"
    );
    connect(translationButton, &QPushButton::clicked, this, &MainWindow::showTranslationExercise);
    menuLayout->addWidget(translationButton);
    
    grammarButton = new QPushButton("Grammar Exercise", this);
    grammarButton->setStyleSheet(
        "QPushButton {"
        "background: qlineargradient(x1:0, y1:0, x2:0, y2:1, "
        "stop:0 #DA70D6, stop:1 #8B4789);"
        "color: white;"
        "border: none;"
        "border-radius: 15px;"
        "padding: 15px 30px;"
        "font-size: 16px;"
        "font-weight: bold;"
        "}"
        "QPushButton:hover {"
        "background: qlineargradient(x1:0, y1:0, x2:0, y2:1, "
        "stop:0 #8B4789, stop:1 #663366);"
        "}"
    );
    connect(grammarButton, &QPushButton::clicked, this, &MainWindow::showGrammarExercise);
    menuLayout->addWidget(grammarButton);
    
    helpButton = new QPushButton("Help (H)", this);
    helpButton->setStyleSheet(
        "QPushButton {"
        "background: qlineargradient(x1:0, y1:0, x2:0, y2:1, "
        "stop:0 #DDA0DD, stop:1 #BA55D3);"
        "color: white;"
        "border: none;"
        "border-radius: 10px;"
        "padding: 10px 20px;"
        "font-size: 14px;"
        "}"
        "QPushButton:hover {"
        "background: qlineargradient(x1:0, y1:0, x2:0, y2:1, "
        "stop:0 #BA55D3, stop:1 #9370DB);"
        "}"
    );
    connect(helpButton, &QPushButton::clicked, this, &MainWindow::showHelp);
    menuLayout->addWidget(helpButton);
    
    menuLayout->addStretch();
    
    stackedWidget->addWidget(mainMenuWidget);
    
    // Exercise widgets
    translationWidget = new TranslationWidget(this);
    connect(translationWidget, &TranslationWidget::exerciseCompleted, this, &MainWindow::onExerciseCompleted);
    connect(translationWidget, &TranslationWidget::exerciseFailed, this, &MainWindow::onExerciseFailed);
    connect(translationWidget, &TranslationWidget::progressUpdated, this, &MainWindow::updateProgress);
    stackedWidget->addWidget(translationWidget);
    
    grammarWidget = new GrammarWidget(this);
    connect(grammarWidget, &GrammarWidget::exerciseCompleted, this, &MainWindow::onExerciseCompleted);
    connect(grammarWidget, &GrammarWidget::exerciseFailed, this, &MainWindow::onExerciseFailed);
    connect(grammarWidget, &GrammarWidget::progressUpdated, this, &MainWindow::updateProgress);
    stackedWidget->addWidget(grammarWidget);
    
    mainLayout->addWidget(stackedWidget);
    
    stackedWidget->setCurrentWidget(mainMenuWidget);
}

void MainWindow::applyTheme()
{
    QPalette palette;
    palette.setColor(QPalette::Window, QColor(255, 240, 245));
    palette.setColor(QPalette::WindowText, QColor(139, 71, 137));
    palette.setColor(QPalette::Base, QColor(255, 228, 235));
    palette.setColor(QPalette::AlternateBase, QColor(255, 218, 228));
    palette.setColor(QPalette::ToolTipBase, QColor(255, 182, 193));
    palette.setColor(QPalette::ToolTipText, QColor(139, 71, 137));
    palette.setColor(QPalette::Text, QColor(139, 71, 137));
    palette.setColor(QPalette::Button, QColor(255, 105, 180));
    palette.setColor(QPalette::ButtonText, QColor(255, 255, 255));
    palette.setColor(QPalette::BrightText, QColor(255, 255, 255));
    palette.setColor(QPalette::Link, QColor(218, 112, 214));
    palette.setColor(QPalette::Highlight, QColor(255, 105, 180));
    palette.setColor(QPalette::HighlightedText, QColor(255, 255, 255));
    
    QApplication::setPalette(palette);
}

void MainWindow::showTranslationExercise()
{
    resetExercise();
    stackedWidget->setCurrentWidget(translationWidget);
    translationWidget->startExercise(currentDifficulty);
    exerciseInProgress = true;
    exerciseTimer->start(1000);
}

void MainWindow::showGrammarExercise()
{
    resetExercise();
    stackedWidget->setCurrentWidget(grammarWidget);
    grammarWidget->startExercise(currentDifficulty);
    exerciseInProgress = true;
    exerciseTimer->start(1000);
}

void MainWindow::showDifficultyDialog()
{
    QDialog dialog(this);
    dialog.setWindowTitle("Select Difficulty");
    dialog.setModal(true);
    
    QVBoxLayout* layout = new QVBoxLayout(&dialog);
    
    QLabel* label = new QLabel("Choose difficulty level:", &dialog);
    layout->addWidget(label);
    
    QComboBox* difficultyCombo = new QComboBox(&dialog);
    difficultyCombo->addItem("Easy (1)", 1);
    difficultyCombo->addItem("Medium (2)", 2);
    difficultyCombo->addItem("Hard (3)", 3);
    difficultyCombo->setCurrentIndex(currentDifficulty - 1);
    layout->addWidget(difficultyCombo);
    
    QHBoxLayout* buttonLayout = new QHBoxLayout();
    QPushButton* okButton = new QPushButton("OK", &dialog);
    QPushButton* cancelButton = new QPushButton("Cancel", &dialog);
    
    buttonLayout->addWidget(okButton);
    buttonLayout->addWidget(cancelButton);
    layout->addLayout(buttonLayout);
    
    connect(okButton, &QPushButton::clicked, &dialog, &QDialog::accept);
    connect(cancelButton, &QPushButton::clicked, &dialog, &QDialog::reject);
    
    if (dialog.exec() == QDialog::Accepted) {
        currentDifficulty = difficultyCombo->currentData().toInt();
        difficultyLabel->setText(QString("Difficulty: %1").arg(currentDifficulty));
    }
}

void MainWindow::updateProgress(int current, int total)
{
    progressBar->setValue(current);
    currentExerciseIndex = current;
}

void MainWindow::onExerciseCompleted(bool success, int score)
{
    exerciseTimer->stop();
    exerciseInProgress = false;
    
    if (success) {
        currentScore += score;
        scoreLabel->setText(QString("Score: %1").arg(currentScore));
        
        QMessageBox::information(this, "Exercise Completed!", 
            QString("Congratulations! You earned %1 points!").arg(score));
    } else {
        QMessageBox::information(this, "Exercise Failed", 
            "You didn't complete all exercises correctly. Try again!");
    }
    
    stackedWidget->setCurrentWidget(mainMenuWidget);
    progressBar->setValue(0);
}

void MainWindow::onExerciseFailed(const QString& reason)
{
    exerciseTimer->stop();
    exerciseInProgress = false;
    
    QMessageBox::warning(this, "Exercise Failed", reason);
    
    stackedWidget->setCurrentWidget(mainMenuWidget);
    progressBar->setValue(0);
}

void MainWindow::showHelp()
{
    HelpDialog dialog(this);
    dialog.exec();
}

void MainWindow::updateTimer()
{
    static int remainingTime = timeLimit;
    remainingTime--;
    
    int minutes = remainingTime / 60;
    int seconds = remainingTime % 60;
    timerLabel->setText(QString("Time: %1:%2").arg(minutes).arg(seconds, 2, 10, QChar('0')));
    
    if (remainingTime <= 0) {
        exerciseTimer->stop();
        onExerciseFailed("Time's up! You ran out of time for this exercise.");
        remainingTime = timeLimit;
    }
}

void MainWindow::resetExercise()
{
    currentExerciseIndex = 0;
    mistakesCount = 0;
    progressBar->setValue(0);
    
    static int remainingTime = timeLimit;
    remainingTime = timeLimit;
    int minutes = remainingTime / 60;
    int seconds = remainingTime % 60;
    timerLabel->setText(QString("Time: %1:%2").arg(minutes).arg(seconds, 2, 10, QChar('0')));
}

void MainWindow::keyPressEvent(QKeyEvent* event)
{
    if (event->key() == Qt::Key_H) {
        showHelp();
    }
    QMainWindow::keyPressEvent(event);
}
