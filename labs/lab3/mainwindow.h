#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QLabel>
#include <QMainWindow>
#include <QStackedWidget>

#include "exercise_data.h"
#include "grammar_widget.h"
#include "translation_widget.h"

class MainWindow : public QMainWindow {
  Q_OBJECT

 public:
  explicit MainWindow(QWidget* parent = nullptr);

 protected:
  void keyPressEvent(QKeyEvent* event) override;

 private slots:
  void OnTranslationClicked();
  void OnGrammarClicked();
  void OnDifficultyAction();
  void OnExerciseFinished(int points);

 private:
  void SetupUi();
  void SetupMenuBar();
  void UpdateScoreDisplay();

  QStackedWidget* stack_;
  QWidget* welcome_page_;
  TranslationWidget* translation_widget_;
  GrammarWidget* grammar_widget_;

  QLabel* score_label_;
  QLabel* difficulty_label_;

  int total_score_ = 0;
  Difficulty difficulty_ = Difficulty::Easy;

  // Track which page is shown: 0=welcome, 1=translation, 2=grammar
  enum Page { Welcome = 0, Translation = 1, Grammar = 2 };
};

#endif
