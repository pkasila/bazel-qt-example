#include "exercisecontroller.h"
#include "questionrepository.h"
#include "stringmatcher.h"

ExerciseController::ExerciseController(QObject *parent) 
    : QObject(parent), timer(new QTimer(this)), currentIndex(0), score(0), maxAttempts(3), currentAttempts(0), timeLeft(0), currentDifficulty(1), currentStreak(0), scoreMultiplier(1.0f) {
    connect(timer, &QTimer::timeout, this, &ExerciseController::onTick);
}

void ExerciseController::startExercise(int difficulty, int numQuestions, const CustomGameConfig& customConf, int filterType) {
    currentDifficulty = difficulty;
    cachedCustomParams = customConf;
    
    int actualQuestionCount = (difficulty == 4) ? customConf.questionsCount : numQuestions;
    
    questions = QuestionRepository::getQuestions(difficulty, actualQuestionCount, filterType);
    currentIndex = 0;
    score = 0;
    currentAttempts = 0;
    currentStreak = 0;
    scoreMultiplier = 1.0f;
    
    if (difficulty == 1) { // Easy
        maxAttempts = 5;
    } else if (difficulty == 2) { // Medium
        maxAttempts = 3;
    } else if (difficulty == 3) { // Hard
        maxAttempts = 1;
    } else if (difficulty == 4) { // Custom
        maxAttempts = customConf.attempts;
    }
    
    if (questions.empty()) {
        endExercise(false);
        return;
    }
    
    emit streakUpdated(currentStreak, scoreMultiplier);
    nextQuestion();
}

void ExerciseController::nextQuestion() {
    if (currentIndex >= (int)questions.size()) {
        endExercise(true); // Won!
        return;
    }
    
    if (currentDifficulty == 1) timeLeft = 45;
    else if (currentDifficulty == 2) timeLeft = 30;
    else if (currentDifficulty == 3) timeLeft = 15;
    else if (currentDifficulty == 4) timeLeft = cachedCustomParams.timeLimit;
    
    emit timeUpdated(timeLeft);
    emit questionStarted(questions[currentIndex]);
    
    timer->start(1000);
}

void ExerciseController::skipQuestion() {
    timer->stop();
    
    // Penalize score for skipping
    score -= 15;
    if (score < 0) score = 0;
    
    currentStreak = 0;
    scoreMultiplier = 1.0f;
    
    currentIndex++;
    
    emit streakUpdated(currentStreak, scoreMultiplier);
    emit questionSkipped();
    
    nextQuestion();
}

void ExerciseController::submitAnswer(const QString& answer) {
    timer->stop();
    
    const std::vector<QString>& expectedList = questions[currentIndex].acceptedAnswers;
    
    if (StringMatcher::isMatch(answer, expectedList)) {
        score += static_cast<int>(10 * scoreMultiplier);
        currentStreak++;
        scoreMultiplier = 1.0f + (currentStreak * 0.2f); // +20% per correct sequential answer
        
        currentIndex++;
        emit streakUpdated(currentStreak, scoreMultiplier);
        emit correctAnswer();
        nextQuestion();
    } else {
        currentAttempts++;
        currentStreak = 0;
        scoreMultiplier = 1.0f;
        
        emit streakUpdated(currentStreak, scoreMultiplier);
        emit wrongAnswer(expectedList.empty() ? "" : expectedList[0]); // show first correct answer as sample
        
        if (currentAttempts >= maxAttempts) {
            endExercise(false);
        } else {
            timer->start(1000);
        }
    }
}

void ExerciseController::onTick() {
    timeLeft--;
    emit timeUpdated(timeLeft);
    if (timeLeft <= 0) {
        timer->stop();
        endExercise(false);
    }
}

void ExerciseController::stop() {
    timer->stop();
}

void ExerciseController::endExercise(bool won) {
    timer->stop();
    // Do not zero out the score on loss, users still did work!
    emit exerciseFinished(won, score);
}

Question ExerciseController::getCurrentQuestion() const {
    if (currentIndex < (int)questions.size()) {
        return questions[currentIndex];
    }
    return Question();
}
