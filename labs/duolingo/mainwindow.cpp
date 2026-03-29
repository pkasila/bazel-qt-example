#include "mainwindow.h"

#include "settingsdialog.h"

#include <QAction>
#include <QCoreApplication>
#include <QDir>
#include <QFile>
#include <QFontDatabase>
#include <QHBoxLayout>
#include <QKeyEvent>
#include <QLabel>
#include <QMenu>
#include <QMenuBar>
#include <QMessageBox>
#include <QProgressBar>
#include <QPushButton>
#include <QRandomGenerator>
#include <QShowEvent>
#include <QSoundEffect>
#include <QStackedWidget>
#include <QTimer>
#include <QUrl>
#include <QVBoxLayout>
#include <QShortcut>
#include <algorithm>
#include <vector>

int MainWindow::calculateLevenshteinDistance(const QString& s1_in, const QString& s2_in) {
    QString s1 = s1_in.trimmed().toLower();
    QString s2 = s2_in.trimmed().toLower();
    const int len1 = s1.length();
    const int len2 = s2.length();
    if (len1 == 0) {
        return len2;
    }
    if (len2 == 0) {
        return len1;
    }
    std::vector<std::vector<int>> d(len1 + 1, std::vector<int>(len2 + 1));
    for (int i = 0; i <= len1; ++i) {
        d[i][0] = i;
    }
    for (int j = 0; j <= len2; ++j) {
        d[0][j] = j;
    }
    for (int j = 1; j <= len2; ++j) {
        for (int i = 1; i <= len1; ++i) {
            int cost = (s1[i - 1] == s2[j - 1]) ? 0 : 1;
            d[i][j] = std::min({d[i - 1][j] + 1, d[i][j - 1] + 1, d[i - 1][j - 1] + cost});
        }
    }
    return d[len1][len2];
}

void MainWindow::applyFeedbackStyleToLabel(QLabel* label, bool isCorrect) {
    if (!label) {
        return;
    }
    QFont feedbackFont = label->font();
    feedbackFont.setBold(true);
    label->setFont(feedbackFont);
    if (isCorrect) {
        label->setStyleSheet(
            "color: #27ae60; padding: 5px; background-color: #e8f8f5; border-radius: 3px;");
    } else {
        label->setStyleSheet(
            "color: #c0392b; padding: 5px; background-color: #fdedec; border-radius: 3px;");
    }
}

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
    , currentDifficultyLevel(Difficulty::Easy)
    , activeExerciseType(ExerciseType::None)
    , tasksCompletedInSet(0)
    , mistakesMadeInSet(0)
    , totalPlayerScore(0)
    , currentTaskIndexInSet(0)
    , correctAnswerSound(new QSoundEffect(this))
    , incorrectAnswerSound(new QSoundEffect(this))
    , exerciseStartCue(new QSoundEffect(this))
    , timerExpiredSound(new QSoundEffect(this))
    , applicationStartSound(new QSoundEffect(this))
    , meditationAudio(new QSoundEffect(this)) {
    setWindowTitle(tr("LinguaLearn Quest"));
    ExerciseData::initializeData();

    createMenus();
    createMainLayout();
    updatePlayerScoreDisplay();
    resetApplicationToIdleState();
    helpShortcut = new QShortcut(QKeySequence(Qt::Key_H), this);
    connect(helpShortcut, &QShortcut::activated, this, &MainWindow::showHelp);

    exerciseSessionTimer = new QTimer(this);
    connect(exerciseSessionTimer, &QTimer::timeout, this, &MainWindow::updateTimer);

    initializeSounds();
    resize(850, 650);
}

MainWindow::~MainWindow() {
}

void MainWindow::showEvent(QShowEvent* event) {
    QMainWindow::showEvent(event);
    static bool appStartSoundPlayed = false;
    if (!appStartSoundPlayed && applicationStartSound && applicationStartSound->isLoaded()) {
        playSoundEffect(applicationStartSound);
        appStartSoundPlayed = true;
    }
}

