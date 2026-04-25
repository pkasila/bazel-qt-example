#include "mainwindow.h"

#include "answerchecker.h"
#include "appstyle.h"
#include "difficultydialog.h"
#include "questionrepository.h"

#include <QAbstractButton>
#include <QAction>
#include <QApplication>
#include <QHBoxLayout>
#include <QEvent>
#include <QKeyEvent>
#include <QKeySequence>
#include <QMenu>
#include <QMenuBar>
#include <QMessageBox>
#include <QRadioButton>
#include <QStyle>

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent), timer(new QTimer(this)), audioPlayer(new AudioPlayer(this)), grammarGroup(new QButtonGroup(this)) {
    setWindowTitle("LinguaSprint");
    resize(1120, 700);
    setMinimumSize(900, 560);
    setStyleSheet(AppStyle::styleSheet());

    createMenuBar();
    createInterface();

    connect(timer, &QTimer::timeout, this, &MainWindow::tickTimer);

    qApp->installEventFilter(this);

    audioPlayer->load();
    updateSidebar();
}

bool MainWindow::eventFilter(QObject* watched, QEvent* event) {
    Q_UNUSED(watched);

    if (QApplication::activeModalWidget() != nullptr || !isActiveWindow()) {
        return QMainWindow::eventFilter(watched, event);
    }

    if (event->type() == QEvent::KeyPress) {
        auto* keyEvent = static_cast<QKeyEvent*>(event);
        const bool helpKeyPressed = keyEvent->key() == Qt::Key_H
            && keyEvent->modifiers() == Qt::NoModifier
            && !keyEvent->isAutoRepeat();

        if (helpKeyPressed) {
            showHint();
            event->accept();
            return true;
        }
    }

    return QMainWindow::eventFilter(watched, event);
}

void MainWindow::createMenuBar() {
    QMenu* settingsMenu = menuBar()->addMenu("Settings");

    QAction* difficultyAction = settingsMenu->addAction("Change difficulty");
    connect(difficultyAction, &QAction::triggered, this, &MainWindow::changeDifficulty);

    QAction* audioAction = settingsMenu->addAction("Audio status");
    connect(audioAction, &QAction::triggered, this, &MainWindow::showAudioStatus);

    QMenu* helpMenu = menuBar()->addMenu("Help");
    QAction* hintAction = helpMenu->addAction("Hint (H)");
    hintAction->setShortcut(QKeySequence(Qt::Key_H));
    connect(hintAction, &QAction::triggered, this, &MainWindow::showHint);
}

