#include "translation_widget.h"

#include <QMessageBox>
#include <QVBoxLayout>

TranslationWidget::TranslationWidget(QWidget* parent) : QWidget(parent) {
  SetupUi();
  timer_ = new QTimer(this);
  connect(timer_, &QTimer::timeout, this, &TranslationWidget::OnTimerTick);
}

void TranslationWidget::SetupUi() {
  QVBoxLayout* layout = new QVBoxLayout(this);
  layout->setContentsMargins(24, 20, 24, 20);
  layout->setSpacing(14);

  title_label_ = new QLabel("📝 Translation Exercise");
  title_label_->setStyleSheet(
      "font-size: 24px; font-weight: 900; color: #4b4b4b;");
  layout->addWidget(title_label_);

  // Timer and progress row
  QHBoxLayout* status_row = new QHBoxLayout();
  timer_label_ = new QLabel("⏱ --:--");
  timer_label_->setStyleSheet(
      "font-size: 16px; color: #ffc800; font-weight: 900;");
  status_row->addWidget(timer_label_);

  wrong_count_label_ = new QLabel("❌ 0 / 3");
  wrong_count_label_->setStyleSheet("font-size: 16px; font-weight: bold; color: #ff4b4b;");
  status_row->addStretch();
  status_row->addWidget(wrong_count_label_);
  layout->addLayout(status_row);

  progress_bar_ = new QProgressBar();
  progress_bar_->setMinimum(0);
  progress_bar_->setMaximum(100);
  progress_bar_->setValue(0);
  progress_bar_->setTextVisible(true);
  progress_bar_->setFormat("%v%");
  progress_bar_->setMinimumHeight(24);
  progress_bar_->setMaximumHeight(24);
  progress_bar_->setStyleSheet(
      "QProgressBar { background: #e5e5e5; border: none; border-radius: 12px; "
      "color: #4b4b4b; font-size: 12px; font-weight: bold; text-align: center; }"
      "QProgressBar::chunk { background: #58cc02; border-radius: 12px; }");
  layout->addWidget(progress_bar_);

  // Instruction
  QLabel* instruction = new QLabel("Translate the following into Russian:");
  instruction->setStyleSheet("font-size: 16px; font-weight: bold; color: #afafaf; margin-top: 10px;");
  layout->addWidget(instruction);

  sentence_label_ = new QLabel();
  sentence_label_->setWordWrap(true);
  sentence_label_->setStyleSheet(
      "font-size: 24px; font-weight: bold; color: #4b4b4b; "
      "background: #ffffff; border: 2px solid #e5e5e5; border-radius: 12px; padding: 20px; "
      "min-height: 60px;");
  layout->addWidget(sentence_label_);

  answer_input_ = new QLineEdit();
  answer_input_->setPlaceholderText("Type your translation here...");
  answer_input_->setStyleSheet(
      "QLineEdit { background: #ffffff; color: #4b4b4b; border: 2px solid "
      "#e5e5e5; border-radius: 12px; padding: 14px 16px; font-size: 18px; font-weight: bold; }"
      "QLineEdit:focus { border-color: #1cb0f6; background: #ddf4ff; }");
  layout->addWidget(answer_input_);

  feedback_label_ = new QLabel();
  feedback_label_->setWordWrap(true);
  feedback_label_->setStyleSheet("font-size: 16px; font-weight: bold; min-height: 24px;");
  feedback_label_->hide();
  layout->addWidget(feedback_label_);

  submit_button_ = new QPushButton("CHECK");
  submit_button_->setFixedHeight(52);
  submit_button_->setCursor(Qt::PointingHandCursor);
  submit_button_->setStyleSheet(
      "QPushButton { background: #58cc02; color: white; border: none; "
      "border-bottom: 4px solid #58a700; border-radius: 12px; font-size: 18px; font-weight: 900; letter-spacing: 1px; }"
      "QPushButton:hover { background: #61e002; }"
      "QPushButton:pressed { background: #58cc02; border-bottom: 2px solid #58a700; margin-top: 2px; }"
      "QPushButton:disabled { background: #e5e5e5; color: #afafaf; border-bottom: 4px solid #cecece; }");
  layout->addWidget(submit_button_);

  layout->addStretch();

  connect(submit_button_, &QPushButton::clicked, this,
          &TranslationWidget::OnSubmit);
  connect(answer_input_, &QLineEdit::returnPressed, this,
          &TranslationWidget::OnSubmit);
}

