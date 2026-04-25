#include "mainwindow.h"

#include "difficulty_dialog.h"
#include "exercise_session.h"
#include "string_utils.h"

#include <QApplication>
#include <QButtonGroup>
#include <QEvent>
#include <QFrame>
#include <QGroupBox>
#include <QAction>
#include <QHBoxLayout>
#include <QLineEdit>
#include <QKeyEvent>
#include <QLabel>
#include <QMenuBar>
#include <QMessageBox>
#include <QProgressBar>
#include <QPushButton>
#include <QRadioButton>
#include <QStackedWidget>
#include <QTextEdit>
#include <QTimer>
#include <QVBoxLayout>

namespace {
QString difficultyToText(DifficultyLevel level) {
    switch (level) {
    case DifficultyLevel::Beginner: return "Beginner";
    case DifficultyLevel::Intermediate: return "Intermediate";
    case DifficultyLevel::Advanced: return "Advanced";
    }
    return "Beginner";
}

QString modeToText(ExerciseMode mode) {
    switch (mode) {
    case ExerciseMode::Translation: return "Translation";
    case ExerciseMode::Grammar: return "Grammar";
    }
    return "Translation";
}

int durationForLevel(DifficultyLevel level) {
    switch (level) {
    case DifficultyLevel::Beginner: return 120;
    case DifficultyLevel::Intermediate: return 90;
    case DifficultyLevel::Advanced: return 75;
    }
    return 120;
}

QFrame *card() {
    auto *frame = new QFrame;
    frame->setObjectName("card");
    frame->setFrameShape(QFrame::StyledPanel);
    frame->setFrameShadow(QFrame::Raised);
    return frame;
}

} // namespace

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent) {
    setWindowTitle("Language Learning Lab");
    resize(1100, 720);
    setMinimumSize(900, 620);

    qApp->installEventFilter(this);

    m_remainingSeconds = durationForLevel(m_level);
    buildUi();
    buildMenu();
    refreshModeUi();
    updateStatusLabels();
    updateTimerLabel();

    setStyleSheet(R"(
        QMainWindow {
            background: #f4f7fb;
        }
        #card {
            background: white;
            border: 1px solid #dce3ef;
            border-radius: 18px;
        }
        QLabel#titleLabel {
            font-size: 24px;
            font-weight: 700;
            color: #20304f;
        }
        QLabel#sectionLabel {
            font-size: 16px;
            font-weight: 600;
            color: #20304f;
        }
        QLabel#statusLabel {
            min-height: 24px;
            font-weight: 600;
        }
        QPushButton {
            padding: 10px 14px;
            border-radius: 12px;
            border: 1px solid #9fb5d1;
            background: #ffffff;
        }
        QPushButton:hover {
            background: #eef4ff;
        }
        QPushButton:pressed {
            background: #dfe9fb;
        }
        QPushButton#primaryButton {
            background: #275efe;
            color: white;
            border: none;
            font-weight: 700;
        }
        QPushButton#primaryButton:hover {
            background: #1f4ed8;
        }
        QProgressBar {
            border: 1px solid #d0dae8;
            border-radius: 10px;
            text-align: center;
            height: 22px;
            background: #ffffff;
        }
        QProgressBar::chunk {
            border-radius: 10px;
            background: #275efe;
        }
        QTextEdit {
            border: 1px solid #c9d4e3;
            border-radius: 14px;
            background: #ffffff;
            padding: 10px;
            font-size: 15px;
        }
        QRadioButton {
            padding: 6px 2px;
            font-size: 15px;
        }
    )");
}

void MainWindow::buildMenu() {
    auto *difficultyMenu = menuBar()->addMenu("Difficulty");
    auto *difficultyAction = difficultyMenu->addAction("Change difficulty...");
    connect(difficultyAction, &QAction::triggered, this, [this]() {
        if (m_session.active()) {
            QMessageBox::information(this, "Difficulty", "Finish the current session before changing difficulty.");
            return;
        }
        DifficultyDialog dialog(m_level, this);
        if (dialog.exec() == QDialog::Accepted) {
            m_level = dialog.selectedDifficulty();
            m_remainingSeconds = durationForLevel(m_level);
            updateStatusLabels();
            updateTimerLabel();
            m_statusLabel->setText("Difficulty changed. Press Start to begin a new session.");
        }
    });
}

