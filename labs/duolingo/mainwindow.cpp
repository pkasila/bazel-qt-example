#include "mainwindow.h"
#include <QApplication>
#include <QEvent>
#include <QKeyEvent>
#include <QMessageBox>
#include <QFrame>
#include <QInputDialog>
#include <QMenu>
#include <QAction>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
      settings("FPGI_BSU", "PolyglotMastery"),
      answerContainerWidget(nullptr), textInput(nullptr)
{
    controller = std::make_unique<ExerciseController>();
    audioPlayer = std::make_unique<AudioPlayer>();
    
    connect(controller.get(), &ExerciseController::questionStarted, this, &MainWindow::onQuestionStarted);
    connect(controller.get(), &ExerciseController::correctAnswer, this, &MainWindow::onCorrectAnswer);
    connect(controller.get(), &ExerciseController::wrongAnswer, this, &MainWindow::onWrongAnswer);
    connect(controller.get(), &ExerciseController::questionSkipped, this, &MainWindow::onQuestionSkipped);
    connect(controller.get(), &ExerciseController::exerciseFinished, this, &MainWindow::onExerciseFinished);
    connect(controller.get(), &ExerciseController::timeUpdated, this, &MainWindow::onTimeUpdated);
    connect(controller.get(), &ExerciseController::streakUpdated, this, &MainWindow::onStreakUpdated);

    isDarkMode = settings.value("isDarkMode", true).toBool();
    isMuted = settings.value("isAudioMuted", false).toBool();

    initUI();
    setupMenuBar();
    applyTheme();

    audioPlayer->setMuted(isMuted);
    refreshMainMenu(); // Set initial high score
    audioPlayer->playMenuMusic();
    
    qApp->installEventFilter(this);
}

MainWindow::~MainWindow() {
    qApp->removeEventFilter(this);
}

void MainWindow::initUI() {
    setWindowTitle("Polyglot Mastery");
    resize(1024, 768);

    stackedWidget = new QStackedWidget(this);
    setCentralWidget(stackedWidget);

    stackedWidget->addWidget(createMainMenu());
    stackedWidget->addWidget(createExerciseScreen());
    stackedWidget->addWidget(createSettingsScreen());
}

void MainWindow::setupMenuBar() {
    QMenu* gameMenu = menuBar()->addMenu("Game");
    
    QAction* diffAction = new QAction("Select Difficulty Dialog...", this);
    connect(diffAction, &QAction::triggered, this, &MainWindow::showDifficultyDialog);
    gameMenu->addAction(diffAction);
    
    QAction* settingsAction = new QAction("Settings", this);
    connect(settingsAction, &QAction::triggered, this, &MainWindow::onOpenSettingsClicked);
    gameMenu->addAction(settingsAction);
}

void MainWindow::showDifficultyDialog() {
    bool ok;
    QStringList items = {"1. Easy (45s, 5 Hearts)", "2. Medium (30s, 3 Hearts)", "3. Hard Mode (15s, 1 Heart)", "4. Custom Mode (Your Settings)"};
    int current = settings.value("difficulty", 1).toInt() - 1;
    
    QString item = QInputDialog::getItem(this, "Select Difficulty", 
                                         "Formal Requirement Dialog for Lab 3:", 
                                         items, current, false, &ok);
    if (ok && !item.isEmpty()) {
        int idx = items.indexOf(item);
        if (idx >= 0) {
            settings.setValue("difficulty", idx + 1);
            if (difficultyCombo) difficultyCombo->setCurrentIndex(idx);
        }
    }
}

