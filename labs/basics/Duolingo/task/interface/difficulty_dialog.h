#ifndef DIFFICULTY_DIALOG_H
#define DIFFICULTY_DIALOG_H

#include <QComboBox>
#include <QDialog>
#include <QLabel>
#include <QPushButton>
#include <QSpinBox>
#include <QVBoxLayout>

class DifficultyDialog : public QDialog {
    Q_OBJECT
   public:
    explicit DifficultyDialog(QWidget* parent = nullptr);
    QString getDifficulty() const;
    int getDuration() const;

   private:
    QComboBox* difficulty_combo_;
    QSpinBox* duration_spin_;
};

#endif  // DIFFICULTY_DIALOG_H
