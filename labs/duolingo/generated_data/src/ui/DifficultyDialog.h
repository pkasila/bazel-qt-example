#ifndef DIFFICULTYDIALOG_H
#define DIFFICULTYDIALOG_H

#include <QDialog>

class QComboBox; // Forward declaration

class DifficultyDialog : public QDialog {
    Q_OBJECT

public:
    explicit DifficultyDialog(QWidget *parent = nullptr);

    void setCurrentDifficulty(int difficultyIndex);
    int getSelectedDifficulty() const;

private:
    QComboBox *m_difficultyComboBox;
};

#endif // DIFFICULTYDIALOG_H