void MainWindow::applyTheme() {
    QString bgColor, textColor, btnColor, btnHoverColor, btnDisabled, inputBg, inputBorder, progressBg, progressChunk, statFrame, highlightColor;

    if (isDarkMode) {
        bgColor = "#0F172A"; textColor = "#F8FAFC";
        btnColor = "#38BDF8"; btnHoverColor = "#0EA5E9"; btnDisabled = "#334155";
        inputBg = "#1E293B"; inputBorder = "#475569";
        progressBg = "#334155"; progressChunk = "#10B981";
        statFrame = "#1E293B"; highlightColor = "#94A3B8";
        if (themeBtn) themeBtn->setText("☀️ Light Mode");
    } else {
        bgColor = "#F8FAFC"; textColor = "#0F172A";
        btnColor = "#0284C7"; btnHoverColor = "#0369A1"; btnDisabled = "#CBD5E1";
        inputBg = "#FFFFFF"; inputBorder = "#CBD5E1"; 
        progressBg = "#E2E8F0"; progressChunk = "#059669";
        statFrame = "#FFFFFF"; highlightColor = "#64748B";
        if (themeBtn) themeBtn->setText("🌙 Dark Mode");
    }

    this->setStyleSheet(QString(R"(
        QMainWindow { background-color: %1; color: %2; }
        QLabel { font-family: -apple-system, 'Segoe UI', Arial, sans-serif; color: %2; }
        QPushButton {
            background-color: %3;
            color: %1;
            border: none;
            border-radius: 12px;
            padding: 14px 28px;
            font-size: 18px;
            font-weight: 800;
        }
        QPushButton:hover { background-color: %4; }
        QPushButton:disabled { background-color: %5; color: %11; }
        QComboBox {
            background-color: %6;
            color: %2;
            border: 2px solid %7;
            border-radius: 10px;
            padding: 10px;
            font-size: 18px;
        }
        QComboBox::drop-down { border: none; }
        QProgressBar {
            background-color: %8;
            border-radius: 8px;
            text-align: center;
            color: transparent;
            min-height: 16px;
            max-height: 16px;
        }
        QProgressBar::chunk {
            background-color: %9;
            border-radius: 8px;
        }
        QRadioButton::indicator {
            width: 24px;
            height: 24px;
            border-radius: 12px;
            border: 2px solid %7;
            background-color: %10;
        }
        QRadioButton::indicator:checked {
            background-color: %3;
            border: 2px solid %3;
        }
        QFrame#StatFrame {
            background-color: %10;
            border-radius: 12px;
        }
        QMessageBox {
            background-color: %1;
        }
        QMessageBox QLabel {
            color: %2;
        }
        QMenuBar {
            background-color: %10;
            color: %2;
        }
        QMenu {
            background-color: %10;
            color: %2;
        }
        QSlider::groove:horizontal {
            border: 1px solid %7;
            height: 8px;
            background: %6;
            margin: 2px 0;
            border-radius: 4px;
        }
        QSlider::handle:horizontal {
            background: %3;
            border: 1px solid %3;
            width: 18px;
            margin: -6px 0;
            border-radius: 9px;
        }
    )").arg(bgColor, textColor, btnColor, btnHoverColor, btnDisabled, inputBg, inputBorder, progressBg, progressChunk, statFrame, highlightColor));

    if (title) title->setStyleSheet(QString("font-size: 56px; font-weight: 900; color: %1;").arg(isDarkMode ? "#38BDF8" : "#0284C7"));
    if (subtitle) subtitle->setStyleSheet(QString("font-size: 20px; color: %1; margin-bottom: 20px;").arg(highlightColor));
    
    QString auxBtnStyle = QString("QPushButton { background-color: %1; color: %2; font-size: 14px; padding: 10px 20px; border: 2px solid %3; } QPushButton:hover { background-color: %3; }").arg(bgColor, textColor, btnColor);
    
    if (themeBtn) themeBtn->setStyleSheet(QString("QPushButton { background-color: %1; color: %2; font-size: 14px; padding: 10px 20px; }").arg(statFrame, textColor));
    if (muteBtn) muteBtn->setStyleSheet(QString("QPushButton { background-color: %1; color: %2; font-size: 14px; padding: 10px 20px; }").arg(statFrame, textColor));
    if (settingsBtn) settingsBtn->setStyleSheet(QString("QPushButton { background-color: %1; color: %2; font-size: 14px; padding: 10px 20px; }").arg(statFrame, textColor));
    if (onlyTransBtn) {
        onlyTransBtn->setStyleSheet(QString("QPushButton { background-color: %1; color: %2; padding: 12px; font-size: 16px; border: 2px solid %3; }").arg(statFrame, textColor, inputBorder));
        onlyGrammarBtn->setStyleSheet(QString("QPushButton { background-color: %1; color: %2; padding: 12px; font-size: 16px; border: 2px solid %3; }").arg(statFrame, textColor, inputBorder));
    }
    
    if (textInput) {
        QString focusColor = isDarkMode ? "#38BDF8" : "#0284C7";
        textInput->setStyleSheet(QString("QLineEdit { background-color: %1; color: %2; border: 2px solid %3; border-radius: 10px; padding: 18px; font-size: 22px; } QLineEdit:focus { border: 2px solid %4; }").arg(inputBg, textColor, inputBorder, focusColor));
    }
    
    for (auto rb : radioButtons) {
        if (rb) {
            rb->setStyleSheet(QString("QRadioButton { font-size: 24px; color: %1; spacing: 15px; padding: 8px; }").arg(textColor));
        }
    }
}

