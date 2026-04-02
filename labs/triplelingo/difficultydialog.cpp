#include "difficultydialog.h"
#include <QVBoxLayout>
#include <QComboBox>
#include <QLabel>
#include <QDialogButtonBox>

DifficultyDialog::DifficultyDialog(const QString& currentDifficulty, const QStringList& levels, QWidget *parent)
    : QDialog(parent)
{
    setWindowTitle(tr("Select Difficulty"));
    setModal(true);

    QVBoxLayout *layout = new QVBoxLayout(this);

    infoLabel = new QLabel(tr("Current difficulty: %1\nSelect new difficulty:").arg(currentDifficulty));
    layout->addWidget(infoLabel);

    difficultyCombo = new QComboBox();
    difficultyCombo->addItems(levels);
    difficultyCombo->setCurrentText(currentDifficulty);
    layout->addWidget(difficultyCombo);

    QDialogButtonBox *buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
    connect(buttonBox, &QDialogButtonBox::accepted, this, &DifficultyDialog::accept);
    connect(buttonBox, &QDialogButtonBox::rejected, this, &DifficultyDialog::reject);
    layout->addWidget(buttonBox);

    setLayout(layout);
}

QString DifficultyDialog::getSelectedDifficulty() const {
    return difficultyCombo->currentText();
}
