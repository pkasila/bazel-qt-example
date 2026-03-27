#include "viewwidget.h"
ProgressWidget::ProgressWidget(QWidget *parent) : QWidget(parent) {
    QVBoxLayout *layout = new QVBoxLayout(this);

    totalProgress = new QProgressBar;
    greenProgress = new QProgressBar;

    green = new QLabel;
    green->setText("Пргресс полностью выученных билетов");
    tot = new QLabel;
    tot->setText("Общий прогресс");

    totalProgress->setMinimumHeight(30);
    greenProgress->setMinimumHeight(30);

    totalProgress->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    greenProgress->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);

    layout->addWidget(green);
    layout->addWidget(totalProgress);
    layout->addWidget(tot);
    layout->addWidget(greenProgress);
}
void ProgressWidget::updateBars(int condition) {
    if (condition == -1) {
        int nw = greenProgress->value() - 1;
        greenProgress->setValue(nw);
    } else if (condition == 1) {
        int nw1 = greenProgress->value() + 1;
        greenProgress->setValue(nw1);
    } else if (condition == 2) {
        int nw1 = greenProgress->value() + 1;
        int nw2 = totalProgress->value() + 1;
        greenProgress->setValue(nw1);
        totalProgress->setValue(nw2);
    } else if (condition == 3) {
        int nw = totalProgress->value() - 1;
        totalProgress->setValue(nw);
    }
}
void ProgressWidget::Clear() {
    totalProgress->setMaximum(tickets_count);
    greenProgress->setMaximum(tickets_count);
    totalProgress->setValue(0);
    greenProgress->setValue(0);
}