void MainWindow::onThemeToggled() {
    isDarkMode = !isDarkMode;
    settings.setValue("isDarkMode", isDarkMode);
    applyTheme();
}

void MainWindow::onMuteToggled() {
    isMuted = !isMuted;
    settings.setValue("isAudioMuted", isMuted);
    audioPlayer->setMuted(isMuted);
    muteBtn->setText(isMuted ? "🔇 Unmute Audio" : "🔊 Mute Audio");
}

QWidget* MainWindow::createMainMenu() {
    QWidget* widget = new QWidget();
    QVBoxLayout* layout = new QVBoxLayout(widget);
    
    QHBoxLayout* topLayout = new QHBoxLayout();
    topLayout->addStretch();
    settingsBtn = new QPushButton("⚙️ Settings");
    settingsBtn->setCursor(Qt::PointingHandCursor);
    connect(settingsBtn, &QPushButton::clicked, this, &MainWindow::onOpenSettingsClicked);
    topLayout->addWidget(settingsBtn);
    layout->addLayout(topLayout);
    
    layout->addStretch(1);
    
    title = new QLabel("🌍 Polyglot Mastery");
    title->setAlignment(Qt::AlignCenter);
    layout->addWidget(title);

    subtitle = new QLabel("Learn languages efficiently. Master your skills.");
    subtitle->setAlignment(Qt::AlignCenter);
    layout->addWidget(subtitle);

    highScoreLabel = new QLabel("High Score: 0");
    highScoreLabel->setStyleSheet("font-size: 22px; font-weight: bold; color: #10B981; margin-bottom: 20px;");
    highScoreLabel->setAlignment(Qt::AlignCenter);
    layout->addWidget(highScoreLabel);
    
    difficultyCombo = new QComboBox();
    difficultyCombo->addItems({"1. Easy (45s, 5 Hearts)", "2. Medium (30s, 3 Hearts)", "3. Hard Mode (15s, 1 Heart)", "4. Custom Mode (Your Settings)"});
    int diffVal = settings.value("difficulty", 1).toInt() - 1;
    if (diffVal >= 0 && diffVal < 4) difficultyCombo->setCurrentIndex(diffVal);
    difficultyCombo->setMinimumWidth(350);
    
    QHBoxLayout* comboLayout = new QHBoxLayout();
    comboLayout->addStretch();
    comboLayout->addWidget(difficultyCombo);
    comboLayout->addStretch();
    layout->addLayout(comboLayout);
    
    layout->addSpacing(30);

    transBtn = new QPushButton("🚀 Start Mixed Challenge");
    transBtn->setCursor(Qt::PointingHandCursor);
    transBtn->setMinimumWidth(350);
    connect(transBtn, &QPushButton::clicked, this, &MainWindow::onStartClicked);
    
    QHBoxLayout* btnLayout = new QHBoxLayout();
    btnLayout->addStretch();
    btnLayout->addWidget(transBtn);
    btnLayout->addStretch();
    layout->addLayout(btnLayout);
    
    layout->addSpacing(10);
    
    onlyTransBtn = new QPushButton("Translation Practice");
    onlyTransBtn->setCursor(Qt::PointingHandCursor);
    connect(onlyTransBtn, &QPushButton::clicked, this, &MainWindow::onStartTranslationOnlyClicked);
    
    onlyGrammarBtn = new QPushButton("Grammar Practice");
    onlyGrammarBtn->setCursor(Qt::PointingHandCursor);
    connect(onlyGrammarBtn, &QPushButton::clicked, this, &MainWindow::onStartGrammarOnlyClicked);
    
    QHBoxLayout* specificBtnLayout = new QHBoxLayout();
    specificBtnLayout->addStretch();
    specificBtnLayout->addWidget(onlyTransBtn);
    specificBtnLayout->addSpacing(10);
    specificBtnLayout->addWidget(onlyGrammarBtn);
    specificBtnLayout->addStretch();
    layout->addLayout(specificBtnLayout);
    
    layout->addStretch(1);
    
    return widget;
}