void MainWindow::createInterface() {
    auto* central = new QWidget(this);
    central->setObjectName("AppShell");

    auto* rootLayout = new QHBoxLayout(central);
    rootLayout->setContentsMargins(18, 18, 18, 18);
    rootLayout->setSpacing(18);

    sidebar = new QFrame(central);
    sidebar->setObjectName("SideBar");
    sidebar->setMinimumWidth(285);
    sidebar->setMaximumWidth(360);

    auto* sideLayout = new QVBoxLayout(sidebar);
    sideLayout->setContentsMargins(24, 24, 24, 24);
    sideLayout->setSpacing(16);

    auto* brandRow = new QHBoxLayout();
    brandRow->setSpacing(12);

    auto* logo = new QLabel("L", sidebar);
    logo->setObjectName("LogoMark");
    logo->setAlignment(Qt::AlignCenter);
    logo->setFixedSize(48, 48);

    auto* brandText = new QVBoxLayout();
    brandText->setSpacing(1);
    auto* appTitle = new QLabel("LinguaSprint", sidebar);
    appTitle->setObjectName("AppTitle");
    auto* appTagline = new QLabel("Daily language workout", sidebar);
    appTagline->setObjectName("MutedText");

    brandText->addWidget(appTitle);
    brandText->addWidget(appTagline);
    brandRow->addWidget(logo);
    brandRow->addLayout(brandText, 1);

    auto* scoreCard = new QFrame(sidebar);
    scoreCard->setObjectName("SideCard");
    auto* scoreLayout = new QVBoxLayout(scoreCard);
    scoreLayout->setContentsMargins(18, 16, 18, 16);
    scoreLayout->setSpacing(7);

    auto* scoreCaption = new QLabel("Your progress", scoreCard);
    scoreCaption->setObjectName("SectionCaption");

    scoreLabel = new QLabel(scoreCard);
    scoreLabel->setObjectName("ScoreLabel");

    difficultyLabel = new QLabel(scoreCard);
    difficultyLabel->setObjectName("MutedText");

    soundLabel = new QLabel(scoreCard);
    soundLabel->setObjectName("MutedText");

    scoreLayout->addWidget(scoreCaption);
    scoreLayout->addWidget(scoreLabel);
    scoreLayout->addWidget(difficultyLabel);
    scoreLayout->addWidget(soundLabel);

    auto* menuCaption = new QLabel("Exercises", sidebar);
    menuCaption->setObjectName("SectionCaption");

    auto* translationButton = new QPushButton("Translation", sidebar);
    translationButton->setObjectName("PrimaryButton");
    auto* grammarButton = new QPushButton("Grammar", sidebar);
    grammarButton->setObjectName("BlueButton");
    auto* difficultyButton = new QPushButton("Difficulty", sidebar);
    difficultyButton->setObjectName("SecondaryButton");

    connect(translationButton, &QPushButton::clicked, this, &MainWindow::startTranslation);
    connect(grammarButton, &QPushButton::clicked, this, &MainWindow::startGrammar);
    connect(difficultyButton, &QPushButton::clicked, this, &MainWindow::changeDifficulty);

    auto* hintCard = new QFrame(sidebar);
    hintCard->setObjectName("HintCard");
    auto* hintLayout = new QVBoxLayout(hintCard);
    hintLayout->setContentsMargins(16, 14, 16, 14);
    hintLayout->setSpacing(5);

    auto* hintTitle = new QLabel("Need help?", hintCard);
    hintTitle->setObjectName("HintTitle");
    auto* helpText = new QLabel("Press H during an exercise to open a task-specific hint.", hintCard);
    helpText->setObjectName("MutedText");
    helpText->setWordWrap(true);

    hintLayout->addWidget(hintTitle);
    hintLayout->addWidget(helpText);

    sideLayout->addLayout(brandRow);
    sideLayout->addSpacing(8);
    sideLayout->addWidget(scoreCard);
    sideLayout->addSpacing(8);
    sideLayout->addWidget(menuCaption);
    sideLayout->addWidget(translationButton);
    sideLayout->addWidget(grammarButton);
    sideLayout->addWidget(difficultyButton);
    sideLayout->addStretch();
    sideLayout->addWidget(hintCard);

    contentStack = new QStackedWidget(central);
    contentStack->setObjectName("ContentStack");

    welcomePage = new QWidget(contentStack);
    auto* welcomeLayout = new QVBoxLayout(welcomePage);
    welcomeLayout->setContentsMargins(34, 34, 34, 34);
    welcomeLayout->setSpacing(18);

    auto* welcomeCard = new QFrame(welcomePage);
    welcomeCard->setObjectName("HeroCard");
    auto* welcomeCardLayout = new QVBoxLayout(welcomeCard);
    welcomeCardLayout->setContentsMargins(44, 44, 44, 44);
    welcomeCardLayout->setSpacing(18);

    auto* pill = new QLabel("Qt Language Learning Lab", welcomeCard);
    pill->setObjectName("PillLabel");
    pill->setAlignment(Qt::AlignCenter);
    pill->setFixedWidth(210);

    auto* welcomeTitle = new QLabel("Train translation and grammar in one clean workspace", welcomeCard);
    welcomeTitle->setObjectName("HeroTitle");
    welcomeTitle->setWordWrap(true);

    auto* welcomeBody = new QLabel("Choose an exercise from the sidebar. The right panel updates dynamically, tracks progress, limits attempts, runs a timer, and keeps hints one key away.", welcomeCard);
    welcomeBody->setWordWrap(true);
    welcomeBody->setObjectName("HeroBody");

    auto* featureRow = new QHBoxLayout();
    featureRow->setSpacing(14);

    auto* featureOne = new QFrame(welcomeCard);
    featureOne->setObjectName("FeatureCard");
    auto* featureOneLayout = new QVBoxLayout(featureOne);
    featureOneLayout->setContentsMargins(18, 18, 18, 18);
    auto* featureOneTitle = new QLabel("Focused practice", featureOne);
    featureOneTitle->setObjectName("FeatureTitle");
    auto* featureOneText = new QLabel("Progress bar, timer, score, and attempt control stay visible while you work.", featureOne);
    featureOneText->setObjectName("MutedText");
    featureOneText->setWordWrap(true);
    featureOneLayout->addWidget(featureOneTitle);
    featureOneLayout->addWidget(featureOneText);

    auto* featureTwo = new QFrame(welcomeCard);
    featureTwo->setObjectName("FeatureCard");
    auto* featureTwoLayout = new QVBoxLayout(featureTwo);
    featureTwoLayout->setContentsMargins(18, 18, 18, 18);
    auto* featureTwoTitle = new QLabel("Instant feedback", featureTwo);
    featureTwoTitle->setObjectName("FeatureTitle");
    auto* featureTwoText = new QLabel("Correct and wrong answers are shown in the interface and supported by sound.", featureTwo);
    featureTwoText->setObjectName("MutedText");
    featureTwoText->setWordWrap(true);
    featureTwoLayout->addWidget(featureTwoTitle);
    featureTwoLayout->addWidget(featureTwoText);

    featureRow->addWidget(featureOne);
    featureRow->addWidget(featureTwo);

    welcomeCardLayout->addWidget(pill);
    welcomeCardLayout->addWidget(welcomeTitle);
    welcomeCardLayout->addWidget(welcomeBody);
    welcomeCardLayout->addSpacing(12);
    welcomeCardLayout->addLayout(featureRow);
    welcomeCardLayout->addStretch();
    welcomeLayout->addWidget(welcomeCard);

    exercisePage = new QWidget(contentStack);
    auto* exerciseLayout = new QVBoxLayout(exercisePage);
    exerciseLayout->setContentsMargins(34, 34, 34, 34);
    exerciseLayout->setSpacing(18);

    auto* topCard = new QFrame(exercisePage);
    topCard->setObjectName("TopCard");
    auto* topCardLayout = new QVBoxLayout(topCard);
    topCardLayout->setContentsMargins(24, 20, 24, 20);
    topCardLayout->setSpacing(12);

    auto* topLayout = new QHBoxLayout();
    topLayout->setSpacing(12);

    exerciseTitleLabel = new QLabel(topCard);
    exerciseTitleLabel->setObjectName("ExerciseTitle");

    timerLabel = new QLabel(topCard);
    timerLabel->setObjectName("TimerBadge");
    timerLabel->setAlignment(Qt::AlignCenter);
    timerLabel->setMinimumWidth(108);

    attemptsLabel = new QLabel(topCard);
    attemptsLabel->setObjectName("AttemptsBadge");
    attemptsLabel->setAlignment(Qt::AlignCenter);
    attemptsLabel->setMinimumWidth(118);

    topLayout->addWidget(exerciseTitleLabel, 1);
    topLayout->addWidget(timerLabel);
    topLayout->addWidget(attemptsLabel);

    statusLabel = new QLabel(topCard);
    statusLabel->setObjectName("MutedText");

    progressBar = new QProgressBar(topCard);
    progressBar->setTextVisible(true);
    progressBar->setFormat("%v / %m completed");

    topCardLayout->addLayout(topLayout);
    topCardLayout->addWidget(statusLabel);
    topCardLayout->addWidget(progressBar);

    auto* questionCard = new QFrame(exercisePage);
    questionCard->setObjectName("Card");
    auto* questionLayout = new QVBoxLayout(questionCard);
    questionLayout->setContentsMargins(34, 34, 34, 34);
    questionLayout->setSpacing(20);

    auto* taskCaption = new QLabel("Current task", questionCard);
    taskCaption->setObjectName("SectionCaption");
    taskCaption->setAlignment(Qt::AlignCenter);

    questionLabel = new QLabel(questionCard);
    questionLabel->setObjectName("QuestionLabel");
    questionLabel->setAlignment(Qt::AlignCenter);
    questionLabel->setWordWrap(true);
    questionLabel->setMinimumHeight(86);

    inputStack = new QStackedWidget(questionCard);
    inputStack->setObjectName("InputStack");

    translationInput = new QLineEdit(inputStack);
    translationInput->setPlaceholderText("Type your answer here...");
    connect(translationInput, &QLineEdit::returnPressed, this, &MainWindow::submitAnswer);

    grammarWidget = new QWidget(inputStack);
    grammarLayout = new QVBoxLayout(grammarWidget);
    grammarLayout->setContentsMargins(0, 0, 0, 0);
    grammarLayout->setSpacing(10);

    inputStack->addWidget(translationInput);
    inputStack->addWidget(grammarWidget);

    feedbackLabel = new QLabel(questionCard);
    feedbackLabel->setWordWrap(true);
    feedbackLabel->setAlignment(Qt::AlignCenter);
    feedbackLabel->hide();

    questionLayout->addWidget(taskCaption);
    questionLayout->addWidget(questionLabel);
    questionLayout->addWidget(inputStack);
    questionLayout->addWidget(feedbackLabel);
    questionLayout->addStretch();

    auto* buttonLayout = new QHBoxLayout();
    auto* backButton = new QPushButton("Back to menu", exercisePage);
    backButton->setObjectName("SecondaryButton");
    submitButton = new QPushButton("Submit answer", exercisePage);
    submitButton->setObjectName("PrimaryButton");

    connect(backButton, &QPushButton::clicked, this, &MainWindow::returnToWelcome);
    connect(submitButton, &QPushButton::clicked, this, &MainWindow::submitAnswer);

    buttonLayout->addWidget(backButton);
    buttonLayout->addStretch();
    buttonLayout->addWidget(submitButton);

    exerciseLayout->addWidget(topCard);
    exerciseLayout->addWidget(questionCard, 1);
    exerciseLayout->addLayout(buttonLayout);

    contentStack->addWidget(welcomePage);
    contentStack->addWidget(exercisePage);

    rootLayout->addWidget(sidebar);
    rootLayout->addWidget(contentStack, 1);
    setCentralWidget(central);
}

