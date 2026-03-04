#ifndef HELPDIALOG_H
#define HELPDIALOG_H

#include <QDialog>
#include <QTextEdit>

class HelpDialog : public QDialog
{
    Q_OBJECT

public:
    explicit HelpDialog(QWidget *parent = nullptr);
    void setHelpText(const QString &text);

private:
    QTextEdit *helpText;
};

#endif // HELPDIALOG_H
