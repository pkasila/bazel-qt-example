#include "mainwindow.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QWidget>
#include <QApplication>
#include <QScreen>
#include <cmath>

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
{
    initUI();
    resetGame();

    setWindowTitle("Infinite Button Reactor");
    resize(800, 600);

    QScreen* screen = QApplication::primaryScreen();
    QRect screenGeometry = screen->geometry();
    int x = (screenGeometry.width() - width()) / 2;
    int y = (screenGeometry.height() - height()) / 2;
    move(x, y);
}

void MainWindow::initUI()
{
    centralWidget = new QWidget(this);
    auto* mainLayout = new QVBoxLayout(centralWidget);
    mainLayout->setContentsMargins(20, 20, 20, 20);
    mainLayout->setSpacing(10);

    scoreLabel = new QLabel("Баллы: 0", centralWidget);
    scoreLabel->setStyleSheet("QLabel { font-size: 18px; font-weight: bold; color: black; }");

    shrinkSpeedLabel = new QLabel("Скорость: 1.0%", centralWidget);
    shrinkSpeedLabel->setStyleSheet("QLabel { font-size: 18px; font-weight: bold; color: black; }");
    shrinkSpeedLabel->setAlignment(Qt::AlignRight);

    auto* topLayout = new QHBoxLayout();
    topLayout->addWidget(scoreLabel);
    topLayout->addStretch();
    topLayout->addWidget(shrinkSpeedLabel);
    mainLayout->addLayout(topLayout);

    sizeProgressBar = new QProgressBar(centralWidget);
    sizeProgressBar->setRange(0, 100);
    sizeProgressBar->setValue(0);
    mainLayout->addWidget(sizeProgressBar);

    mainLayout->addStretch();

    auto* buttonContainer = new QWidget(centralWidget);
    auto* buttonLayout = new QHBoxLayout(buttonContainer);
    buttonLayout->addStretch();


    gameButton = new QPushButton("Кликни!", centralWidget);
    gameButton->setStyleSheet(
        "QPushButton { "
        "background-color: #FFFACD; "
        "color: black; "
        "border: 3px solid black; "
        "border-radius: 5px; "
        "font-size: 16px; "
        "font-weight: bold; "
        "}"
    );
    gameButton->setFixedSize(INITIAL_WIDTH, INITIAL_HEIGHT);
    buttonLayout->addWidget(gameButton);

    buttonLayout->addStretch();
    mainLayout->addWidget(buttonContainer);

    gameOverLabel = new QLabel(centralWidget);
    gameOverLabel->setStyleSheet("QLabel { font-size: 32px; font-weight: bold; color: red; text-align: center; }");
    gameOverLabel->setAlignment(Qt::AlignCenter);
    gameOverLabel->setText("BOOM!");
    gameOverLabel->hide();
    mainLayout->addWidget(gameOverLabel);

    restartButton = new QPushButton("Перезапуск", centralWidget);
    restartButton->setStyleSheet(
        "QPushButton { "
        "background-color: #FFB6C6; "
        "color: black; "
        "border: 2px solid black; "
        "border-radius: 5px; "
        "font-size: 14px; "
        "font-weight: bold; "
        "padding: 8px 16px; "
        "}"
    );
    mainLayout->addWidget(restartButton, 0, Qt::AlignCenter);
    restartButton->hide();

    mainLayout->addStretch();

    setCentralWidget(centralWidget);

    centralWidget->setStyleSheet("QWidget { background-color: #87CEEB; }");

    updateTimer = new QTimer(this);
    gameOverTimer = new QTimer(this);
    gameOverTimer->setSingleShot(true);

    connect(gameButton, &QPushButton::clicked, this, &MainWindow::onButtonClicked);
    connect(updateTimer, &QTimer::timeout, this, &MainWindow::onTimerTick);
    connect(gameOverTimer, &QTimer::timeout, this, &MainWindow::triggerGameOver);
    connect(restartButton, &QPushButton::clicked, this, &MainWindow::onRestartClicked);
}

void MainWindow::resetGame()
{
    level = 0.0;
    levelMult = 1.0;
    difficulty = 0;
    currentWidth = INITIAL_WIDTH;
    currentHeight = INITIAL_HEIGHT;
    gameOver = false;

    updateScoreLabel();
    updateProgressBar();
    updateShrinkSpeedLabel();

    centralWidget->setStyleSheet("QWidget { background-color: #87CEEB; }");
    gameButton->show();
    gameOverLabel->hide();
    restartButton->hide();
    shrinkSpeedLabel->show();
    sizeProgressBar->show();
    scoreLabel->setStyleSheet("QLabel { font-size: 18px; font-weight: bold; color: black; }");
    gameButton->setEnabled(true);
    gameButton->setStyleSheet(
        "QPushButton { "
        "background-color: #FFFACD; "
        "color: black; "
        "border: 3px solid black; "
        "border-radius: 5px; "
        "font-size: 16px; "
        "font-weight: bold; "
        "}"
    );

    updateTimer->start(static_cast<int>(INITIAL_TIMER_MS));

    gameButton->setFocus();
}