void MainWindow::initializeSounds() {
    QString soundPathPrefix = "labs/duolingo/data/";


    applicationStartSound->setSource(QUrl::fromLocalFile(soundPathPrefix + "boot_up.wav"));
    correctAnswerSound->setSource(QUrl::fromLocalFile(soundPathPrefix + "correct.wav"));
    incorrectAnswerSound->setSource(QUrl::fromLocalFile(soundPathPrefix + "incorrect.wav"));
    exerciseStartCue->setSource(QUrl::fromLocalFile(soundPathPrefix + "exercise_start.wav"));
    timerExpiredSound->setSource(QUrl::fromLocalFile(soundPathPrefix + "time_up.wav"));
    meditationAudio->setSource(QUrl::fromLocalFile(soundPathPrefix + "meditation.wav"));

    auto checkLoad = [](QSoundEffect* sfx, const QString& name) {
        if (!sfx->isLoaded()) {
            qWarning() << "Sound Load Fail:" << name << "from" << sfx->source().toString();
        }
    };
    checkLoad(applicationStartSound, "boot_up.wav");
    checkLoad(correctAnswerSound, "correct.wav");
    checkLoad(incorrectAnswerSound, "incorrect.wav");
    checkLoad(exerciseStartCue, "exercise_start.wav");
    checkLoad(timerExpiredSound, "time_up.wav");
    checkLoad(meditationAudio, "meditation.wav");

    float volume = 0.6f;
    applicationStartSound->setVolume(volume * 0.7f);
    correctAnswerSound->setVolume(volume);
    incorrectAnswerSound->setVolume(volume);
    exerciseStartCue->setVolume(volume * 0.8f);
    timerExpiredSound->setVolume(volume);
    meditationAudio->setVolume(volume * 0.5f);
}

void MainWindow::playSoundEffect(QSoundEffect* sound, bool shouldLoop) {
    if (sound && sound->isLoaded()) {
        if (sound->isPlaying()) {
            sound->stop();
        }
        sound->setLoopCount(shouldLoop ? QSoundEffect::Infinite : 1);
        sound->play();
    } else if (sound) {
        qWarning() << "Sound not loaded/null, cannot play:" << sound->source().toString();
    }
}

void MainWindow::createMenus() {
    settingsMenu = menuBar()->addMenu(tr("&Settings"));
    changeDifficultyAction = new QAction(tr("&Change Difficulty..."), this);
    connect(changeDifficultyAction, &QAction::triggered, this, &MainWindow::openSettingsDialog);
    settingsMenu->addAction(changeDifficultyAction);
    QMenu *helpMenu = menuBar()->addMenu(tr("&Help"));
    QAction *aboutAction = new QAction(tr("Show &Help"), this);
    connect(aboutAction, &QAction::triggered, this, &MainWindow::showHelp);
    helpMenu->addAction(aboutAction);
}

