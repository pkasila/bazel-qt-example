#include "mainwindow.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QMessageBox>
#include <QKeyEvent>
#include <QDebug>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), 
      game(nullptr), 
      timer(nullptr), 
      timerInterval(500),
      linesLabel(nullptr) {
    
    setWindowTitle("I-Tetris");
    setMinimumSize(550, 650);
    setStyleSheet("background-color: #2c3e50;");
    
    setupUI();
    
    game = new Game(this);
    setupConnections();
    
    timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &MainWindow::updateTimer);
    
    updateField();
    installKeyFilter();
}

void MainWindow::setupUI() {
    QWidget *centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);
    
    QHBoxLayout *mainLayout = new QHBoxLayout(centralWidget);
    
    //Игровое поле
    fieldFrame = new QFrame();
    fieldFrame->setFrameStyle(QFrame::Box | QFrame::Raised);
    fieldFrame->setLineWidth(3);
    fieldFrame->setStyleSheet("background-color: #1a1a2e;");
    
    QGridLayout *gridLayout = new QGridLayout(fieldFrame);
    gridLayout->setSpacing(1);
    gridLayout->setContentsMargins(5, 5, 5, 5);
    
    for (int i = 0; i < 20; i++) {
        for (int j = 0; j < 10; j++) {
            cells[i][j] = new QLabel();
            cells[i][j]->setFixedSize(25, 25);
            cells[i][j]->setStyleSheet("background-color: #16213e;");
            gridLayout->addWidget(cells[i][j], i, j);
        }
    }
    mainLayout->addWidget(fieldFrame);
    
    //Панель управления
    QWidget *panel = new QWidget();
    panel->setFixedWidth(180);
    QVBoxLayout *panelLayout = new QVBoxLayout(panel);
    panelLayout->setSpacing(15);
    
    QLabel *titleLabel = new QLabel("I-TETRIS");
    titleLabel->setStyleSheet("color: white; font-size: 20px; font-weight: bold;");
    titleLabel->setAlignment(Qt::AlignCenter);
    panelLayout->addWidget(titleLabel);
    
    lcdScore = new QLCDNumber();
    lcdScore->setDigitCount(6);
    lcdScore->setStyleSheet("background-color: #34495e; color: #27ae60;");
    lcdScore->display(0);
    panelLayout->addWidget(lcdScore);
    
    scoreLabel = new QLabel("Счёт: 0");
    scoreLabel->setStyleSheet("color: white; font-size: 14px;");
    scoreLabel->setAlignment(Qt::AlignCenter);
    panelLayout->addWidget(scoreLabel);
    
    linesLabel = new QLabel("Линии: 0");
    linesLabel->setStyleSheet("color: white; font-size: 14px;");
    linesLabel->setAlignment(Qt::AlignCenter);
    panelLayout->addWidget(linesLabel);
    
    QLabel *speedLabel = new QLabel("Скорость:");
    speedLabel->setStyleSheet("color: white;");
    panelLayout->addWidget(speedLabel);
    
    speedCombo = new QComboBox();
    speedCombo->addItems({"Медленно", "Средне", "Быстро"});
    speedCombo->setCurrentIndex(1);
    panelLayout->addWidget(speedCombo);
    
    startButton = new QPushButton("▶ Старт");
    startButton->setFixedHeight(35);
    startButton->setStyleSheet(
        "QPushButton { background-color: #27ae60; color: white; "
        "border-radius: 5px; font-size: 14px; }"
        "QPushButton:hover { background-color: #229954; }");
    panelLayout->addWidget(startButton);
    
    resetButton = new QPushButton("⏹ Сброс");
    resetButton->setFixedHeight(35);
    resetButton->setStyleSheet(
        "QPushButton { background-color: #e74c3c; color: white; "
        "border-radius: 5px; font-size: 14px; }"
        "QPushButton:hover { background-color: #c0392b; }");
    panelLayout->addWidget(resetButton);
    
    QLabel *controlsLabel = new QLabel(
        "← → : Движение\n"
        "↑   : Вращение\n"
        "↓   : Вниз\n"
        "Пробел : Пауза\n"
        "R   : Рестарт");
    controlsLabel->setStyleSheet("color: white; font-size: 11px;");
    panelLayout->addWidget(controlsLabel);
    
    panelLayout->addStretch();
    mainLayout->addWidget(panel);
}

void MainWindow::setupConnections() {
    connect(startButton, &QPushButton::clicked, this, &MainWindow::onStartClicked);
    connect(resetButton, &QPushButton::clicked, game, &Game::reset);
    connect(speedCombo, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &MainWindow::onSpeedChanged);
    connect(game, &Game::scoreUpdated, this, &MainWindow::onScoreUpdated);
    connect(game, &Game::scoreUpdated, lcdScore, QOverload<int>::of(&QLCDNumber::display));
    connect(game, &Game::gameOverSignal, this, &MainWindow::onGameOver);
    connect(game, &Game::pausedSignal, this, &MainWindow::onPaused);
    connect(game, &Game::fieldUpdated, this, &MainWindow::updateField);
    connect(game, &Game::linesUpdated, this, &MainWindow::onLinesUpdated);
}

