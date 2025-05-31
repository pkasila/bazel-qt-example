#ifndef DIFFICULTYDIALOG_H
#define DIFFICULTYDIALOG_H

#include <QDialog>
#include <QRadioButton>
#include <QPushButton>
#include <QVBoxLayout>
#include <QButtonGroup>

class DifficultyDialog : public QDialog
{
    Q_OBJECT

public:
    explicit DifficultyDialog(const QString& currentDifficulty, QWidget *parent = nullptr);
    QString selectedDifficulty() const;

private:
    QVBoxLayout *mainLayout;
    QRadioButton *easyButton;
    QRadioButton *mediumButton;
    QRadioButton *hardButton;
    QButtonGroup *buttonGroup;
    QPushButton *okButton;
    QPushButton *cancelButton;

    QString m_selectedDifficulty;
};

#endif // DIFFICULTYDIALOG_H
