#include "mainwindow.h"

#include <QAction>
#include <QHBoxLayout>
#include <QKeyEvent>
#include <QLabel>
#include <QMenuBar>
#include <QMessageBox>
#include <QPushButton>
#include <QVBoxLayout>

#include "difficulty_dialog.h"

MainWindow::MainWindow(QWidget* parent) : QMainWindow(parent) {
  SetupUi();
  SetupMenuBar();
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
      "background: #ffffff; border-right: 1px solid #e0e0e0;");

  QVBoxLayout* sidebar_layout = new QVBoxLayout(sidebar);
  sidebar_layout->setContentsMargins(16, 20, 16, 20);
  sidebar_layout->setSpacing(12);

  // App title
  QLabel* app_title = new QLabel("🌍 LinguaLearn");
  app_title->setStyleSheet(
      "font-size: 20px; font-weight: bold; color: #58cc02; "
      "padding-bottom: 8px; border: none;");
  sidebar_layout->addWidget(app_title);

  // Score display
  QWidget* score_card = new QWidget();
  score_card->setStyleSheet(
      "background: #f7f7f7; border-radius: 12px; border: 2px solid #e5e5e5;");
  QVBoxLayout* score_layout = new QVBoxLayout(score_card);
  score_layout->setContentsMargins(12, 10, 12, 10);

  QLabel* score_title = new QLabel("⭐ Total Score");
  score_title->setStyleSheet(
      "font-size: 13px; font-weight: bold; color: #afafaf; border: none;");
  score_layout->addWidget(score_title);

  score_label_ = new QLabel("0");
  score_label_->setStyleSheet(
      "font-size: 28px; font-weight: 900; color: #ffc800; border: none;");
  score_layout->addWidget(score_label_);

  sidebar_layout->addWidget(score_card);

  // Difficulty display
  QWidget* diff_card = new QWidget();
  diff_card->setStyleSheet(
      "background: #f7f7f7; border-radius: 12px; border: 2px solid #e5e5e5;");
  QVBoxLayout* diff_layout = new QVBoxLayout(diff_card);
  diff_layout->setContentsMargins(12, 10, 12, 10);

  QLabel* diff_title = new QLabel("📊 Difficulty");
  diff_title->setStyleSheet(
      "font-size: 13px; font-weight: bold; color: #afafaf; border: none;");
  diff_layout->addWidget(diff_title);

  difficulty_label_ = new QLabel("🟢 Easy");
  difficulty_label_->setStyleSheet(
      "font-size: 16px; font-weight: 900; color: #58cc02; border: none;");
  diff_layout->addWidget(difficulty_label_);
  sidebar_layout->addWidget(diff_card);

  // Separator
  QFrame* separator = new QFrame();
  separator->setFrameShape(QFrame::HLine);
  separator->setStyleSheet("color: #e5e5e5; border: none; max-height: 2px; "
                           "background: #e5e5e5;");
  sidebar_layout->addWidget(separator);

  // Exercise label
  QLabel* exercises_label = new QLabel("EXERCISES");
  exercises_label->setStyleSheet(
      "font-size: 12px; font-weight: 900; color: #afafaf; "
      "letter-spacing: 1px; padding-top: 4px; border: none;");
  sidebar_layout->addWidget(exercises_label);

  // Translation button
  QPushButton* translation_btn = new QPushButton("📝 Translation");
  translation_btn->setFixedHeight(48);
  translation_btn->setCursor(Qt::PointingHandCursor);
  translation_btn->setStyleSheet(
      "QPushButton { background: #ffffff; color: #4b4b4b; border: 2px solid #e5e5e5; "
      "border-bottom: 4px solid #e5e5e5; border-radius: 12px; font-size: 15px; font-weight: bold; "
      "text-align: left; padding-left: 14px; }"
      "QPushButton:hover { background: #f7f7f7; }"
      "QPushButton:pressed { background: #e5e5e5; border-bottom: 2px solid #e5e5e5; margin-top: 2px; }");
  sidebar_layout->addWidget(translation_btn);

  // Grammar button
  QPushButton* grammar_btn = new QPushButton("📖 Grammar");
  grammar_btn->setFixedHeight(48);
  grammar_btn->setCursor(Qt::PointingHandCursor);
  grammar_btn->setStyleSheet(
      "QPushButton { background: #ffffff; color: #4b4b4b; border: 2px solid #e5e5e5; "
      "border-bottom: 4px solid #e5e5e5; border-radius: 12px; font-size: 15px; font-weight: bold; "
      "text-align: left; padding-left: 14px; }"
      "QPushButton:hover { background: #f7f7f7; }"
      "QPushButton:pressed { background: #e5e5e5; border-bottom: 2px solid #e5e5e5; margin-top: 2px; }");
  sidebar_layout->addWidget(grammar_btn);

  sidebar_layout->addStretch();

  // Help hint
  QLabel* help_hint = new QLabel("Press H for hint");
  help_hint->setStyleSheet(
      "font-size: 12px; font-weight: bold; color: #afafaf; border: none;");
  help_hint->setAlignment(Qt::AlignCenter);
  sidebar_layout->addWidget(help_hint);

  main_layout->addWidget(sidebar);

  // ── Right content area (stacked widget) ──
  stack_ = new QStackedWidget();
  stack_->setStyleSheet("background: #fafafa; border: none;");

  // Welcome page
  welcome_page_ = new QWidget();
  QVBoxLayout* welcome_layout = new QVBoxLayout(welcome_page_);
  welcome_layout->setAlignment(Qt::AlignCenter);

  QLabel* welcome_emoji = new QLabel("🌍");
  welcome_emoji->setStyleSheet("font-size: 80px; border: none;");
  welcome_emoji->setAlignment(Qt::AlignCenter);
  welcome_layout->addWidget(welcome_emoji);

  QLabel* welcome_title = new QLabel("Welcome to LinguaLearn!");
  welcome_title->setStyleSheet(
      "font-size: 32px; font-weight: 900; color: #4b4b4b; border: none;");
  welcome_title->setAlignment(Qt::AlignCenter);
  welcome_layout->addWidget(welcome_title);

  QLabel* welcome_sub = new QLabel(
      "Learn Russian through interactive exercises.\n"
      "Choose Translation or Grammar from the sidebar to begin!");
  welcome_sub->setStyleSheet(
      "font-size: 16px; font-weight: bold; color: #afafaf; border: none;");
  welcome_sub->setAlignment(Qt::AlignCenter);
  welcome_sub->setWordWrap(true);
  welcome_layout->addWidget(welcome_sub);

  QLabel* welcome_hint = new QLabel(
      "\n💡 Tip: Change difficulty via Settings → Difficulty in the menu bar.\n"
      "Press H during an exercise for a hint.");
  welcome_hint->setStyleSheet(
      "font-size: 14px; font-weight: bold; color: #afafaf; border: none;");
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
  connect(translation_widget_, &TranslationWidget::ExerciseFinished, this,
          &MainWindow::OnExerciseFinished);
  connect(grammar_widget_, &GrammarWidget::ExerciseFinished, this,
          &MainWindow::OnExerciseFinished);

  // Global stylesheet
  central->setStyleSheet("background: #fafafa; color: #333333;");
}

