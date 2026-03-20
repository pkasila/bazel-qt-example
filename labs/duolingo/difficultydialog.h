#ifndef DIFFICULTYDIALOG_H
#define DIFFICULTYDIALOG_H

#include <QDialog>
#include <QVBoxLayout>
#include <QRadioButton>
#include <QButtonGroup>
#include <QDialogButtonBox>

class DifficultyDialog : public QDialog
{
    Q_OBJECT

public:
    enum DifficultyLevel {
        Easy,
        Medium,
        Hard
    };
    Q_ENUM(DifficultyLevel)

    explicit DifficultyDialog(DifficultyLevel currentLevel = Easy, QWidget *parent = nullptr);
    DifficultyLevel getSelectedDifficulty() const;
    QString getDifficultyString(DifficultyLevel level) const;

private:
    QVBoxLayout *mainLayout;
    QButtonGroup *levelButtonGroup;
    QDialogButtonBox *buttonBox;
};

#endif // DIFFICULTYDIALOG_H