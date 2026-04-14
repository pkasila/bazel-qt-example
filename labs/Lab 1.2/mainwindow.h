#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QLabel>
#include <QPushButton>
#include <QProgressBar>
#include <QTimer>
#include <QWidget>

class MainWindow : public QMainWindow {
public:
    explicit MainWindow(QWidget* parent = nullptr);

private:
    void initUI();
    void updateButtonSize();
    void updateProgressBar();
    void checkExplosion();
    void createNewButton();
    void triggerExplosion();
    void triggerGameOver();
    void resetGame();
    void updateScoreLabel();
    void updateShrinkSpeedLabel();
    QString getRank(int score);
    void onButtonClicked();
    void onTimerTick();
    void onRestartClicked();

    static constexpr int INITIAL_WIDTH = 100;
    static constexpr int INITIAL_HEIGHT = 100;
    static constexpr int INITIAL_TIMER_MS = 100;
    static constexpr double GROWTH_FACTOR = 0.10;
    static constexpr double SHRINK_FACTOR = 0.01;
    static constexpr double EXPLOSION_THRESHOLD = 10.0;
    static constexpr double GAME_OVER_THRESHOLD = 0.33;
    static constexpr int GAME_OVER_DELAY = 2000;

    QWidget* centralWidget = nullptr;
    QPushButton* gameButton = nullptr;
    QLabel* scoreLabel = nullptr;
    QLabel* shrinkSpeedLabel = nullptr;
    QProgressBar* sizeProgressBar = nullptr;
    QPushButton* restartButton = nullptr;
    QLabel* gameOverLabel = nullptr;

    double level = 0.0;
    double levelMult = 1.0;
    int difficulty = 0;
    double currentWidth = INITIAL_WIDTH;
    double currentHeight = INITIAL_HEIGHT;
    bool gameOver = false;
    QTimer* updateTimer = nullptr;
    QTimer* gameOverTimer = nullptr;
};

#endif