void MainWindow::createMainLayout() {
    centralWidgetContainer = new QWidget();
    QVBoxLayout* mainVLayout = new QVBoxLayout(centralWidgetContainer);
    mainVLayout->setContentsMargins(15, 15, 15, 15);
    mainVLayout->setSpacing(10);

    QHBoxLayout* topControlsLayout = new QHBoxLayout();
    translationModeButton = new QPushButton(tr("📚 Translation"));
    grammarSingleModeButton = new QPushButton(tr("✍️ Grammar (Single)"));
    grammarMultiModeButton = new QPushButton(tr("✅ Grammar (Multi)"));

    QString exerciseButtonStyle =
        "QPushButton { background-color: #5dade2; color: white; border: none; padding: 10px; "
        "border-radius: 5px; min-width: 160px; font-weight: bold;}"
        "QPushButton:hover { background-color: #3498db; }"
        "QPushButton:disabled { background-color: #aed6f1; }";
    translationModeButton->setStyleSheet(exerciseButtonStyle);
    grammarSingleModeButton->setStyleSheet(exerciseButtonStyle);
    grammarMultiModeButton->setStyleSheet(exerciseButtonStyle);

    topControlsLayout->addWidget(translationModeButton);
    topControlsLayout->addWidget(grammarSingleModeButton);
    topControlsLayout->addWidget(grammarMultiModeButton);
    topControlsLayout->addStretch(1);

    meditationToggleButton = new QPushButton(tr("🧘 Meditate"));
    meditationToggleButton->setStyleSheet(
        "QPushButton { background-color: #1abc9c; color: white; border: none; padding: 10px; "
        "border-radius: 5px; min-width: 120px; font-weight: bold;}"
        "QPushButton:hover { background-color: #16a085; }");
    meditationToggleButton->setCursor(Qt::PointingHandCursor);
    meditationToggleButton->setCheckable(true);
    topControlsLayout->addWidget(meditationToggleButton);
    topControlsLayout->addSpacing(10);

    stopCurrentExerciseButton = new QPushButton(tr("🚫 Stop Exercise"));
    stopCurrentExerciseButton->setStyleSheet(
        "QPushButton { background-color: #e74c3c; color: white; border: none; padding: 10px; "
        "border-radius: 5px; min-width: 120px; font-weight: bold;}"
        "QPushButton:hover { background-color: #c0392b; }"
        "QPushButton:disabled { background-color: #f5b7b1; }");
    stopCurrentExerciseButton->setCursor(Qt::PointingHandCursor);
    stopCurrentExerciseButton->setVisible(false);
    topControlsLayout->addWidget(stopCurrentExerciseButton);

    QHBoxLayout* infoLayout = new QHBoxLayout();
    taskProgressBar = new QProgressBar();
    taskProgressBar->setStyleSheet(
        "QProgressBar { border: 1px solid grey; border-radius: 5px; text-align: center; height: "
        "20px; }"
        "QProgressBar::chunk { background-color: #52be80; width: 10px; margin: 0.5px; "
        "border-radius: 2px; }");
    currentScoreLabel = new QLabel();
    remainingTimeLabel = new QLabel();
    QFont infoFont = currentScoreLabel->font();
    infoFont.setBold(true);
    infoFont.setPointSize(infoFont.pointSize() + 1);
    currentScoreLabel->setFont(infoFont);
    remainingTimeLabel->setFont(infoFont);
    currentScoreLabel->setStyleSheet("color: #2c3e50;");
    remainingTimeLabel->setStyleSheet("color: #e74c3c;");
    infoLayout->addWidget(new QLabel(tr("🚀 Progress:")));
    infoLayout->addWidget(taskProgressBar, 1);
    infoLayout->addSpacing(20);
    infoLayout->addWidget(new QLabel(tr("⭐ Score:")));
    infoLayout->addWidget(currentScoreLabel);
    infoLayout->addSpacing(20);
    infoLayout->addWidget(new QLabel(tr("⏳ Time:")));
    infoLayout->addWidget(remainingTimeLabel);

    exerciseDisplayArea = new QStackedWidget();
    exerciseDisplayArea->setStyleSheet(
        "QWidget { background-color: white; border-radius: 8px; padding: 10px; }");
    welcomeMessageLabel = new QLabel(
        tr("👋 Welcome to LinguaLearn Quest!\n\nChoose an exercise to begin your adventure."));
    welcomeMessageLabel->setAlignment(Qt::AlignCenter);
    welcomeMessageLabel->setWordWrap(true);
    QFont welcomeFont = welcomeMessageLabel->font();
    welcomeFont.setPointSize(18);
    welcomeFont.setBold(true);
    welcomeMessageLabel->setFont(welcomeFont);
    welcomeMessageLabel->setStyleSheet(
        "color: #2980b9; background-color: transparent; border: none;");
    translationExerciseWidget = new TranslationExerciseWidget();
    grammarSingleChoiceExerciseWidget = new GrammarExerciseWidget();
    grammarMultiChoiceExerciseWidget = new MultiCorrectGrammarExerciseWidget();
    exerciseDisplayArea->addWidget(welcomeMessageLabel);
    exerciseDisplayArea->addWidget(translationExerciseWidget);
    exerciseDisplayArea->addWidget(grammarSingleChoiceExerciseWidget);
    exerciseDisplayArea->addWidget(grammarMultiChoiceExerciseWidget);

    userNotificationLabel = new QLabel(tr("Status: Ready to embark on a learning journey!"));
    userNotificationLabel->setAlignment(Qt::AlignCenter);
    userNotificationLabel->setMinimumHeight(40);
    userNotificationLabel->setFrameStyle(QFrame::Panel | QFrame::Sunken);
    userNotificationLabel->setStyleSheet(
        "background-color: #eaf2f8; border-radius: 5px; padding: 5px; color: #34495e;");

    mainVLayout->addLayout(topControlsLayout);
    mainVLayout->addLayout(infoLayout);
    mainVLayout->addWidget(exerciseDisplayArea, 1);
    mainVLayout->addWidget(userNotificationLabel);

    setCentralWidget(centralWidgetContainer);

    connect(
        translationModeButton, &QPushButton::clicked, this, &MainWindow::startTranslationExercise);
    connect(
        grammarSingleModeButton, &QPushButton::clicked, this,
        &MainWindow::startGrammarSingleExercise);
    connect(
        grammarMultiModeButton, &QPushButton::clicked, this,
        &MainWindow::startGrammarMultiExercise);
    connect(
        stopCurrentExerciseButton, &QPushButton::clicked, this, &MainWindow::stopCurrentExercise);
    connect(
        meditationToggleButton, &QPushButton::clicked, this, &MainWindow::onMeditateButtonClicked);

    connect(
        translationExerciseWidget, &TranslationExerciseWidget::submitClicked, this,
        &MainWindow::handleTranslationSubmit);
    connect(
        grammarSingleChoiceExerciseWidget, &GrammarExerciseWidget::submitClicked, this,
        &MainWindow::handleGrammarSingleSubmit);
    connect(
        grammarMultiChoiceExerciseWidget, &MultiCorrectGrammarExerciseWidget::submitClicked, this,
        &MainWindow::handleGrammarMultiSubmit);
}

