#include "helpdialog.h"

HelpDialog::HelpDialog(const QString& helpText, QWidget *parent)
    : QDialog(parent),
      mainLayout(new QVBoxLayout(this)),
      helpLabel(new QLabel(helpText, this)),
      buttonBox(new QDialogButtonBox(QDialogButtonBox::Ok))
{
    setWindowTitle("Справка по упражнению");
    helpLabel->setWordWrap(true);
    helpLabel->setStyleSheet("padding: 10px;"); 

    mainLayout->addWidget(helpLabel);
    mainLayout->addWidget(buttonBox);

    connect(buttonBox, &QDialogButtonBox::accepted, this, &QDialog::accept);

    setMinimumWidth(300);
}