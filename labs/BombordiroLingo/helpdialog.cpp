#include "helpdialog.h"
#include <QVBoxLayout>
#include <QDialogButtonBox>

HelpDialog::HelpDialog(QWidget *parent)
    : QDialog(parent),
    helpText(new QTextEdit(this))
{
    setWindowTitle("Help");
    resize(400, 300);

    helpText->setReadOnly(true);

    QDialogButtonBox *buttonBox = new QDialogButtonBox(
        QDialogButtonBox::Ok, this);
    connect(buttonBox, &QDialogButtonBox::accepted, this, &QDialog::accept);

    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->addWidget(helpText);
    layout->addWidget(buttonBox);
}

void HelpDialog::setHelpText(const QString &text)
{
    helpText->setText(text);
}
