#include "difficultydialog.h"

DifficultyDialog::DifficultyDialog(const QString& currentDifficulty, QWidget *parent)
    : QDialog(parent), m_selectedDifficulty(currentDifficulty)
{
    setWindowTitle("Set Difficulty");
    setModal(true);

    mainLayout = new QVBoxLayout(this);

    easyButton = new QRadioButton("Easy (5 tasks, 3 mistakes, 120s)", this);
    mediumButton = new QRadioButton("Medium (7 tasks, 2 mistakes, 90s)", this);
    hardButton = new QRadioButton("Hard (10 tasks, 1 mistake, 60s)", this);

    buttonGroup = new QButtonGroup(this);
    buttonGroup->addButton(easyButton);
    buttonGroup->addButton(mediumButton);
    buttonGroup->addButton(hardButton);

    if (currentDifficulty == "Easy") easyButton->setChecked(true);
    else if (currentDifficulty == "Medium") mediumButton->setChecked(true);
    else if (currentDifficulty == "Hard") hardButton->setChecked(true);
    else mediumButton->setChecked(true);

    okButton = new QPushButton("OK", this);
    cancelButton = new QPushButton("Cancel", this);

    QHBoxLayout *buttonLayout = new QHBoxLayout();
    buttonLayout->addWidget(okButton);
    buttonLayout->addWidget(cancelButton);

    mainLayout->addWidget(easyButton);
    mainLayout->addWidget(mediumButton);
    mainLayout->addWidget(hardButton);
    mainLayout->addLayout(buttonLayout);

    setLayout(mainLayout);

    connect(okButton, &QPushButton::clicked, this, &DifficultyDialog::accept);
    connect(cancelButton, &QPushButton::clicked, this, &DifficultyDialog::reject);
}

QString DifficultyDialog::selectedDifficulty() const {
    if (easyButton->isChecked()) return "Easy";
    if (mediumButton->isChecked()) return "Medium";
    if (hardButton->isChecked()) return "Hard";
    return "Medium";
}
