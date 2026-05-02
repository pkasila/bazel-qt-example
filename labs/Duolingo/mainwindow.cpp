#include "mainwindow.h"
#include <QMessageBox>
#include <QApplication>
#include <QMenu>
#include <QFrame>
#include <QRandomGenerator>

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), score(0), errors(0), timeLeft(0), currentStep(0), difficultyLevel(1) {
    setupUI();
    applyStyles();
    sessionTimer = new QTimer(this);
    connect(sessionTimer, &QTimer::timeout, this, &MainWindow::updateTimer);
}

void MainWindow::setupUI() {
    setWindowTitle("Duolingo Language Practice");
    setMinimumSize(700, 640);

    menuBar = new QMenuBar(this);
    setMenuBar(menuBar);
    QMenu *settingsMenu = menuBar->addMenu("Settings");
    difficultyAction = settingsMenu->addAction("Difficulty: Easy");
    connect(difficultyAction, &QAction::triggered, this, &MainWindow::showDifficultyDialog);

    QWidget *centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);
    QVBoxLayout *mainLayout = new QVBoxLayout(centralWidget);
    mainLayout->setContentsMargins(20, 20, 20, 20);
    mainLayout->setSpacing(18);

    QHBoxLayout *headerLayout = new QHBoxLayout();
    QLabel *titleLabel = new QLabel("Language Practice", this);
    titleLabel->setObjectName("appTitle");
    headerLayout->addWidget(titleLabel);
    headerLayout->addStretch();
    scoreLabel = new QLabel("Score: 0", this);
    timerLabel = new QLabel("Time: 00:00", this);
    errorLabel = new QLabel("Errors: 0/3", this);
    headerLayout->addWidget(scoreLabel);
    headerLayout->addSpacing(14);
    headerLayout->addWidget(timerLabel);
    headerLayout->addSpacing(14);
    headerLayout->addWidget(errorLabel);
    mainLayout->addLayout(headerLayout);

    progressBar = new QProgressBar(this);
    progressBar->setVisible(false);
    progressBar->setTextVisible(true);
    mainLayout->addWidget(progressBar);

    QFrame *modeFrame = new QFrame(this);
    modeFrame->setObjectName("modeFrame");
    modeFrame->setFrameShape(QFrame::NoFrame);
    QHBoxLayout *modeLayout = new QHBoxLayout(modeFrame);
    modeLayout->setSpacing(16);
    QPushButton *btnTrans = new QPushButton("Translation", this);
    QPushButton *btnGrammar = new QPushButton("Grammar", this);
    btnTrans->setObjectName("modeButton");
    btnGrammar->setObjectName("modeButton");
    modeLayout->addWidget(btnTrans);
    modeLayout->addWidget(btnGrammar);
    mainLayout->addWidget(modeFrame);

    exerciseStack = new QStackedWidget(this);
    exerciseStack->setObjectName("exerciseStack");
    mainLayout->addWidget(exerciseStack, 1);

    QWidget *transPage = new QWidget(this);
    QVBoxLayout *transLayout = new QVBoxLayout(transPage);
    transLayout->setContentsMargins(20, 20, 20, 20);
    transLayout->setSpacing(16);
    translationLabel = new QLabel("Press Translation to begin.", this);
    translationLabel->setObjectName("exerciseTitle");
    translationInput = new QTextEdit(this);
    translationInput->setPlaceholderText("Enter the translation here...");
    translationInput->setFixedHeight(140);
    transLayout->addWidget(translationLabel);
    transLayout->addWidget(translationInput);
    exerciseStack->addWidget(transPage);

    QWidget *gramPage = new QWidget(this);
    QVBoxLayout *gramLayout = new QVBoxLayout(gramPage);
    gramLayout->setContentsMargins(20, 20, 20, 20);
    gramLayout->setSpacing(16);
    grammarLabel = new QLabel("Press Grammar to begin.", this);
    grammarLabel->setObjectName("exerciseTitle");
    grammarOptionsWidget = new QWidget(this);
    grammarOptionsLayout = new QVBoxLayout(grammarOptionsWidget);
    grammarOptionsLayout->setSpacing(10);
    grammarGroup = new QButtonGroup(grammarOptionsWidget);
    grammarGroup->setExclusive(true);
    gramLayout->addWidget(grammarLabel);
    gramLayout->addWidget(grammarOptionsWidget);
    exerciseStack->addWidget(gramPage);

    QPushButton *submitBtn = new QPushButton("Submit", this);
    submitBtn->setObjectName("submitButton");
    connect(submitBtn, &QPushButton::clicked, this, &MainWindow::checkAnswer);
    mainLayout->addWidget(submitBtn);

    connect(btnTrans, &QPushButton::clicked, this, &MainWindow::startTranslationSession);
    connect(btnGrammar, &QPushButton::clicked, this, &MainWindow::startGrammarSession);
}

