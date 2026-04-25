#include "mainwindow.h"

#include <QAction>
#include <QHBoxLayout>
#include <QKeyEvent>
#include <QLabel>
#include <QMenuBar>
#include <QMessageBox>
#include <QPushButton>
#include <QShortcut>
#include <QVBoxLayout>

#include "difficulty_dialog.h"

MainWindow::MainWindow(QWidget* parent) : QMainWindow(parent) {
  SetupUi();
  setWindowTitle("LinguaLearn — Language Learning App");
}

void MainWindow::SetupUi() {
  QWidget* central = new QWidget(this);
  setCentralWidget(central);

  QHBoxLayout* main_layout = new QHBoxLayout(central);
  main_layout->setContentsMargins(0, 0, 0, 0);
  main_layout->setSpacing(0);

  // ── Left sidebar ──
  QWidget* sidebar = new QWidget();
  sidebar->setFixedWidth(220);
  sidebar->setStyleSheet(
      "background: #181825; border-right: 1px solid #313244;");

  QVBoxLayout* sidebar_layout = new QVBoxLayout(sidebar);
  sidebar_layout->setContentsMargins(16, 20, 16, 20);
  sidebar_layout->setSpacing(12);

  // App title
  QLabel* app_title = new QLabel("🌍 LinguaLearn");
  app_title->setStyleSheet(
      "font-size: 18px; font-weight: bold; color: #89b4fa; "
      "padding-bottom: 8px; border: none;");
  sidebar_layout->addWidget(app_title);

  // Score display
  QWidget* score_card = new QWidget();
  score_card->setStyleSheet(
      "QWidget { background: qlineargradient(x1:0, y1:0, x2:1, y2:1, stop:0 "
      "#f9e2af, stop:1 #fab387); border-radius: 12px; }");
  QVBoxLayout* score_layout = new QVBoxLayout(score_card);
  score_layout->setContentsMargins(16, 14, 16, 14);

  QLabel* score_title = new QLabel("⭐ TOTAL SCORE");
  score_title->setStyleSheet(
      "font-size: 11px; font-weight: bold; color: #7c6020; border: none; "
      "background: transparent; letter-spacing: 1px;");
  score_layout->addWidget(score_title);

  score_label_ = new QLabel("0");
  score_label_->setStyleSheet(
      "font-size: 32px; font-weight: bold; color: #11111b; border: none; "
      "background: transparent;");
  score_layout->addWidget(score_label_);

  sidebar_layout->addWidget(score_card);

  // Difficulty display
  QWidget* diff_card = new QWidget();
  diff_card->setStyleSheet(
      "QWidget { background: qlineargradient(x1:0, y1:0, x2:1, y2:1, stop:0 "
      "#89b4fa, stop:1 #cba6f7); border-radius: 12px; }");
  QVBoxLayout* diff_layout = new QVBoxLayout(diff_card);
  diff_layout->setContentsMargins(16, 14, 16, 14);

  QLabel* diff_title = new QLabel("📊 DIFFICULTY");
  diff_title->setStyleSheet(
      "font-size: 11px; font-weight: bold; color: #2e305c; border: none; "
      "background: transparent; letter-spacing: 1px;");
  diff_layout->addWidget(diff_title);

  difficulty_label_ = new QLabel("Easy");
  difficulty_label_->setStyleSheet(
      "font-size: 20px; font-weight: bold; color: #11111b; border: none; "
      "background: transparent;");
  diff_layout->addWidget(difficulty_label_);
  sidebar_layout->addWidget(diff_card);

  // Separator
  QFrame* separator = new QFrame();
  separator->setFrameShape(QFrame::HLine);
  separator->setStyleSheet("color: #313244; border: none; max-height: 1px; "
                           "background: #313244;");
  sidebar_layout->addWidget(separator);

  // Exercise label
  QLabel* exercises_label = new QLabel("EXERCISES");
  exercises_label->setStyleSheet(
      "font-size: 11px; font-weight: bold; color: #6c7086; "
      "letter-spacing: 2px; padding-top: 4px; border: none;");
  sidebar_layout->addWidget(exercises_label);

  // Translation button
  QPushButton* translation_btn = new QPushButton("📝 Translation");
  translation_btn->setFixedHeight(44);
  translation_btn->setCursor(Qt::PointingHandCursor);
  translation_btn->setStyleSheet(
      "QPushButton { background: #313244; color: #cdd6f4; border: none; "
      "border-radius: 10px; font-size: 14px; font-weight: bold; "
      "text-align: left; padding-left: 14px; }"
      "QPushButton:hover { background: #45475a; }"
      "QPushButton:pressed { background: #585b70; }");
  sidebar_layout->addWidget(translation_btn);

  // Grammar button
  QPushButton* grammar_btn = new QPushButton("📖 Grammar");
  grammar_btn->setFixedHeight(44);
  grammar_btn->setCursor(Qt::PointingHandCursor);
  grammar_btn->setStyleSheet(
      "QPushButton { background: #313244; color: #cdd6f4; border: none; "
      "border-radius: 10px; font-size: 14px; font-weight: bold; "
      "text-align: left; padding-left: 14px; }"
      "QPushButton:hover { background: #45475a; }"
      "QPushButton:pressed { background: #585b70; }");
  sidebar_layout->addWidget(grammar_btn);

  // Difficulty button
  QPushButton* diff_btn = new QPushButton("⚙️ Difficulty");
  diff_btn->setFixedHeight(44);
  diff_btn->setCursor(Qt::PointingHandCursor);
  diff_btn->setStyleSheet(
      "QPushButton { background: #313244; color: #cdd6f4; border: none; "
      "border-radius: 10px; font-size: 14px; font-weight: bold; "
      "text-align: left; padding-left: 14px; }"
      "QPushButton:hover { background: #45475a; }"
      "QPushButton:pressed { background: #585b70; }");
  sidebar_layout->addWidget(diff_btn);

  sidebar_layout->addStretch();

  // Help hint
  QLabel* help_hint = new QLabel("Press H for hint");
  help_hint->setStyleSheet(
      "font-size: 11px; color: #585b70; font-style: italic; border: none;");
  help_hint->setAlignment(Qt::AlignCenter);
  sidebar_layout->addWidget(help_hint);

  main_layout->addWidget(sidebar);

  // ── Right content area (stacked widget) ──
  stack_ = new QStackedWidget();
  stack_->setStyleSheet("background: #1e1e2e;");

  // Welcome page
  welcome_page_ = new QWidget();
  QVBoxLayout* welcome_layout = new QVBoxLayout(welcome_page_);
  welcome_layout->setAlignment(Qt::AlignCenter);

  QLabel* welcome_emoji = new QLabel("🌍");
  welcome_emoji->setStyleSheet("font-size: 64px; border: none;");
  welcome_emoji->setAlignment(Qt::AlignCenter);
  welcome_layout->addWidget(welcome_emoji);

  QLabel* welcome_title = new QLabel("Welcome to LinguaLearn!");
  welcome_title->setStyleSheet(
      "font-size: 28px; font-weight: bold; color: #cdd6f4; border: none;");
  welcome_title->setAlignment(Qt::AlignCenter);
  welcome_layout->addWidget(welcome_title);

  QLabel* welcome_sub = new QLabel(
      "Learn Russian through interactive exercises.\n"
      "Choose Translation or Grammar from the sidebar to begin!");
  welcome_sub->setStyleSheet(
      "font-size: 15px; color: #a6adc8; border: none;");
  welcome_sub->setAlignment(Qt::AlignCenter);
  welcome_sub->setWordWrap(true);
  welcome_layout->addWidget(welcome_sub);

  QLabel* welcome_hint = new QLabel(
      "\n💡 Tip: Change difficulty via the Difficulty button in the sidebar.\n"
      "Press H during an exercise for a hint.");
  welcome_hint->setStyleSheet(
      "font-size: 13px; color: #585b70; border: none;");
  welcome_hint->setAlignment(Qt::AlignCenter);
  welcome_hint->setWordWrap(true);
  welcome_layout->addWidget(welcome_hint);

  stack_->addWidget(welcome_page_);       // index 0
  translation_widget_ = new TranslationWidget();
  stack_->addWidget(translation_widget_); // index 1
  grammar_widget_ = new GrammarWidget();
  stack_->addWidget(grammar_widget_);     // index 2

  main_layout->addWidget(stack_, 1);

  // ── Connections ──
  connect(translation_btn, &QPushButton::clicked, this,
          &MainWindow::OnTranslationClicked);
  connect(grammar_btn, &QPushButton::clicked, this,
          &MainWindow::OnGrammarClicked);
  connect(diff_btn, &QPushButton::clicked, this,
          &MainWindow::OnDifficultyAction);
  connect(translation_widget_, &TranslationWidget::ExerciseFinished, this,
          &MainWindow::OnExerciseFinished);
  connect(grammar_widget_, &GrammarWidget::ExerciseFinished, this,
          &MainWindow::OnExerciseFinished);

  // Global stylesheet
  central->setStyleSheet("background: #1e1e2e;");

  // Hint shortcut binding (привязка к key code)
  QShortcut* hint_shortcut = new QShortcut(QKeySequence(Qt::Key_H), this);
  connect(hint_shortcut, &QShortcut::activated, this, &MainWindow::ShowHint);
}