void MainWindow::openSettingsDialog() {
    SettingsDialog dialog(currentDifficultyLevel, this);
    if (dialog.exec() == QDialog::Accepted) {
        currentDifficultyLevel = dialog.selectedDifficulty();
        userNotificationLabel->setText(tr("Difficulty level updated! Choose your challenge."));
        applyFeedbackStyleToLabel(userNotificationLabel, true);
        if (activeExerciseType != ExerciseType::None) {
            concludeExerciseSession(false);
        }
    }
}

void MainWindow::startTranslationExercise() {
    activeExerciseType = ExerciseType::Translation;
    setupNewExerciseSet(ExerciseType::Translation);
}

void MainWindow::startGrammarSingleExercise() {
    activeExerciseType = ExerciseType::GrammarSingle;
    setupNewExerciseSet(ExerciseType::GrammarSingle);
}

void MainWindow::startGrammarMultiExercise() {
    activeExerciseType = ExerciseType::GrammarMulti;
    setupNewExerciseSet(ExerciseType::GrammarMulti);
}

void MainWindow::setupNewExerciseSet(ExerciseType type) {
    if (meditationAudio && meditationAudio->isPlaying()) {
        meditationAudio->stop();
        meditationToggleButton->setChecked(false);
        meditationToggleButton->setText(tr("🧘 Meditate"));
        meditationToggleButton->setStyleSheet(
            "QPushButton { background-color: #1abc9c; color: white; border: none; padding: 10px; "
            "border-radius: 5px; min-width: 120px; font-weight: bold;}"
            "QPushButton:hover { background-color: #16a085; }");
    }

    resetApplicationToIdleState();
    activeExerciseType = type;
    playSoundEffect(exerciseStartCue);

    const auto& tasksForDifficultyTrans = ExerciseData::translationTasks[currentDifficultyLevel];
    const auto& tasksForDifficultyGrammarSingle =
        ExerciseData::grammarTasks[currentDifficultyLevel];
    const auto& tasksForDifficultyGrammarMulti =
        ExerciseData::multiGrammarTasks[currentDifficultyLevel];
    int tasksToLoad = 0;

    if (type == ExerciseType::Translation) {
        currentTranslationTaskSet = tasksForDifficultyTrans;
        std::shuffle(
            currentTranslationTaskSet.begin(), currentTranslationTaskSet.end(),
            *QRandomGenerator::global());
        if (currentTranslationTaskSet.size() > TOTAL_TASKS_PER_EXERCISE) {
            currentTranslationTaskSet.resize(TOTAL_TASKS_PER_EXERCISE);
        }
        tasksToLoad = currentTranslationTaskSet.size();
    } else if (type == ExerciseType::GrammarSingle) {
        currentGrammarSingleTaskSet = tasksForDifficultyGrammarSingle;
        std::shuffle(
            currentGrammarSingleTaskSet.begin(), currentGrammarSingleTaskSet.end(),
            *QRandomGenerator::global());
        if (currentGrammarSingleTaskSet.size() > TOTAL_TASKS_PER_EXERCISE) {
            currentGrammarSingleTaskSet.resize(TOTAL_TASKS_PER_EXERCISE);
        }
        tasksToLoad = currentGrammarSingleTaskSet.size();
    } else if (type == ExerciseType::GrammarMulti) {
        currentGrammarMultiTaskSet = tasksForDifficultyGrammarMulti;
        std::shuffle(
            currentGrammarMultiTaskSet.begin(), currentGrammarMultiTaskSet.end(),
            *QRandomGenerator::global());
        if (currentGrammarMultiTaskSet.size() > TOTAL_TASKS_PER_EXERCISE) {
            currentGrammarMultiTaskSet.resize(TOTAL_TASKS_PER_EXERCISE);
        }
        tasksToLoad = currentGrammarMultiTaskSet.size();
    }

    taskProgressBar->setMaximum(tasksToLoad);
    taskProgressBar->setVisible(true);
    remainingTimeLabel->setVisible(true);
    secondsRemainingInSession = EXERCISE_TIME_SECONDS;
    remainingTimeLabel->setText(QString::number(secondsRemainingInSession) + "s");
    remainingTimeLabel->setStyleSheet("color: #e74c3c;");
    exerciseSessionTimer->start(1000);

    translationModeButton->setEnabled(false);
    grammarSingleModeButton->setEnabled(false);
    grammarMultiModeButton->setEnabled(false);
    stopCurrentExerciseButton->setVisible(true);

    presentNextTask();
}

