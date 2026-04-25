#pragma once

#include <QButtonGroup>
#include <QDialog>

class DifficultyDialog : public QDialog {
    Q_OBJECT

public:
    explicit DifficultyDialog(int currentDifficulty, QWidget* parent = nullptr);
    int selectedDifficulty() const;

private:
    QButtonGroup* difficultyGroup = nullptr;
};
