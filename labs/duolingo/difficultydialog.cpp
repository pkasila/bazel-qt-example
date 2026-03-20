#include "difficultydialog.h"
#include <QLabel>

DifficultyDialog::DifficultyDialog(DifficultyLevel currentLevel, QWidget *parent)
    : QDialog(parent),
      mainLayout(new QVBoxLayout(this)),
      levelButtonGroup(new QButtonGroup(this)),
      buttonBox(new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel))
{
    setWindowTitle("Выберите уровень сложности");
    mainLayout->addWidget(new QLabel("Выберите уровень сложности:"));

    QRadioButton *easyButton = new QRadioButton("Легкий");
    QRadioButton *mediumButton = new QRadioButton("Средний");
    QRadioButton *hardButton = new QRadioButton("Сложный");

    levelButtonGroup->addButton(easyButton, Easy);
    levelButtonGroup->addButton(mediumButton, Medium);
    levelButtonGroup->addButton(hardButton, Hard);

    mainLayout->addWidget(easyButton);
    mainLayout->addWidget(mediumButton);
    mainLayout->addWidget(hardButton);

    mainLayout->addWidget(buttonBox);

    // Установить текущий выбранный уровень
    levelButtonGroup->button(currentLevel)->setChecked(true);

    connect(buttonBox, &QDialogButtonBox::accepted, this, &QDialog::accept);
    connect(buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);
}

DifficultyDialog::DifficultyLevel DifficultyDialog::getSelectedDifficulty() const
{
    return static_cast<DifficultyLevel>(levelButtonGroup->checkedId());
}

QString DifficultyDialog::getDifficultyString(DifficultyLevel level) const
{
    switch (level) {
        case Easy: return "Легкий";
        case Medium: return "Средний";
        case Hard: return "Сложный";
        default: return "Неизвестный";
    }
}