void MainWindow::onButtonClicked()
{
    if (gameOver) return;

    level += levelMult;
    updateScoreLabel();

    difficulty++;

    currentWidth *= (1.0 + GROWTH_FACTOR);
    currentHeight *= (1.0 + GROWTH_FACTOR);

    updateButtonSize();
    checkExplosion();
}

void MainWindow::onTimerTick()
{
    if (gameOver) return;

    double difficultyMultiplier = std::pow(1.01, difficulty);
    double shrinkFactor = SHRINK_FACTOR * difficultyMultiplier;
    currentWidth *= (1.0 - shrinkFactor);
    currentHeight *= (1.0 - shrinkFactor);

    updateButtonSize();
    updateProgressBar();
    updateShrinkSpeedLabel();

    double currentSize = currentWidth;
    double gameOverSize = INITIAL_WIDTH * GAME_OVER_THRESHOLD;

    if (currentSize <= gameOverSize) {
        updateTimer->stop();
        gameOverTimer->start(GAME_OVER_DELAY);
        gameButton->setEnabled(false);
    }
}

void MainWindow::onRestartClicked()
{
    resetGame();
}

void MainWindow::updateButtonSize()
{
    int newWidth = static_cast<int>(currentWidth);
    int newHeight = static_cast<int>(currentHeight);

    if (newWidth < 20) newWidth = 20;
    if (newHeight < 20) newHeight = 20;

    gameButton->setFixedSize(newWidth, newHeight);

    int fontSize = static_cast<int>(newWidth / 8.0);
    if (fontSize < 8) fontSize = 8;
    if (fontSize > 24) fontSize = 24;

    gameButton->setStyleSheet(
        QString(
            "QPushButton { "
            "background-color: #FFFACD; "
            "color: black; "
            "border: 3px solid black; "
            "border-radius: 5px; "
            "font-size: %1px; "
            "font-weight: bold; "
            "}"
        ).arg(fontSize)
    );
}

void MainWindow::updateProgressBar()
{
    double maxSize = INITIAL_WIDTH * EXPLOSION_THRESHOLD;
    double progress = (currentWidth / maxSize) * 100;

    if (progress > 100) progress = 100;
    if (progress < 0) progress = 0;

    sizeProgressBar->setValue(static_cast<int>(progress));
}

void MainWindow::checkExplosion()
{
    double explosionSize = INITIAL_WIDTH * EXPLOSION_THRESHOLD;

    if (currentWidth >= explosionSize) {
        updateTimer->stop();
        triggerExplosion();
    }
}

void MainWindow::triggerExplosion()
{
gameOverLabel->setText("BOOM!");
    gameOverLabel->show();
    gameButton->hide();

    centralWidget->setStyleSheet("QWidget { background-color: black; }");

    difficulty = std::max(0, difficulty - 10);

    levelMult *= 2.0;

    QTimer::singleShot(500, this, [this]() {
        gameOverLabel->hide();
        createNewButton();
    });
}

void MainWindow::createNewButton()
{
    currentWidth = INITIAL_WIDTH;
    currentHeight = INITIAL_HEIGHT;

    updateButtonSize();
    updateProgressBar();
    updateShrinkSpeedLabel();

    gameButton->show();
    shrinkSpeedLabel->show();
    centralWidget->setStyleSheet("QWidget { background-color: #87CEEB; }");

    double difficultyMultiplier = std::pow(1.01, difficulty);
    updateTimer->start(static_cast<int>(INITIAL_TIMER_MS / difficultyMultiplier));

    gameButton->setFocus();
}

void MainWindow::triggerGameOver()
{
    gameOver = true;
    updateTimer->stop();

    centralWidget->setStyleSheet("QWidget { background-color: black; }");

    QString rank = getRank(static_cast<int>(level));
    gameOverLabel->setText(QString("GAME OVER!\nРанг: %1").arg(rank));
    gameOverLabel->show();

    gameButton->hide();
    shrinkSpeedLabel->hide();
    sizeProgressBar->hide();
    scoreLabel->setStyleSheet("QLabel { font-size: 18px; font-weight: bold; color: red; }");

    restartButton->show();
    restartButton->setFocus();
}

void MainWindow::updateScoreLabel()
{
    scoreLabel->setText(QString("Баллы: %1 (x%2)").arg(static_cast<int>(level)).arg(levelMult, 0, 'f', 1));
}

void MainWindow::updateShrinkSpeedLabel()
{
    double difficultyMultiplier = std::pow(1.01, difficulty);
    shrinkSpeedLabel->setText(QString("Скорость уменьшения: x%1").arg(difficultyMultiplier, 0, 'f', 2));
}

QString MainWindow::getRank(int score) {
    if (score >= 10000) {
        return "Autoclicker";
    } else if (score >= 2500) {
        return "S+";
    } else if (score >= 1500) {
        return "S";
    } else if (score >= 1000) {
        if (score == 1337) {
            return "LEET";
        }
        return "A";
    } else if (score >= 500) {
        return "B";
    } else if (score >= 200) {
        return "C";
    } else if (score >= 100) {
        return "D";
    } else {
        return "F";
    }
}