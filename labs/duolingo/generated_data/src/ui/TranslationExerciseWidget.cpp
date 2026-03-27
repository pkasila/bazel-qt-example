#include "TranslationExerciseWidget.h"
#include "core/ScoringSystem.h"    // Для логики подсчета очков
#include "db/ExerciseRepository.h" // <--- ДОБАВЛЕНО ЭТОТ INCLUDE

#include <QDebug>
#include <QHBoxLayout>
#include <QLabel>
#include <QProgressBar>
#include <QPushButton>
#include <QTextEdit>
#include <QTimer>
#include <QVBoxLayout>

TranslationExerciseWidget::TranslationExerciseWidget(QWidget *parent)
    : ExerciseViewBase(parent) {
  setupUi();
  // loadNewExerciseSet(); // Вызывается MainWindow, когда этот виджет
  // становится текущим
}

void TranslationExerciseWidget::setupUi() {
  QVBoxLayout *mainLayout = new QVBoxLayout(this);
  mainLayout->setSpacing(10);

  // Верхняя панель для прогресса, очков, таймера
  QHBoxLayout *statsLayout = new QHBoxLayout();
  m_progressBar = new QProgressBar(this);
  m_progressBar->setTextVisible(true);
  m_scoreLabel = new QLabel(tr("Score: 0 | Errors: 0/3"), this);
  m_timerLabel = new QLabel(tr("Time: 03:00"), this);
  statsLayout->addWidget(m_progressBar, 2);
  statsLayout->addWidget(m_scoreLabel, 1);
  statsLayout->addWidget(m_timerLabel, 1);
  mainLayout->addLayout(statsLayout);

  m_instructionLabel =
      new QLabel(tr("Translate the following sentence:"), this);
  m_instructionLabel->setAlignment(Qt::AlignCenter);
  QFont instructionFont = m_instructionLabel->font();
  instructionFont.setPointSize(instructionFont.pointSize() + 2);
  m_instructionLabel->setFont(instructionFont);
  mainLayout->addWidget(m_instructionLabel);

  m_taskTextLabel =
      new QLabel("<i>Original sentence will appear here...</i>", this);
  m_taskTextLabel->setAlignment(Qt::AlignCenter);
  m_taskTextLabel->setWordWrap(true);
  QFont taskFont = m_taskTextLabel->font();
  taskFont.setPointSize(taskFont.pointSize() + 4);
  taskFont.setBold(true);
  m_taskTextLabel->setFont(taskFont);
  mainLayout->addWidget(m_taskTextLabel);

  m_answerTextEdit = new QTextEdit(this);
  m_answerTextEdit->setFixedHeight(100);
  mainLayout->addWidget(m_answerTextEdit);

  m_feedbackLabel = new QLabel("", this);
  m_feedbackLabel->setAlignment(Qt::AlignCenter);
  QFont feedbackFont = m_feedbackLabel->font();
  feedbackFont.setPointSize(feedbackFont.pointSize() + 1);
  m_feedbackLabel->setFont(feedbackFont);
  mainLayout->addWidget(m_feedbackLabel);

  m_submitButton = new QPushButton(tr("Submit Answer"), this);
  m_submitButton->setFixedHeight(40);
  connect(m_submitButton, &QPushButton::clicked, this,
          &TranslationExerciseWidget::processAnswer);
  mainLayout->addWidget(m_submitButton, 0, Qt::AlignHCenter);

  mainLayout->addStretch(1);
  setLayout(mainLayout);
  updateProgressDisplay();
}

void TranslationExerciseWidget::displayCurrentExercise() {
  if (m_currentExerciseIndex < 0 ||
      m_currentExerciseIndex >= m_currentExerciseSet.size())
    return;

  const auto &data = m_currentExerciseData;
  m_taskTextLabel->setText(data.originalText);
  m_answerTextEdit->clear();
  m_answerTextEdit->setFocus();
  if (m_submitButton)
    m_submitButton->setEnabled(true);
  if (m_progressBar)
    m_progressBar->setFormat(QString("%1/%2")
                                 .arg(m_currentExerciseIndex + 1)
                                 .arg(m_currentExerciseSet.size()));
}

void TranslationExerciseWidget::processAnswer() {
  if (m_currentExerciseIndex < 0 ||
      m_currentExerciseIndex >= m_currentExerciseSet.size() ||
      !m_scoringSystem || !m_exerciseRepository)
    return; // Добавлена проверка m_exerciseRepository
  if (m_submitButton)
    m_submitButton->setEnabled(false);

  QString userAnswer = m_answerTextEdit->toPlainText().trimmed();
  if (userAnswer.isEmpty()) {
    m_feedbackLabel->setText(
        tr("<font color='orange'>Please enter a translation.</font>"));
    if (m_submitButton)
      m_submitButton->setEnabled(true);
    return;
  }

  const auto &currentEx = m_currentExerciseData;
  bool isCorrect = userAnswer.compare(currentEx.correctAnswerText.trimmed(),
                                      Qt::CaseInsensitive) == 0;

  if (isCorrect) {
    m_feedbackLabel->setText(tr("<font color='green'><b>Correct!</b></font>"));
    m_scoringSystem->recordCorrectAnswer();
    m_exerciseRepository->markExerciseAsSolved(
        0, currentEx.id); // Теперь компилятор знает об этом методе
    QTimer::singleShot(1000, this, [this]() { this->nextExercise(); });
  } else {
    m_feedbackLabel->setText(tr("<font color='red'><b>Incorrect.</b> The "
                                "correct answer was: <i>%1</i></font>")
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
