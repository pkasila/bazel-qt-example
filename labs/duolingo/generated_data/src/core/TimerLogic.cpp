#include "TimerLogic.h"

TimerLogic::TimerLogic(QObject *parent) : QObject(parent) {
    m_timer.setInterval(1000); // Tick every second
    connect(&m_timer, &QTimer::timeout, this, &TimerLogic::onTimeout);
    m_durationSeconds = 0;
    m_remainingSeconds = 0;
}

void TimerLogic::start(int durationSeconds) {
    if (durationSeconds <= 0) return;
    m_durationSeconds = durationSeconds;
    m_remainingSeconds = durationSeconds;
    m_timer.start();
    emit timerTick(m_remainingSeconds);
}

void TimerLogic::stop() {
    m_timer.stop();
}

void TimerLogic::reset(int newDurationSeconds) {
    stop();
    if (newDurationSeconds > 0) {
        m_durationSeconds = newDurationSeconds;
    }
    m_remainingSeconds = m_durationSeconds;
    // emit timerTick(m_remainingSeconds); // Optionally emit tick on reset
}

bool TimerLogic::isActive() const {
    return m_timer.isActive();
}

int TimerLogic::getRemainingTimeSeconds() const {
    return m_remainingSeconds;
}

int TimerLogic::getInitialDuration() const {
    return m_durationSeconds;
}

void TimerLogic::onTimeout() {
    if (m_remainingSeconds > 0) {
        m_remainingSeconds--;
        emit timerTick(m_remainingSeconds);
    }
    if (m_remainingSeconds <= 0) {
        // Ensure it only emits finished once if it was already 0
        if (m_timer.isActive()) { // Check if timer was running
            stop();
            emit timerFinished();
        }
    }
}