void MainWindow::applyStyles() {
    setStyleSheet(
        "QMainWindow { background: qlineargradient(x1:0, y1:0, x2:0, y2:1, stop:0 #ffffff, stop:1 #f0f0f0); }"
        "#appTitle { font-size: 28px; font-weight: 800; color: #58cc02; text-shadow: 1px 1px 2px rgba(0,0,0,0.1); }"
        "QLabel { color: #3c3c3c; font-family: 'Segoe UI', sans-serif; }"
        "QPushButton { background-color: #58cc02; color: white; border: none; border-radius: 20px; padding: 14px 24px; font-size: 16px; font-weight: 700; min-width: 120px; }"
        "QPushButton#submitButton { background-color: #1cb0f6; }"
        "QPushButton:hover { background-color: #4caf50; transform: scale(1.05); }"
        "QPushButton#submitButton:hover { background-color: #0a84ff; }"
        "QPushButton:pressed { background-color: #388e3c; }"
        "QPushButton#submitButton:pressed { background-color: #0056cc; }"
        "#modeButton { min-width: 160px; font-size: 18px; }"
        "QProgressBar { background: #e0e0e0; border: none; border-radius: 15px; min-height: 20px; text-align: center; color: white; }"
        "QProgressBar::chunk { background: qlineargradient(x1:0, y1:0, x2:0, y2:1, stop:0 #58cc02, stop:1 #4caf50); border-radius: 15px; }"
        "QTextEdit { background: #ffffff; color: #3c3c3c; border: 2px solid #d0d0d0; border-radius: 16px; padding: 14px; font-size: 16px; selection-background-color: #58cc02; }"
        "QTextEdit:focus { border-color: #58cc02; }"
        "QRadioButton { font-size: 16px; color: #3c3c3c; spacing: 10px; }"
        "QRadioButton::indicator { width: 18px; height: 18px; border-radius: 9px; border: 3px solid #58cc02; background: white; }"
        "QRadioButton::indicator:checked { background: #58cc02; border-color: #58cc02; }"
        "QRadioButton::indicator:hover { border-color: #4caf50; }"
        "QFrame#modeFrame { background: #ffffff; border: 2px solid #e0e0e0; border-radius: 20px; padding: 16px; }"
        "QStackedWidget#exerciseStack { background: #ffffff; border: 2px solid #e0e0e0; border-radius: 20px; }"
        "QLabel#exerciseTitle { font-size: 20px; font-weight: 700; color: #58cc02; }"
        "QMenuBar { background: #ffffff; color: #3c3c3c; border-bottom: 1px solid #e0e0e0; }"
        "QMenu { background: #ffffff; color: #3c3c3c; border: 1px solid #d0d0d0; border-radius: 8px; }"
        "QMenu::item { padding: 8px 16px; border-radius: 4px; }"
        "QMenu::item:selected { background: #58cc02; color: white; }"
        "QMessageBox { background: #ffffff; color: #3c3c3c; border-radius: 16px; }"
        "QDialog { background: #ffffff; color: #3c3c3c; border-radius: 16px; }"
    );
}

QList<Exercise> MainWindow::buildTranslationExercises() const {
    QList<Exercise> exercises;
    if (difficultyLevel == 1) {
        exercises = {
            {Exercise::Translation, "Hello", "Привет", {}, "A simple greeting."},
            {Exercise::Translation, "Thank you", "Спасибо", {}, "A common polite phrase."},
            {Exercise::Translation, "Friend", "Друг", {}, "A person you trust."}
        };
    } else if (difficultyLevel == 2) {
        exercises = {
            {Exercise::Translation, "Beautiful", "Красивый", {}, "Used to describe something pleasing."},
            {Exercise::Translation, "Library", "Библиотека", {}, "A place with many books."},
            {Exercise::Translation, "Adventure", "Приключение", {}, "An exciting experience."},
            {Exercise::Translation, "Weather", "Погода", {}, "What you check before going outside."}
        };
    } else {
        exercises = {
            {Exercise::Translation, "Opportunity", "Возможность", {}, "A chance to do something."},
            {Exercise::Translation, "Environment", "Окружающая среда", {}, "The world around us."},
            {Exercise::Translation, "Consequence", "Последствие", {}, "What happens after an action."},
            {Exercise::Translation, "Achievement", "Достижение", {}, "A success after effort."},
            {Exercise::Translation, "Curiosity", "Любознательность", {}, "A desire to learn more."}
        };
    }
    return exercises;
}

