#ifndef MAINWINDOW_H
#define MAINWINDOW_H


#include <QMainWindow>
#include <QStackedWidget>
#include <QWidget>
#include <QLabel>
#include <QPushButton>
#include <QProgressBar>
#include <QSettings>


class TranslationExercise;
class GrammarExercise;


struct ProgressData {
    int totalPoints = 0;
    int difficultyLevel = 1;
};


class MainWindow : public QMainWindow {
    Q_OBJECT


public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();


private:
    QStackedWidget* stackedWidget;
    QWidget* startScreenWidget;
    QLabel* titleLabel;
    QProgressBar* progressBar;
    QLabel* scoreLabel;
    QPushButton* translationButton;
    QPushButton* grammarButton;
    ProgressData progressData;


    TranslationExercise* translationExerciseWidget = nullptr;
    GrammarExercise* grammarExerciseWidget = nullptr;


private slots:
    void showTranslationExercise();
    void showGrammarExercise();
    void setDifficultyLevel();
    void showHelpDialog();
    void backToMainMenu();
    void exerciseCompleted();
    void handleGameOver();


private:
    void initializeUI();
    void createStartScreen();
    void createMenuActions();
    void loadGameProgress();
    void saveGameProgress();
    void updateProgressDisplay();
};


#endif // MAINWINDOW_H
