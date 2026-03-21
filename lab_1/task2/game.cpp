#include "game.h"

Game::Game(QObject *parent) 
    : QObject(parent), pieceX(0), pieceY(0), pieceHorizontal(true),
      score(0), lines(0), speed(2), m_gameOver(false), m_paused(false) {
    
    field.resize(HEIGHT);
    for (int i = 0; i < HEIGHT; i++) {
        field[i].resize(WIDTH);
        field[i].fill(EMPTY);
    }
}

void Game::start() {
    reset();
    spawnPiece();
}

void Game::reset() {
    for (int i = 0; i < HEIGHT; i++) {
        field[i].fill(EMPTY);
    }
    
    score = 0;
    lines = 0;
    m_gameOver = false;
    m_paused = false;
    
    emit scoreUpdated(score);
    emit linesUpdated(lines);
    emit pausedSignal(false);
    emit fieldUpdated();
}

void Game::spawnPiece() {
    pieceX = WIDTH / 2 - 2;
    pieceY = 0;
    pieceHorizontal = true;
    
    if (!isValidPosition(pieceX, pieceY, pieceHorizontal)) {
        m_gameOver = true;
        emit gameOverSignal();
    }
    emit fieldUpdated();
}

bool Game::isValidPosition(int x, int y, bool horizontal) const {
    if (horizontal) {
        for (int i = 0; i < 4; i++) {
            if (x + i < 0 || x + i >= WIDTH) return false;
            if (y < 0 || y >= HEIGHT) return false;
            if (field[y][x + i] != EMPTY) return false;
        }
    } else {
        for (int i = 0; i < 4; i++) {
            if (x < 0 || x >= WIDTH) return false;
            if (y + i < 0 || y + i >= HEIGHT) return false;
            if (field[y + i][x] != EMPTY) return false;
        }
    }
    return true;
}

void Game::moveLeft() {
    if (m_gameOver || m_paused) return;
    if (isValidPosition(pieceX - 1, pieceY, pieceHorizontal)) {
        pieceX--;
        emit fieldUpdated();
    }
}

void Game::moveRight() {
    if (m_gameOver || m_paused) return;
    if (isValidPosition(pieceX + 1, pieceY, pieceHorizontal)) {
        pieceX++;
        emit fieldUpdated();
    }
}

void Game::rotate() {
    if (m_gameOver || m_paused) return;
    bool newHorizontal = !pieceHorizontal;
    
    if (isValidPosition(pieceX, pieceY, newHorizontal)) {
        pieceHorizontal = newHorizontal;
        emit fieldUpdated();
        return;
    }
    if (isValidPosition(pieceX - 1, pieceY, newHorizontal)) {
        pieceX--;
        pieceHorizontal = newHorizontal;
        emit fieldUpdated();
        return;
    }
    if (isValidPosition(pieceX + 1, pieceY, newHorizontal)) {
        pieceX++;
        pieceHorizontal = newHorizontal;
        emit fieldUpdated();
    }
}

void Game::moveDown() {
    if (m_gameOver || m_paused) return;
    if (isValidPosition(pieceX, pieceY + 1, pieceHorizontal)) {
        pieceY++;
        emit fieldUpdated();
    } else {
        lockPiece();
    }
}

void Game::lockPiece() {
    if (pieceHorizontal) {
        for (int i = 0; i < 4; i++)
            field[pieceY][pieceX + i] = FILLED;
    } else {
        for (int i = 0; i < 4; i++)
            field[pieceY + i][pieceX] = FILLED;
    }
    
    clearLines();
    spawnPiece();
}

void Game::clearLines() {
    int linesCleared = 0;
    
    for (int i = HEIGHT - 1; i >= 0; i--) {
        bool lineFull = true;
        for (int j = 0; j < WIDTH; j++) {
            if (field[i][j] == EMPTY) {
                lineFull = false;
                break;
            }
        }
        
        if (lineFull) {
            linesCleared++;
            for (int k = i; k > 0; k--)
                field[k] = field[k - 1];
            field[0].fill(EMPTY);
            i++;
        }
    }
    
    if (linesCleared > 0) {
        lines += linesCleared;
        score += linesCleared * 100;
        emit scoreUpdated(score);
        emit linesUpdated(lines);
        emit fieldUpdated();
    }
}

void Game::pause() {
    if (m_gameOver) return;
    m_paused = !m_paused;
    emit pausedSignal(m_paused);
}

bool Game::isGameOver() const { return m_gameOver; }
bool Game::isPaused() const { return m_paused; }
int Game::getScore() const { return score; }
int Game::getLines() const { return lines; }

void Game::setSpeed(int newSpeed) {
    speed = qBound(1, newSpeed, 3);
}

QVector<QVector<int>> Game::getField() const { return field; }
int Game::getPieceX() const { return pieceX; }
int Game::getPieceY() const { return pieceY; }
bool Game::isPieceHorizontal() const { return pieceHorizontal; }