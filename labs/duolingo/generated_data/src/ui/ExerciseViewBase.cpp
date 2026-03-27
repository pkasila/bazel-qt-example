#include "ExerciseViewBase.h"
#include "app/SettingsManager.h"
#include "core/TimerLogic.h"
#include "core/ScoringSystem.h"
#include "core/ExerciseGenerator.h"
#include "db/DatabaseManager.h"
#include "db/ExerciseRepository.h"
#include "db/StatisticsRepository.h"
#include "db/StatisticsRepository.h" // For UserSessionRecord struct

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QProgressBar>
#include <QMessageBox>
#include <QDateTime>
#include <QDebug>


ExerciseViewBase::ExerciseViewBase(QWidget *parent)
    : QWidget(parent), m_currentExerciseIndex(-1) {

    // These should ideally be injected or singletons via Application class
    // For simplicity, creating them here. Make sure DatabaseManager::instance().connect() was called.
    DatabaseManager& dbm = DatabaseManager::instance(); // Ensure DB is connected
    if (!dbm.isConnected()) {
        qCritical("ExerciseViewBase: Database is not connected!");
        // Potentially disable functionality or throw
    }
    m_exerciseRepository = new ExerciseRepository(dbm);
    m_exerciseGenerator = new ExerciseGenerator(m_exerciseRepository);
    m_statisticsRepository = new StatisticsRepository(dbm);
    m_timerLogic = new TimerLogic(this);
    m_scoringSystem = new ScoringSystem(1, 3); // Default, will be updated

    connect(m_timerLogic, &TimerLogic::timerTick, this, &ExerciseViewBase::handleTimerTick);
    connect(m_timerLogic, &TimerLogic::timerFinished, this, &ExerciseViewBase::handleTimerFinished);
}

ExerciseViewBase::~ExerciseViewBase() {
    delete m_timerLogic;
    delete m_scoringSystem;
    delete m_exerciseGenerator;
    delete m_exerciseRepository;
    delete m_statisticsRepository;
}

void ExerciseViewBase::loadNewExerciseSet() {
    m_currentExerciseSet.clear();
    m_currentExerciseIndex = -1;
    
    int appDifficultyId = SettingsManager::instance().getCurrentDifficulty(); // 0, 1, 2
    QString langCode = SettingsManager::instance().getCurrentLanguage(); // "en", "fr"
    ExerciseType type = getExerciseTypeForGenerator();
    
    // User ID for excluding solved exercises (0 for default user)
    int userId = 0; 
    m_currentExerciseSet = m_exerciseGenerator->generateExerciseSet(type, appDifficultyId, langCode, 5, userId); // N=5 default

    if (m_currentExerciseSet.isEmpty()) {
        QMessageBox::warning(this, tr("No Exercises"), tr("Could not load exercises for the selected criteria. Please check DB content and settings."));
        if (m_submitButton) m_submitButton->setEnabled(false);
        if (m_taskTextLabel) m_taskTextLabel->setText(tr("No exercises available."));
        if (m_progressBar) m_progressBar->setValue(0);
        if (m_scoreLabel) m_scoreLabel->setText(tr("Score: - | Errors: -"));
        if (m_timerLabel) m_timerLabel->setText(tr("Time: --:--"));
        return;
    }

    // Fetch difficulty details (points, error limit) from DB based on appDifficultyId
    // This requires a method in SettingsManager or a new Repository for DifficultyLevels
    // For now, hardcoding as before:
    int basePoints = 1;
    int maxErrors = 3; 
    int sessionDurationSeconds = 180; // 3 minutes default

    // TODO: Query DifficultyLevels table using appDifficultyId (0,1,2) to get actual points, error_limit
    // e.g. SELECT points_base, error_limit FROM DifficultyLevels WHERE level_name = 'Easy'/'Medium'/'Hard'
    // For now, hardcode based on index
    if (appDifficultyId == 0)      { basePoints = 1; maxErrors = 5; sessionDurationSeconds = 300; } // Easy
    else if (appDifficultyId == 1) { basePoints = 2; maxErrors = 3; sessionDurationSeconds = 180; } // Medium
    else if (appDifficultyId == 2) { basePoints = 3; maxErrors = 2; sessionDurationSeconds = 120; } // Hard

    delete m_scoringSystem;
    m_scoringSystem = new ScoringSystem(basePoints, maxErrors);

    m_timerLogic->reset(sessionDurationSeconds); // Reset with new duration
    m_timerLogic->start(sessionDurationSeconds);
    
    m_sessionStartTimeISO = QDateTime::currentDateTime().toString(Qt::ISODate);

    nextExercise();
    if (m_submitButton) m_submitButton->setEnabled(true);
    if (m_feedbackLabel) m_feedbackLabel->clear();
}

void ExerciseViewBase::nextExercise() {
    m_currentExerciseIndex++;
    if (m_currentExerciseIndex < m_currentExerciseSet.size()) {
        m_currentExerciseData = m_currentExerciseSet.at(m_currentExerciseIndex);
        displayCurrentExercise(); // Implemented by derived class
        if (m_feedbackLabel) m_feedbackLabel->clear();
    } else {
        // All N exercises attempted
        bool allCorrectAndNoErrorLimit = (m_scoringSystem->getCorrectAnswersCount() == m_currentExerciseSet.size()) && 
                                         !m_scoringSystem->hasExceededErrorLimit();
        endSession(allCorrectAndNoErrorLimit, 
                   allCorrectAndNoErrorLimit ? tr("Congratulations! All tasks completed correctly.") : tr("Session finished."));
    }
    updateProgressDisplay();
}

