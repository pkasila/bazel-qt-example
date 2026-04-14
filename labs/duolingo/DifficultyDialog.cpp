#include "DifficultyDialog.h"

#include <QtWidgets/QFormLayout>

DifficultyDialog::DifficultyDialog(int curQ, int curL, int curT, int curMD, QWidget* parent)
    : QDialog(parent) {
    setWindowTitle("Настройки сложности");
    setMinimumSize(400, 250);
    QFormLayout* layout = new QFormLayout(this);
    layout->setSpacing(15);
    layout->setContentsMargins(20, 20, 20, 20);

    questionsBox = new QSpinBox(this);
    questionsBox->setRange(1, 100);
    questionsBox->setValue(curQ);
    questionsBox->setStyleSheet("QSpinBox { font-size: 16px; padding: 5px; }");

    livesBox = new QSpinBox(this);
    livesBox->setRange(1, 100);
    livesBox->setValue(curL);
    livesBox->setStyleSheet("QSpinBox { font-size: 16px; padding: 5px; }");

    timeBox = new QSpinBox(this);
    timeBox->setRange(5, 300);
    timeBox->setValue(curT);
    timeBox->setSuffix(" сек");
    timeBox->setStyleSheet("QSpinBox { font-size: 16px; padding: 5px; }");

    mathDiffBox = new QComboBox(this);
    mathDiffBox->addItems({"Легко", "Средне", "Сложно", "Ультра Хард"});
    mathDiffBox->setCurrentIndex(curMD);
    mathDiffBox->setStyleSheet("QComboBox { font-size: 16px; padding: 5px; }");

    layout->addRow("Количество вопросов:", questionsBox);
    layout->addRow("Количество жизней:", livesBox);
    layout->addRow("Время на попытку:", timeBox);
    layout->addRow("Сложность математики:", mathDiffBox);

    okBtn = new QPushButton("Сохранить", this);
    okBtn->setStyleSheet(
        "QPushButton { background-color: #58cc02; color: white; font-weight: bold; font-size: "
        "16px; border-radius: 10px; padding: 10px; } QPushButton:hover { background-color: "
        "#61e002; }");
    layout->addRow(okBtn);

    connect(okBtn, &QPushButton::clicked, this, &QDialog::accept);
}

int DifficultyDialog::getQuestions() const {
    return questionsBox->value();
}

int DifficultyDialog::getLives() const {
    return livesBox->value();
}

int DifficultyDialog::getTime() const {
    return timeBox->value();
}

int DifficultyDialog::getMathDifficulty() const {
    return mathDiffBox->currentIndex();
}