QWidget* MainWindow::createSettingsScreen() {
    QWidget* widget = new QWidget();
    QVBoxLayout* layout = new QVBoxLayout(widget);
    
    QLabel* head = new QLabel("⚙️ Settings");
    head->setStyleSheet("font-size: 40px; font-weight: bold;");
    head->setAlignment(Qt::AlignCenter);
    layout->addWidget(head);
    layout->addSpacing(30);
    
    QHBoxLayout* topBtnsLayout = new QHBoxLayout();
    topBtnsLayout->addStretch();
    
    themeBtn = new QPushButton(isDarkMode ? "☀️ Light Mode" : "🌙 Dark Mode");
    themeBtn->setCursor(Qt::PointingHandCursor);
    themeBtn->setMinimumWidth(200);
    connect(themeBtn, &QPushButton::clicked, this, &MainWindow::onThemeToggled);
    topBtnsLayout->addWidget(themeBtn);
    topBtnsLayout->addSpacing(20);
    
    muteBtn = new QPushButton(isMuted ? "🔇 Unmute Audio" : "🔊 Mute Audio");
    muteBtn->setCursor(Qt::PointingHandCursor);
    muteBtn->setMinimumWidth(200);
    connect(muteBtn, &QPushButton::clicked, this, &MainWindow::onMuteToggled);
    topBtnsLayout->addWidget(muteBtn);
    
    topBtnsLayout->addStretch();
    layout->addLayout(topBtnsLayout);
    layout->addSpacing(20);
    
    QLabel* customLabel = new QLabel("--- Custom Mode Configuration ---");
    customLabel->setAlignment(Qt::AlignCenter);
    customLabel->setStyleSheet("font-size: 20px; font-weight: bold; color: #10B981;");
    layout->addWidget(customLabel);
    
    QHBoxLayout* timeLay = new QHBoxLayout();
    customTimeLabel = new QLabel("Time Per Question: 45s");
    customTimeLabel->setFixedWidth(250);
    customTimeSlider = new QSlider(Qt::Horizontal);
    customTimeSlider->setRange(5, 120);
    customTimeSlider->setValue(settings.value("customTime", 45).toInt());
    connect(customTimeSlider, &QSlider::valueChanged, [this](int v){ customTimeLabel->setText(QString("Time Per Question: %1s").arg(v)); });
    customTimeSlider->valueChanged(customTimeSlider->value()); 
    timeLay->addStretch(); timeLay->addWidget(customTimeLabel); timeLay->addWidget(customTimeSlider); timeLay->addStretch();
    layout->addLayout(timeLay);
    
    QHBoxLayout* livesLay = new QHBoxLayout();
    customLivesLabel = new QLabel("Lives (Hearts): 5");
    customLivesLabel->setFixedWidth(250);
    customLivesSlider = new QSlider(Qt::Horizontal);
    customLivesSlider->setRange(1, 10);
    customLivesSlider->setValue(settings.value("customLives", 5).toInt());
    connect(customLivesSlider, &QSlider::valueChanged, [this](int v){ customLivesLabel->setText(QString("Lives (Hearts): %1").arg(v)); });
    customLivesSlider->valueChanged(customLivesSlider->value());
    livesLay->addStretch(); livesLay->addWidget(customLivesLabel); livesLay->addWidget(customLivesSlider); livesLay->addStretch();
    layout->addLayout(livesLay);

    QHBoxLayout* qLay = new QHBoxLayout();
    customQuestionsLabel = new QLabel("Questions Count: 10");
    customQuestionsLabel->setFixedWidth(250);
    customQuestionsSlider = new QSlider(Qt::Horizontal);
    customQuestionsSlider->setRange(2, 50);
    customQuestionsSlider->setValue(settings.value("customQuestions", 10).toInt());
    connect(customQuestionsSlider, &QSlider::valueChanged, [this](int v){ customQuestionsLabel->setText(QString("Questions Count: %1").arg(v)); });
    customQuestionsSlider->valueChanged(customQuestionsSlider->value());
    qLay->addStretch(); qLay->addWidget(customQuestionsLabel); qLay->addWidget(customQuestionsSlider); qLay->addStretch();
    layout->addLayout(qLay);

    layout->addStretch(1);

    QPushButton* saveBtn = new QPushButton("💾 Save & Return");
    saveBtn->setMinimumWidth(300);
    connect(saveBtn, &QPushButton::clicked, this, &MainWindow::onSaveSettingsClicked);
    QHBoxLayout* bLay = new QHBoxLayout(); bLay->addStretch(); bLay->addWidget(saveBtn); bLay->addStretch();
    layout->addLayout(bLay);
    
    return widget;
}

