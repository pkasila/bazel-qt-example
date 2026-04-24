#pragma once

#include <QComboBox>
#include <QDialog>

class DifficultyDialog : public QDialog {
   public:
    explicit DifficultyDialog(QWidget* parent = nullptr);
    int getN() const;
    QComboBox* getCombo() const;

   private:
    QComboBox* m_combo;
};