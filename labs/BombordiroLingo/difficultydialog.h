#ifndef DIFFICULTYDIALOG_H
#define DIFFICULTYDIALOG_H

#include <QDialog>
#include <QRadioButton>
#include <QButtonGroup>

class DifficultyDialog : public QDialog
{
    Q_OBJECT

public:
    explicit DifficultyDialog(QWidget *parent = nullptr);
    void setDifficulty(const QString &difficulty);
    QString getDifficulty() const;

private:
    QRadioButton *easyRadio;
    QRadioButton *mediumRadio;
    QRadioButton *hardRadio;
    QButtonGroup *difficultyGroup;
};

#endif // DIFFICULTYDIALOG_H
