#ifndef ADDGRAMMARDIALOG_H
#define ADDGRAMMARDIALOG_H

#include <QDialog>
#include <QStringList>

#include <QLineEdit>
#include <QSpinBox>
#include <QDialogButtonBox>

class AddGrammarDialog : public QDialog {
    Q_OBJECT

public:
    AddGrammarDialog(QWidget *parent = nullptr);

    QString getQuestionText() const;
    QStringList getOptions() const;
    int getCorrectOptionIndex() const;

private slots:
    void validateAndAccept();

private:
    void setupUi();

    QLineEdit *questionEdit;
    QLineEdit *optionEdits[4];
    QSpinBox *correctOptionSpin;
    QDialogButtonBox *buttonBox;
};

#endif // ADDGRAMMARDIALOG_H