void MainWindow::onOpenSettingsClicked() {
    stackedWidget->setCurrentIndex(2);
}

void MainWindow::onSaveSettingsClicked() {
    settings.setValue("customTime", customTimeSlider->value());
    settings.setValue("customLives", customLivesSlider->value());
    settings.setValue("customQuestions", customQuestionsSlider->value());
    stackedWidget->setCurrentIndex(0); 
}

void MainWindow::refreshMainMenu() {
    int highScore = settings.value("highScore", 0).toInt();
    highScoreLabel->setText(QString("High Score: %1").arg(highScore));
}

QFrame* createStatBlock(QLabel** outLabel, const QString& initText, const QString& colorHex) {
    QFrame* frame = new QFrame();
    frame->setObjectName("StatFrame");
    QHBoxLayout* lay = new QHBoxLayout(frame);
    lay->setContentsMargins(15, 10, 15, 10);
    *outLabel = new QLabel(initText);
    (*outLabel)->setStyleSheet(QString("font-size: 20px; font-weight: bold; color: %1;").arg(colorHex));
    lay->addWidget(*outLabel);
    return frame;
}

QWidget* MainWindow::createExerciseScreen() {
    QWidget* widget = new QWidget();
    QVBoxLayout* layout = new QVBoxLayout(widget);

    QHBoxLayout* headerLayout = new QHBoxLayout();
    backButton = new QPushButton("⬅️ Menu");
    connect(backButton, &QPushButton::clicked, this, &MainWindow::onBackToMenu);
    
    progressLabel = new QLabel("0 / 0");
    progressBar = new QProgressBar();
    progressBar->setRange(0, 100);
    progressBar->setValue(0);
    
    headerLayout->addWidget(backButton);
    headerLayout->addSpacing(20);
    headerLayout->addWidget(progressBar, 1);
    headerLayout->addSpacing(10);
    headerLayout->addWidget(progressLabel);
    headerLayout->addSpacing(20);
    
    headerLayout->addWidget(createStatBlock(&timeLabel, "⏱️ 0s", ""));
    headerLayout->addSpacing(10);
    headerLayout->addWidget(createStatBlock(&attemptsLabel, "❤️ 0", "#F43F5E"));
    headerLayout->addSpacing(10);
    headerLayout->addWidget(createStatBlock(&streakLabel, "🔥 x1.0", "#F59E0B"));
    headerLayout->addSpacing(10);
    headerLayout->addWidget(createStatBlock(&scoreLabel, "🏆 0", "#10B981"));

    layout->addLayout(headerLayout);
    layout->addStretch(1);

    questionTextLabel = new QLabel("");
    questionTextLabel->setAlignment(Qt::AlignCenter);
    questionTextLabel->setWordWrap(true);
    questionTextLabel->setStyleSheet("font-size: 34px; font-weight: bold;");
    layout->addWidget(questionTextLabel);
    
    layout->addSpacing(40);

    answerContainerWidget = new QWidget();
    layout->addWidget(answerContainerWidget);
    
    layout->addSpacing(50);

    submitButton = new QPushButton("CHECK ANSWER ✓");
    submitButton->setMinimumWidth(250);
    connect(submitButton, &QPushButton::clicked, this, &MainWindow::onSubmit);
    
    skipButton = new QPushButton("SKIP ⏭️ (-15 pts)");
    skipButton->setMinimumWidth(200);
    skipButton->setStyleSheet("background-color: #F59E0B;");
    connect(skipButton, &QPushButton::clicked, this, &MainWindow::onSkipClicked);

    hintButton = new QPushButton("HINT (H) 💡");
    hintButton->setMinimumWidth(200);
    hintButton->setStyleSheet("background-color: #8B5CF6;");
    connect(hintButton, &QPushButton::clicked, this, &MainWindow::onHintClicked);
    
    QHBoxLayout* submitLayout = new QHBoxLayout();
    submitLayout->addStretch();
    submitLayout->addWidget(hintButton);
    submitLayout->addSpacing(15);
    submitLayout->addWidget(skipButton);
    submitLayout->addSpacing(15);
    submitLayout->addWidget(submitButton);
    submitLayout->addStretch();
    layout->addLayout(submitLayout);
    
    layout->addStretch(1);

    return widget;
}

