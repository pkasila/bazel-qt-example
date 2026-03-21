#ifndef GAME_H
#define GAME_H

#include <QObject>
#include <QVector>

class Game : public QObject {
    Q_OBJECT

public:
    explicit Game(QObject *parent = nullptr);
    
    void start();
    void reset();
    void moveLeft();
    void moveRight();
    void rotate();
    void moveDown();
    void pause();
    
    bool isGameOver() const;
    bool isPaused() const;
    int getScore() const;
    int getLines() const;
    
    void setSpeed(int speed);
    QVector<QVector<int>> getField() const;
    int getPieceX() const;
    int getPieceY() const;
    bool isPieceHorizontal() const;

signals:
    void fieldUpdated();
    void scoreUpdated(int score);
    void linesUpdated(int lines);
    void gameOverSignal();
    void pausedSignal(bool);

private:
    inline static const int WIDTH = 10;
    inline static const int HEIGHT = 20;
    inline static const int EMPTY = 0;
    inline static const int FILLED = 1;
    
    QVector<QVector<int>> field;
    int pieceX, pieceY;
    bool pieceHorizontal;
    int score, lines, speed;
    bool m_gameOver, m_paused;
    
    bool isValidPosition(int x, int y, bool horizontal) const;
    void lockPiece();
    void clearLines();
    void spawnPiece();
};

#endif // GAME_H