void MainWindow::UpdateScoreDisplay() {
  score_label_->setText(QString::number(total_score_));
}

void MainWindow::OnTranslationClicked() {
  grammar_widget_->StopExercise();
  stack_->setCurrentIndex(Page::Translation);
  translation_widget_->StartExercise(difficulty_);
}

void MainWindow::OnGrammarClicked() {
  translation_widget_->StopExercise();
  stack_->setCurrentIndex(Page::Grammar);
  grammar_widget_->StartExercise(difficulty_);
}

void MainWindow::OnDifficultyAction() {
  DifficultyDialog dialog(difficulty_, this);
  if (dialog.exec() == QDialog::Accepted) {
    difficulty_ = dialog.GetSelectedDifficulty();
    switch (difficulty_) {
      case Difficulty::Easy:
        difficulty_label_->setText("Easy");
        break;
      case Difficulty::Medium:
        difficulty_label_->setText("Medium");
        break;
      case Difficulty::Hard:
        difficulty_label_->setText("Hard");
        break;
    }
  }
}

void MainWindow::OnExerciseFinished(int points) {
  total_score_ += points;
  UpdateScoreDisplay();
}

void MainWindow::ShowHint() {
  QString hint;
  int current_page = stack_->currentIndex();

  if (current_page == Page::Translation) {
    hint = translation_widget_->GetCurrentHint();
  } else if (current_page == Page::Grammar) {
    hint = grammar_widget_->GetCurrentHint();
  } else {
    hint = "Start an exercise first to get hints!";
  }

  QMessageBox hint_box(this);
  hint_box.setWindowTitle("💡 Hint");
  hint_box.setText(hint);
  hint_box.setIcon(QMessageBox::Information);
  hint_box.setStyleSheet(
      "QMessageBox { background: #1e1e2e; }"
      "QMessageBox QLabel { color: #cdd6f4; font-size: 14px; }"
      "QPushButton { background: #89b4fa; color: #1e1e2e; border: none; "
      "border-radius: 6px; padding: 6px 18px; font-weight: bold; }"
      "QPushButton:hover { background: #74c7ec; }");
  hint_box.exec();
}