void MainWindow::presentNextTask() {
    QString taskMessage;
    bool taskIsAvailable = false;

    if (activeExerciseType == ExerciseType::Translation) {
        if (currentTaskIndexInSet < currentTranslationTaskSet.size()) {
            translationExerciseWidget->setTask(currentTranslationTaskSet[currentTaskIndexInSet]);
            exerciseDisplayArea->setCurrentWidget(translationExerciseWidget);
            taskMessage = tr("Translate: Task %1 of %2")
                              .arg(currentTaskIndexInSet + 1)
                              .arg(currentTranslationTaskSet.size());
            taskIsAvailable = true;
        } else {
            concludeExerciseSession(true);
            return;
        }
    } else if (activeExerciseType == ExerciseType::GrammarSingle) {
        if (currentTaskIndexInSet < currentGrammarSingleTaskSet.size()) {
            grammarSingleChoiceExerciseWidget->setTask(
                currentGrammarSingleTaskSet[currentTaskIndexInSet]);
            exerciseDisplayArea->setCurrentWidget(grammarSingleChoiceExerciseWidget);
            taskMessage = tr("Grammar (Single): Task %1 of %2")
                              .arg(currentTaskIndexInSet + 1)
                              .arg(currentGrammarSingleTaskSet.size());
            taskIsAvailable = true;
        } else {
            concludeExerciseSession(true);
            return;
        }
    } else if (activeExerciseType == ExerciseType::GrammarMulti) {
        if (currentTaskIndexInSet < currentGrammarMultiTaskSet.size()) {
            grammarMultiChoiceExerciseWidget->setTask(
                currentGrammarMultiTaskSet[currentTaskIndexInSet]);
            exerciseDisplayArea->setCurrentWidget(grammarMultiChoiceExerciseWidget);
            taskMessage = tr("Grammar (Multi): Task %1 of %2")
                              .arg(currentTaskIndexInSet + 1)
                              .arg(currentGrammarMultiTaskSet.size());
            taskIsAvailable = true;
        } else {
            concludeExerciseSession(true);
            return;
        }
    }

    if (taskIsAvailable) {
        userNotificationLabel->setText(taskMessage);
        userNotificationLabel->setStyleSheet(
            "background-color: #eaf2f8; border-radius: 5px; padding: 5px; color: #34495e;");
    }
}

