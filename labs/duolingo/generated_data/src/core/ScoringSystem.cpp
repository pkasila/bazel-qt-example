#include "ScoringSystem.h"

ScoringSystem::ScoringSystem(int basePointsPerCorrect, int maxErrorsAllowed)
    : m_basePoints(basePointsPerCorrect), m_maxErrors(maxErrorsAllowed),
      m_currentScore(0), m_errorsMade(0), m_correctAnswers(0) {}

void ScoringSystem::recordCorrectAnswer() {
    m_currentScore += m_basePoints;
    m_correctAnswers++;
}

void ScoringSystem::recordIncorrectAnswer() {
    m_errorsMade++;
}

int ScoringSystem::getCurrentScore() const {
    return m_currentScore;
}

int ScoringSystem::getErrorsMade() const {
    return m_errorsMade;
}

int ScoringSystem::getCorrectAnswersCount() const {
    return m_correctAnswers;
}

int ScoringSystem::getMaxErrorsAllowed() const {
    return m_maxErrors;
}

bool ScoringSystem::hasExceededErrorLimit() const {
    return m_errorsMade >= m_maxErrors;
}

void ScoringSystem::reset() {
    m_currentScore = 0;
    m_errorsMade = 0;
    m_correctAnswers = 0;
}