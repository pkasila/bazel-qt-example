#ifndef HELPDIALOG_H
#define HELPDIALOG_H

#include <QDialog>
#include <QString>

class QTextBrowser; // Forward declaration

class HelpDialog : public QDialog {
    Q_OBJECT

public:
    explicit HelpDialog(const QString& helpText, QWidget *parent = nullptr);

private:
    QTextBrowser *m_helpTextBrowser;
};

#endif // HELPDIALOG_H