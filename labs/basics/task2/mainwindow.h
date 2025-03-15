#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPushButton>
#include <QLabel>
#include <QComboBox>
#include <QSlider>
#include <QLineEdit>
#include <QFont>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void handleButtonClick();
    void resetGame();
    void updatePlayerName(const QString &name);
    void synchronizeSymbols(int index);
    void updateFontSize(int value);
    void setLightTheme();
    void setDarkTheme();
    void updateGameStatus();

private:
    void checkWinner();
    void disableButtons();
    void showWinnerDialog(const QString &playerName);
    void showDrawDialog();

    Ui::MainWindow *ui;
    QPushButton *buttons[3][3];
    QLabel *statusLabel;
    bool isXTurn;
    int movesLeft;
};

#endif // MAINWINDOW_H