void MainWindow::handleTranslationSubmit() {
    if (activeExerciseType != ExerciseType::Translation ||
        currentTaskIndexInSet >= currentTranslationTaskSet.size()) {
        return;
    }
    QString userAnswer = translationExerciseWidget->getUserAnswer();
    QString correctAnswer = currentTranslationTaskSet[currentTaskIndexInSet].correctAnswer;

    int distance = calculateLevenshteinDistance(userAnswer, correctAnswer);
    int shorterLength = qMin(userAnswer.trimmed().length(), correctAnswer.trimmed().length());
    int tolerance = (shorterLength > 0)
                        ? static_cast<int>(shorterLength * LEVENSHTEIN_SIMILARITY_THRESHOLD)
                        : 0;
    if (shorterLength <= 5 && tolerance == 0 && shorterLength > 0) {
        tolerance = 1;
    }
    if (shorterLength == 0 && userAnswer.trimmed().isEmpty() &&
        !correctAnswer.trimmed().isEmpty()) {
        distance = correctAnswer.trimmed().length();
    }

    bool isExactlyCorrect = (userAnswer.trimmed().toLower() == correctAnswer.trimmed().toLower());
    bool isSimilarEnough = (!isExactlyCorrect && distance <= tolerance);
    bool isFinalAnswerCorrect = isExactlyCorrect || isSimilarEnough;

    if (isFinalAnswerCorrect) {
        playSoundEffect(correctAnswerSound);
        tasksCompletedInSet++;
        taskProgressBar->setValue(tasksCompletedInSet);
        currentTaskIndexInSet++;
        userNotificationLabel->setText(
            isExactlyCorrect ? tr("🌟 Perfect!")
                             : tr("👍 Close! Expected: \"%1\"").arg(correctAnswer));
        applyFeedbackStyleToLabel(userNotificationLabel, true);
        QTimer::singleShot(1800, this, &MainWindow::presentNextTask);
    } else {
        playSoundEffect(incorrectAnswerSound);
        mistakesMadeInSet++;
        userNotificationLabel->setText(
            tr("🤔 Not quite (Mistake %1/%2). Expected: \"%3\"")
                .arg(mistakesMadeInSet)
                .arg(MAX_MISTAKES_ALLOWED)
                .arg(correctAnswer));
        applyFeedbackStyleToLabel(userNotificationLabel, false);
        translationExerciseWidget->clearAnswer();
        if (mistakesMadeInSet >= MAX_MISTAKES_ALLOWED) {
            QTimer::singleShot(3000, this, [this]() {
                concludeExerciseSession(false);
            });
        }
    }
}

void MainWindow::handleGrammarSingleSubmit() {
    if (activeExerciseType != ExerciseType::GrammarSingle ||
        currentTaskIndexInSet >= currentGrammarSingleTaskSet.size()) {
        return;
    }
    int userAnswerIndex = grammarSingleChoiceExerciseWidget->getSelectedOption();
    int correctAnswerIndex = currentGrammarSingleTaskSet[currentTaskIndexInSet].correctOptionIndex;

    if (userAnswerIndex == -1) {
        userNotificationLabel->setText(tr("⚠️ Please select an option!"));
        applyFeedbackStyleToLabel(userNotificationLabel, false);
        return;
    }
    if (userAnswerIndex == correctAnswerIndex) {
        playSoundEffect(correctAnswerSound);
        tasksCompletedInSet++;
        taskProgressBar->setValue(tasksCompletedInSet);
        currentTaskIndexInSet++;
        userNotificationLabel->setText(tr("🏆 Correct!"));
        applyFeedbackStyleToLabel(userNotificationLabel, true);
        QTimer::singleShot(1500, this, &MainWindow::presentNextTask);
    } else {
        playSoundEffect(incorrectAnswerSound);
        mistakesMadeInSet++;
        userNotificationLabel->setText(
            tr("❌ Incorrect (Mistake %1/%2).").arg(mistakesMadeInSet).arg(MAX_MISTAKES_ALLOWED));
        applyFeedbackStyleToLabel(userNotificationLabel, false);
        if (mistakesMadeInSet >= MAX_MISTAKES_ALLOWED) {
            grammarSingleChoiceExerciseWidget->showCorrectAnswer(correctAnswerIndex);
            QTimer::singleShot(2500, this, [this]() {
                concludeExerciseSession(false);
            });
        }
    }
}

