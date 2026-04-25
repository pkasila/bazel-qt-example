#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QStackedWidget>
#include <QMenuBar>
#include <QProgressBar>
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QTimer>
#include <QDialog>
#include <QComboBox>
#include <QMessageBox>
#include <QTextEdit>
#include <QRadioButton>
#include <QButtonGroup>
#include <QLineEdit>
#include <QGroupBox>
#include <QGridLayout>
#include <QSpinBox>
#include <QApplication>

class TranslationWidget;
class GrammarWidget;
class HelpDialog;

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void showTranslationExercise();
    void showGrammarExercise();
    void showDifficultyDialog();
    void updateProgress(int current, int total);
    void onExerciseCompleted(bool success, int score);
    void onExerciseFailed(const QString& reason);
    void showHelp();
    void updateTimer();

private:
    void setupUI();
    void setupMenuBar();
    void setupMainLayout();
    void applyTheme();
    void resetExercise();
    
    // UI Components
    QStackedWidget* stackedWidget;
    QWidget* mainMenuWidget;
    TranslationWidget* translationWidget;
    GrammarWidget* grammarWidget;
    
    // Menu and controls
    QProgressBar* progressBar;
    QLabel* scoreLabel;
    QLabel* timerLabel;
    QLabel* titleLabel;
    QLabel* difficultyLabel;
    
    // Main menu buttons
    QPushButton* translationButton;
    QPushButton* grammarButton;
    QPushButton* helpButton;
    
    // Timers and state
    QTimer* exerciseTimer;
    int currentScore;
    int currentDifficulty;
    int timeLimit;
    bool exerciseInProgress;
    
    // Exercise data
    int currentExerciseIndex;
    int totalExercises;
    int mistakesCount;
    int maxMistakes;
};

#endif // MAINWINDOW_H
