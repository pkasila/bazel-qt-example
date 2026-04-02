#ifndef DIFFICULTYDIALOG_H
#define DIFFICULTYDIALOG_H

#include <QDialog>

class QComboBox;
class QLabel;

class DifficultyDialog : public QDialog
{
    Q_OBJECT

public:
    explicit DifficultyDialog(const QString& currentDifficulty, const QStringList& levels, QWidget *parent = nullptr);
    QString getSelectedDifficulty() const;

private:
    QComboBox *difficultyCombo;
    QLabel *infoLabel;
};

#endif // DIFFICULTYDIALOG_H
