#include "AddMedicineDialog.h"


AddMedicineDialog::AddMedicineDialog(QWidget *parent)
    : QDialog(parent) {
    setWindowTitle("Информация о лекарстве");
    setFixedWidth(300);

    QVBoxLayout *mainLayout = new QVBoxLayout(this);

    QFormLayout *infoLayout = new QFormLayout();

    nameEdit = new QLineEdit(this);

    dosageSpinBox = new QSpinBox(this);

    measureComboBox = new QComboBox(this);
    for (const auto& measureUnit : measureInfo::measureUnits) {
        measureComboBox->addItem(measureUnit);
    }

    timeEdit = new QTimeEdit(QTime::currentTime(), this);
    timeEdit->setMinimumTime(QTime::currentTime());

    notesEdit = new QTextEdit(this);
    notesEdit->setFixedHeight(50);

    QHBoxLayout *dosageLayout = new QHBoxLayout();
    dosageLayout->addWidget(dosageSpinBox);
    dosageLayout->addWidget(measureComboBox);

    infoLayout->addRow("Название лекарства:", nameEdit);
    infoLayout->addRow("Дозировка:", dosageLayout);
    infoLayout->addRow("Время приёма:", timeEdit);
    infoLayout->addRow("Заметки", notesEdit);

    mainLayout->addLayout(infoLayout);

    QHBoxLayout *buttonLayout = new QHBoxLayout();
    okButton = new QPushButton("OK", this);
    cancelButton = new QPushButton("Отмена", this);
    buttonLayout->addWidget(okButton);
    buttonLayout->addWidget(cancelButton);

    mainLayout->addLayout(buttonLayout);

    connect(okButton, &QPushButton::clicked, this, &AddMedicineDialog::confirmMedicine);
    connect(cancelButton, &QPushButton::clicked, this, &QDialog::reject);
}

void AddMedicineDialog::confirmMedicine() {
    if (nameEdit->text().isEmpty() || dosageSpinBox->value() == 0) {
        QMessageBox::warning(this, "Ошибка", "Заполните все поля!");
        return;
    }
    accept();
}

Medicine AddMedicineDialog::getMedicine() const {
    return {nameEdit->text(),
            QString::number(dosageSpinBox->value()) + " " +
                measureComboBox->currentText(),
            timeEdit->time(),
            notesEdit->toPlainText()
    };
}
