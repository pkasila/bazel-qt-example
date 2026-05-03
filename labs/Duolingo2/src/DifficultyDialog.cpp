#include "DifficultyDialog.h"
#include <QVBoxLayout>
#include <QPushButton>
#include <QLabel>

DifficultyDialog::DifficultyDialog(QWidget *parent) : QDialog(parent) {
    setWindowTitle("Настройки сложности");
    setModal(true);
    setFixedSize(300, 200);

    QVBoxLayout *layout = new QVBoxLayout(this);
    
    QLabel *label = new QLabel("Выберите уровень сложности:", this);
    label->setStyleSheet("font-weight: bold; font-size: 14px;");

    easyBtn = new QRadioButton("Новичок (5 жизней, 60 сек)", this);
    mediumBtn = new QRadioButton("Студент (3 жизни, 30 сек)", this);
    hardBtn = new QRadioButton("Лингвист (1 жизнь, 15 сек)", this);
    mediumBtn->setChecked(true); // Default

    QPushButton *okBtn = new QPushButton("Сохранить", this);
    okBtn->setStyleSheet("background-color: #1CB0F6; color: white; border-radius: 8px; padding: 8px; font-weight: bold;");
    connect(okBtn, &QPushButton::clicked, this, &QDialog::accept);

    layout->addWidget(label);
    layout->addWidget(easyBtn);
    layout->addWidget(mediumBtn);
    layout->addWidget(hardBtn);
    layout->addWidget(okBtn);
}

int DifficultyDialog::getSelectedLives() const {
    if (easyBtn->isChecked()) return 5;
    if (hardBtn->isChecked()) return 1;
    return 3;
}

int DifficultyDialog::getSelectedTime() const {
    if (easyBtn->isChecked()) return 60;
    if (hardBtn->isChecked()) return 15;
    return 30;
}
