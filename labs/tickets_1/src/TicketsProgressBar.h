#ifndef TICKETSPROGRESSBAR_H
#define TICKETSPROGRESSBAR_H
#include <QWidget>
#include <QProgressBar>

class TicketsProgressBar : public QProgressBar {
    Q_OBJECT

public:
    explicit TicketsProgressBar(QWidget *parent = nullptr);

public slots:
    void incPossibleScore(int additionalPossiblePoints);

    void incCurrentScore(int additionalPoints);
};

#endif // TICKETSPROGRESSBAR_H
