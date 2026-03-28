#include "mainwindow.h"

#include <QDebug>
#include <QHBoxLayout>
#include <QMenu>

MainWindow::MainWindow(QWidget* parent) : QMainWindow(parent) {
    setupUI();
}

MainWindow::~MainWindow() {
}

void MainWindow::setupUI() {
    setWindowTitle("Drozdini Linguini");
    setMinimumSize(600, 400);

    menuBar = new QMenuBar(this);
    setMenuBar(menuBar);
    QMenu* modeMenu = menuBar->addMenu("&Difficulty");
    QAction* beginner = modeMenu->addAction("Beginner");
    QAction* intermediate = modeMenu->addAction("Intermediate");
    QAction* advanced = modeMenu->addAction("Advanced");
    connect(beginner, &QAction::triggered, this, &MainWindow::setModeBeginner);
    connect(intermediate, &QAction::triggered, this, &MainWindow::setModeIntermediate);
    connect(advanced, &QAction::triggered, this, &MainWindow::setModeAdvanced);

    centralWidget = new QWidget(this);
    layout = new QVBoxLayout;
    centralWidget->setLayout(layout);

    // Score label
    scoreLabel = new QLabel("Score: 0", this);
    layout->addWidget(scoreLabel);

    // Stacked widget for modes
    stackedWidget = new QStackedWidget(this);
    translate = new TranslateMode(this);
    grammar = new GrammarMode(this);
    stackedWidget->addWidget(translate);
    stackedWidget->addWidget(grammar);
    layout->addWidget(stackedWidget);

    QHBoxLayout* buttonLayout = new QHBoxLayout;
    translateButton = new QPushButton("Translation", this);
    grammarButton = new QPushButton("Grammar", this);
    translateButton->setStyleSheet("QPushButton { padding: 10px; font-size: 14px; }");
    grammarButton->setStyleSheet("QPushButton { padding: 10px; font-size: 14px; }");
    buttonLayout->addWidget(translateButton);
    buttonLayout->addWidget(grammarButton);
    layout->addLayout(buttonLayout);

    connect(translateButton, &QPushButton::clicked, this, &MainWindow::changeModeTranslate);
    connect(grammarButton, &QPushButton::clicked, this, &MainWindow::changeModeGrammar);
    connect(translate, &TranslateMode::answerSubmitted, this, &MainWindow::showTranslationFeedback);
    connect(grammar, &GrammarMode::answerSubmitted, this, &MainWindow::showGrammarFeedback);
    connect(translate, &TranslateMode::scoreUpdated, this, [this](int newScore) {
        scoreLabel->setText("Score: " + QString::number(newScore));
    });
    connect(grammar, &GrammarMode::scoreUpdated, this, [this](int newScore) {
        scoreLabel->setText("Score: " + QString::number(newScore));
    });

    setCentralWidget(centralWidget);
    changeModeTranslate();
}

void MainWindow::changeModeTranslate() {
    stackedWidget->setCurrentWidget(translate);
    translate->setMode(mode);
    translate->loadTask();
}

void MainWindow::changeModeGrammar() {
    stackedWidget->setCurrentWidget(grammar);
    grammar->setMode(mode);
    grammar->startSession();
}

void MainWindow::setModeBeginner() {
    mode = 1;
    translate->setMode(mode);
    grammar->setMode(mode);
    if (stackedWidget->currentWidget() == translate) {
        translate->loadTask();
    } else {
        grammar->loadTask(mode);
    }
}

void MainWindow::setModeIntermediate() {
    mode = 2;
    translate->setMode(mode);
    grammar->setMode(mode);
    if (stackedWidget->currentWidget() == translate) {
        translate->loadTask();
    } else {
        grammar->loadTask(mode);
    }
}

void MainWindow::setModeAdvanced() {
    mode = 3;
    translate->setMode(mode);
    grammar->setMode(mode);
    if (stackedWidget->currentWidget() == translate) {
        translate->loadTask();
    } else {
        grammar->loadTask(mode);
    }
}

void MainWindow::showTranslationFeedback(bool isCorrect) {
    if (isCorrect) {
        translate->loadTask();
    }
}

void MainWindow::showGrammarFeedback(int answerId, bool isCorrect) {
    if (isCorrect) {
        grammar->loadTask(mode);
    }
}