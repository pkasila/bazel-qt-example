#ifndef DIFFICULTYDIALOG_H
#define DIFFICULTYDIALOG_H

#include <QComboBox>
#include <QPushButton>
#include <QDialog>

#include "exercisedata.h"

class DifficultyDialog : public QDialog
{
    Q_OBJECT
public:
    explicit DifficultyDialog(QWidget *parent = nullptr);
    ~DifficultyDialog();
    Difficulty GetSelectedDifficulty() const;
    void SetCurrentDifficulty(Difficulty difficulty);

private:
    QComboBox* difficulty_combo_box_;
    QPushButton* ok_btn_;
    QPushButton* cancel_btn_;
    Difficulty cur_difficulty_ = Difficulty::Easy;
};

#endif // DIFFICULTYDIALOG_H
