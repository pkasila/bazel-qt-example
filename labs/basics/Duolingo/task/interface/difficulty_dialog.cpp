#include "difficulty_dialog.h"

DifficultyDialog::DifficultyDialog(QWidget* parent) : QDialog(parent) {
    setWindowTitle("Select Difficulty");
    setMinimumWidth(300);

    auto* layout = new QVBoxLayout(this);
    layout->setContentsMargins(20, 20, 20, 20);
    layout->setSpacing(15);

    auto* label = new QLabel("Choose your learning level:", this);
    layout->addWidget(label);

    difficulty_combo_ = new QComboBox(this);
    difficulty_combo_->addItems({"Beginner", "Intermediate", "Advanced"});
    layout->addWidget(difficulty_combo_);

    auto* timeLabel = new QLabel("Exercise duration (seconds):", this);
    layout->addWidget(timeLabel);

    duration_spin_ = new QSpinBox(this);
    duration_spin_->setRange(10, 300);
    duration_spin_->setValue(60);
    layout->addWidget(duration_spin_);

    auto* ok_btn = new QPushButton("OK", this);
    connect(ok_btn, &QPushButton::clicked, this, &QDialog::accept);
    layout->addWidget(ok_btn);
}

QString DifficultyDialog::getDifficulty() const {
    return difficulty_combo_->currentText();
}

int DifficultyDialog::getDuration() const {
    return duration_spin_->value();
}