void TranslationWidget::StartExercise(Difficulty difficulty) {
  difficulty_ = difficulty;
  auto params = GetDifficultyParams(difficulty);
  tasks_ = SampleTranslationTasks(difficulty);
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
  answer_input_->setEnabled(true);

  ShowTask();
  timer_->start(1000);
}

void TranslationWidget::StopExercise() {
  if (active_) {
    active_ = false;
    timer_->stop();
  }
}

QString TranslationWidget::GetCurrentHint() const {
  if (!active_ || current_task_ >= static_cast<int>(tasks_.size())) {
    return "Start a Translation exercise first!";
  }
  return tasks_[current_task_].hint;
}

void TranslationWidget::ShowTask() {
  if (current_task_ >= static_cast<int>(tasks_.size())) {
    FinishExercise(true);
    return;
  }
  sentence_label_->setText(
      QString("\"%1\"").arg(tasks_[current_task_].english));
  answer_input_->clear();
  answer_input_->setFocus();
  feedback_label_->hide();
  feedback_label_->clear();

  int pct = static_cast<int>(100.0 * current_task_ / tasks_.size());
  progress_bar_->setValue(pct);
}

void TranslationWidget::OnSubmit() {
  if (!active_ || current_task_ >= static_cast<int>(tasks_.size())) {
    return;
  }

  const QString kSuccessStyle = "background-color: rgba(166, 227, 161, 0.15); color: #a6e3a1; border: 1px solid rgba(166, 227, 161, 0.5); border-radius: 6px; padding: 8px 12px; font-size: 14px; font-weight: bold;";
  const QString kErrorStyle = "background-color: rgba(243, 139, 168, 0.15); color: #f38ba8; border: 1px solid rgba(243, 139, 168, 0.5); border-radius: 6px; padding: 8px 12px; font-size: 14px; font-weight: bold;";
  const QString kWarningStyle = "background-color: rgba(249, 226, 175, 0.15); color: #f9e2af; border: 1px solid rgba(249, 226, 175, 0.5); border-radius: 6px; padding: 8px 12px; font-size: 14px; font-weight: bold;";

  QString user_answer = answer_input_->text();
  if (user_answer.trimmed().isEmpty()) {
    feedback_label_->setStyleSheet(kWarningStyle);
    feedback_label_->setText("⚠️ Please enter a translation before submitting.");
    feedback_label_->show();
    return;
  }

  const auto& task = tasks_[current_task_];

  if (CheckTranslation(user_answer, task.russian)) {
    feedback_label_->setStyleSheet(kSuccessStyle);
    feedback_label_->setText("✅ Correct!");
    feedback_label_->show();
    current_task_++;
    answer_input_->setEnabled(false);
    submit_button_->setEnabled(false);
    // Short delay before showing next task
    QTimer::singleShot(600, this, [this]() {
      if (active_) {
        submit_button_->setEnabled(true);
        answer_input_->setEnabled(true);
        ShowTask();
      }
    });
  } else {
    wrong_attempts_++;
    wrong_count_label_->setText(
        QString("❌ %1 / %2").arg(wrong_attempts_).arg(max_wrong_));

    feedback_label_->setStyleSheet(kErrorStyle);
    if (wrong_attempts_ >= max_wrong_) {
      feedback_label_->setText(
          QString("❌ Wrong! Expected: \"%1\".").arg(task.russian));
    } else {
      feedback_label_->setText(
          QString("❌ Wrong! Expected: \"%1\". Moving to next task.")
              .arg(task.russian));
    }
    feedback_label_->show();

    current_task_++;
    answer_input_->setEnabled(false);
    submit_button_->setEnabled(false);

    if (wrong_attempts_ >= max_wrong_) {
      QTimer::singleShot(1500, this, [this]() {
        FinishExercise(false);
      });
      return;
    }

    QTimer::singleShot(1500, this, [this]() {
      if (active_) {
        submit_button_->setEnabled(true);
        answer_input_->setEnabled(true);
        ShowTask();
      }
    });
  }
}

void TranslationWidget::OnTimerTick() {
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
    answer_input_->setEnabled(false);

    QMessageBox::warning(this, "Time's Up!",
                         "The time allocated for this exercise has expired.\n"
                         "You did not earn any points.");
    emit ExerciseFinished(0);
  }
}

void TranslationWidget::FinishExercise(bool success) {
  timer_->stop();
  active_ = false;
  submit_button_->setEnabled(false);
  answer_input_->setEnabled(false);

  if (success) {
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
