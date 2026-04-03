#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>

#include "difficultydialog.h"

DifficultyDialog::DifficultyDialog(QWidget *parent) : QDialog(parent) {
    setWindowTitle("Select difficulty");

    QLabel* label = new QLabel("Select difficulty:", this);
    difficulty_combo_box_ = new QComboBox(this);
    ok_btn_ = new QPushButton("OK", this);
    cancel_btn_ = new QPushButton("Cancel", this);

    difficulty_combo_box_->addItem("Easy", QVariant::fromValue(Difficulty::Easy));
    difficulty_combo_box_->addItem("Medium", QVariant::fromValue(Difficulty::Medium));
    difficulty_combo_box_->addItem("Hard", QVariant::fromValue(Difficulty::Hard));

    connect(ok_btn_, &QPushButton::clicked, this, &QDialog::accept);
    connect(cancel_btn_, &QPushButton::clicked, this, &QDialog::reject);

    QHBoxLayout *combo_layout = new QHBoxLayout();
    combo_layout->addWidget(label);
    combo_layout->addWidget(difficulty_combo_box_);

    QHBoxLayout *btn_layout = new QHBoxLayout();
    btn_layout->addStretch();
    btn_layout->addWidget(ok_btn_);
    btn_layout->addWidget(cancel_btn_);

    QVBoxLayout *main_layout = new QVBoxLayout(this);
    main_layout->addLayout(combo_layout);
    main_layout->addLayout(btn_layout);

    setLayout(main_layout);

    SetCurrentDifficulty(Difficulty::Easy);
}

DifficultyDialog::~DifficultyDialog() {

}

Difficulty DifficultyDialog::GetSelectedDifficulty() const {
    return difficulty_combo_box_->currentData().value<Difficulty>();
}

void DifficultyDialog::SetCurrentDifficulty(Difficulty difficulty) {
    cur_difficulty_ = difficulty;
    for(int i = 0; i < difficulty_combo_box_->count(); i++) {
        if (difficulty_combo_box_->itemData(i).value<Difficulty>() == difficulty) {
            difficulty_combo_box_->setCurrentIndex(i);
            break;
        }
    }
}
