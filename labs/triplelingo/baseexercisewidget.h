#ifndef BASEEXERCISEWIDGET_H
#define BASEEXERCISEWIDGET_H

#include <QWidget>
#include <QVariant>
#include "exercisestructures.h"

class BaseExerciseWidget : public QWidget
{
    Q_OBJECT

public:
    explicit BaseExerciseWidget(QWidget *parent = nullptr);
    virtual ~BaseExerciseWidget() = default;

    virtual void loadTask(const QVariant& taskData) = 0;
    virtual QString getUserAnswer() const = 0;
    virtual void clearFields() = 0;
    virtual QString getCurrentTaskHelpText() const;

signals:
    void helpRequested();

protected:
    void keyPressEvent(QKeyEvent *event) override;
    QVariant currentLoadedTaskData;
};

#endif // BASEEXERCISEWIDGET_H
