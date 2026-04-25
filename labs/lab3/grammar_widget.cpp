#include "grammar_widget.h"

#include <QMessageBox>
#include <QSpacerItem>

GrammarWidget::GrammarWidget(QWidget* parent) : QWidget(parent) {
  SetupUi();
  timer_ = new QTimer(this);
  connect(timer_, &QTimer::timeout, this, &GrammarWidget::OnTimerTick);

  correct_sound_.setSource(QUrl::fromLocalFile("labs/lab3/sounds/correct.wav"));
  wrong_sound_.setSource(QUrl::fromLocalFile("labs/lab3/sounds/wrong.wav"));
  complete_sound_.setSource(QUrl::fromLocalFile("labs/lab3/sounds/complete.wav"));
  correct_sound_.setVolume(0.5f);
  wrong_sound_.setVolume(0.5f);
  complete_sound_.setVolume(0.6f);
}

void GrammarWidget::SetupUi() {
  QVBoxLayout* layout = new QVBoxLayout(this);
  layout->setContentsMargins(24, 20, 24, 20);
  layout->setSpacing(14);

  title_label_ = new QLabel("📖 Grammar Exercise");
  title_label_->setStyleSheet(
      "font-size: 20px; font-weight: bold; color: #f9e2af;");
  layout->addWidget(title_label_);

  // Timer and progress row
  QHBoxLayout* status_row = new QHBoxLayout();
  timer_label_ = new QLabel("⏱ --:--");
  timer_label_->setStyleSheet(
      "font-size: 15px; color: #f9e2af; font-weight: bold;");
  status_row->addWidget(timer_label_);

  wrong_count_label_ = new QLabel("❌ 0 / 3");
  wrong_count_label_->setStyleSheet("font-size: 14px; color: #f38ba8;");
  status_row->addStretch();
  status_row->addWidget(wrong_count_label_);
  layout->addLayout(status_row);

  progress_bar_ = new QProgressBar();
  progress_bar_->setMinimum(0);
  progress_bar_->setMaximum(100);
  progress_bar_->setValue(0);
  progress_bar_->setTextVisible(true);
  progress_bar_->setFormat("%v%");
  progress_bar_->setFixedHeight(22);
  progress_bar_->setStyleSheet(
      "QProgressBar { background: #313244; border: none; border-radius: 11px; "
      "color: #cdd6f4; font-size: 11px; text-align: center; }"
      "QProgressBar::chunk { background: qlineargradient(x1:0, y1:0, x2:1, "
      "y2:0, stop:0 #f9e2af, stop:1 #fab387); border-radius: 11px; }");
  layout->addWidget(progress_bar_);

  // Instruction
  QLabel* instruction = new QLabel("Fill in the blank with the correct word:");
  instruction->setStyleSheet("font-size: 13px; color: #a6adc8;");
  layout->addWidget(instruction);

  sentence_label_ = new QLabel();
  sentence_label_->setWordWrap(true);
  sentence_label_->setStyleSheet(
      "font-size: 22px; font-weight: bold; color: #cdd6f4; "
      "background: #313244; border-radius: 10px; padding: 18px; "
      "min-height: 50px;");
  layout->addWidget(sentence_label_);

  // Options area
  QWidget* options_container = new QWidget();
  options_layout_ = new QVBoxLayout(options_container);
  options_layout_->setSpacing(8);
  options_layout_->setContentsMargins(8, 4, 8, 4);
  layout->addWidget(options_container);

  button_group_ = new QButtonGroup(this);

  feedback_label_ = new QLabel();
  feedback_label_->setWordWrap(true);
  feedback_label_->setStyleSheet("font-size: 14px; min-height: 24px;");
  feedback_label_->hide();
  layout->addWidget(feedback_label_);

  submit_button_ = new QPushButton("Submit");
  submit_button_->setFixedHeight(44);
  submit_button_->setStyleSheet(
      "QPushButton { background: #f9e2af; color: #1e1e2e; border: none; "
      "border-radius: 10px; font-size: 15px; font-weight: bold; }"
      "QPushButton:hover { background: #fab387; }"
      "QPushButton:pressed { background: #eba0ac; }"
      "QPushButton:disabled { background: #45475a; color: #6c7086; }");
  layout->addWidget(submit_button_);

  layout->addStretch();

  connect(submit_button_, &QPushButton::clicked, this,
          &GrammarWidget::OnSubmit);
}

void GrammarWidget::ClearOptions() {
  for (auto* btn : option_buttons_) {
    options_layout_->removeWidget(btn);
    button_group_->removeButton(btn);
    delete btn;
  }
  option_buttons_.clear();
}

void GrammarWidget::StartExercise(Difficulty difficulty) {
  difficulty_ = difficulty;
  auto params = GetDifficultyParams(difficulty);
  tasks_ = SampleGrammarTasks(difficulty);
  current_task_ = 0;
  wrong_attempts_ = 0;
  max_wrong_ = params.max_wrong;
  points_ = params.points_on_success;
  remaining_seconds_ = params.timer_seconds;
  active_ = true;

  progress_bar_->setMaximum(100);
  progress_bar_->setValue(0);
  feedback_label_->hide();
  feedback_label_->clear();
  wrong_count_label_->setText(
      QString("❌ 0 / %1").arg(max_wrong_));
  submit_button_->setEnabled(true);

  ShowTask();
  timer_->start(1000);
}

void GrammarWidget::StopExercise() {
  if (active_) {
    active_ = false;
    timer_->stop();
  }
}

