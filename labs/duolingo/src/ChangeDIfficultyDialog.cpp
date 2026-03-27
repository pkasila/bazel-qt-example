#include "ChangeDIfficultyDialog.h"

#include "Settings.h"
#include "TaskDifficulty.h"

#include <QComboBox>
#include <QPushButton>
#include <QString>
#include <QVBoxLayout>
#include <memory>

ChangeDIfficultyDialog::ChangeDIfficultyDialog(
    const std::shared_ptr<Settings>& settings, QWidget* parent)
    : QDialog(parent) {
    setWindowTitle("Выбрать сложность");

    auto* variants = new QComboBox(this);
    variants->addItem(QString("Легкая"));
    variants->addItem(QString("Средняя"));
    variants->addItem(QString("Сложная"));
    variants->setCurrentIndex(static_cast<int>(settings->GetTaskDifficulty()));

    auto* layout = new QVBoxLayout;
    layout->addWidget(variants);

    auto* accept_button = new QPushButton(QString("Принять изменения"));
    layout->addWidget(accept_button);

    connect(accept_button, &QPushButton::clicked, this, [this, variants, &settings] {
        settings->SetTaskDifficulty(static_cast<TaskDifficulty>(variants->currentIndex()));
        this->accept();
    });

    setLayout(layout);

    resize(300, 200);
}
