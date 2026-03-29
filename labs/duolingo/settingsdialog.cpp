#include "settingsdialog.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QComboBox>
#include <QPushButton>
#include <QLabel>

SettingsDialog::SettingsDialog(Difficulty currentDifficulty, QWidget *parent)
    : QDialog(parent), m_currentDifficulty(currentDifficulty) {
    setWindowTitle(tr("Difficulty Settings"));

    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(15, 15, 15, 15);
    mainLayout->setSpacing(10);

    QLabel *difficultyLabel = new QLabel(tr("Select Difficulty Level:"));
    difficultyLabel->setStyleSheet("color: #34495e; font-weight: bold;");

    difficultyComboBox = new QComboBox();
    difficultyComboBox->addItem(tr("Easy Peasy"), QVariant::fromValue(Difficulty::Easy));
    difficultyComboBox->addItem(tr("Getting Tricky"), QVariant::fromValue(Difficulty::Medium));
    difficultyComboBox->addItem(tr("Brain Buster"), QVariant::fromValue(Difficulty::Hard));
    difficultyComboBox->setStyleSheet("QComboBox { padding: 5px; border: 1px solid #bdc3c7; border-radius: 3px; }");


    int currentIndex = 0;
    switch(currentDifficulty) {
        case Difficulty::Easy: currentIndex = 0; break;
        case Difficulty::Medium: currentIndex = 1; break;
        case Difficulty::Hard: currentIndex = 2; break;
    }
    difficultyComboBox->setCurrentIndex(currentIndex);

    QHBoxLayout *buttonLayout = new QHBoxLayout();
    okButton = new QPushButton(tr("Apply"));
    cancelButton = new QPushButton(tr("Cancel"));

    QString dialogButtonStyle = "QPushButton { background-color: #5dade2; color: white; border: none; padding: 8px 15px; border-radius: 5px; }"
                                "QPushButton:hover { background-color: #3498db; }";
    okButton->setStyleSheet(dialogButtonStyle);
    cancelButton->setStyleSheet("QPushButton { background-color: #e74c3c; color: white; border: none; padding: 8px 15px; border-radius: 5px; }"
                              "QPushButton:hover { background-color: #c0392b; }");


    buttonLayout->addStretch();
    buttonLayout->addWidget(okButton);
    buttonLayout->addWidget(cancelButton);

    mainLayout->addWidget(difficultyLabel);
    mainLayout->addWidget(difficultyComboBox);
    mainLayout->addSpacing(20);
    mainLayout->addLayout(buttonLayout);

    connect(okButton, &QPushButton::clicked, this, &SettingsDialog::accept);
    connect(cancelButton, &QPushButton::clicked, this, &SettingsDialog::reject);

    setLayout(mainLayout);
}

Difficulty SettingsDialog::selectedDifficulty() const {
    return difficultyComboBox->currentData().value<Difficulty>();
}