void MainWindow::buildUi() {
    auto *central = new QWidget;
    auto *mainLayout = new QHBoxLayout(central);
    mainLayout->setContentsMargins(18, 18, 18, 18);
    mainLayout->setSpacing(18);

    auto *left = card();
    auto *leftLayout = new QVBoxLayout(left);
    leftLayout->setContentsMargins(20, 20, 20, 20);
    leftLayout->setSpacing(12);

    m_titleLabel = new QLabel("Language Learning Project");
    m_titleLabel->setObjectName("titleLabel");
    m_titleLabel->setWordWrap(true);
    leftLayout->addWidget(m_titleLabel);

    auto *modeCard = card();
    auto *modeLayout = new QVBoxLayout(modeCard);
    modeLayout->setContentsMargins(14, 14, 14, 14);
    m_modeLabel = new QLabel;
    m_modeLabel->setObjectName("sectionLabel");
    modeLayout->addWidget(m_modeLabel);

    m_translationButton = new QPushButton("Translation");
    m_translationButton->setCheckable(true);
    m_grammarButton = new QPushButton("Grammar");
    m_grammarButton->setCheckable(true);
    connect(m_translationButton, &QPushButton::clicked, this, [this]() {
        if (m_session.active()) {
            m_statusLabel->setText("Finish the current session before switching mode.");
            return;
        }
        m_mode = ExerciseMode::Translation;
        refreshModeUi();
        m_statusLabel->setText("Translation mode selected.");
    });
    connect(m_grammarButton, &QPushButton::clicked, this, [this]() {
        if (m_session.active()) {
            m_statusLabel->setText("Finish the current session before switching mode.");
            return;
        }
        m_mode = ExerciseMode::Grammar;
        refreshModeUi();
        m_statusLabel->setText("Grammar mode selected.");
    });
    modeLayout->addWidget(m_translationButton);
    modeLayout->addWidget(m_grammarButton);
    leftLayout->addWidget(modeCard);

    auto *controlCard = card();
    auto *controlLayout = new QVBoxLayout(controlCard);
    controlLayout->setContentsMargins(14, 14, 14, 14);
    m_difficultyLabel = new QLabel;
    m_difficultyLabel->setObjectName("sectionLabel");
    controlLayout->addWidget(m_difficultyLabel);

    m_startButton = new QPushButton("Start exercise");
    m_startButton->setObjectName("primaryButton");
    
    m_endButton = new QPushButton("End exercise");
    m_endButton->setEnabled(false);

    auto *btnLayout = new QHBoxLayout;
    btnLayout->setContentsMargins(0, 0, 0, 0);
    btnLayout->addWidget(m_startButton);
    btnLayout->addWidget(m_endButton);
    controlLayout->addLayout(btnLayout);

    m_scoreLabel = new QLabel;
    m_wrongLabel = new QLabel;
    m_timeLabel = new QLabel;
    m_progressLabel = new QLabel;
    controlLayout->addWidget(m_scoreLabel);
    controlLayout->addWidget(m_wrongLabel);
    controlLayout->addWidget(m_timeLabel);
    controlLayout->addWidget(m_progressLabel);

    m_progressBar = new QProgressBar;
    m_progressBar->setRange(0, 1);
    m_progressBar->setValue(0);
    controlLayout->addWidget(m_progressBar);
    leftLayout->addWidget(controlCard);

    leftLayout->addStretch(1);
    mainLayout->addWidget(left, 0);

    auto *right = card();
    auto *rightLayout = new QVBoxLayout(right);
    rightLayout->setContentsMargins(20, 20, 20, 20);
    rightLayout->setSpacing(12);

    auto *header = new QLabel("Exercise");
    header->setObjectName("sectionLabel");
    rightLayout->addWidget(header);

    m_promptLabel = new QLabel;
    m_promptLabel->setWordWrap(true);
    m_promptLabel->setMinimumHeight(70);
    m_promptLabel->setStyleSheet("font-size: 18px; font-weight: 600; color: #15243b;");
    rightLayout->addWidget(m_promptLabel);

    m_stack = new QStackedWidget;

    m_emptyPage = new QWidget;
    {
        auto *layout = new QVBoxLayout(m_emptyPage);
        auto *label = new QLabel("Press 'Start exercise' to begin.");
        label->setAlignment(Qt::AlignCenter);
        label->setStyleSheet("color: #6c7a92; font-size: 16px; font-style: italic;");
        layout->addStretch();
        layout->addWidget(label);
        layout->addStretch();
    }

    m_translationPage = new QWidget;
    {
        auto *layout = new QVBoxLayout(m_translationPage);
        layout->setContentsMargins(0, 0, 0, 0);
        m_translationEdit = new QTextEdit;
        m_translationEdit->setPlaceholderText("Type your translation here...");
        layout->addWidget(m_translationEdit);
    }

    m_grammarPage = new QWidget;
    {
        auto *layout = new QVBoxLayout(m_grammarPage);
        layout->setContentsMargins(0, 0, 0, 0);
        auto *group = new QGroupBox("Choose one answer");
        auto *groupLayout = new QVBoxLayout(group);
        m_grammarGroup = new QButtonGroup(this);
        m_grammarGroup->setExclusive(true);
        for (int i = 0; i < 4; ++i) {
            auto *radio = new QRadioButton;
            radio->setAutoExclusive(false);
            m_grammarButtons.push_back(radio);
            m_grammarGroup->addButton(radio, i);
            groupLayout->addWidget(radio);
        }
        layout->addWidget(group);
        layout->addStretch(1);
    }

    m_stack->addWidget(m_emptyPage);
    m_stack->addWidget(m_translationPage);
    m_stack->addWidget(m_grammarPage);
    rightLayout->addWidget(m_stack, 1);

    m_hintLabel = new QLabel("Press H for a short hint about the current task.");
    m_hintLabel->setWordWrap(true);
    m_hintLabel->setStyleSheet("color: #4e5f7a;");
    rightLayout->addWidget(m_hintLabel);

    m_statusLabel = new QLabel;
    m_statusLabel->setObjectName("statusLabel");
    rightLayout->addWidget(m_statusLabel);

    m_submitButton = new QPushButton("Submit");
    m_submitButton->setObjectName("primaryButton");
    m_submitButton->setEnabled(false);
    rightLayout->addWidget(m_submitButton);

    mainLayout->addWidget(right, 1);
    setCentralWidget(central);

    m_timer = new QTimer(this);
    m_timer->setInterval(1000);
    connect(m_timer, &QTimer::timeout, this, [this]() {
        if (m_remainingSeconds > 0) {
            --m_remainingSeconds;
            updateTimerLabel();
        }
        if (m_remainingSeconds <= 0 && m_session.active()) {
            m_session.markTimedOut();
            m_timer->stop();
            finishSession("Time is up", "The allowed time expired.", false);
        }
    });

    connect(m_startButton, &QPushButton::clicked, this, [this]() { startSession(); });
    connect(m_endButton, &QPushButton::clicked, this, &MainWindow::endCurrentSession);
    connect(m_submitButton, &QPushButton::clicked, this, [this]() {
        if (!m_session.active()) {
            QMessageBox::information(this, "Session is not active", "Press Start exercise first.");
            return;
        }

        if (m_mode == ExerciseMode::Grammar && !hasSelectedGrammarAnswer()) {
            m_statusLabel->setText("Select one option before submitting.");
            m_statusLabel->setStyleSheet("color: #8a5a00; font-weight: 600;");
            return;
        }

        ExerciseSession::SubmitResult result = ExerciseSession::SubmitResult::NotStarted;
        if (m_mode == ExerciseMode::Translation) {
            result = m_session.submitTranslationAnswer(m_translationEdit->toPlainText());
        } else {
            result = m_session.submitGrammarAnswer(selectedGrammarAnswer());
        }

        switch (result) {
        case ExerciseSession::SubmitResult::Correct:
            QApplication::beep();
            applyResultFeedback("Correct answer.", true);
            updateStatusLabels();
            loadCurrentTask();
            break;
        case ExerciseSession::SubmitResult::Wrong:
            QApplication::beep();
            applyResultFeedback("Incorrect. Try again.", false);
            updateStatusLabels();
            break;
        case ExerciseSession::SubmitResult::Finished:
            QApplication::beep();
            m_totalScore += m_session.score();
            updateStatusLabels();
            finishSession("Exercise completed", QString("Great job! You earned %1 points.").arg(m_session.score()), true);
            break;
        case ExerciseSession::SubmitResult::Failed:
            QApplication::beep();
            updateStatusLabels();
            finishSession("Too many mistakes", "The exercise ended after too many wrong attempts.", false);
            break;
        case ExerciseSession::SubmitResult::NotStarted:
            break;
        }
    });
}

