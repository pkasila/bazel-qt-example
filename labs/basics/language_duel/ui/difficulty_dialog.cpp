#include "ui/difficulty_dialog.h"

#include <QComboBox>
#include <QDialogButtonBox>
#include <QFormLayout>
#include <QLabel>
#include <QVBoxLayout>

DifficultyDialog::DifficultyDialog(Difficulty currentDifficulty, QWidget* parent)
    : QDialog(parent) {
    setWindowTitle("Select difficulty");
    setModal(true);
    resize(320, 140);

    auto* root = new QVBoxLayout(this);
    auto* title = new QLabel("Choose the exercise difficulty:");
    title->setWordWrap(true);

    combo_ = new QComboBox(this);
    combo_->addItem("Easy");
    combo_->addItem("Medium");
    combo_->addItem("Hard");

    switch (currentDifficulty) {
        case Difficulty::Easy:
            combo_->setCurrentIndex(0);
            break;
        case Difficulty::Medium:
            combo_->setCurrentIndex(1);
            break;
        case Difficulty::Hard:
            combo_->setCurrentIndex(2);
            break;
    }

    auto* form = new QFormLayout();
    form->addRow("Difficulty:", combo_);

    auto* buttons = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, this);
    connect(buttons, &QDialogButtonBox::accepted, this, &QDialog::accept);
    connect(buttons, &QDialogButtonBox::rejected, this, &QDialog::reject);

    root->addWidget(title);
    root->addLayout(form);
    root->addStretch();
    root->addWidget(buttons);
}

Difficulty DifficultyDialog::selectedDifficulty() const {
    switch (combo_->currentIndex()) {
        case 0:
            return Difficulty::Easy;
        case 1:
            return Difficulty::Medium;
        case 2:
        default:
            return Difficulty::Hard;
    }
}
