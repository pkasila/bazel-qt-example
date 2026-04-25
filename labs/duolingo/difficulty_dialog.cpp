#include "difficulty_dialog.h"

#include <QDialogButtonBox>
#include <QFormLayout>
#include <QGroupBox>
#include <QLabel>
#include <QRadioButton>
#include <QVBoxLayout>

DifficultyDialog::DifficultyDialog(DifficultyLevel current, QWidget *parent)
    : QDialog(parent) {
    setWindowTitle("Select difficulty");
    setModal(true);

    auto *root = new QVBoxLayout(this);
    auto *info = new QLabel("Choose a difficulty level for the next session:");
    info->setWordWrap(true);
    root->addWidget(info);

    auto *box = new QGroupBox("Difficulty");
    auto *form = new QVBoxLayout(box);

    m_beginner = new QRadioButton("Beginner");
    m_intermediate = new QRadioButton("Intermediate");
    m_advanced = new QRadioButton("Advanced");

    form->addWidget(m_beginner);
    form->addWidget(m_intermediate);
    form->addWidget(m_advanced);

    root->addWidget(box);

    switch (current) {
    case DifficultyLevel::Beginner: m_beginner->setChecked(true); break;
    case DifficultyLevel::Intermediate: m_intermediate->setChecked(true); break;
    case DifficultyLevel::Advanced: m_advanced->setChecked(true); break;
    }

    auto *buttons = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
    connect(buttons, &QDialogButtonBox::accepted, this, &QDialog::accept);
    connect(buttons, &QDialogButtonBox::rejected, this, &QDialog::reject);
    root->addWidget(buttons);
}

DifficultyLevel DifficultyDialog::selectedDifficulty() const {
    if (m_intermediate->isChecked()) {
        return DifficultyLevel::Intermediate;
    }
    if (m_advanced->isChecked()) {
        return DifficultyLevel::Advanced;
    }
    return DifficultyLevel::Beginner;
}
