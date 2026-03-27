#include "HelpDialog.h"
#include <QVBoxLayout>
#include <QTextBrowser>
#include <QPushButton>

HelpDialog::HelpDialog(const QString& helpText, QWidget *parent) : QDialog(parent) {
    setWindowTitle(tr("Help"));
    setMinimumSize(400, 300);

    QVBoxLayout *layout = new QVBoxLayout(this);

    m_helpTextBrowser = new QTextBrowser(this);
    m_helpTextBrowser->setHtml(helpText); // Use setHtml for rich text if needed

    layout->addWidget(m_helpTextBrowser);

    QPushButton *okButton = new QPushButton(tr("OK"), this);
    connect(okButton, &QPushButton::clicked, this, &QDialog::accept);
    layout->addWidget(okButton);
    layout->setAlignment(okButton, Qt::AlignRight);


    setLayout(layout);
}