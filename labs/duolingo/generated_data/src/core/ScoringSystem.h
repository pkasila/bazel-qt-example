#ifndef SCORINGSYSTEM_H
#define SCORINGSYSTEM_H

// Basic stub
class ScoringSystem {
public:
    ScoringSystem(int basePointsPerCorrect, int maxErrorsAllowed);

    void recordCorrectAnswer();
    void recordIncorrectAnswer();

    int getCurrentScore() const;
    int getErrorsMade() const;
    int getCorrectAnswersCount() const;
    int getMaxErrorsAllowed() const; // Added getter
    bool hasExceededErrorLimit() const;
    
    void reset();

private:
    int m_basePoints;
    int m_maxErrors;
    int m_currentScore;
    int m_errorsMade;
    int m_correctAnswers;
};

#endif // SCORINGSYSTEM_H