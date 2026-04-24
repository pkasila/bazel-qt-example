#include "level_dialog.h"
#include <QCheckBox>
#include <QLabel>

LevelDialog::LevelDialog(QWidget *parent) : QDialog(parent) {
    setWindowTitle("Settings");
    QVBoxLayout *layout = new QVBoxLayout(this);

    easyBtn = new QRadioButton("Easy", this);
    mediumBtn = new QRadioButton("Medium", this);
    hardBtn = new QRadioButton("Hard", this);
    easyBtn->setChecked(true);

    timerCheckBox = new QCheckBox("Enable Timer", this);
    timerCheckBox->setChecked(true);

    QPushButton *okBtn = new QPushButton("Confirm", this);
    connect(okBtn, &QPushButton::clicked, this, &QDialog::accept);

    layout->addWidget(new QLabel("Select Difficulty:"));
    layout->addWidget(easyBtn);
    layout->addWidget(mediumBtn);
    layout->addWidget(hardBtn);
    layout->addWidget(new QLabel("Options:"));
    layout->addWidget(timerCheckBox);
    layout->addWidget(okBtn);
}

Difficulty LevelDialog::getSelectedDifficulty() const {
    if (mediumBtn->isChecked()) return Difficulty::Medium;
    if (hardBtn->isChecked()) return Difficulty::Hard;
    return Difficulty::Easy;
}

bool LevelDialog::isTimerEnabled() const {
    return timerCheckBox->isChecked();
}