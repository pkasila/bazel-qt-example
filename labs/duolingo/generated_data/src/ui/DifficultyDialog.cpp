#include "DifficultyDialog.h"
#include <QVBoxLayout>
#include <QComboBox>
#include <QDialogButtonBox>
#include <QLabel>

DifficultyDialog::DifficultyDialog(QWidget *parent) : QDialog(parent) {
    setWindowTitle(tr("Change Difficulty"));
    setModal(true);

    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->addWidget(new QLabel(tr("Select difficulty level:"), this));

    m_difficultyComboBox = new QComboBox(this);
    m_difficultyComboBox->addItem(tr("Easy"), 0);   // Value 0
    m_difficultyComboBox->addItem(tr("Medium"), 1); // Value 1
    m_difficultyComboBox->addItem(tr("Hard"), 2);   // Value 2
    // TODO: Load these from DifficultyLevels table in DB or SettingsManager

    layout->addWidget(m_difficultyComboBox);

    QDialogButtonBox *buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, this);
    connect(buttonBox, &QDialogButtonBox::accepted, this, &QDialog::accept);
    connect(buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);
    layout->addWidget(buttonBox);

    setLayout(layout);
}

void DifficultyDialog::setCurrentDifficulty(int difficultyIndex) {
    m_difficultyComboBox->setCurrentIndex(difficultyIndex); // Assuming index matches value for simplicity
}

int DifficultyDialog::getSelectedDifficulty() const {
    return m_difficultyComboBox->currentData().toInt(); // Or currentIndex() if values match index
}