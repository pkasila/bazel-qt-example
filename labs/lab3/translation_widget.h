#ifndef TRANSLATION_WIDGET_H
#define TRANSLATION_WIDGET_H

#include <QLabel>
#include <QLineEdit>
#include <QProgressBar>
#include <QPushButton>
#include <QSoundEffect>
#include <QTimer>
#include <QWidget>
#include <vector>

#include "exercise_data.h"

class TranslationWidget : public QWidget {
  Q_OBJECT

 public:
  explicit TranslationWidget(QWidget* parent = nullptr);

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

  QLabel* title_label_;
  QLabel* sentence_label_;
  QLineEdit* answer_input_;
  QPushButton* submit_button_;
  QProgressBar* progress_bar_;
  QLabel* timer_label_;
  QLabel* feedback_label_;
  QLabel* wrong_count_label_;

  QTimer* timer_;
  int remaining_seconds_ = 0;

  std::vector<TranslationTask> tasks_;
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