void MainWindow::startTranslation() {
    loadExercise(ExerciseType::Translation);
}

void MainWindow::startGrammar() {
    loadExercise(ExerciseType::Grammar);
}

void MainWindow::loadExercise(ExerciseType type) {
    currentType = type;
    currentQuestions = QuestionRepository::questionsFor(type);
    currentQuestionIndex = 0;
    remainingAttempts = maxAttemptsForDifficulty();
    timeLeft = type == ExerciseType::Translation ? 75 : 60;

    progressBar->setMaximum(currentQuestions.size());
    progressBar->setValue(0);
    exerciseTitleLabel->setText(QuestionRepository::titleFor(type));
    timerLabel->setText(QString("%1 s").arg(timeLeft));
    attemptsLabel->setText(QString("%1 attempts").arg(remainingAttempts));
    setFeedback(QString(), true);

    contentStack->setCurrentWidget(exercisePage);
    showNextQuestion();
    timer->start(1000);
}

void MainWindow::showNextQuestion() {
    if (currentQuestionIndex >= currentQuestions.size()) {
        finishExercise(true, "Exercise complete. Score increased by 100 points.");
        return;
    }

    const Question& question = currentQuestions[currentQuestionIndex];
    progressBar->setValue(currentQuestionIndex);
    statusLabel->setText(QString("Question %1 of %2").arg(currentQuestionIndex + 1).arg(currentQuestions.size()));
    attemptsLabel->setText(QString("%1 attempts").arg(remainingAttempts));
    questionLabel->setText(question.text);
    setFeedback(QString(), true);

    if (currentType == ExerciseType::Translation) {
        inputStack->setCurrentWidget(translationInput);
        translationInput->clear();
        translationInput->setFocus();
    } else {
        inputStack->setCurrentWidget(grammarWidget);
        setGrammarOptions(question.options);
    }
}

