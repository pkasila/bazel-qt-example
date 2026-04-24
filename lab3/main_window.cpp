#include "main_window.h"

#include <QComboBox>
#include <QDialog>
#include <QFont>
#include <QHBoxLayout>
#include <QLabel>
#include <QList>
#include <QMenuBar>
#include <QMessageBox>
#include <QProgressBar>
#include <QPushButton>
#include <QShortcut>
#include <QStackedWidget>
#include <QString>
#include <QTimer>
#include <QVBoxLayout>

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
    , m_difficulty(5)
    , m_totalPoints(0)
    , m_idx(0)
    , m_wrongAttempts(0)
    , m_timeLeft(60)
    , m_isPerfect(true)
    , m_btnSubmitEnabled(true)
    , m_isGrammar(false) {
    setupUI();
    setupMenu();
    setupShortcuts();
    applyDesign();
    m_timer = new QTimer(this);
    connect(m_timer, &QTimer::timeout, this, [this]() {
        m_timeLeft--;
        m_lblTimer->setText(QString("Time: %1s").arg(m_timeLeft));
        if (m_timeLeft <= 0) {
            m_timer->stop();
            finishSession("Time is up! Exercise finished.");
        }
    });
}

void MainWindow::startExercise(bool isGrammar) {
    m_isGrammar = isGrammar;
    m_questions = ExerciseGenerator::generate(m_difficulty, isGrammar);
    m_idx = 0;
    m_wrongAttempts = 0;
    m_isPerfect = true;
    m_timeLeft = 60;
    m_progressBar->setValue(0);
    m_lblFeedback->clear();
    m_btnSubmitEnabled = true;
    m_timer->start(1000);
    m_stacked->setCurrentWidget(m_exercisePage);
    loadQuestion();
}

void MainWindow::loadQuestion() {
    if (m_idx < m_questions.size()) {
        if (m_isGrammar) {
            m_grammarWidget->setQuestion(m_questions[m_idx]);
            m_stackedEx->setCurrentWidget(m_grammarWidget);
        } else {
            m_translationWidget->setQuestion(m_questions[m_idx]);
            m_stackedEx->setCurrentWidget(m_translationWidget);
        }
    }
}

void MainWindow::handleAnswer(bool isCorrect) {
    if (!m_btnSubmitEnabled) {
        return;
    }
    if (isCorrect) {
        m_lblFeedback->setText("Correct! Great job!");
        m_lblFeedback->setStyleSheet("color: #2e7d32; font-weight: bold; font-size: 16px;");
    } else {
        m_lblFeedback->setText("Incorrect. Try to learn from mistakes!");
        m_lblFeedback->setStyleSheet("color: #c62828; font-weight: bold; font-size: 16px;");
        m_wrongAttempts++;
        m_isPerfect = false;
    }
    m_idx++;
    m_progressBar->setValue(m_idx);
    m_btnSubmitEnabled = false;
    if (m_wrongAttempts >= 3) {
        QTimer::singleShot(
            800, this, [this]() { finishSession("Error limit (3) exceeded. Exercise finished."); });
    } else if (m_idx >= m_difficulty) {
        QTimer::singleShot(800, this, [this]() {
            if (m_isPerfect) {
                m_totalPoints += 10;
                m_lblPoints->setText(QString("Points: %1").arg(m_totalPoints));
                finishSession(QString("Perfect! All %1 tasks completed without errors. +10 points!")
                                  .arg(m_difficulty));
            } else {
                finishSession("Exercise finished. Errors were made, no points awarded.");
            }
        });
    } else {
        QTimer::singleShot(1200, this, [this]() {
            m_btnSubmitEnabled = true;
            loadQuestion();
            m_lblFeedback->clear();
        });
    }
}

void MainWindow::submitCurrentAnswer() {
    if (!m_btnSubmitEnabled) {
        return;
    }
    bool ok = m_isGrammar ? m_grammarWidget->checkAnswer() : m_translationWidget->checkAnswer();
    handleAnswer(ok);
}

void MainWindow::finishSession(const QString& message) {
    m_timer->stop();
    m_btnSubmitEnabled = false;
    QMessageBox::information(this, "Session Result", message);
    QTimer::singleShot(1000, this, [this]() { m_stacked->setCurrentWidget(m_menuPage); });
}

