#include "difficultydialog.h"
#include <QVBoxLayout>
#include <QDialogButtonBox>
#include "constants.h"

DifficultyDialog::DifficultyDialog(QWidget *parent)
    : QDialog(parent),
    easyRadio(new QRadioButton("Easy", this)),
    mediumRadio(new QRadioButton("Medium", this)),
    hardRadio(new QRadioButton("Hard", this)),
    difficultyGroup(new QButtonGroup(this))
{
    setWindowTitle("Select Difficulty");
    resize(300, 150);

    mediumRadio->setChecked(true);
    difficultyGroup->addButton(easyRadio, 0);
    difficultyGroup->addButton(mediumRadio, 1);
    difficultyGroup->addButton(hardRadio, 2);

    QDialogButtonBox *buttonBox = new QDialogButtonBox(
        QDialogButtonBox::Ok | QDialogButtonBox::Cancel, this);

    connect(buttonBox, &QDialogButtonBox::accepted, this, &QDialog::accept);
    connect(buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);

    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->addWidget(easyRadio);
    layout->addWidget(mediumRadio);
    layout->addWidget(hardRadio);
    layout->addWidget(buttonBox);
}

void DifficultyDialog::setDifficulty(const QString &difficulty)
{
    if (difficulty == Constants::EASY_DIFFICULTY) {
        easyRadio->setChecked(true);
    } else if (difficulty == Constants::HARD_DIFFICULTY) {
        hardRadio->setChecked(true);
    } else {
        mediumRadio->setChecked(true);
    }
}

QString DifficultyDialog::getDifficulty() const
{
    if (easyRadio->isChecked()) {
        return Constants::EASY_DIFFICULTY;
    } else if (hardRadio->isChecked()) {
        return Constants::HARD_DIFFICULTY;
    }
    return Constants::MEDIUM_DIFFICULTY;
}
