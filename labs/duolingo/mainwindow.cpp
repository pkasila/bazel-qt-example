#include "mainwindow.h"
#include "translationexercise.h"
#include "grammarexercise.h"
#include <QMenu>
#include <QMenuBar>
#include <QAction>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QMessageBox>
#include <QInputDialog>
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonParseError>
#include <QDebug>
#include <cmath>


MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), progressData({0, 1}) {
    initializeUI();
    loadGameProgress();
    updateProgressDisplay();


    stackedWidget->addWidget(startScreenWidget);


    translationExerciseWidget = new TranslationExercise(progressData.difficultyLevel, this);
    grammarExerciseWidget = new GrammarExercise(progressData.difficultyLevel, this);


    if (translationExerciseWidget && grammarExerciseWidget) {
        connect(translationExerciseWidget, &TranslationExercise::exerciseFinished, this, &MainWindow::exerciseCompleted);
        connect(grammarExerciseWidget, &GrammarExercise::exerciseFinished, this, &MainWindow::exerciseCompleted);
        connect(translationExerciseWidget, &TranslationExercise::gameOverSignal, this, &MainWindow::handleGameOver);
        connect(grammarExerciseWidget, &GrammarExercise::gameOverSignal, this, &MainWindow::handleGameOver);


        stackedWidget->addWidget(translationExerciseWidget);
        stackedWidget->addWidget(grammarExerciseWidget);
    } else {
        qDebug() << "Error: Failed to create exercise widgets!";
    }


    stackedWidget->setCurrentIndex(0); // Show start screen initially
    setCentralWidget(stackedWidget);
    createMenuActions();


    setStyleSheet(R"(
  QMainWindow { background-color: #f5f5f5; }
  QLabel { font-size: 16px; color: #333; }
  QPushButton {
  background-color: #4CAF50;
  color: white;
  padding: 15px;
  border-radius: 10px;
  font-size: 18px;
  min-width: 250px;
  }
  QPushButton:hover { background-color: #45a049; }
  QProgressBar { border: 1px solid #ccc; border-radius: 5px; text-align: center; }
  QProgressBar::chunk { background-color: #4CAF50; border-radius: 4px; }
  QMenuBar { background-color: #e0e0e0; }
  )");
}


MainWindow::~MainWindow() {
    delete translationExerciseWidget;
    delete grammarExerciseWidget;
}


void MainWindow::initializeUI() {
    stackedWidget = new QStackedWidget(this);
    startScreenWidget = new QWidget(this);
    QVBoxLayout* mainLayout = new QVBoxLayout(startScreenWidget);
    mainLayout->setContentsMargins(30, 30, 30, 30); // Add some padding around the main layout


    mainLayout->addStretch(1); // Add stretch at the top to push content to the center


    titleLabel = new QLabel("Language Learning App", this);
    titleLabel->setStyleSheet("font-size: 36px; font-weight: bold; color: #2c3e50;");
    titleLabel->setAlignment(Qt::AlignCenter);
    mainLayout->addWidget(titleLabel);
    mainLayout->addSpacing(40);


    QHBoxLayout* progressLayout = new QHBoxLayout();
    QLabel* progressLabel = new QLabel("Прогресс:", this);
    progressBar = new QProgressBar(this);
    progressBar->setRange(0, 100);
    scoreLabel = new QLabel(QString("Очки: %1").arg(progressData.totalPoints), this);


    progressLayout->addWidget(progressLabel);
    progressLayout->addWidget(progressBar, 1); // Give more stretch to the progress bar
    progressLayout->addWidget(scoreLabel);
    mainLayout->addLayout(progressLayout);
    mainLayout->addSpacing(30);


    translationButton = new QPushButton("Упражнение: Перевод", this);
    grammarButton = new QPushButton("Упражнение: Грамматика", this);


    mainLayout->addWidget(translationButton);
    mainLayout->addWidget(grammarButton);
    mainLayout->addSpacing(20);


    connect(translationButton, &QPushButton::clicked, this, &MainWindow::showTranslationExercise);
    connect(grammarButton, &QPushButton::clicked, this, &MainWindow::showGrammarExercise);


    mainLayout->addStretch(1); // Add stretch at the bottom to push content to the center
    startScreenWidget->setLayout(mainLayout);
}


void MainWindow::createMenuActions() {
    QMenu* optionsMenu = menuBar()->addMenu("Опции");
    QAction* difficultyAction = new QAction("Сложность", this);
    connect(difficultyAction, &QAction::triggered, this, &MainWindow::setDifficultyLevel);
    optionsMenu->addAction(difficultyAction);


    QMenu* helpMenu = menuBar()->addMenu("Помощь");
    QAction* helpAction = new QAction("Как играть", this);
    connect(helpAction, &QAction::triggered, this, &MainWindow::showHelpDialog);
    helpMenu->addAction(helpAction);


    QAction* backToMenuAction = new QAction("В главное меню", this);
    connect(backToMenuAction, &QAction::triggered, this, &MainWindow::backToMainMenu);
    menuBar()->addAction(backToMenuAction);
}


void MainWindow::showTranslationExercise() {
    if (translationExerciseWidget) {
        translationExerciseWidget->loadQuestions();
        translationExerciseWidget->nextQuestion();
        translationExerciseWidget->startTimer(); // Запускаем таймер при показе упражнения
        stackedWidget->setCurrentIndex(1);
    }
}


void MainWindow::showGrammarExercise() {
    if (grammarExerciseWidget) {
        grammarExerciseWidget->loadQuestions();
        grammarExerciseWidget->nextQuestion();
        grammarExerciseWidget->startTimer(); // Запускаем таймер при показе упражнения
        stackedWidget->setCurrentIndex(2);
    }
}


void MainWindow::setDifficultyLevel() {
    bool ok;
    int level = QInputDialog::getInt(this, "Выберите уровень",
                                     "Введите уровень (1-5):", progressData.difficultyLevel, 1, 5, 1, &ok);
    if (ok) {
        progressData.difficultyLevel = level;
        if (translationExerciseWidget) translationExerciseWidget->setDifficulty(level);
        if (grammarExerciseWidget) grammarExerciseWidget->setDifficulty(level);
        QMessageBox::information(this, "Выбор уровня", QString("Уровень установлен: %1").arg(level));
        saveGameProgress();
        updateProgressDisplay();
    }
}


void MainWindow::showHelpDialog() {
    QMessageBox::information(this, "Помощь", "Нажмите Подтвердить для проверки ответа. За каждое правильное задание вы получаете очки.");
}


void MainWindow::backToMainMenu() {
    if (translationExerciseWidget) translationExerciseWidget->stopTimer(); // Останавливаем таймер при возврате
    if (grammarExerciseWidget) grammarExerciseWidget->stopTimer();   // Останавливаем таймер при возврате
    stackedWidget->setCurrentIndex(0);
}


void MainWindow::exerciseCompleted() {
    int pointsToAdd = progressData.difficultyLevel;
    progressData.totalPoints += pointsToAdd;
    updateProgressDisplay();
    saveGameProgress();
    backToMainMenu();
}


void MainWindow::handleGameOver() {
    QMessageBox::critical(this, "Игра окончена", "Вы исчерпали количество попыток или время истекло.");
    backToMainMenu();
}


void MainWindow::loadGameProgress() {
    QString filePath = "labs/duolingo/progress.json";
    QFile file(filePath);
    if (!file.exists()) {
        qDebug() << "Файл прогресса не найден. Создаю новый.";
        saveGameProgress();
        return;
    }
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qDebug() << "Не удалось открыть файл прогресса для чтения: " << file.errorString();
        progressData.totalPoints = 0;
        progressData.difficultyLevel = 1;
        updateProgressDisplay();
        return;
    }


    QByteArray jsonData = file.readAll();
    file.close();


    QJsonParseError error;
    QJsonDocument document = QJsonDocument::fromJson(jsonData, &error);
    if (error.error != QJsonParseError::NoError) {
        qDebug() << "Ошибка парсинга JSON файла прогресса: " << error.errorString() << " строка: " << error.offset;
        progressData.totalPoints = 0;
        progressData.difficultyLevel = 1;
        updateProgressDisplay();
        return;
    }


    if (document.isObject()) {
        QJsonObject rootObject = document.object();
        progressData.totalPoints = rootObject.value("totalPoints").toInt(0);
        progressData.difficultyLevel = rootObject.value("difficultyLevel").toInt(1);
        updateProgressDisplay();
    } else {
        qDebug() << "Неверный формат файла прогресса (ожидался JSON-объект).";
        progressData.totalPoints = 0;
        progressData.difficultyLevel = 1;
        updateProgressDisplay();
    }
}


void MainWindow::saveGameProgress() {
    QString filePath = "progress.json";
    QFile file(filePath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        qDebug() << "Не удалось открыть файл прогресса для записи: " << file.errorString();
        return;
    }


    QJsonObject rootObject;
    rootObject["totalPoints"] = progressData.totalPoints;
    rootObject["difficultyLevel"] = progressData.difficultyLevel;


    QJsonDocument document(rootObject);
    file.write(document.toJson(QJsonDocument::Indented));
    file.close();


    qDebug() << "Прогресс сохранен в " << filePath;
}


void MainWindow::updateProgressDisplay() {
    progressBar->setValue(progressData.totalPoints % 101);
    scoreLabel->setText(QString("Очки: %1").arg(progressData.totalPoints));
}
