#ifndef EXERCISEVIEWBASE_H
#define EXERCISEVIEWBASE_H

#include <QWidget>
#include "models/ExerciseData.h"
#include <QList> // For QList member

class QLabel;
class QPushButton;
class QProgressBar;
class TimerLogic;
class ScoringSystem;
class ExerciseGenerator;
class ExerciseRepository;
class StatisticsRepository; // Forward declaration

class ExerciseViewBase : public QWidget {
    Q_OBJECT
public:
    explicit ExerciseViewBase(QWidget *parent = nullptr);
    virtual ~ExerciseViewBase(); // Make destructor virtual

    virtual void loadNewExerciseSet();
    virtual QString getHelpText() const;

signals:
    void sessionFinishedSignal(bool success, int score); // Renamed to avoid conflict

protected:
    virtual void setupUi() = 0;
    virtual void displayCurrentExercise() = 0;
    virtual void processAnswer() = 0;
    virtual ExerciseType getExerciseTypeForGenerator() = 0; // Derived classes must specify their type
    
    void nextExercise();
    void endSession(bool successfullyCompletedByContent, const QString& reasonMessage); // Renamed param
    void updateProgressDisplay();

    QList<ExerciseData> m_currentExerciseSet;
    int m_currentExerciseIndex;
    ExerciseData m_currentExerciseData;

    TimerLogic* m_timerLogic;
    ScoringSystem* m_scoringSystem;
    ExerciseGenerator* m_exerciseGenerator;
    ExerciseRepository* m_exerciseRepository;
    StatisticsRepository* m_statisticsRepository; // For saving sessions

    // Common UI elements
    QLabel* m_instructionLabel;
    QLabel* m_taskTextLabel;
    QPushButton* m_submitButton;
    QProgressBar* m_progressBar;
    QLabel* m_scoreLabel;
    QLabel* m_timerLabel;
    QLabel* m_feedbackLabel;

    QString m_sessionStartTimeISO; // To record actual session start

private slots:
    void handleTimerTick(int remainingSeconds);
    void handleTimerFinished();
};

#endif // EXERCISEVIEWBASE_H