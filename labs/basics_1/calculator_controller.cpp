//
// Created by blalex on 3/14/25.
//

#include "calculator_controller.h"

#include <QPushButton>
#include <QVBoxLayout>
#include <qlabel.h>
#include <unordered_set>

CalculatorController::CalculatorController(QObject* parent) : QObject(parent) {
}

void CalculatorController::UpdateValues(QLineEdit* edited) {
    if (std::size(edited->text()) == 0 ||
        edited->text()[std::size(edited->text()) - 1] == QString(".") || is_work_) {
        return;
    }
    is_work_ = true;
    Field changed_field{};
    for (const auto& field : fields_) {
        if (field.value_ == edited) {
            changed_field = field;
        }
    }

    const double base =
        ToBase(changed_field.unit_->currentText(), changed_field.value_->text().toDouble());
    for (const auto& field : fields_) {
        if (field.value_ != changed_field.value_) {
            field.value_->setText(QString::number(FromBase(field.unit_->currentText(), base)));
        }
    }
    is_work_ = false;
}

void CalculatorController::UpdateUnit(QLineEdit* edited, QString unit) {
    Field* changed_field = nullptr;

    for (auto& field : fields_) {
        if (field.value_ == edited) {
            changed_field = &field;
        }
    }

    if (units_.contains(unit)) {
        changed_field->unit_->setCurrentText(changed_field->prev_value_);
        QDialog errorDialog(edited->window());
        errorDialog.setWindowTitle("Invalid choise");
        errorDialog.setModal(true);

        QVBoxLayout* layout = new QVBoxLayout(&errorDialog);
        QLabel* label = new QLabel("You cannot choose it.");
        QPushButton* okButton = new QPushButton("OK");

        layout->addWidget(label);
        layout->addWidget(okButton);

        QObject::connect(okButton, &QPushButton::clicked, &errorDialog, &QDialog::accept);

        errorDialog.exec();

        return;
    }

    const double base = ToBase(changed_field->prev_value_, edited->text().toDouble());
    changed_field->prev_value_ = unit;

    edited->setText(QString::number(FromBase(unit, base)));
}

float CalculatorController::ToBase(const QString& unit, const double number) {
    return to_base_[unit] * number;
}

float CalculatorController::FromBase(const QString& unit, const double number) {
    return from_base_[unit] * number;
}