void MainWindow::clearGrammarOptions() {
    const auto buttons = grammarGroup->buttons();
    for (QAbstractButton* button : buttons) {
        grammarGroup->removeButton(button);
    }

    while (QLayoutItem* item = grammarLayout->takeAt(0)) {
        if (QWidget* widget = item->widget()) {
            widget->deleteLater();
        }
        delete item;
    }
}

void MainWindow::setGrammarOptions(const QStringList& options) {
    clearGrammarOptions();

    for (int i = 0; i < options.size(); ++i) {
        auto* radio = new QRadioButton(options[i], grammarWidget);
        grammarGroup->addButton(radio, i);
        grammarLayout->addWidget(radio);
        if (i == 0) {
            radio->setChecked(true);
        }
    }
    grammarLayout->addStretch();
}

void MainWindow::submitAnswer() {
    if (contentStack->currentWidget() != exercisePage || currentQuestionIndex >= currentQuestions.size()) {
        return;
    }

    const Question& question = currentQuestions[currentQuestionIndex];
    bool isCorrect = false;

    if (currentType == ExerciseType::Translation) {
        isCorrect = AnswerChecker::isTranslationCorrect(translationInput->text(), question.correctAnswer);
    } else if (const QAbstractButton* checked = grammarGroup->checkedButton()) {
        isCorrect = checked->text() == question.correctAnswer;
    }

    if (isCorrect) {
        audioPlayer->playCorrect();
        setFeedback("Correct. Moving to the next task.", true);
        ++currentQuestionIndex;
        progressBar->setValue(currentQuestionIndex);
        if (currentQuestionIndex >= currentQuestions.size()) {
            finishExercise(true, "Exercise complete. Score increased by 100 points.");
        } else {
            showNextQuestion();
        }
        return;
    }

    audioPlayer->playWrong();
    --remainingAttempts;
    attemptsLabel->setText(QString("%1 attempts").arg(remainingAttempts));

    if (remainingAttempts <= 0) {
        finishExercise(false, "No attempts left. Try again from the menu.");
    } else {
        setFeedback("Not quite. Try again or press H for a hint.", false);
    }
}

