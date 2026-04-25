#include "difficultydialog.h"

#include "appstyle.h"

#include <QDialogButtonBox>
#include <QLabel>
#include <QRadioButton>
#include <QVBoxLayout>

DifficultyDialog::DifficultyDialog(int currentDifficulty, QWidget* parent)
    : QDialog(parent), difficultyGroup(new QButtonGroup(this)) {
    setWindowTitle("Difficulty");
    setModal(true);
    setMinimumWidth(360);
    setStyleSheet(AppStyle::styleSheet());

    auto* layout = new QVBoxLayout(this);
    layout->setContentsMargins(24, 24, 24, 24);
    layout->setSpacing(14);

    auto* title = new QLabel("Choose the challenge level", this);
    title->setObjectName("ExerciseTitle");
    title->setWordWrap(true);
    layout->addWidget(title);

    auto* subtitle = new QLabel("Harder levels allow fewer mistakes before the exercise stops.", this);
    subtitle->setObjectName("MutedText");
    subtitle->setWordWrap(true);
    layout->addWidget(subtitle);

    auto* easy = new QRadioButton("Easy - 3 attempts", this);
    auto* medium = new QRadioButton("Medium - 2 attempts", this);
    auto* hard = new QRadioButton("Hard - 1 attempt", this);

    difficultyGroup->addButton(easy, 0);
    difficultyGroup->addButton(medium, 1);
    difficultyGroup->addButton(hard, 2);

    layout->addWidget(easy);
    layout->addWidget(medium);
    layout->addWidget(hard);

    if (currentDifficulty == 1) {
        medium->setChecked(true);
    } else if (currentDifficulty == 2) {
        hard->setChecked(true);
    } else {
        easy->setChecked(true);
    }

    auto* buttons = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, this);
    connect(buttons, &QDialogButtonBox::accepted, this, &QDialog::accept);
    connect(buttons, &QDialogButtonBox::rejected, this, &QDialog::reject);
    layout->addWidget(buttons);
}

int DifficultyDialog::selectedDifficulty() const {
    return difficultyGroup->checkedId();
}
