#include "FPSCounter.h"

#include <QLabel>
#include <QWidget>
#include <QApplication>

FPSCounter::FPSCounter(QWidget* parent) : QLabel(parent) {
    setText("FPS: ~~~");

    timer_ = new QTimer;
    connect(timer_, &QTimer::timeout, this, &FPSCounter::UpdateFPS);
    timer_->start(1000);

    qApp->installEventFilter(this);
}

bool FPSCounter::eventFilter(QObject* obj, QEvent* event) {
    if (event->type() == QEvent::Paint) {
        frames_++;
    }
    return QLabel::eventFilter(obj, event);
}

void FPSCounter::UpdateFPS() {
    setText(QString("FPS: %1").arg(frames_));
    frames_ = 0;
}
