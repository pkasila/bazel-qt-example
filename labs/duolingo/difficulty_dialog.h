#ifndef DIFFICULTY_DIALOG_H
#define DIFFICULTY_DIALOG_H

#include "task.h"

#include <QDialog>

class QRadioButton;

class DifficultyDialog final : public QDialog {
public:
    explicit DifficultyDialog(DifficultyLevel current, QWidget *parent = nullptr);

    DifficultyLevel selectedDifficulty() const;

private:
    QRadioButton *m_beginner = nullptr;
    QRadioButton *m_intermediate = nullptr;
    QRadioButton *m_advanced = nullptr;
};

#endif // DIFFICULTY_DIALOG_H
