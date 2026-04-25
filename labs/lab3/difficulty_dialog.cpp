#include "difficulty_dialog.h"

#include <QDialogButtonBox>
#include <QGroupBox>
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>

DifficultyDialog::DifficultyDialog(Difficulty current, QWidget* parent)
    : QDialog(parent) {
  setWindowTitle("Select Difficulty");
  setFixedSize(360, 310);
  setStyleSheet(
      "QDialog { background: #181825; }"
      "QLabel { color: #cdd6f4; font-size: 16px; font-weight: bold; }"
      "QGroupBox { border: none; margin-top: 0px; padding: 0px; }"
      "QRadioButton { "
      "  background: #1e1e2e; color: #cdd6f4; font-size: 14px; font-weight: bold; "
      "  padding: 14px; border-radius: 10px; border: 1px solid #313244; "
      "  margin-bottom: 8px; "
      "}"
      "QRadioButton:hover { background: #313244; border-color: #45475a; }"
      "QRadioButton:checked { background: #313244; border-color: #89b4fa; color: #89b4fa; }"
      "QRadioButton::indicator { width: 18px; height: 18px; }"
      "QPushButton { background: #89b4fa; color: #11111b; border: none; "
      "border-radius: 8px; padding: 10px 24px; font-weight: bold; "
      "font-size: 14px; }"
      "QPushButton:hover { background: #b4befe; }"
      "QPushButton:pressed { background: #74c7ec; }"
      "QPushButton#cancelBtn { background: #313244; color: #cdd6f4; }"
      "QPushButton#cancelBtn:hover { background: #45475a; }"
  );

  QVBoxLayout* layout = new QVBoxLayout(this);
  layout->setContentsMargins(20, 24, 20, 20);
  layout->setSpacing(14);

  QLabel* title = new QLabel("Choose difficulty level");
  layout->addWidget(title);

  QGroupBox* group = new QGroupBox();
  QVBoxLayout* group_layout = new QVBoxLayout(group);
  group_layout->setContentsMargins(0, 0, 0, 0);

  easy_radio_ = new QRadioButton("🟢  Easy (5 tasks, 2 min)");
  easy_radio_->setCursor(Qt::PointingHandCursor);
  medium_radio_ = new QRadioButton("🟡  Medium (8 tasks, 1.5 min)");
  medium_radio_->setCursor(Qt::PointingHandCursor);
  hard_radio_ = new QRadioButton("🔴  Hard (10 tasks, 1 min)");
  hard_radio_->setCursor(Qt::PointingHandCursor);

  group_layout->addWidget(easy_radio_);
  group_layout->addWidget(medium_radio_);
  group_layout->addWidget(hard_radio_);
  layout->addWidget(group);

  switch (current) {
    case Difficulty::Easy:
      easy_radio_->setChecked(true);
      break;
    case Difficulty::Medium:
      medium_radio_->setChecked(true);
      break;
    case Difficulty::Hard:
      hard_radio_->setChecked(true);
      break;
  }

  layout->addStretch();

  QDialogButtonBox* buttons =
      new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
  
  QPushButton* ok_btn = buttons->button(QDialogButtonBox::Ok);
  if (ok_btn) {
    ok_btn->setText("Apply");
    ok_btn->setCursor(Qt::PointingHandCursor);
  }
  
  QPushButton* cancel_btn = buttons->button(QDialogButtonBox::Cancel);
  if (cancel_btn) {
    cancel_btn->setObjectName("cancelBtn");
    cancel_btn->setCursor(Qt::PointingHandCursor);
  }
  
  layout->addWidget(buttons);

  connect(buttons, &QDialogButtonBox::accepted, this, &QDialog::accept);
  connect(buttons, &QDialogButtonBox::rejected, this, &QDialog::reject);
}

Difficulty DifficultyDialog::GetSelectedDifficulty() const {
  if (hard_radio_->isChecked()) {
    return Difficulty::Hard;
  }
  if (medium_radio_->isChecked()) {
    return Difficulty::Medium;
  }
  return Difficulty::Easy;
}
