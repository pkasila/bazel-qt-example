#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "exercise_session.h"

#include <QMainWindow>
#include <QVector>

class QLabel;
class QProgressBar;
class QPushButton;
class QRadioButton;
class QButtonGroup;
class QStackedWidget;
class QTextEdit;
class QTimer;

class MainWindow final : public QMainWindow {
public:
    explicit MainWindow(QWidget *parent = nullptr);

protected:
    bool eventFilter(QObject *watched, QEvent *event) override;

private:
    void buildUi();
    void buildMenu();
    void refreshModeUi();
    void startSession();
    void loadCurrentTask();
    void updateStatusLabels();
    void updateTimerLabel();
    void finishSession(const QString &title, const QString &text, bool success);
    void endCurrentSession();
    QString currentHint() const;
    QString currentModeText() const;
    void applyResultFeedback(const QString &message, bool positive);
    QString selectedGrammarAnswer() const;
    bool hasSelectedGrammarAnswer() const;
    void clearGrammarSelection();

private:
    ExerciseMode m_mode = ExerciseMode::Translation;
    DifficultyLevel m_level = DifficultyLevel::Beginner;
    ExerciseSession m_session;
    int m_totalScore = 0;
    int m_remainingSeconds = 0;

    QTimer *m_timer = nullptr;

    QPushButton *m_translationButton = nullptr;
    QPushButton *m_grammarButton = nullptr;
    QPushButton *m_startButton = nullptr;
    QPushButton *m_endButton = nullptr;
    QPushButton *m_submitButton = nullptr;

    QLabel *m_titleLabel = nullptr;
    QLabel *m_modeLabel = nullptr;
    QLabel *m_difficultyLabel = nullptr;
    QLabel *m_scoreLabel = nullptr;
    QLabel *m_wrongLabel = nullptr;
    QLabel *m_timeLabel = nullptr;
    QLabel *m_progressLabel = nullptr;
    QLabel *m_promptLabel = nullptr;
    QLabel *m_statusLabel = nullptr;
    QLabel *m_hintLabel = nullptr;
    QProgressBar *m_progressBar = nullptr;
    QStackedWidget *m_stack = nullptr;
    QTextEdit *m_translationEdit = nullptr;
    QVector<QRadioButton*> m_grammarButtons;
    QButtonGroup *m_grammarGroup = nullptr;

    QWidget *m_emptyPage = nullptr;
    QWidget *m_translationPage = nullptr;
    QWidget *m_grammarPage = nullptr;
};

#endif // MAINWINDOW_H