void MainWindow::showHelp() {
    if (m_stacked->currentWidget() != m_exercisePage) {
        QMessageBox::information(
            this, "Help",
            "Start an exercise first to see hints.\n\nControls:\n- Press H for hints\n- Press "
            "Enter to submit\n- Use Settings to change difficulty");
        return;
    }
    QString hint;
    if (m_isGrammar) {
        hint = m_grammarWidget->getHint();
    } else {
        hint = m_translationWidget->getHint();
    }
    if (hint.isEmpty()) {
        QMessageBox::information(this, "Hint", "No hint available for this question.");
    } else {
        QMessageBox::information(this, "Hint", QString("Rule/Hint:\n\n%1").arg(hint));
    }
}

void MainWindow::setupUI() {
    setWindowTitle("LingoLearn - Master Languages");
    resize(800, 650);
    auto* central = new QWidget(this);
    setCentralWidget(central);
    auto* mainLayout = new QVBoxLayout(central);
    auto* topPanel = new QHBoxLayout();

    m_btnSettings = new QPushButton("Settings", this);
    m_btnSettings->setMinimumHeight(40);
    m_btnSettings->setMinimumWidth(120);
    m_btnSettings->setFont(QFont("Arial", 11, QFont::Bold));
    m_btnSettings->setStyleSheet(R"(
        QPushButton {
            background-color: #FF9800;
            color: white;
            border: 2px solid #F57C00;
            border-radius: 8px;
            padding: 8px 16px;
            font-weight: bold;
        }
        QPushButton:hover { background-color: #FB8C00; border: 2px solid #EF6C00; }
        QPushButton:pressed { background-color: #F57C00; }
    )");
    m_btnSettings->setToolTip("Change difficulty level and game settings");
    connect(m_btnSettings, &QPushButton::clicked, this, [this]() {
        DifficultyDialog dlg(this);
        int idx = (m_difficulty == 3) ? 0 : (m_difficulty == 5 ? 1 : 2);
        if (dlg.getCombo()) {
            dlg.getCombo()->setCurrentIndex(idx);
        }
        if (dlg.exec() == QDialog::Accepted) {
            m_difficulty = dlg.getN();
            m_progressBar->setMaximum(m_difficulty);
            m_lblDifficulty->setText(
                QString("Level: %1")
                    .arg(m_difficulty == 3 ? "Easy" : (m_difficulty == 5 ? "Medium" : "Hard")));
        }
    });

    auto* infoLayout = new QHBoxLayout();
    m_lblPoints = new QLabel("Points: 0", this);
    m_lblPoints->setFont(QFont("Arial", 12, QFont::Bold));
    m_lblPoints->setStyleSheet("color: #2196F3;");

    m_lblDifficulty = new QLabel("Level: Medium", this);
    m_lblDifficulty->setFont(QFont("Arial", 11, QFont::Bold));

    m_lblTimer = new QLabel("Time: 60s", this);
    m_lblTimer->setFont(QFont("Arial", 12, QFont::Bold));
    m_lblTimer->setStyleSheet("color: #F44336;");

    infoLayout->addWidget(m_lblPoints);
    infoLayout->addSpacing(20);
    infoLayout->addWidget(m_lblDifficulty);
    infoLayout->addSpacing(20);
    infoLayout->addWidget(m_lblTimer);
    infoLayout->addStretch();

    topPanel->addWidget(m_btnSettings);
    topPanel->addLayout(infoLayout);
    mainLayout->addLayout(topPanel);

    m_progressBar = new QProgressBar(this);
    m_progressBar->setMaximum(m_difficulty);
    m_progressBar->setMinimumHeight(25);
    m_progressBar->setFormat(" %p% (%v/%m)");
    mainLayout->addWidget(m_progressBar);

    m_stacked = new QStackedWidget(this);

    m_menuPage = new QWidget();
    auto* menuLayout = new QVBoxLayout(m_menuPage);
    menuLayout->addStretch();
    auto* title = new QLabel("Welcome to LingoLearn!", m_menuPage);
    title->setAlignment(Qt::AlignCenter);
    title->setFont(QFont("Arial", 24, QFont::Bold));
    title->setStyleSheet("color: #4CAF50; margin: 20px;");

    auto* subtitle = new QLabel("Choose exercise type to start learning:", m_menuPage);
    subtitle->setAlignment(Qt::AlignCenter);
    subtitle->setFont(QFont("Arial", 14));
    subtitle->setStyleSheet("color: #666; margin-bottom: 30px;");

    m_btnTranslation = new QPushButton("Translation Exercise", m_menuPage);
    m_btnTranslation->setMinimumHeight(60);
    m_btnTranslation->setFont(QFont("Arial", 14, QFont::Bold));

    m_btnGrammar = new QPushButton("Grammar Exercise", m_menuPage);
    m_btnGrammar->setMinimumHeight(60);
    m_btnGrammar->setFont(QFont("Arial", 14, QFont::Bold));

    auto* helpText =
        new QLabel("Press H for hints | Use Settings to change difficulty", m_menuPage);
    helpText->setAlignment(Qt::AlignCenter);
    helpText->setStyleSheet("color: #999; font-style: italic; margin-top: 30px;");

    connect(m_btnTranslation, &QPushButton::clicked, this, [this]() { startExercise(false); });
    connect(m_btnGrammar, &QPushButton::clicked, this, [this]() { startExercise(true); });

    menuLayout->addStretch();
    menuLayout->addWidget(title);
    menuLayout->addWidget(subtitle);
    menuLayout->addSpacing(30);
    menuLayout->addWidget(m_btnTranslation);
    menuLayout->addSpacing(15);
    menuLayout->addWidget(m_btnGrammar);
    menuLayout->addWidget(helpText);
    menuLayout->addStretch();

    m_exercisePage = new QWidget();
    auto* exLayout = new QVBoxLayout(m_exercisePage);
    m_stackedEx = new QStackedWidget(m_exercisePage);
    m_translationWidget = new TranslationWidget(m_exercisePage);
    m_grammarWidget = new GrammarWidget(m_exercisePage);

    m_translationWidget->setOnEnterPressed([this]() {
        if (m_btnSubmitEnabled) {
            submitCurrentAnswer();
        }
    });

    m_stackedEx->addWidget(m_translationWidget);
    m_stackedEx->addWidget(m_grammarWidget);

    m_lblFeedback = new QLabel("", m_exercisePage);
    m_lblFeedback->setAlignment(Qt::AlignCenter);
    m_lblFeedback->setFont(QFont("Arial", 14));
    m_lblFeedback->setMinimumHeight(40);

    auto* btnSubmit = new QPushButton("Submit Answer", m_exercisePage);
    btnSubmit->setMinimumHeight(50);
    btnSubmit->setFont(QFont("Arial", 14, QFont::Bold));
    connect(btnSubmit, &QPushButton::clicked, this, [this]() { submitCurrentAnswer(); });

    exLayout->addWidget(m_stackedEx);
    exLayout->addWidget(m_lblFeedback);
    exLayout->addWidget(btnSubmit);

    m_stacked->addWidget(m_menuPage);
    m_stacked->addWidget(m_exercisePage);
    mainLayout->addWidget(m_stacked);
}

void MainWindow::setupMenu() {
    auto* bar = menuBar();
    auto* settingsMenu = bar->addMenu("Settings");
    auto* diffAction = settingsMenu->addAction("Change Difficulty...");
    diffAction->setToolTip("Open difficulty settings dialog");
    diffAction->setShortcut(QKeySequence("Ctrl+D"));
    connect(diffAction, &QAction::triggered, this, [this]() {
        DifficultyDialog dlg(this);
        int idx = (m_difficulty == 3) ? 0 : (m_difficulty == 5 ? 1 : 2);
        if (dlg.getCombo()) {
            dlg.getCombo()->setCurrentIndex(idx);
        }
        if (dlg.exec() == QDialog::Accepted) {
            m_difficulty = dlg.getN();
            m_progressBar->setMaximum(m_difficulty);
            m_lblDifficulty->setText(
                QString("Level: %1")
                    .arg(m_difficulty == 3 ? "Easy" : (m_difficulty == 5 ? "Medium" : "Hard")));
        }
    });

    settingsMenu->addSeparator();
    auto* resetAction = settingsMenu->addAction("Reset Progress");
    resetAction->setToolTip("Reset all points and progress");
    resetAction->setShortcut(QKeySequence("Ctrl+R"));
    connect(resetAction, &QAction::triggered, this, [this]() {
        QMessageBox::StandardButton reply = QMessageBox::question(
            this, "Reset Progress",
            "Are you sure you want to reset all points?\n\nThis action cannot be undone!",
            QMessageBox::Yes | QMessageBox::No);
        if (reply == QMessageBox::Yes) {
            m_totalPoints = 0;
            m_lblPoints->setText("Points: 0");
            QMessageBox::information(this, "Reset Complete", "Progress has been reset.");
        }
    });

    auto* helpMenu = bar->addMenu("Help");
    auto* helpAction = helpMenu->addAction("How to Play");
    helpAction->setToolTip("Show help and controls");
    helpAction->setShortcut(QKeySequence("F1"));
    connect(helpAction, &QAction::triggered, this, [this]() {
        QMessageBox::information(
            this, "How to Play",
            "How to Use LingoLearn:\n\n"
            "1. Choose Exercise: Select Translation or Grammar from menu\n"
            "2. Answer Questions: Type translation or choose correct option\n"
            "3. Submit: Press Enter or click Submit button\n"
            "4. Earn Points: Get 10 points for perfect exercises!\n\n"
            "Keyboard Shortcuts:\n"
            "- Enter - Submit answer\n"
            "- H - Show hint\n"
            "- Ctrl+D - Change difficulty\n"
            "- F1 - Show this help\n\n"
            "Tip: Use Settings button to adjust difficulty!");
    });

    auto* hintAction = helpMenu->addAction("Hint");
    hintAction->setToolTip("Show hint for current question (or press H)");
    connect(hintAction, &QAction::triggered, this, &MainWindow::showHelp);

    helpMenu->addSeparator();
    auto* aboutAction = helpMenu->addAction("About LingoLearn");
    connect(aboutAction, &QAction::triggered, this, [this]() {
        QMessageBox::about(
            this, "About LingoLearn",
            "LingoLearn v1.0\n\n"
            "An interactive language learning application.\n\n"
            "Features:\n"
            "- Translation exercises\n"
            "- Grammar practice\n"
            "- Points system\n"
            "- Time challenges\n"
            "- Multiple difficulty levels\n\n"
            "Built with Qt and C++");
    });
}

void MainWindow::setupShortcuts() {
    auto* hShortcut = new QShortcut(QKeySequence(Qt::Key_H), this);
    hShortcut->setContext(Qt::WindowShortcut);
    connect(hShortcut, &QShortcut::activated, this, &MainWindow::showHelp);

    auto* enterShortcut = new QShortcut(QKeySequence(Qt::Key_Return), this);
    enterShortcut->setContext(Qt::WindowShortcut);
    connect(enterShortcut, &QShortcut::activated, this, [this]() {
        if (m_stacked->currentWidget() == m_exercisePage && m_btnSubmitEnabled) {
            submitCurrentAnswer();
        }
    });

    auto* numpadEnter = new QShortcut(QKeySequence(Qt::Key_Enter), this);
    numpadEnter->setContext(Qt::WindowShortcut);
    connect(numpadEnter, &QShortcut::activated, this, [this]() {
        if (m_stacked->currentWidget() == m_exercisePage && m_btnSubmitEnabled) {
            submitCurrentAnswer();
        }
    });
}

void MainWindow::applyDesign() {
    setStyleSheet(R"(
        QMainWindow { background-color: #f5f5f5; }
        QPushButton {
            background-color: #4CAF50; color: white; border: none;
            padding: 12px 24px; border-radius: 8px; font-size: 14px;
            font-weight: bold;
        }
        QPushButton:hover { background-color: #45a049; }
        QPushButton:pressed { background-color: #3d8b40; }
        QPushButton:disabled { background-color: #bdbdbd; }
        QProgressBar { 
            border: 2px solid #ddd; border-radius: 8px; 
            text-align: center; height: 25px; background-color: white;
        }
        QProgressBar::chunk { 
            background-color: qlineargradient(x1:0, y1:0, x2:1, y2:0, stop:0 #2196F3, stop:1 #4CAF50); 
            border-radius: 6px; 
        }
        QLineEdit, QGroupBox { 
            background-color: white; border: 2px solid #ddd; 
            border-radius: 8px; padding: 10px; font-size: 13px;
        }
        QLineEdit:focus { border: 2px solid #2196F3; }
        QComboBox { padding: 8px; border: 2px solid #ddd; border-radius: 6px; font-size: 13px; background-color: white; }
        QLabel { color: #333; }
        QRadioButton { spacing: 10px; font-size: 13px; padding: 5px; }
        QRadioButton::indicator { width: 20px; height: 20px; }
    )");
}