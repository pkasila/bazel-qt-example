#include "countwidget.h"
CountWidget::CountWidget(QWidget *parent)
    : QWidget(parent)
{
    spinBox = new QSpinBox;
    spinBox->setMinimum(0);
    spinBox->setMaximum(100);

    label = new QLabel("Выбрано билетов: 0");

    QHBoxLayout *layout = new QHBoxLayout;
    layout->addWidget(spinBox);
    layout->addWidget(label);
    setLayout(layout);

    connect(spinBox, QOverload<int>::of(&QSpinBox::valueChanged),
            [this](int value) {
                label->setText(QString("Выбрано билетов: ") + QString::number(value));
                emit ticketCountChanged(value);
            });
}