void MainWindow::refreshModeUi() {
    m_translationButton->setChecked(m_mode == ExerciseMode::Translation);
    m_grammarButton->setChecked(m_mode == ExerciseMode::Grammar);
    m_translationButton->setStyleSheet(m_mode == ExerciseMode::Translation ? "font-weight: 700;" : "");
    m_grammarButton->setStyleSheet(m_mode == ExerciseMode::Grammar ? "font-weight: 700;" : "");
    
    if (m_session.active()) {
        m_stack->setCurrentWidget(m_mode == ExerciseMode::Translation ? m_translationPage : m_grammarPage);
    } else {
        m_stack->setCurrentWidget(m_emptyPage);
        m_promptLabel->clear();
        m_hintLabel->setText("Press H for a short hint about the current task.");
    }
    
    m_modeLabel->setText(QString("Current mode: %1").arg(modeToText(m_mode)));
    m_difficultyLabel->setText(QString("Difficulty: %1").arg(difficultyToText(m_level)));
}

void MainWindow::startSession() {
    m_session.start(m_mode, m_level);
    m_remainingSeconds = m_session.config().durationSeconds;
    updateStatusLabels();
    updateTimerLabel();
    loadCurrentTask();
    m_submitButton->setEnabled(true);
    m_startButton->setEnabled(false);
    m_endButton->setEnabled(true);
    m_translationButton->setEnabled(false);
    m_grammarButton->setEnabled(false);
    m_timer->start();
    m_statusLabel->setStyleSheet("");
    m_statusLabel->setText(QString("Session started. %1 tasks, %2 seconds, max %3 mistakes.")
                           .arg(m_session.totalTasks())
                           .arg(m_session.config().durationSeconds)
                           .arg(m_session.config().maxWrongAttempts));
}