QList<Exercise> MainWindow::buildGrammarExercises() const {
    QList<Exercise> exercises;
    if (difficultyLevel == 1) {
        exercises = {
            {Exercise::Grammar, "He ___ a student.", "is", {"am", "is", "are"}, "Use 'is' for third person singular."},
            {Exercise::Grammar, "They ___ happy.", "are", {"am", "is", "are"}, "Use 'are' for plural subjects."},
            {Exercise::Grammar, "I ___ a teacher.", "am", {"am", "is", "are"}, "Use 'am' with I."}
        };
    } else if (difficultyLevel == 2) {
        exercises = {
            {Exercise::Grammar, "She ___ going to school.", "is", {"is", "was", "are"}, "Present continuous for third person."},
            {Exercise::Grammar, "We ___ finished our homework.", "have", {"has", "have", "had"}, "Present perfect for plural subjects."},
            {Exercise::Grammar, "It ___ raining now.", "is", {"are", "is", "was"}, "Use 'is' with it."},
            {Exercise::Grammar, "They ___ tennis yesterday.", "played", {"play", "played", "playing"}, "Use past simple for finished actions."}
        };
    } else {
        exercises = {
            {Exercise::Grammar, "If he ___ earlier, he would have caught the train.", "had left", {"left", "had left", "has left"}, "Third conditional uses 'had' + past participle."},
            {Exercise::Grammar, "By tomorrow she ___ the report.", "will have finished", {"will finish", "will have finished", "finished"}, "Future perfect shows completion."},
            {Exercise::Grammar, "I wish I ___ more time.", "had", {"have", "had", "will have"}, "Use wish with past tense for regrets."},
            {Exercise::Grammar, "He suggested that she ___ earlier.", "arrive", {"arrive", "arrives", "arrived"}, "After suggestion use base verb."}
        };
    }
    return exercises;
}

QString MainWindow::difficultyName() const {
    if (difficultyLevel == 1) return "Easy";
    if (difficultyLevel == 2) return "Medium";
    return "Hard";
}

void MainWindow::startTranslationSession() {
    currentSession = buildTranslationExercises();
    currentSession = currentSession.mid(0, qMin(currentSession.size(), 3 + difficultyLevel));
    currentStep = 0;
    errors = 0;
    timeLeft = TIME_LIMIT_BASE + difficultyLevel * 20;
    progressBar->setMaximum(currentSession.size());
    progressBar->setVisible(true);
    progressBar->setValue(0);
    sessionTimer->start(1000);
    errorLabel->setText("Errors: 0/3");
    loadNextQuestion();
}

void MainWindow::startGrammarSession() {
    currentSession = buildGrammarExercises();
    currentSession = currentSession.mid(0, qMin(currentSession.size(), 3 + difficultyLevel));
    currentStep = 0;
    errors = 0;
    timeLeft = TIME_LIMIT_BASE + difficultyLevel * 20;
    progressBar->setMaximum(currentSession.size());
    progressBar->setVisible(true);
    progressBar->setValue(0);
    sessionTimer->start(1000);
    errorLabel->setText("Errors: 0/3");
    loadNextQuestion();
}

void MainWindow::updateTimer() {
    timeLeft--;
    timerLabel->setText(QString("Time: %1:%2").arg(timeLeft / 60, 2, 10, QChar('0')).arg(timeLeft % 60, 2, 10, QChar('0')));
    if (timeLeft <= 0) {
        endSession(false, "Time is out!");
    }
}