void MainWindow::tickTimer() {
    --timeLeft;
    timerLabel->setText(QString("%1 s").arg(timeLeft));

    if (timeLeft <= 0) {
        finishExercise(false, "Time is over. Try again from the menu.");
    }
}

void MainWindow::finishExercise(bool success, const QString& message) {
    timer->stop();

    if (success) {
        score += 100;
        updateSidebar();
        QMessageBox::information(this, "Success", message);
    } else {
        QMessageBox::critical(this, "Exercise stopped", message);
    }

    contentStack->setCurrentWidget(welcomePage);
}

void MainWindow::changeDifficulty() {
    DifficultyDialog dialog(difficulty, this);
    if (dialog.exec() == QDialog::Accepted) {
        difficulty = dialog.selectedDifficulty();
        updateSidebar();
    }
}

void MainWindow::showHint() {
    if (contentStack->currentWidget() == exercisePage && currentQuestionIndex < currentQuestions.size()) {
        QMessageBox::information(this, "Hint", currentQuestions[currentQuestionIndex].hint);
    } else {
        QMessageBox::information(this, "Hint", "Start Translation or Grammar, then press H to see a task-specific hint.");
    }
}

void MainWindow::showAudioStatus() {
    QMessageBox::information(this, "Audio status", audioPlayer->statusText());
}

void MainWindow::returnToWelcome() {
    timer->stop();
    contentStack->setCurrentWidget(welcomePage);
}

void MainWindow::updateSidebar() {
    scoreLabel->setText(QString("%1 pts").arg(score));
    difficultyLabel->setText(QString("Difficulty: %1").arg(difficultyName()));
    soundLabel->setText(audioPlayer->isReady() ? "Sound: ready" : "Sound: not found");
}

void MainWindow::setFeedback(const QString& text, bool success) {
    if (text.isEmpty()) {
        feedbackLabel->hide();
        return;
    }

    feedbackLabel->setText(text);
    feedbackLabel->setObjectName(success ? "FeedbackSuccess" : "FeedbackError");
    feedbackLabel->style()->unpolish(feedbackLabel);
    feedbackLabel->style()->polish(feedbackLabel);
    feedbackLabel->show();
}

int MainWindow::maxAttemptsForDifficulty() const {
    return 3 - difficulty;
}

QString MainWindow::difficultyName() const {
    if (difficulty == 1) {
        return "Medium";
    }
    if (difficulty == 2) {
        return "Hard";
    }
    return "Easy";
}