QString GrammarWidget::GetCurrentHint() const {
  if (!active_ || current_task_ >= static_cast<int>(tasks_.size())) {
    return "Start a Grammar exercise first!";
  }
  return tasks_[current_task_].hint;
}

void GrammarWidget::ShowTask() {
  if (current_task_ >= static_cast<int>(tasks_.size())) {
    FinishExercise(true);
    return;
  }

  const auto& task = tasks_[current_task_];
  sentence_label_->setText(task.sentence);

  ClearOptions();

  for (int i = 0; i < task.options.size(); ++i) {
    QRadioButton* radio = new QRadioButton(task.options[i]);
    radio->setStyleSheet(
        "QRadioButton { color: #cdd6f4; font-size: 15px; "
        "padding: 8px 12px; background: #313244; border-radius: 8px; }"
        "QRadioButton:hover { background: #45475a; }"
        "QRadioButton::indicator { width: 18px; height: 18px; }");
    options_layout_->addWidget(radio);
    button_group_->addButton(radio, i);
    option_buttons_.append(radio);
  }

  feedback_label_->hide();
  feedback_label_->clear();

  int pct = static_cast<int>(100.0 * current_task_ / tasks_.size());
  progress_bar_->setValue(pct);
}

void GrammarWidget::OnSubmit() {
  if (!active_ || current_task_ >= static_cast<int>(tasks_.size())) {
    return;
  }

  const QString kSuccessStyle = "background-color: rgba(166, 227, 161, 0.15); color: #a6e3a1; border: 1px solid rgba(166, 227, 161, 0.5); border-radius: 6px; padding: 8px 12px; font-size: 14px; font-weight: bold;";
  const QString kErrorStyle = "background-color: rgba(243, 139, 168, 0.15); color: #f38ba8; border: 1px solid rgba(243, 139, 168, 0.5); border-radius: 6px; padding: 8px 12px; font-size: 14px; font-weight: bold;";
  const QString kWarningStyle = "background-color: rgba(249, 226, 175, 0.15); color: #f9e2af; border: 1px solid rgba(249, 226, 175, 0.5); border-radius: 6px; padding: 8px 12px; font-size: 14px; font-weight: bold;";

  int selected = button_group_->checkedId();
  if (selected < 0) {
    feedback_label_->setStyleSheet(kWarningStyle);
    feedback_label_->setText("⚠️ Please select an answer first.");
    feedback_label_->show();
    return;
  }

  const auto& task = tasks_[current_task_];

  if (selected == task.correct_index) {
    correct_sound_.play();
    feedback_label_->setStyleSheet(kSuccessStyle);
    feedback_label_->setText("✅ Correct!");
    feedback_label_->show();
    current_task_++;
    // Disable options during delay
    for (auto* btn : option_buttons_) {
      btn->setEnabled(false);
    }
    QTimer::singleShot(600, this, [this]() {
      if (active_) {
        ShowTask();
      }
    });
  } else {
    wrong_sound_.play();
    wrong_attempts_++;
    wrong_count_label_->setText(
        QString("❌ %1 / %2").arg(wrong_attempts_).arg(max_wrong_));

    feedback_label_->setStyleSheet(kErrorStyle);
    if (wrong_attempts_ >= max_wrong_) {
      feedback_label_->setText(
          QString("❌ Wrong! The correct answer is: \"%1\".")
              .arg(task.options[task.correct_index]));
    } else {
      feedback_label_->setText(
          QString("❌ Wrong! The correct answer is: \"%1\". Moving to next task.")
              .arg(task.options[task.correct_index]));
    }
    feedback_label_->show();
    
    // Move to next task after wrong answer on grammar
    current_task_++;
    for (auto* btn : option_buttons_) {
      btn->setEnabled(false);
    }

    if (wrong_attempts_ >= max_wrong_) {
      QTimer::singleShot(1500, this, [this]() {
        FinishExercise(false);
      });
      return;
    }

    QTimer::singleShot(1500, this, [this]() {
      if (active_) {
        ShowTask();
      }
    });
  }
}

void GrammarWidget::OnTimerTick() {
  if (!active_) {
    return;
  }
  remaining_seconds_--;
  int mins = remaining_seconds_ / 60;
  int secs = remaining_seconds_ % 60;
  timer_label_->setText(
      QString("⏱ %1:%2")
          .arg(mins, 2, 10, QChar('0'))
          .arg(secs, 2, 10, QChar('0')));

  if (remaining_seconds_ <= 0) {
    timer_->stop();
    active_ = false;
    submit_button_->setEnabled(false);
    for (auto* btn : option_buttons_) {
      btn->setEnabled(false);
    }

    QMessageBox::warning(this, "Time's Up!",
                         "The time allocated for this exercise has expired.\n"
                         "You did not earn any points.");
    emit ExerciseFinished(0);
  }
}

void GrammarWidget::FinishExercise(bool success) {
  timer_->stop();
  active_ = false;
  submit_button_->setEnabled(false);
  for (auto* btn : option_buttons_) {
    btn->setEnabled(false);
  }

  if (success) {
    complete_sound_.play();
    progress_bar_->setValue(100);
    QMessageBox::information(
        this, "Exercise Complete! 🎉",
        QString("Congratulations! You completed all tasks!\n"
                "You earned %1 points!")
            .arg(points_));
    emit ExerciseFinished(points_);
  } else {
    QMessageBox::warning(
        this, "Exercise Failed",
        QString("Too many wrong answers (%1/%2).\n"
                "The exercise is over. No points earned.")
            .arg(wrong_attempts_)
            .arg(max_wrong_));
    emit ExerciseFinished(0);
  }
}