void MainWindow::startSpecificExercise(int difficulty, int filterType) {
    settings.setValue("difficulty", difficulty);
    difficultyCombo->setCurrentIndex(difficulty - 1);
    stackedWidget->setCurrentIndex(1);
    
    CustomGameConfig conf;
    conf.timeLimit = settings.value("customTime", 45).toInt();
    conf.attempts = settings.value("customLives", 5).toInt();
    conf.questionsCount = settings.value("customQuestions", 10).toInt();
    
    int numQuestions = (difficulty == 4) ? conf.questionsCount : 7;
    progressLabel->setText(QString("0 / %1").arg(numQuestions));
    progressBar->setRange(0, numQuestions);
    progressBar->setValue(0);
    scoreLabel->setText("🏆 0");
    
    controller->startExercise(difficulty, numQuestions, conf, filterType);
    attemptsLabel->setText(QString("❤️ %1").arg(controller->getAttemptsLeft()));
    audioPlayer->playGameMusic();
}

void MainWindow::onStartClicked() {
    startSpecificExercise(difficultyCombo->currentIndex() + 1, 0);
}

void MainWindow::onStartTranslationOnlyClicked() {
    startSpecificExercise(difficultyCombo->currentIndex() + 1, 1);
}

void MainWindow::onStartGrammarOnlyClicked() {
    startSpecificExercise(difficultyCombo->currentIndex() + 1, 2);
}

void MainWindow::clearDynamicInput() {
    if (answerContainerWidget) {
        delete answerContainerWidget;
        answerContainerWidget = nullptr;
    }
    radioButtons.clear();
    textInput = nullptr;
}