void MainWindow::endCurrentSession() {
    if (!m_session.active()) {
        return;
    }

    auto reply = QMessageBox::question(this, "End exercise", 
        "Are you sure you want to end the current exercise early?", 
        QMessageBox::Yes | QMessageBox::No);
    
    if (reply != QMessageBox::Yes) {
        return;
    }

    m_session.reset();
    m_timer->stop();
    
    m_remainingSeconds = durationForLevel(m_level);
    updateTimerLabel();

    m_submitButton->setEnabled(false);
    m_startButton->setEnabled(true);
    m_endButton->setEnabled(false);
    m_translationButton->setEnabled(true);
    m_grammarButton->setEnabled(true);

    m_statusLabel->setText("Exercise ended early.");
    m_statusLabel->setStyleSheet("");
    updateStatusLabels();
}

void MainWindow::loadCurrentTask() {
    if (!m_session.active()) {
        return;
    }

    const Task &task = m_session.currentTask();
    m_statusLabel->setStyleSheet("");
    m_promptLabel->setText(task.prompt);
    m_hintLabel->setText(task.hint.isEmpty() ? "Press H for help." : task.hint);

    if (task.type == Task::Type::Translation) {
        m_stack->setCurrentWidget(m_translationPage);
        m_translationEdit->clear();
        m_translationEdit->setFocus();
    } else {
        m_stack->setCurrentWidget(m_grammarPage);
        clearGrammarSelection();
        for (int i = 0; i < m_grammarButtons.size(); ++i) {
            const QString text = (i < task.options.size()) ? task.options[i] : QString();
            m_grammarButtons[i]->setText(text);
        }
        if (!m_grammarButtons.isEmpty()) {
            m_grammarButtons[0]->setFocus();
        }
    }

    m_progressBar->setRange(0, m_session.totalTasks());
    m_progressBar->setValue(m_session.currentIndex());
    updateStatusLabels();
}

