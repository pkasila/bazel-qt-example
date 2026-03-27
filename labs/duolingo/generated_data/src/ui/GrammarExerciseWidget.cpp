#include "GrammarExerciseWidget.h"
#include "core/ScoringSystem.h"
#include "db/ExerciseRepository.h" // Этот include здесь нужен!

#include <QDebug>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QProgressBar>
#include <QPushButton>
#include <QRadioButton>
#include <QTimer>
#include <QVBoxLayout>

GrammarExerciseWidget::GrammarExerciseWidget(QWidget *parent)
    : ExerciseViewBase(parent) {
  setupUi();
}

void GrammarExerciseWidget::setupUi() {
  QVBoxLayout *mainLayout = new QVBoxLayout(this);
  mainLayout->setSpacing(10);

  QHBoxLayout *statsLayout = new QHBoxLayout();
  m_progressBar = new QProgressBar(this);
  m_progressBar->setTextVisible(true);
  m_scoreLabel = new QLabel(tr("Score: 0 | Errors: 0/3"), this);
  m_timerLabel = new QLabel(tr("Time: 03:00"), this);
  statsLayout->addWidget(m_progressBar, 2);
  statsLayout->addWidget(m_scoreLabel, 1);
  statsLayout->addWidget(m_timerLabel, 1);
  mainLayout->addLayout(statsLayout);

  m_instructionLabel = new QLabel(tr("Choose the correct option:"), this);
  m_instructionLabel->setAlignment(Qt::AlignCenter);
  QFont instructionFont = m_instructionLabel->font();
  instructionFont.setPointSize(instructionFont.pointSize() + 2);
  m_instructionLabel->setFont(instructionFont);
  mainLayout->addWidget(m_instructionLabel);

  m_taskTextLabel =
      new QLabel("<i>Grammar question will appear here...</i>", this);
  m_taskTextLabel->setAlignment(Qt::AlignCenter);
  m_taskTextLabel->setWordWrap(true);
  QFont taskFont = m_taskTextLabel->font();
  taskFont.setPointSize(taskFont.pointSize() + 4);
  taskFont.setBold(true);
  m_taskTextLabel->setFont(taskFont);
  mainLayout->addWidget(m_taskTextLabel);

  m_optionsGroup = new QGroupBox(tr("Options"), this);
  m_radioButtonsLayout = new QVBoxLayout();
  m_optionsGroup->setLayout(m_radioButtonsLayout);
  mainLayout->addWidget(m_optionsGroup);

  m_feedbackLabel = new QLabel("", this);
  m_feedbackLabel->setAlignment(Qt::AlignCenter);
  QFont feedbackFont = m_feedbackLabel->font();
  feedbackFont.setPointSize(feedbackFont.pointSize() + 1);
  m_feedbackLabel->setFont(feedbackFont);
  mainLayout->addWidget(m_feedbackLabel);

  m_submitButton = new QPushButton(tr("Submit Answer"), this);
  m_submitButton->setFixedHeight(40);
  connect(m_submitButton, &QPushButton::clicked, this,
          &GrammarExerciseWidget::processAnswer);
  mainLayout->addWidget(m_submitButton, 0, Qt::AlignHCenter);

  mainLayout->addStretch(1);
  setLayout(mainLayout);
  updateProgressDisplay();
}

void GrammarExerciseWidget::clearRadioButtons() {
  for (QRadioButton *rb : m_radioButtons) {
    m_radioButtonsLayout->removeWidget(rb);
    delete rb;
  }
  m_radioButtons.clear();
}

void GrammarExerciseWidget::displayCurrentExercise() {
  if (m_currentExerciseIndex < 0 ||
      m_currentExerciseIndex >= m_currentExerciseSet.size())
    return;

  clearRadioButtons();
  const auto &data = m_currentExerciseData;
  m_taskTextLabel->setText(data.originalText);

  QStringList options = data.grammarOptions;
  if (options.isEmpty() && data.additionalData.contains("options")) {
    QVariantList optionsVariantList =
        data.additionalData.value("options").toList();
    for (const QVariant &v : optionsVariantList) {
      options.append(v.toString());
    }
  }

  if (options.isEmpty()) {
    qWarning() << "No grammar options for exercise ID:" << data.id
               << data.originalText;
    m_taskTextLabel->setText(
        tr("Error: No options provided for this grammar exercise."));
    if (m_submitButton)
      m_submitButton->setEnabled(false);
    return;
  }
  if (m_submitButton)
    m_submitButton->setEnabled(true);

  for (const QString &optionText : options) {
    QRadioButton *rb = new QRadioButton(optionText, m_optionsGroup);
    m_radioButtonsLayout->addWidget(rb);
    m_radioButtons.append(rb);
  }
  if (!m_radioButtons.isEmpty()) {
    m_radioButtons.first()->setChecked(true);
  }
  if (m_progressBar)
    m_progressBar->setFormat(QString("%1/%2")
                                 .arg(m_currentExerciseIndex + 1)
                                 .arg(m_currentExerciseSet.size()));
}

void GrammarExerciseWidget::processAnswer() {
  if (m_currentExerciseIndex < 0 ||
      m_currentExerciseIndex >= m_currentExerciseSet.size() ||
      !m_scoringSystem || !m_exerciseRepository)
    return;
  if (m_submitButton)
    m_submitButton->setEnabled(false);

  QString selectedAnswer;
  for (QRadioButton *rb : m_radioButtons) {
    if (rb->isChecked()) {
      selectedAnswer = rb->text();
      break;
    }
  }

  if (selectedAnswer.isEmpty()) {
    m_feedbackLabel->setText(
        tr("<font color='orange'>Please select an option.</font>"));
    if (m_submitButton)
      m_submitButton->setEnabled(true);
    return;
  }

  const auto &currentEx = m_currentExerciseData;
  bool isCorrect =
      (selectedAnswer.compare(currentEx.correctAnswerText.trimmed(),
                              Qt::CaseInsensitive) == 0);

  if (isCorrect) {
    m_feedbackLabel->setText(tr("<font color='green'><b>Correct!</b></font>"));
    m_scoringSystem->recordCorrectAnswer();
    m_exerciseRepository->markExerciseAsSolved(0, currentEx.id);
    QTimer::singleShot(1000, this, [this]() { this->nextExercise(); });
  } else {
    m_feedbackLabel->setText(tr("<font color='red'><b>Incorrect.</b> Correct "
                                "answer was: <i>%1</i></font>")
                                 .arg(currentEx.correctAnswerText));
    m_scoringSystem->recordIncorrectAnswer();
    if (m_scoringSystem->hasExceededErrorLimit()) {
      endSession(false, tr("Too many errors. Session ended."));
      return;
    }
    QTimer::singleShot(2500, this, [this]() { this->nextExercise(); });
  }
  updateProgressDisplay();
}
