#ifndef HELPDIALOG_H
#define HELPDIALOG_H

#include <QDialog>
#include <QVBoxLayout>
#include <QLabel>
#include <QDialogButtonBox>

class HelpDialog : public QDialog
{
    Q_OBJECT

public:
    explicit HelpDialog(const QString& helpText, QWidget *parent = nullptr);

private:
    QVBoxLayout *mainLayout;
    QLabel *helpLabel;
    QDialogButtonBox *buttonBox;
};

#endif // HELPDIALOG_H