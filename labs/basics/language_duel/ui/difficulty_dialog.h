#pragma once

#include <QDialog>

#include "core/models.h"

class QComboBox;

class DifficultyDialog : public QDialog {
    Q_OBJECT

public:
    explicit DifficultyDialog(Difficulty currentDifficulty, QWidget* parent = nullptr);

    Difficulty selectedDifficulty() const;

private:
    QComboBox* combo_ = nullptr;
};
