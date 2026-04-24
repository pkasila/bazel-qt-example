#ifndef LEVEL_DIALOG_H
#define LEVEL_DIALOG_H

#include <QDialog>
#include <QRadioButton>
#include <QVBoxLayout>
#include <QPushButton>
#include <QCheckBox>
#include "exercise_data.h"

class LevelDialog : public QDialog {
public:
    explicit LevelDialog(QWidget *parent = nullptr);
    Difficulty getSelectedDifficulty() const;
    bool isTimerEnabled() const;

private:
    QRadioButton *easyBtn;
    QRadioButton *mediumBtn;
    QRadioButton *hardBtn;
    QCheckBox *timerCheckBox;
};

#endif