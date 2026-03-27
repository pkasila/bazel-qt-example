#ifndef TIMERLOGIC_H
#define TIMERLOGIC_H

#include <QObject> // QTimer needs QObject
#include <QTimer>

class TimerLogic : public QObject {
    Q_OBJECT
public:
    explicit TimerLogic(QObject *parent = nullptr);

    void start(int durationSeconds); // Duration for the whole set of N exercises
    void stop();
    void reset(int newDurationSeconds = 0); // Allow resetting with new duration
    bool isActive() const;
    int getRemainingTimeSeconds() const;
    int getInitialDuration() const; // Added getter

signals:
    void timerTick(int remainingSeconds);
    void timerFinished();

private slots:
    void onTimeout();

private:
    QTimer m_timer;
    int m_durationSeconds;
    int m_remainingSeconds;
};

#endif // TIMERLOGIC_H