#pragma once
#include <QDialog>
#include <QComboBox>
#include <QPushButton>

class DifficultyDialog : public QDialog {
    Q_OBJECT
public:
    explicit DifficultyDialog(int currentLevel, QWidget *parent = nullptr);
    int getSelectedLevel() const;
private:
    QComboBox *combo;
    QPushButton *btnOk;
};