void MainWindow::handleGrammarMultiSubmit() {
    if (activeExerciseType != ExerciseType::GrammarMulti ||
        currentTaskIndexInSet >= currentGrammarMultiTaskSet.size()) {
        return;
    }
    QSet<int> userAnswerIndices = grammarMultiChoiceExerciseWidget->getSelectedIndices();
    const QSet<int>& correctAnswerIndices =
        currentGrammarMultiTaskSet[currentTaskIndexInSet].correctOptionIndices;

    if (userAnswerIndices.isEmpty()) {
        userNotificationLabel->setText(tr("⚠️ Please select at least one option!"));
        applyFeedbackStyleToLabel(userNotificationLabel, false);
        return;
    }
    if (userAnswerIndices == correctAnswerIndices) {
        playSoundEffect(correctAnswerSound);
        tasksCompletedInSet++;
        taskProgressBar->setValue(tasksCompletedInSet);
        currentTaskIndexInSet++;
        userNotificationLabel->setText(tr("🎉 All correct!"));
        applyFeedbackStyleToLabel(userNotificationLabel, true);
        QTimer::singleShot(1800, this, &MainWindow::presentNextTask);
    } else {
        playSoundEffect(incorrectAnswerSound);
        mistakesMadeInSet++;
        userNotificationLabel->setText(tr("🧐 Some selections were off (Mistake %1/%2).")
                                           .arg(mistakesMadeInSet)
                                           .arg(MAX_MISTAKES_ALLOWED));
        applyFeedbackStyleToLabel(userNotificationLabel, false);
        if (mistakesMadeInSet >= MAX_MISTAKES_ALLOWED) {
            grammarMultiChoiceExerciseWidget->showCorrectAnswers(
                correctAnswerIndices, currentGrammarMultiTaskSet[currentTaskIndexInSet].options);
            QTimer::singleShot(3500, this, [this]() {
                concludeExerciseSession(false);
            });
        }
    }
}

void MainWindow::onMeditateButtonClicked() {
    if (!meditationAudio) {
        return;
    }
    if (meditationToggleButton->isChecked()) {
        playSoundEffect(meditationAudio, true);
        meditationToggleButton->setText(tr("🧘 Stop Meditating"));
        meditationToggleButton->setStyleSheet("QPushButton { background-color: #16a085; ... }");
    } else {
        if (meditationAudio->isPlaying()) {
            meditationAudio->stop();
        }
        meditationToggleButton->setText(tr("🧘 Meditate"));
        meditationToggleButton->setStyleSheet("QPushButton { background-color: #1abc9c; ... }");
    }
}

void MainWindow::stopCurrentExercise() {
    if (activeExerciseType != ExerciseType::None) {
        if (QMessageBox::question(
                this, tr("Stop Exercise"), tr("Stop current exercise? Progress will be lost."),
                QMessageBox::Yes | QMessageBox::No) == QMessageBox::Yes) {
            concludeExerciseSession(false);
        }
    }
}

void MainWindow::updateTimer() {
    secondsRemainingInSession--;
    remainingTimeLabel->setText(QString::number(secondsRemainingInSession) + "s");
    if (secondsRemainingInSession <= 5 && secondsRemainingInSession > 0) {
        remainingTimeLabel->setStyleSheet("color: #d35400; font-weight: bold;");
    } else if (secondsRemainingInSession == 0) {
        remainingTimeLabel->setStyleSheet("color: #c0392b; font-weight: bold;");
    }
    if (secondsRemainingInSession <= 0) {
        exerciseSessionTimer->stop();
        exerciseTimeUp();
    }
}

