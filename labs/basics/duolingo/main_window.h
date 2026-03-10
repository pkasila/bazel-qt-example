#pragma once

#include <QAction>
#include <QHBoxLayout>
#include <QLabel>
#include <QMainWindow>
#include <QMediaPlayer>
#include <QMenu>
#include <QMenuBar>
#include <QProgressBar>
#include <QPushButton>
#include <QRadioButton>
#include <QStackedWidget>
#include <QTextEdit>
#include <QTimer>
#include <QVBoxLayout>
#include <vector>

// NOLINTBEGIN
class MainWindow : public QMainWindow {
    Q_OBJECT

   public:
    explicit MainWindow(QWidget* parent = nullptr);
    ~MainWindow() override = default;

   private slots:
    void showTranslationExercise();
    void showGrammarExercise();
    void showDifficultyDialog();
    void showHelp();
    void updateTimer();
    void checkAnswer();
    void nextExercise();

   private:
    void setupUI();
    void setupMenuBar();
    void setupExerciseWidgets();
    void setGrammarExercise();
    void startExercise();
    void endExercise();
    void updateScore(int points);
    void showResultDialog(bool success);
    void resetExercise();

    QStackedWidget* stackedWidget_;
    QProgressBar* progressBar_;
    QLabel* scoreLabel_;
    QLabel* timerLabel_;
    QPushButton* translationButton_;
    QPushButton* grammarButton_;
    QPushButton* submitButton_;

    QTimer* exerciseTimer_;
    int remainingTime_;
    int maxTime_ = 600;
    int currentScore_;
    int currentExercise_;
    int totalExercises_;
    int difficultyLevel_ = 1;
    int maxAttempts_;
    int currentAttempts_;

    // std::vector<QString> translationExerciseTexts;
    // std::vector<QString> grammarExerciseTexts;
    // std::vector<QString> grammarExerciseAnswers;
    QLabel* translationPrompt_;
    QLabel* grammarPrompt_;
    std::vector<QRadioButton*> currentGrammarOptions_;
    std::vector<int> currentExercises_;
    QWidget* translationWidget_;
    QWidget* grammarWidget_;
    QTextEdit* translationInput_;
    QButtonGroup* grammarOptions_;
    QVBoxLayout* grammarLayout_;
    QMediaPlayer* successSound_;
    QMediaPlayer* failedSound_;
};

// NOLINTEND