void MainWindow::animatePropertyFade(QWidget* widget) {
    QGraphicsOpacityEffect *eff = new QGraphicsOpacityEffect(this);
    widget->setGraphicsEffect(eff);
    QPropertyAnimation *a = new QPropertyAnimation(eff, "opacity");
    a->setDuration(300);
    a->setStartValue(0);
    a->setEndValue(1);
    a->start(QPropertyAnimation::DeleteWhenStopped);
}

void MainWindow::shakeWindow() {
    QPropertyAnimation *a = new QPropertyAnimation(this, "pos");
    a->setDuration(300);
    a->setLoopCount(3); 
    
    QPoint pos = this->pos();
    a->setKeyValueAt(0, pos);
    a->setKeyValueAt(0.25, pos + QPoint(10, 0));
    a->setKeyValueAt(0.75, pos + QPoint(-10, 0));
    a->setKeyValueAt(1, pos);
    a->start(QPropertyAnimation::DeleteWhenStopped);
}

void MainWindow::onQuestionStarted(const Question& question) {
    questionTextLabel->setText(question.text);
    currentHint = question.hint;
    animatePropertyFade(questionTextLabel);
    
    clearDynamicInput();
    
    answerContainerWidget = new QWidget();
    QVBoxLayout* dLayout = new QVBoxLayout(answerContainerWidget);
    dLayout->setContentsMargins(0,0,0,0);
    
    QVBoxLayout* parentLayout = qobject_cast<QVBoxLayout*>(stackedWidget->widget(1)->layout());
    if (parentLayout) {
        parentLayout->insertWidget(3, answerContainerWidget);
    }
    
    QHBoxLayout* centerRw = new QHBoxLayout();
    centerRw->addStretch();
    
    if (question.type == QuestionType::Translation) {
        textInput = new QLineEdit();
        textInput->setPlaceholderText("Type your translation here...");
        textInput->setMinimumWidth(600);
        centerRw->addWidget(textInput);
        textInput->setFocus();
    } else if (question.type == QuestionType::Grammar) {
        QWidget* innerRw = new QWidget();
        QVBoxLayout* innerRv = new QVBoxLayout(innerRw);
        innerRv->setSpacing(20);

        for (const QString& opt : question.options) {
            QRadioButton* rb = new QRadioButton(opt);
            radioButtons.append(rb);
            innerRv->addWidget(rb);
        }
        centerRw->addWidget(innerRw);
    }
    
    centerRw->addStretch();
    dLayout->addLayout(centerRw);
    
    applyTheme(); // Refresh theme for new widgets
    animatePropertyFade(answerContainerWidget);
    
    submitButton->setEnabled(true);
    skipButton->setEnabled(true);
    hintButton->setEnabled(true);
}

void MainWindow::onCorrectAnswer() {
    audioPlayer->playSuccess();
    
    int progress = controller->getProgress();
    progressBar->setValue(progress);
    progressLabel->setText(QString("%1 / %2").arg(progress).arg(controller->getTotalQuestions()));
    scoreLabel->setText(QString("🏆 %1").arg(controller->getCurrentScore()));
}

void MainWindow::onWrongAnswer(const QString& expected) {
    audioPlayer->playFail();
    shakeWindow(); // BOOM! Juice effect
    
    int currentLives = controller->getAttemptsLeft();
    attemptsLabel->setText(QString("❤️ %1").arg(currentLives));
    
    if (currentLives == 1) {
        audioPlayer->playLowHpMusic();
    }
    
    QMessageBox::warning(this, "Oops", "Incorrect! Try again.");
}

void MainWindow::onQuestionSkipped() {
    int progress = controller->getProgress();
    progressBar->setValue(progress);
    progressLabel->setText(QString("%1 / %2").arg(progress).arg(controller->getTotalQuestions()));
    scoreLabel->setText(QString("🏆 %1").arg(controller->getCurrentScore()));
}

void MainWindow::onStreakUpdated(int streak, float multiplier) {
    streakLabel->setText(QString("🔥 %1 (x%2)").arg(streak).arg(QString::number(multiplier, 'f', 1)));
}

