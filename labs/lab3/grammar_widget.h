#ifndef GRAMMAR_WIDGET_H
#define GRAMMAR_WIDGET_H

#include <QButtonGroup>
#include <QLabel>
#include <QProgressBar>
#include <QPushButton>
#include <QRadioButton>
#include <QSoundEffect>
#include <QTimer>
#include <QVBoxLayout>
#include <QWidget>
#include <vector>

#include "exercise_data.h"

class GrammarWidget : public QWidget {
  Q_OBJECT

 public:
  explicit GrammarWidget(QWidget* parent = nullptr);

  void StartExercise(Difficulty difficulty);
  void StopExercise();
  [[nodiscard]] QString GetCurrentHint() const;

 signals:
  void ExerciseFinished(int points);

 private slots:
  void OnSubmit();
  void OnTimerTick();

 private:
  void SetupUi();
  void ShowTask();
  void FinishExercise(bool success);
  void ClearOptions();

  QLabel* title_label_;
  QLabel* sentence_label_;
  QVBoxLayout* options_layout_;
  QButtonGroup* button_group_;
  QVector<QRadioButton*> option_buttons_;
  QPushButton* submit_button_;
  QProgressBar* progress_bar_;
  QLabel* timer_label_;
  QLabel* feedback_label_;
  QLabel* wrong_count_label_;

  QTimer* timer_;
  int remaining_seconds_ = 0;

  std::vector<GrammarTask> tasks_;
  int current_task_ = 0;
  int wrong_attempts_ = 0;
  int max_wrong_ = 3;
  int points_ = 0;
  Difficulty difficulty_ = Difficulty::Easy;
  bool active_ = false;

  QSoundEffect correct_sound_;
  QSoundEffect wrong_sound_;
  QSoundEffect complete_sound_;
};

#endif
