#pragma once

#include "difficulty_dialog.h"
#include "exercise_data.h"
#include "exercise_widgets.h"

#include <QList>
#include <QMainWindow>

class QStackedWidget;
class QTimer;
class QProgressBar;
class QLabel;
class QPushButton;
class QVBoxLayout;
class QHBoxLayout;

class MainWindow : public QMainWindow {
   public:
    explicit MainWindow(QWidget* parent = nullptr);
    void showHelp();

   private:
    void setupUI();
    void setupMenu();
    void setupShortcuts();
    void applyDesign();
    void startExercise(bool isGrammar);
    void loadQuestion();
    void handleAnswer(bool isCorrect);
    void submitCurrentAnswer();
    void finishSession(const QString& message);

    int m_difficulty;
    int m_totalPoints;
    int m_idx;
    int m_wrongAttempts;
    int m_timeLeft;
    bool m_isPerfect;
    bool m_btnSubmitEnabled;
    bool m_isGrammar;

    QList<Question> m_questions;
    QTimer* m_timer;

    QStackedWidget* m_stacked;
    QWidget* m_menuPage;
    QWidget* m_exercisePage;
    QStackedWidget* m_stackedEx;
    TranslationWidget* m_translationWidget;
    GrammarWidget* m_grammarWidget;
    QProgressBar* m_progressBar;
    QLabel* m_lblPoints;
    QLabel* m_lblDifficulty;
    QLabel* m_lblTimer;
    QLabel* m_lblFeedback;
    QPushButton* m_btnTranslation;
    QPushButton* m_btnGrammar;
    QPushButton* m_btnSettings;
};