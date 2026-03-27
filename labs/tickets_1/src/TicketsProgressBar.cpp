#include "TicketsProgressBar.h"

TicketsProgressBar::TicketsProgressBar(QWidget *parent) :
    QProgressBar(parent) {
    setValue(0);
    setMaximum(0);
}

void TicketsProgressBar::incPossibleScore(int additionalPossiblePoints) {
    setRange(0, maximum() + additionalPossiblePoints);
}

void TicketsProgressBar::incCurrentScore(int additionalPoints) {
    setValue(value() + additionalPoints);
}
