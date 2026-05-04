#include "difficultydialog.h"
#include <QVBoxLayout>

DifficultyDialog::DifficultyDialog(int currentLevel, QWidget *parent) : QDialog(parent) {
    setWindowTitle("Select Turf (Difficulty)");
    QVBoxLayout *layout = new QVBoxLayout(this);

    combo = new QComboBox(this);
    combo->addItem("Rookie (Easy)", 1);
    combo->addItem("Hustler (Medium)", 2);
    combo->addItem("Kingpin (Hard)", 3);
    combo->setCurrentIndex(currentLevel - 1);

    btnOk = new QPushButton("Confirm", this);
    connect(btnOk, &QPushButton::clicked, this, &QDialog::accept);

    layout->addWidget(combo);
    layout->addWidget(btnOk);
}

int DifficultyDialog::getSelectedLevel() const {
    return combo->currentData().toInt();
}