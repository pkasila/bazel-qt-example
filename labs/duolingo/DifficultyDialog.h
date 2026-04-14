#ifndef DIFFICULTYDIALOG_H
#define DIFFICULTYDIALOG_H

#include <QtCore/QString>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QDialog>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpinBox>

class DifficultyDialog : public QDialog {
    Q_OBJECT
   public:
    explicit DifficultyDialog(int curQ, int curL, int curT, int curMD, QWidget* parent = nullptr);
    int getQuestions() const;
    int getLives() const;
    int getTime() const;
    int getMathDifficulty() const;

   private:
    QSpinBox* questionsBox;
    QSpinBox* livesBox;
    QSpinBox* timeBox;
    QComboBox* mathDiffBox;
    QPushButton* okBtn;
};

#endif