void MainWindow::exerciseTimeUp() {
    playSoundEffect(timerExpiredSound);
    concludeExerciseSession(false);
}

void MainWindow::concludeExerciseSession(bool wasSuccessful) {
    exerciseSessionTimer->stop();
    if (meditationAudio && meditationAudio->isPlaying()) {
        meditationAudio->stop();
        meditationToggleButton->setChecked(false);
        meditationToggleButton->setText(tr("🧘 Meditate"));
        meditationToggleButton->setStyleSheet("QPushButton { background-color: #1abc9c; ... }");
    }

    ExerciseType endedType = activeExerciseType;
    activeExerciseType = ExerciseType::None;
    exerciseDisplayArea->setCurrentWidget(welcomeMessageLabel);

    bool earnedPoints = false;
    if (wasSuccessful) {
        bool allTasksDone = false;
        if (endedType == ExerciseType::Translation && !currentTranslationTaskSet.isEmpty()) {
            allTasksDone = (tasksCompletedInSet >= currentTranslationTaskSet.size());
        } else if (
            endedType == ExerciseType::GrammarSingle && !currentGrammarSingleTaskSet.isEmpty()) {
            allTasksDone = (tasksCompletedInSet >= currentGrammarSingleTaskSet.size());
        } else if (
            endedType == ExerciseType::GrammarMulti && !currentGrammarMultiTaskSet.isEmpty()) {
            allTasksDone = (tasksCompletedInSet >= currentGrammarMultiTaskSet.size());
        }

        if (allTasksDone && mistakesMadeInSet < MAX_MISTAKES_ALLOWED) {
            totalPlayerScore += 10;
            earnedPoints = true;
        }
    }

    updatePlayerScoreDisplay();
    resetApplicationToIdleState();

    translationModeButton->setEnabled(true);
    grammarSingleModeButton->setEnabled(true);
    grammarMultiModeButton->setEnabled(true);
    stopCurrentExerciseButton->setVisible(false);
    meditationToggleButton->setEnabled(true);
}

void MainWindow::updatePlayerScoreDisplay() {
    currentScoreLabel->setText(QString::number(totalPlayerScore));
}

void MainWindow::resetApplicationToIdleState() {
    tasksCompletedInSet = 0;
    mistakesMadeInSet = 0;
    currentTaskIndexInSet = 0;
    taskProgressBar->setValue(0);
    taskProgressBar->setMaximum(TOTAL_TASKS_PER_EXERCISE);
    remainingTimeLabel->setText(QString::number(EXERCISE_TIME_SECONDS) + "s");
    remainingTimeLabel->setStyleSheet("color: #e74c3c;");

    currentTranslationTaskSet.clear();
    currentGrammarSingleTaskSet.clear();
    currentGrammarMultiTaskSet.clear();

    taskProgressBar->setVisible(false);
    remainingTimeLabel->setVisible(false);
    stopCurrentExerciseButton->setVisible(false);

    if (translationExerciseWidget) {
        translationExerciseWidget->clearAnswer();
    }
    if (grammarMultiChoiceExerciseWidget) {
        grammarMultiChoiceExerciseWidget->clearSelection();
    }

    if (exerciseDisplayArea->currentWidget() != welcomeMessageLabel &&
        activeExerciseType == ExerciseType::None) {
        exerciseDisplayArea->setCurrentWidget(welcomeMessageLabel);
    }
    if (activeExerciseType == ExerciseType::None) {
        userNotificationLabel->setText(tr("Status: Ready for your next learning quest!"));
        userNotificationLabel->setStyleSheet(
            "background-color: #eaf2f8; border-radius: 5px; padding: 5px; color: #34495e;");
    }
}

void MainWindow::showHelp() {
    QString helpText = "У тебя всё получится! Ла-ла-ла-ла";
    QMessageBox::information(this, tr("Help"), helpText);
}