void MainWindow::updateStatusLabels() {
    m_scoreLabel->setText(QString("Score: %1").arg(m_totalScore));
    m_wrongLabel->setText(QString("Wrong attempts: %1 / %2").arg(m_session.wrongAttempts()).arg(m_session.config().maxWrongAttempts));
    const int total = qMax(1, m_session.totalTasks());
    m_progressBar->setRange(0, total);
    m_progressBar->setValue(qMin(m_session.currentIndex(), total));
    m_progressLabel->setText(QString("Progress: %1 / %2").arg(m_session.currentIndex()).arg(m_session.totalTasks()));
    refreshModeUi();
}

void MainWindow::updateTimerLabel() {
    const int minutes = m_remainingSeconds / 60;
    const int seconds = m_remainingSeconds % 60;
    m_timeLabel->setText(QString("Time left: %1:%2")
                             .arg(minutes, 2, 10, QChar('0'))
                             .arg(seconds, 2, 10, QChar('0')));
}

void MainWindow::finishSession(const QString &title, const QString &text, bool success) {
    m_timer->stop();
    m_submitButton->setEnabled(false);
    m_startButton->setEnabled(true);
    m_endButton->setEnabled(false);
    m_translationButton->setEnabled(true);
    m_grammarButton->setEnabled(true);

    if (success) {
        m_statusLabel->setText(text);
        QMessageBox::information(this, title, text);
    } else {
        m_statusLabel->setText(text);
        QMessageBox::warning(this, title, text);
    }

    updateStatusLabels();
}

QString MainWindow::currentHint() const {
    if (!m_session.active()) {
        return "Press Start exercise to begin a new session.";
    }
    return m_session.currentTask().hint;
}

QString MainWindow::currentModeText() const {
    return modeToText(m_mode);
}

void MainWindow::applyResultFeedback(const QString &message, bool positive) {
    m_statusLabel->setText(message);
    m_statusLabel->setStyleSheet(positive ? "color: #188038; font-weight: 600;" : "color: #c62828; font-weight: 600;");
}

QString MainWindow::selectedGrammarAnswer() const {
    for (auto *button : m_grammarButtons) {
        if (button->isChecked()) {
            return button->text();
        }
    }
    return {};
}

bool MainWindow::hasSelectedGrammarAnswer() const {
    return !selectedGrammarAnswer().isEmpty();
}

void MainWindow::clearGrammarSelection() {
    const bool previousExclusive = m_grammarGroup->exclusive();
    m_grammarGroup->setExclusive(false);
    for (auto *button : m_grammarButtons) {
        button->setChecked(false);
    }
    m_grammarGroup->setExclusive(previousExclusive);
}

bool MainWindow::eventFilter(QObject *watched, QEvent *event) {
    if (event->type() == QEvent::KeyPress) {
        auto *keyEvent = static_cast<QKeyEvent*>(event);
        const QWidget *focus = QApplication::focusWidget();
        const bool typing = qobject_cast<const QTextEdit*>(focus) || qobject_cast<const QLineEdit*>(focus);
        if (keyEvent->key() == Qt::Key_H && !typing) {
            QString helpText = currentHint();
            if (!m_session.active()) {
                helpText = QStringLiteral("Current mode: %1. Current difficulty: %2.\n\nPress Start exercise to generate a new set of tasks.")
                                .arg(currentModeText(), difficultyToText(m_level));
            }
            QMessageBox::information(this, "Help", helpText);
            return true;
        }
    }
    return QMainWindow::eventFilter(watched, event);
}