void MainWindow::onExerciseFinished(bool won, int finalScore) {
    audioPlayer->stopAllMusic();
    
    if (won) {
        audioPlayer->playSuccess();
        QMessageBox::information(this, "Victory!", QString("Epic! You conquered the challenge and earned %1 points.").arg(finalScore));
        
        int currentHighScore = settings.value("highScore", 0).toInt();
        if (finalScore > currentHighScore) {
            settings.setValue("highScore", finalScore);
        }
    } else {
        audioPlayer->playFail();
        QMessageBox::critical(this, "Game Over", "Attempts exhausted or time is strictly over. Try again!");
    }
    
    refreshMainMenu();
    stackedWidget->setCurrentIndex(0);
    audioPlayer->playMenuMusic();
}

void MainWindow::onTimeUpdated(int secondsLeft) {
    timeLabel->setText(QString("⏱️ %1s").arg(secondsLeft));
    bool isUrgent = (secondsLeft <= 5);
    QString urgentColor = isDarkMode ? "#EF4444" : "#DC2626";
    QString normalColor = isDarkMode ? "#F8FAFC" : "#0F172A";

    if (isUrgent) {
        timeLabel->setStyleSheet(QString("color: %1; font-weight: bold; font-size: 20px;").arg(urgentColor));
    } else {
        timeLabel->setStyleSheet(QString("color: %1; font-weight: bold; font-size: 20px;").arg(normalColor));
    }
}

void MainWindow::onSubmit() {
    if (textInput) {
        QString tx = textInput->text().trimmed();
        if (tx.isEmpty()) return;
        submitButton->setEnabled(false);
        skipButton->setEnabled(false);
        QTimer::singleShot(200, this, [this, tx]() {
            controller->submitAnswer(tx);
            if (submitButton) submitButton->setEnabled(true);
            if (skipButton) skipButton->setEnabled(true);
        });
    } else if (!radioButtons.isEmpty()) {
        QString selected;
        for (auto rb : radioButtons) {
            if (rb->isChecked()) {
                selected = rb->text();
                break;
            }
        }
        if (selected.isEmpty()) return; 
        submitButton->setEnabled(false);
        skipButton->setEnabled(false);
        QTimer::singleShot(200, this, [this, selected]() {
            controller->submitAnswer(selected);
            if (submitButton) submitButton->setEnabled(true);
            if (skipButton) skipButton->setEnabled(true);
        });
    }
}

void MainWindow::onSkipClicked() {
    submitButton->setEnabled(false);
    skipButton->setEnabled(false);
    QTimer::singleShot(200, this, [this]() {
        controller->skipQuestion();
    });
}

void MainWindow::onHintClicked() {
    if (stackedWidget->currentIndex() == 1 && !currentHint.isEmpty()) {
        QMessageBox::information(this, "Hint", currentHint);
    }
}

void MainWindow::onBackToMenu() {
    controller->stop();
    refreshMainMenu();
    stackedWidget->setCurrentIndex(0);
    audioPlayer->playMenuMusic();
}

bool MainWindow::eventFilter(QObject *watched, QEvent *event) {
    if (event->type() == QEvent::KeyPress) {
        QKeyEvent *keyEvent = static_cast<QKeyEvent *>(event);
        QString keyText = keyEvent->text().toUpper();
        
        if (keyEvent->key() == Qt::Key_H || keyText == "Р" || keyText == "H") {
            if (textInput && textInput->hasFocus() && textInput->isVisible()) {
                return false; 
            }
            onHintClicked();
            return true; 
        }
        
        if (keyEvent->key() == Qt::Key_Escape) {
            if (textInput && textInput->hasFocus()) {
                textInput->clearFocus();
                return true;
            }
        }
        
        if (keyEvent->key() == Qt::Key_Return || keyEvent->key() == Qt::Key_Enter) {
            if (stackedWidget->currentIndex() == 1 && submitButton->isEnabled()) {
                onSubmit();
                return true;
            }
        }
    }
    return QMainWindow::eventFilter(watched, event);
}
