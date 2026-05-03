#pragma once
#include <QDialog>
#include <QRadioButton>

class DifficultyDialog : public QDialog {
    Q_OBJECT
public:
    explicit DifficultyDialog(QWidget *parent = nullptr);
    int getSelectedTime() const;
    int getSelectedLives() const;

private:
    QRadioButton *easyBtn;
    QRadioButton *mediumBtn;
    QRadioButton *hardBtn;
};