void ExerciseViewBase::endSession(bool successfullyCompletedByContent, const QString& reasonMessage) {
    if (!m_timerLogic->isActive() && m_currentExerciseIndex < m_currentExerciseSet.size() && m_currentExerciseIndex != -1) {
        // If timer finished earlier, this might be called again. Avoid double processing.
        // Or if already ended by errors.
        // A more robust state machine might be needed for complex cases.
        // For now, just ensure timer is stopped.
    }
    m_timerLogic->stop();
    if (m_submitButton) m_submitButton->setEnabled(false);

    // Final check for success: all N tasks done, within error limit, and within time.
    // successfullyCompletedByContent is true if N tasks were done and errors were not exceeded.
    // We also need to check if the timer ran out if it wasn't already the cause.
    bool trulySuccessful = successfullyCompletedByContent && (reasonMessage != tr("Time's up!"));
                                     
    int finalScore = 0;
    if (trulySuccessful) {
        finalScore = m_scoringSystem->getCurrentScore();
    }

    QMessageBox::information(this, tr("Session Ended"), reasonMessage + tr("\nYour score: %1").arg(finalScore));
    
    UserSessionRecord sessionRecord;
    sessionRecord.userId = 0; // Default user
    sessionRecord.difficultyId = SettingsManager::instance().getCurrentDifficulty(); // App's 0-indexed
    sessionRecord.sessionStartTime = m_sessionStartTimeISO;
    sessionRecord.sessionEndTime = QDateTime::currentDateTime().toString(Qt::ISODate);
    sessionRecord.totalTasksInSession = m_currentExerciseSet.size();
    sessionRecord.tasksCorrectlyCompleted = m_scoringSystem->getCorrectAnswersCount();
    sessionRecord.errorsMade = m_scoringSystem->getErrorsMade();
    sessionRecord.finalScore = finalScore; // Only non-zero if trulySuccessful
    sessionRecord.wasSuccessful = trulySuccessful;
    
    int initialDuration = m_timerLogic->getInitialDuration();
    int remainingTime = m_timerLogic->getRemainingTimeSeconds();
    sessionRecord.timeSpentSeconds = initialDuration > 0 ? (initialDuration - remainingTime) : 0;


    if(m_statisticsRepository) {
        m_statisticsRepository->saveUserSession(sessionRecord);
    } else {
        qWarning("StatisticsRepository is null, cannot save session.");
    }
    
    // Mark all correctly answered exercises in this session as solved
    if (m_exerciseRepository) {
        // This part needs careful implementation: iterate through answers if details are stored
        // or simply mark all from m_currentExerciseSet if that's the policy.
        // For now, if the session was successful, mark all as solved.
        // A more granular approach would be to mark them as they are answered correctly.
        // if (trulySuccessful) {
        //    for(const auto& ex : m_currentExerciseSet) {
        //        m_exerciseRepository->markExerciseAsSolved(0, ex.id);
        //    }
        // }
    }


    emit sessionFinishedSignal(trulySuccessful, finalScore);
}


void ExerciseViewBase::updateProgressDisplay() {
    if (m_currentExerciseSet.isEmpty() && m_currentExerciseIndex == -1) { // Handle initial state or no exercises
        if (m_progressBar) { m_progressBar->setMaximum(1); m_progressBar->setValue(0); }
        if (m_scoreLabel) { m_scoreLabel->setText(tr("Score: - | Errors: -/-")); }
        return;
    }

    if (m_progressBar) {
        m_progressBar->setMaximum(m_currentExerciseSet.size());
        // Ensure value is within range, especially if m_currentExerciseIndex is -1 initially
        m_progressBar->setValue(qMax(0, m_currentExerciseIndex +1));
    }
    if (m_scoreLabel && m_scoringSystem) {
        m_scoreLabel->setText(tr("Score: %1 | Errors: %2/%3")
            .arg(m_scoringSystem->getCurrentScore())
            .arg(m_scoringSystem->getErrorsMade())
            .arg(m_scoringSystem->getMaxErrorsAllowed()));
    }
}


void ExerciseViewBase::handleTimerTick(int remainingSeconds) {
    if (m_timerLabel) {
        int minutes = remainingSeconds / 60;
        int seconds = remainingSeconds % 60;
        m_timerLabel->setText(tr("Time: %1:%2").arg(minutes, 2, 10, QChar('0')).arg(seconds, 2, 10, QChar('0')));
    }
}

void ExerciseViewBase::handleTimerFinished() {
    // Check if session is still considered active (not already ended by other means)
    if (m_currentExerciseIndex >= 0 && m_currentExerciseIndex < m_currentExerciseSet.size()) {
         // And if submit button is enabled (crude check for active session)
        if (m_submitButton && m_submitButton->isEnabled()) {
            endSession(false, tr("Time's up!"));
        }
    }
}

QString ExerciseViewBase::getHelpText() const {
    if (m_currentExerciseIndex >= 0 && m_currentExerciseIndex < m_currentExerciseSet.size()) {
        const auto& currentData = m_currentExerciseSet.at(m_currentExerciseIndex);
        return currentData.hintText.isEmpty() ? 
               tr("No specific hint for this exercise. Try your best!") : 
               currentData.hintText;
    }
    return tr("No exercise loaded. Select an exercise type from the menu.");
}