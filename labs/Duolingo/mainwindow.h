#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QLabel>
#include <QProgressBar>
#include <QStackedWidget>
#include <QWidget>
#include <QTextEdit>
#include <QTimer>
#include <QButtonGroup>
#include <QRadioButton>
#include <QKeyEvent>
#include <QList>
#include <QMenuBar>
#include <QAction>
#include <QDialog>
#include <QComboBox>
#include <QDialogButtonBox>

const int TIME_LIMIT_BASE = 60;
const int MAX_ERRORS = 3;

struct Exercise {
    enum Type { Translation, Grammar };
    Type type;
    QString question;
    QString answer;
    QList<QString> options;
    QString hint;
};

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);

private slots:
    void startTranslationSession();
    void startGrammarSession();
    void updateTimer();
    void loadNextQuestion();
    void checkAnswer();
    void showDifficultyDialog();
    void setDifficulty(int level);

private:
    void setupUI();
    void applyStyles();
    void endSession(bool success, QString message);
    void keyPressEvent(QKeyEvent *event) override;
    QList<Exercise> buildTranslationExercises() const;
    QList<Exercise> buildGrammarExercises() const;
    QString difficultyName() const;

    int score;
    int errors;
    int timeLeft;
    int currentStep;
    int difficultyLevel;
    QList<Exercise> currentSession;
    QTimer *sessionTimer;
    
    QMenuBar *menuBar;
    QAction *difficultyAction;
    
    QLabel *scoreLabel;
    QLabel *timerLabel;
    QLabel *errorLabel;
    QProgressBar *progressBar;
    QStackedWidget *exerciseStack;
    QLabel *translationLabel;
    QTextEdit *translationInput;
    QLabel *grammarLabel;
    QWidget *grammarOptionsWidget;
    QVBoxLayout *grammarOptionsLayout;
    QButtonGroup *grammarGroup;
};

#endif
