#ifndef DIFFICULTY_DIALOG_H
#define DIFFICULTY_DIALOG_H

#include <QDialog>
#include <QRadioButton>

#include "exercise_data.h"

class DifficultyDialog : public QDialog {
  Q_OBJECT

 public:
  explicit DifficultyDialog(Difficulty current, QWidget* parent = nullptr);

  [[nodiscard]] Difficulty GetSelectedDifficulty() const;

 private:
  QRadioButton* easy_radio_;
  QRadioButton* medium_radio_;
  QRadioButton* hard_radio_;
};

#endif
