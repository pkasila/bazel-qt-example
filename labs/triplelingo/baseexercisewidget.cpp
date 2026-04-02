#include "baseexercisewidget.h"
#include <QKeyEvent>

BaseExerciseWidget::BaseExerciseWidget(QWidget *parent) : QWidget(parent) {
    setFocusPolicy(Qt::StrongFocus);
}

void BaseExerciseWidget::keyPressEvent(QKeyEvent *event) {
    if (event->key() == Qt::Key_H) {
        emit helpRequested();
    } else {
        QWidget::keyPressEvent(event);
    }
}

QString BaseExerciseWidget::getCurrentTaskHelpText() const {
    if (currentLoadedTaskData.canConvert<TranslationTask>()) {
        return currentLoadedTaskData.value<TranslationTask>().helpText;
    } else if (currentLoadedTaskData.canConvert<GrammarTask>()) {
        return currentLoadedTaskData.value<GrammarTask>().helpText;
    }
    return "No specific help available.";
}
