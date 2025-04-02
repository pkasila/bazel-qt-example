#include "AddTicketsDialog.h"

// Диалог добавления билетов
AddTicketsDialog::AddTicketsDialog(QWidget *parent) : QDialog(parent) {
    setWindowTitle(WindowTitles::addTicketsDialogWindowTitle);
    QVBoxLayout *layout = new QVBoxLayout(this);
    QLabel *label = new QLabel(WindowTexts::addTicketsDialogQuestion, this);
    label->setAlignment(Qt::AlignCenter);
    additionalTicketCountInput = new QSpinBox(this);

    QHBoxLayout *buttonLayout = new QHBoxLayout;
    okButton = new CursorChangingButton(ButtonTexts::ok, this);
    cancelButton = new CursorChangingButton(ButtonTexts::cancel, this);
    buttonLayout->addWidget(okButton);
    buttonLayout->addWidget(cancelButton);

    layout->addWidget(label);
    layout->addWidget(additionalTicketCountInput);
    layout->addLayout(buttonLayout);

    connect(okButton, &QPushButton::clicked, this, &QDialog::accept);
    connect(cancelButton, &QPushButton::clicked, this, &QDialog::reject);
    setLayout(layout);

    setFixedSize(Geometry::addTicketsDialogWindowSize);
}


int AddTicketsDialog::getTicketCount() {
    if (!InputUtils::validateInput(this,
            additionalTicketCountInput->text())) {
        return 0;
    }
    return additionalTicketCountInput->text().toInt();
}
