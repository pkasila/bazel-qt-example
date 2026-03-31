#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QWidget>
#include <QGridLayout>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QLabel>
#include <QMessageBox>
#include <QInputDialog>
#include <vector>

class MainWindow : public QWidget {
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);

private:
    QGridLayout *boardLayout;
    QVBoxLayout *mainLayout;
    QHBoxLayout *infoLayout;

    QLabel *playerLabel;
    QPushButton *resetButton;

    int boardSize;
    std::vector<std::vector<int>> board;
    int currentPlayer;
    std::vector<QPushButton*> buttons;

    void initUI();
    void updateBoard();
    bool isValidMove(int row, int col);
    void makeMove(int row, int col);
    void checkWin(int row, int col);
    void switchPlayer();
    void resetGame();

private slots:
    void onButtonClicked(int row, int col);

};

#endif