void MainWindow::SetupMenuBar() {
  QMenuBar* menu_bar = menuBar();
  menu_bar->setStyleSheet(
      "QMenuBar { background: #ffffff; color: #333333; font-size: 13px; "
      "border-bottom: 1px solid #e0e0e0; }"
      "QMenuBar::item { padding: 6px 14px; }"
      "QMenuBar::item:selected { background: #e0e0e0; }"
      "QMenu { background: #ffffff; color: #333333; border: 1px solid "
      "#e0e0e0; }"
      "QMenu::item { padding: 6px 24px; }"
      "QMenu::item:selected { background: #e0e0e0; }");

  // File menu
  QMenu* file_menu = menu_bar->addMenu("&File");
  QAction* exit_action = file_menu->addAction("Exit");
  exit_action->setShortcut(QKeySequence("Ctrl+Q"));
  connect(exit_action, &QAction::triggered, this, &QWidget::close);

  // Settings menu
  QMenu* settings_menu = menu_bar->addMenu("&Settings");
  QAction* difficulty_action = settings_menu->addAction("Difficulty...");
  connect(difficulty_action, &QAction::triggered, this,
          &MainWindow::OnDifficultyAction);
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
        difficulty_label_->setText("🟢 Easy");
        difficulty_label_->setStyleSheet(
            "font-size: 16px; font-weight: bold; color: #a6e3a1; "
            "border: none;");
        break;
      case Difficulty::Medium:
        difficulty_label_->setText("🟡 Medium");
        difficulty_label_->setStyleSheet(
            "font-size: 16px; font-weight: bold; color: #f9e2af; "
            "border: none;");
        break;
      case Difficulty::Hard:
        difficulty_label_->setText("🔴 Hard");
        difficulty_label_->setStyleSheet(
            "font-size: 16px; font-weight: bold; color: #f38ba8; "
            "border: none;");
        break;
    }
  }
}

void MainWindow::OnExerciseFinished(int points) {
  total_score_ += points;
  UpdateScoreDisplay();
}

void MainWindow::keyPressEvent(QKeyEvent* event) {
  if (event->key() == Qt::Key_H) {
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
    return;
  }
  QMainWindow::keyPressEvent(event);
}
