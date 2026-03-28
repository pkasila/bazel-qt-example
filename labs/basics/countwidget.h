#ifndef COUNTWIDGET_H
#define COUNTWIDGET_H

#include <QWidget>
#include <QSpinBox>
#include <QLabel>
#include <QVBoxLayout>

class CountWidget : public QWidget
{
    Q_OBJECT

public:
    explicit CountWidget(QWidget *parent = nullptr);


    signals:
        void ticketCountChanged(int count);

private:
    QSpinBox *spinBox;
    QLabel *label;
};

#endif // COUNTWIDGET_H
