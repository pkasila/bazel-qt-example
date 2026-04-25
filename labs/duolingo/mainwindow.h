#pragma once

#include <QMainWindow>
#include <QStackedWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QProgressBar>
#include <QSettings>
#include <QLineEdit>
#include <QRadioButton>
#include <QLabel>
#include <QPushButton>
#include <QComboBox>
#include <QPropertyAnimation>
#include <QGraphicsOpacityEffect>
#include <QSlider>
#include <QMenuBar>
#include <memory>
#include "exercisecontroller.h"
#include "audioplayer.h"

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override;

protected:
    bool eventFilter(QObject *watched, QEvent *event) override;

private slots:
    void onStartClicked();
    void onStartTranslationOnlyClicked();
    void onStartGrammarOnlyClicked();
    
    void onOpenSettingsClicked();
    void onSaveSettingsClicked();
    
    void showDifficultyDialog(); // For MenuBar requirement
    
    void onQuestionStarted(const Question& question);
    void onCorrectAnswer();
    void onWrongAnswer(const QString& expected);
    void onQuestionSkipped();
    void onExerciseFinished(bool won, int finalScore);
    void onTimeUpdated(int secondsLeft);
    void onStreakUpdated(int streak, float multiplier);

    void onSubmit();
    void onSkipClicked();
    void onHintClicked();
    void onBackToMenu();
    void onThemeToggled();
    void onMuteToggled();

private:
    void initUI();
    void setupMenuBar();
    QWidget* createMainMenu();
    QWidget* createExerciseScreen();
    QWidget* createSettingsScreen();
    void refreshMainMenu();
    void clearDynamicInput();
    void applyTheme();
    void shakeWindow();
    void animatePropertyFade(QWidget* widget);
    
    void startSpecificExercise(int difficulty, int filterType); // 0=mixed, 1=translation, 2=grammar
    
    QStackedWidget* stackedWidget;
    
    // Main Menu parts
    QLabel* title;
    QLabel* subtitle;
    QLabel* highScoreLabel;
    QComboBox* difficultyCombo;
    QPushButton* transBtn;
    QPushButton* onlyTransBtn;
    QPushButton* onlyGrammarBtn;
    QPushButton* settingsBtn;
    QPushButton* themeBtn; 
    QPushButton* muteBtn; 
    
    // Exercise UI parts
    QPushButton* backButton;
    QLabel* progressLabel;
    QProgressBar* progressBar;
    QLabel* questionTextLabel;
    QLabel* scoreLabel;
    QLabel* timeLabel;
    QLabel* attemptsLabel;
    QLabel* streakLabel;
    
    QPushButton* submitButton;
    QPushButton* skipButton;
    QPushButton* hintButton;
    
    QWidget* answerContainerWidget;
    
    QLineEdit* textInput;
    QList<QRadioButton*> radioButtons;
    
    // Settings UI parts
    QSlider* customTimeSlider;
    QLabel* customTimeLabel;
    QSlider* customLivesSlider;
    QLabel* customLivesLabel;
    QSlider* customQuestionsSlider;
    QLabel* customQuestionsLabel;
    
    std::unique_ptr<ExerciseController> controller;
    std::unique_ptr<AudioPlayer> audioPlayer;
    QSettings settings;
    
    QString currentHint;
    bool isDarkMode;
    bool isMuted;
};