void MainWindow::loadNextQuestion() {
    if (currentStep >= currentSession.size()) {
        score += 100 * difficultyLevel;
        scoreLabel->setText(QString("Score: %1").arg(score));
        endSession(true, "All tasks completed! +" + QString::number(100 * difficultyLevel) + " points.");
        return;
    }
    progressBar->setValue(currentStep);
    Exercise ex = currentSession[currentStep];
    if (ex.type == Exercise::Translation) {
        exerciseStack->setCurrentIndex(0);
        translationLabel->setText(QString("Translate to Russian: %1").arg(ex.question));
        translationInput->clear();
    } else {
        exerciseStack->setCurrentIndex(1);
        grammarLabel->setText(QString("Choose the correct form: %1").arg(ex.question));
        QList<QAbstractButton*> buttons = grammarGroup->buttons();
        for (auto btn : buttons) {
            grammarOptionsLayout->removeWidget(btn);
            grammarGroup->removeButton(btn);
            delete btn;
        }
        QList<QString> options = ex.options;
        if (!options.contains(ex.answer)) {
            options.append(ex.answer);
        }
        std::shuffle(options.begin(), options.end(), *QRandomGenerator::global());
        for (const QString &opt : options) {
            QRadioButton *rb = new QRadioButton(opt, grammarOptionsWidget);
            grammarOptionsLayout->addWidget(rb);
            grammarGroup->addButton(rb);
        }
    }
}

void MainWindow::checkAnswer() {
    if (currentSession.isEmpty() || currentStep >= currentSession.size()) return;
    Exercise ex = currentSession[currentStep];
    bool correct = false;
    if (ex.type == Exercise::Translation) {
        QString answer = translationInput->toPlainText().trimmed();
        if (answer.isEmpty()) {
            QMessageBox::warning(this, "Empty answer", "Please enter a translation before submitting.");
            return;
        }
        correct = (answer.toLower() == ex.answer.toLower());
    } else {
        QAbstractButton *selected = grammarGroup->checkedButton();
        if (!selected) {
            QMessageBox::warning(this, "Choose answer", "Please select one option before submitting.");
            return;
        }
        correct = (selected->text() == ex.answer);
    }

    if (correct) {
        currentStep++;
        progressBar->setValue(currentStep);
        if (currentStep < currentSession.size()) {
            QMessageBox::information(this, "Correct", "Good job! Next question.");
        }
        loadNextQuestion();
    } else {
        errors++;
        errorLabel->setText(QString("Errors: %1/3").arg(errors));
        if (errors >= MAX_ERRORS) {
            endSession(false, "Too many errors!");
            return;
        }
        QMessageBox::warning(this, "Incorrect", "That answer is incorrect. Try again.");
    }
}

void MainWindow::endSession(bool success, QString message) {
    sessionTimer->stop();
    QMessageBox::information(this, success ? "Success" : "Failed", message);
    progressBar->setVisible(false);
    currentSession.clear();
    currentStep = 0;
    translationLabel->setText("Press Translation to begin.");
    translationInput->clear();
    grammarLabel->setText("Press Grammar to begin.");
    QList<QAbstractButton*> buttons = grammarGroup->buttons();
    for (auto btn : buttons) {
        grammarOptionsLayout->removeWidget(btn);
        grammarGroup->removeButton(btn);
        delete btn;
    }
}

void MainWindow::keyPressEvent(QKeyEvent *event) {
    if (event->key() == Qt::Key_H) {
        if (!currentSession.isEmpty() && currentStep < currentSession.size()) {
            QMessageBox::information(this, "Hint", currentSession[currentStep].hint);
            return;
        }
    }
    QMainWindow::keyPressEvent(event);
}

void MainWindow::showDifficultyDialog() {
    QDialog dialog(this);
    dialog.setWindowTitle("Select Difficulty");
    QVBoxLayout *layout = new QVBoxLayout(&dialog);
    QLabel *label = new QLabel("Choose difficulty level:", &dialog);
    layout->addWidget(label);
    QComboBox *combo = new QComboBox(&dialog);
    combo->addItem("Easy", 1);
    combo->addItem("Medium", 2);
    combo->addItem("Hard", 3);
    combo->setCurrentIndex(difficultyLevel - 1);
    layout->addWidget(combo);
    QDialogButtonBox *buttons = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, &dialog);
    layout->addWidget(buttons);
    connect(buttons, &QDialogButtonBox::accepted, &dialog, &QDialog::accept);
    connect(buttons, &QDialogButtonBox::rejected, &dialog, &QDialog::reject);
    if (dialog.exec() == QDialog::Accepted) {
        setDifficulty(combo->currentData().toInt());
    }
}

void MainWindow::setDifficulty(int level) {
    difficultyLevel = level;
    difficultyAction->setText(QString("Difficulty: %1").arg(difficultyName()));
}