void MainWindow::onLinesUpdated(int lines) {
    if (linesLabel) {
        linesLabel->setText("Линии: " + QString::number(lines));
    }
}

void MainWindow::installKeyFilter() {
    startButton->installEventFilter(this);
    resetButton->installEventFilter(this);
    speedCombo->installEventFilter(this);
    lcdScore->installEventFilter(this);
    scoreLabel->installEventFilter(this);
    fieldFrame->installEventFilter(this);
    
    for (int i = 0; i < 20; i++) {
        for (int j = 0; j < 10; j++) {
            cells[i][j]->installEventFilter(this);
        }
    }
}

bool MainWindow::eventFilter(QObject *obj, QEvent *event) {
    if (event->type() == QEvent::KeyPress) {
        QKeyEvent *keyEvent = static_cast<QKeyEvent *>(event);
        
        switch (keyEvent->key()) {
            case Qt::Key_Left:
                game->moveLeft();
                return true;
            case Qt::Key_Right:
                game->moveRight();
                return true;
            case Qt::Key_Up:
                game->rotate();
                return true;
            case Qt::Key_Down:
                game->moveDown();
                return true;
            case Qt::Key_Space:
                game->pause();
                return true;
            case Qt::Key_R:
                game->reset();
                timer->stop();
                updateField();
                return true;
            default:
                break;
        }
    }
    return QMainWindow::eventFilter(obj, event);
}

void MainWindow::onStartClicked() {
    if (game) {
        game->start();
        timer->start(timerInterval);
    }
}

void MainWindow::onResetClicked() {
    if (game) {
        game->reset();
        timer->stop();
        updateField();
    }
}

void MainWindow::onSpeedChanged(int index) {
    if (game) {
        game->setSpeed(index + 1);
    }
    switch (index) {
        case 0: timerInterval = 800; break;
        case 1: timerInterval = 500; break;
        case 2: timerInterval = 200; break;
    }
    if (timer->isActive()) {
        timer->stop();
        timer->start(timerInterval);
    }
}

void MainWindow::onScoreUpdated(int score) {
    if (scoreLabel) {
        scoreLabel->setText("Счёт: " + QString::number(score));
    }
}

void MainWindow::onGameOver() {
    timer->stop();
    if (fieldFrame) {
        fieldFrame->setStyleSheet("background-color: #c0392b;");
    }
    QMessageBox::information(this, "Игра окончена!",
                             "Ваш счёт: " + QString::number(game ? game->getScore() : 0));
    if (fieldFrame) {
        fieldFrame->setStyleSheet("background-color: #1a1a2e;");
    }
}

void MainWindow::onPaused(bool paused) {
    if (paused) {
        timer->stop();
        if (startButton) startButton->setText("▶ Продолжить");
    } else {
        timer->start(timerInterval);
        if (startButton) startButton->setText("▶ Старт");
    }
}

void MainWindow::updateTimer() {
    if (game) {
        game->moveDown();
    }
}

void MainWindow::updateField() {
    if (!game) return;
    
    QVector<QVector<int>> field = game->getField();
    int pieceX = game->getPieceX();
    int pieceY = game->getPieceY();
    bool horizontal = game->isPieceHorizontal();
    
    for (int i = 0; i < 20; i++) {
        for (int j = 0; j < 10; j++) {
            bool isPiece = false;
            
            if (horizontal) {
                if (i == pieceY && j >= pieceX && j < pieceX + 4)
                    isPiece = true;
            } else {
                if (j == pieceX && i >= pieceY && i < pieceY + 4)
                    isPiece = true;
            }
            
            if (isPiece)
                cells[i][j]->setStyleSheet("background-color: #27ae60;");
            else if (field[i][j] == 1)
                cells[i][j]->setStyleSheet("background-color: #3498db;");
            else
                cells[i][j]->setStyleSheet("background-color: #16213e;");
        }
    }
}

void MainWindow::keyPressEvent(QKeyEvent *event) {
    if (!game) {
        QMainWindow::keyPressEvent(event);
        return;
    }
    
    switch (event->key()) {
        case Qt::Key_Left: game->moveLeft(); break;
        case Qt::Key_Right: game->moveRight(); break;
        case Qt::Key_Up: game->rotate(); break;
        case Qt::Key_Down: game->moveDown(); break;
        case Qt::Key_Space: game->pause(); break;
        case Qt::Key_R: game->reset(); timer->stop(); updateField(); break;
        default: QMainWindow::keyPressEvent(event);
    }
}