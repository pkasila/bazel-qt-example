#ifndef ADDTRANSLATIONDIALOG_H
#define ADDTRANSLATIONDIALOG_H

#include <QDialog>
#include <QString>

#include <QLineEdit>
#include <QComboBox>
#include <QDialogButtonBox>

class AddTranslationDialog : public QDialog {
    Q_OBJECT

public:
    AddTranslationDialog(QWidget *parent = nullptr);

    QString getOriginalSentence() const;
    QString getTranslatedSentence() const;
    int getLevel() const;

private slots:
    void validateAndAccept();

private:
    void setupUi();

    QLineEdit *originalEdit;
    QLineEdit *translationEdit;
    QComboBox *levelCombo;
    QDialogButtonBox *buttonBox;
};

#endif // ADDTRANSLATIONDIALOG_H
