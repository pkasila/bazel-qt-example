#include "mainwindow.h"

MainWindow::MainWindow(QWidget *parent) : QWidget(parent), boardSize(6), currentPlayer(1) {
    initUI();
}

void MainWindow::initUI() {
    mainLayout = new QVBoxLayout(this);
    setLayout(mainLayout);

    bool ok;
    boardSize = QInputDialog::getInt(this, "Выбор размера доски", "Введите размер доски (минимум 4):", 6, 4, 20);
    if (boardSize <= 3) {
        QMessageBox::warning(this, "Ошибка", "Размер доски должен быть не менее 4.");
        return;
    }

    // Создать доску
    board = std::vector<std::vector<int>>(boardSize, std::vector<int>(boardSize, 0));
    buttons.resize(boardSize * boardSize);

    boardLayout = new QGridLayout();
    mainLayout->addLayout(boardLayout);

    for (int row = 0; row < boardSize; ++row) {
        for (int col = 0; col < boardSize; ++col) {
            QPushButton *button = new QPushButton(this);
            button->setFixedSize(50, 50);
            button->setStyleSheet("background-color: white;");
            connect(button, &QPushButton::clicked, [this, row, col]() { onButtonClicked(row, col); });
            boardLayout->addWidget(button, row, col);
            buttons[row * boardSize + col] = button;
        }
    }

    infoLayout = new QHBoxLayout();
    mainLayout->addLayout(infoLayout);

    playerLabel = new QLabel("Ход игрока 1", this);
    infoLayout->addWidget(playerLabel);

    resetButton = new QPushButton("Новая игра", this);
    connect(resetButton, &QPushButton::clicked, this, &MainWindow::resetGame);
    infoLayout->addWidget(resetButton);

    updateBoard();
    setWindowTitle("4 в ряд");
    resize(400, 400);
}

void MainWindow::updateBoard() {
    for (int row = 0; row < boardSize; ++row) {
        for (int col = 0; col < boardSize; ++col) {
            int index = row * boardSize + col;
            QPushButton *button = buttons[index];
            if (board[row][col] == 0) {
                button->setText("");
            } else if (board[row][col] == 1) {
                button->setText("X");
            } else if (board[row][col] == 2) {
                button->setText("O");
            }
        }
    }
}

bool MainWindow::isValidMove(int row, int col) {
    return board[row][col] == 0;
}

void MainWindow::makeMove(int row, int col) {
    board[row][col] = currentPlayer;
    updateBoard();
}

void MainWindow::checkWin(int row, int col) {
    int piece = board[row][col];

    std::vector<std::pair<int, int>> directions = {
        {0, 1},
        {1, 0},
        {1, 1},
        {1, -1}
    };

    for (const auto& dir : directions) {
        int dx = dir.first;
        int dy = dir.second;

        int count = 1;

        for (int i = 1; i < 4; ++i) {
            int newRow = row + i * dx;
            int newCol = col + i * dy;

            if (newRow >= 0 && newRow < boardSize && newCol >= 0 && newCol < boardSize &&
                board[newRow][newCol] == piece) {
                count++;
            } else {
                break;
            }
        }

        for (int i = 1; i < 4; ++i) {
            int newRow = row - i * dx;
            int newCol = col - i * dy;

            if (newRow >= 0 && newRow < boardSize && newCol >= 0 && newCol < boardSize &&
                board[newRow][newCol] == piece) {
                count++;
            } else {
                break;
            }
        }

        if (count >= 4) {
            QMessageBox::information(this, "Победа!", QString("Игрок %1 выиграл!").arg(currentPlayer));
            resetGame();
            return;
        }
    }
}

void MainWindow::switchPlayer() {
    currentPlayer = 3 - currentPlayer; // 1 -> 2, 2 -> 1
    playerLabel->setText(currentPlayer == 1 ? "Ход игрока 1" : "Ход игрока 2");
}

void MainWindow::onButtonClicked(int row, int col) {
    if (isValidMove(row, col)) {
        makeMove(row, col);
        checkWin(row, col);
        switchPlayer();
    }
}

void MainWindow::resetGame() {
    board = std::vector<std::vector<int>>(boardSize, std::vector<int>(boardSize, 0));
    currentPlayer = 1;
    playerLabel->setText("Ход игрока 1");
    updateBoard();
}
