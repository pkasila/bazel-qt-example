#include "difficulty_dialog.h"

#include <QFont>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>

DifficultyDialog::DifficultyDialog(QWidget* parent) : QDialog(parent) {
    setWindowTitle("Difficulty Settings");
    setFixedSize(350, 180);
    auto* layout = new QVBoxLayout(this);

    auto* lbl = new QLabel("Select difficulty level:", this);
    lbl->setFont(QFont("Arial", 11, QFont::Bold));

    m_combo = new QComboBox(this);
    m_combo->addItems(
        {"Easy (3 tasks) - Beginner level", "Medium (5 tasks) - Intermediate level",
         "Hard (7 tasks) - Advanced level"});
    m_combo->setToolTip("Choose how many tasks you want to complete");

    auto* infoLabel = new QLabel("Tip: Start with Easy if you're new!", this);
    infoLabel->setStyleSheet("color: #2196F3; font-style: italic;");

    auto* btnLayout = new QHBoxLayout();
    auto* btnOk = new QPushButton("Apply", this);
    btnOk->setToolTip("Apply selected difficulty and close");
    auto* btnCancel = new QPushButton("Cancel", this);
    btnCancel->setToolTip("Close without changes");
    btnLayout->addWidget(btnOk);
    btnLayout->addWidget(btnCancel);

    layout->addWidget(lbl);
    layout->addWidget(m_combo);
    layout->addWidget(infoLabel);
    layout->addSpacing(10);
    layout->addLayout(btnLayout);

    connect(btnOk, &QPushButton::clicked, this, &QDialog::accept);
    connect(btnCancel, &QPushButton::clicked, this, &QDialog::reject);
}

int DifficultyDialog::getN() const {
    QString txt = m_combo->currentText();
    if (txt.contains("Easy")) {
        return 3;
    }
    if (txt.contains("Hard")) {
        return 7;
    }
    return 5;
}

QComboBox* DifficultyDialog::getCombo() const {
    return m_combo;
}