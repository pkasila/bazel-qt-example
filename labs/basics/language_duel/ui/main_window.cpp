#include "ui/main_window.h"

#include <QAction>
#include <algorithm>
#include <cmath>
#include <functional>
#include <initializer_list>
#include <QApplication>
#include <QCheckBox>
#include <QDate>
#include <QDialog>
#include <QDialogButtonBox>
#include <QFrame>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QKeyEvent>
#include <QKeySequence>
#include <QLabel>
#include <QMenuBar>
#include <QMessageBox>
#include <QPainter>
#include <QPainterPath>
#include <QPaintEvent>
#include <QProgressBar>
#include <QPushButton>
#include <QRandomGenerator>
#include <QScrollArea>
#include <QSettings>
#include <QSignalBlocker>
#include <QSizePolicy>
#include <QStackedWidget>
#include <QStatusBar>
#include <QStyle>
#include <QTextEdit>
#include <QTimer>
#include <QVBoxLayout>
#include <QtGlobal>

#include "core/text_matcher.h"
#include "core/semantic_similarity_model.h"
#include "ui/difficulty_dialog.h"
#include "ui/exercise_pages.h"

namespace {
class CatMascotWidget : public QWidget {
public:
    explicit CatMascotWidget(QWidget* parent = nullptr)
        : QWidget(parent) {
        setFixedSize(160, 118);
        setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    }

    void setMood(const QString& mood) {
        mood_ = mood;
        update();
    }

protected:
    void paintEvent(QPaintEvent*) override {
        QPainter painter(this);
        painter.setRenderHint(QPainter::Antialiasing, true);

        const QRectF box = rect().adjusted(5, 5, -5, -5);
        QLinearGradient bg(box.topLeft(), box.bottomRight());
        bg.setColorAt(0.0, QColor("#fff8df"));
        bg.setColorAt(1.0, QColor("#d6f2ff"));
        painter.setPen(QPen(QColor("#23423a"), 2));
        painter.setBrush(bg);
        painter.drawRoundedRect(box, 18, 18);

        painter.setPen(Qt::NoPen);
        painter.setBrush(QColor("#f5b15f"));
        painter.scale(width() / 184.0, height() / 132.0);
        painter.drawEllipse(QRectF(52, 37, 80, 74));

        QPainterPath leftEar;
        leftEar.moveTo(62, 45);
        leftEar.lineTo(73, 16);
        leftEar.lineTo(90, 45);
        leftEar.closeSubpath();
        QPainterPath rightEar;
        rightEar.moveTo(94, 45);
        rightEar.lineTo(113, 16);
        rightEar.lineTo(122, 47);
        rightEar.closeSubpath();
        painter.setBrush(QColor("#e99445"));
        painter.drawPath(leftEar);
        painter.drawPath(rightEar);
        painter.setBrush(QColor("#ffe0b6"));
        painter.drawEllipse(QRectF(67, 31, 13, 18));
        painter.drawEllipse(QRectF(104, 31, 13, 18));

        painter.setBrush(QColor("#fff3dd"));
        painter.drawEllipse(QRectF(70, 73, 20, 17));
        painter.drawEllipse(QRectF(95, 73, 20, 17));

        painter.setPen(QPen(QColor("#263733"), 3, Qt::SolidLine, Qt::RoundCap));
        if (mood_ == "sleepy") {
            painter.drawArc(QRectF(70, 59, 17, 10), 0, 180 * 16);
            painter.drawArc(QRectF(99, 59, 17, 10), 0, 180 * 16);
        } else if (mood_ == "panic") {
            painter.drawEllipse(QRectF(73, 58, 7, 9));
            painter.drawEllipse(QRectF(104, 58, 7, 9));
        } else {
            painter.drawEllipse(QRectF(73, 60, 7, 7));
            painter.drawEllipse(QRectF(104, 60, 7, 7));
        }

        painter.setBrush(QColor("#263733"));
        painter.setPen(Qt::NoPen);
        painter.drawEllipse(QRectF(88, 72, 9, 7));

        painter.setPen(QPen(QColor("#263733"), 2, Qt::SolidLine, Qt::RoundCap));
        if (mood_ == "sad" || mood_ == "panic") {
            painter.drawArc(QRectF(82, 82, 22, 14), 25 * 16, 130 * 16);
        } else {
            painter.drawArc(QRectF(82, 78, 22, 16), 200 * 16, 140 * 16);
        }

        painter.drawLine(72, 77, 39, 69);
        painter.drawLine(72, 82, 38, 83);
        painter.drawLine(111, 77, 145, 69);
        painter.drawLine(111, 82, 146, 83);

        painter.setBrush(QColor("#e99445"));
        painter.setPen(Qt::NoPen);
        painter.drawEllipse(QRectF(43, 101, 25, 15));
        painter.drawEllipse(QRectF(116, 101, 25, 15));

        if (mood_ == "party") {
            painter.setPen(QPen(QColor("#ff5e70"), 3));
            painter.drawLine(35, 34, 25, 20);
            painter.setPen(QPen(QColor("#2a9df4"), 3));
            painter.drawLine(145, 34, 160, 20);
            painter.setPen(QPen(QColor("#f2c230"), 3));
            painter.drawPoint(38, 22);
            painter.drawPoint(151, 29);
            painter.drawPoint(29, 48);
        }
    }

private:
    QString mood_ = "ready";
};

QString formatSeconds(int seconds) {
    const int mins = seconds / 60;
    const int secs = seconds % 60;
    return QString("%1:%2").arg(mins, 2, 10, QLatin1Char('0')).arg(secs, 2, 10, QLatin1Char('0'));
}

QPushButton* makeActionButton(const QString& text, const QIcon& icon, QWidget* parent = nullptr) {
    auto* button = new QPushButton(icon, text, parent);
    button->setMinimumHeight(42);
    button->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    return button;
}
}  // namespace

MainWindow::MainWindow() {
    buildUi();
    loadProgress();
    buildMenu();
    applyStyle();
    initializeAudio();
    refreshHeader();
    resize(1080, 680);
    setMinimumSize(960, 620);
    setWindowTitle("Language Duel");
}

void MainWindow::buildUi() {
    centralHost_ = new QWidget(this);
    setCentralWidget(centralHost_);

    auto* root = new QHBoxLayout(centralHost_);
    root->setContentsMargins(18, 18, 18, 18);
    root->setSpacing(18);

    auto* sidebar = new QWidget(this);
    sidebar->setObjectName("Sidebar");
    sidebar->setFixedWidth(290);
    sidebar->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);

    auto* sideLayout = new QVBoxLayout(sidebar);
    sideLayout->setContentsMargins(16, 16, 16, 16);
    sideLayout->setSpacing(8);

    auto* appTitle = new QLabel("Language Duel");
    appTitle->setObjectName("AppTitle");

    auto* appSubtitle = new QLabel("meow-powered Qt lab");
    appSubtitle->setObjectName("SecondaryLabel");
    appSubtitle->setWordWrap(true);

    catMascot_ = new CatMascotWidget(this);
    catSpeechLabel_ = new QLabel("Professor Cat is watching your grammar.");
    catSpeechLabel_->setObjectName("CatSpeech");
    catSpeechLabel_->setWordWrap(true);
    catSpeechLabel_->setMinimumHeight(44);

    scoreLabel_ = new QLabel("Score: 0");
    difficultyLabel_ = new QLabel("Difficulty: Easy");
    modeLabel_ = new QLabel("Mode: idle");
    streakLabel_ = new QLabel("Combo: x1");
    bestStreakLabel_ = new QLabel("Best streak: 0");
    accuracyLabel_ = new QLabel("Accuracy: --");
    levelLabel_ = new QLabel("Level: 1");
    badgeLabel_ = new QLabel("Badges: none yet");
    aiRiskLabel_ = new QLabel("CatAI risk: --");
    soundLabel_ = new QLabel("Sound: on");
    timerLabel_ = new QLabel("Time: --:--");
    statusLabel_ = new QLabel("Ready to start");
    statusLabel_->setObjectName("StatusLabel");
    statusLabel_->setWordWrap(true);
    badgeLabel_->setWordWrap(true);

    progressBar_ = new QProgressBar(this);
    progressBar_->setRange(0, 100);
    progressBar_->setValue(0);
    progressBar_->setTextVisible(true);

    auto* translationButton = makeActionButton(
        "Start Translation",
        style()->standardIcon(QStyle::SP_FileDialogDetailedView),
        this);
    auto* grammarButton = makeActionButton(
        "Start Grammar",
        style()->standardIcon(QStyle::SP_DialogApplyButton),
        this);
    auto* dailyButton = makeActionButton(
        "Daily Cat Quest",
        style()->standardIcon(QStyle::SP_ComputerIcon),
        this);
    auto* difficultyButton = makeActionButton(
        "Change Difficulty",
        style()->standardIcon(QStyle::SP_FileDialogContentsView),
        this);

    connect(translationButton, &QPushButton::clicked, this, &MainWindow::startTranslation);
    connect(grammarButton, &QPushButton::clicked, this, &MainWindow::startGrammar);
    connect(dailyButton, &QPushButton::clicked, this, &MainWindow::startDailyChallenge);
    connect(difficultyButton, &QPushButton::clicked, this, &MainWindow::chooseDifficulty);

    sideLayout->addWidget(appTitle);
    sideLayout->addWidget(appSubtitle);
    sideLayout->addSpacing(4);
    sideLayout->addWidget(catMascot_, 0, Qt::AlignHCenter);
    sideLayout->addWidget(catSpeechLabel_);
    sideLayout->addSpacing(8);
    sideLayout->addWidget(scoreLabel_);
    sideLayout->addWidget(difficultyLabel_);
    sideLayout->addWidget(modeLabel_);
    sideLayout->addWidget(streakLabel_);
    sideLayout->addWidget(bestStreakLabel_);
    sideLayout->addWidget(accuracyLabel_);
    sideLayout->addWidget(levelLabel_);
    sideLayout->addWidget(badgeLabel_);
    sideLayout->addWidget(aiRiskLabel_);
    sideLayout->addWidget(soundLabel_);
    sideLayout->addWidget(timerLabel_);
    sideLayout->addWidget(progressBar_);
    sideLayout->addWidget(statusLabel_);
    sideLayout->addSpacing(10);
    sideLayout->addWidget(translationButton);
    sideLayout->addWidget(grammarButton);
    sideLayout->addWidget(dailyButton);
    sideLayout->addWidget(difficultyButton);
    sideLayout->addStretch();

    stack_ = new QStackedWidget(this);
    startPage_ = new StartPage(this);
    translationPage_ = new TranslationPage(this);
    grammarPage_ = new GrammarPage(this);

    stack_->addWidget(startPage_);
    stack_->addWidget(translationPage_);
    stack_->addWidget(grammarPage_);

    connect(startPage_, &StartPage::translationRequested, this, &MainWindow::startTranslation);
    connect(startPage_, &StartPage::grammarRequested, this, &MainWindow::startGrammar);
    connect(translationPage_, &TranslationPage::submitRequested, this, &MainWindow::submitTranslation);
    connect(grammarPage_, &GrammarPage::submitRequested, this, &MainWindow::submitGrammar);

    auto* sidebarScroll = new QScrollArea(this);
    sidebarScroll->setObjectName("SidebarScroll");
    sidebarScroll->setWidget(sidebar);
    sidebarScroll->setWidgetResizable(true);
    sidebarScroll->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    sidebarScroll->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    sidebarScroll->setFixedWidth(312);
    sidebarScroll->setFrameShape(QFrame::NoFrame);

    root->addWidget(sidebarScroll, 0);
    root->addWidget(stack_, 1);
    stack_->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    timer_ = new QTimer(this);
    connect(timer_, &QTimer::timeout, this, &MainWindow::tick);

    statusBar()->showMessage("Press H during a session for a contextual hint.");
}

void MainWindow::buildMenu() {
    auto* trainingMenu = menuBar()->addMenu("&Training");

    auto* translationAction = new QAction("Start translation", this);
    translationAction->setShortcut(QKeySequence("Ctrl+T"));
    connect(translationAction, &QAction::triggered, this, &MainWindow::startTranslation);
    trainingMenu->addAction(translationAction);

    auto* grammarAction = new QAction("Start grammar", this);
    grammarAction->setShortcut(QKeySequence("Ctrl+G"));
    connect(grammarAction, &QAction::triggered, this, &MainWindow::startGrammar);
    trainingMenu->addAction(grammarAction);

    auto* dailyAction = new QAction("Daily Cat Quest", this);
    dailyAction->setShortcut(QKeySequence("Ctrl+D"));
    connect(dailyAction, &QAction::triggered, this, &MainWindow::startDailyChallenge);
    trainingMenu->addAction(dailyAction);

    trainingMenu->addSeparator();

    auto* reviewAction = new QAction("Review mistakes", this);
    connect(reviewAction, &QAction::triggered, this, &MainWindow::showLastMistakes);
    trainingMenu->addAction(reviewAction);

    auto* catTipAction = new QAction("Ask Professor Cat", this);
    catTipAction->setShortcut(QKeySequence("Ctrl+M"));
    connect(catTipAction, &QAction::triggered, this, &MainWindow::showCatTip);
    trainingMenu->addAction(catTipAction);

    auto* labsMenu = menuBar()->addMenu("&Labs");

    auto* catAiAction = new QAction("CatAI Tutor Lab", this);
    connect(catAiAction, &QAction::triggered, this, &MainWindow::showCatAiModelCard);
    labsMenu->addAction(catAiAction);

    auto* semanticAction = new QAction("Semantic AI Lab", this);
    semanticAction->setShortcut(QKeySequence("Ctrl+Shift+S"));
    connect(semanticAction, &QAction::triggered, this, &MainWindow::showSemanticLab);
    labsMenu->addAction(semanticAction);

    auto* soundLabAction = new QAction("Sound Lab...", this);
    soundLabAction->setShortcut(QKeySequence("Ctrl+L"));
    connect(soundLabAction, &QAction::triggered, this, &MainWindow::showSoundLab);
    labsMenu->addAction(soundLabAction);

    auto* settingsMenu = menuBar()->addMenu("&Settings");

    auto* difficultyAction = new QAction("Change difficulty...", this);
    connect(difficultyAction, &QAction::triggered, this, &MainWindow::chooseDifficulty);
    settingsMenu->addAction(difficultyAction);

    auto* achievementsAction = new QAction("Achievements...", this);
    connect(achievementsAction, &QAction::triggered, this, &MainWindow::showAchievements);
    settingsMenu->addAction(achievementsAction);

    settingsMenu->addSeparator();

    soundAction_ = new QAction("Sound enabled", this);
    soundAction_->setCheckable(true);
    soundAction_->setChecked(soundEnabled_);
    connect(soundAction_, &QAction::toggled, this, &MainWindow::toggleSound);
    settingsMenu->addAction(soundAction_);

    auto* testSoundAction = new QAction("Test sound", this);
    connect(testSoundAction, &QAction::triggered, this, &MainWindow::testSound);
    settingsMenu->addAction(testSoundAction);

    auto* resetAction = new QAction("Reset local progress", this);
    connect(resetAction, &QAction::triggered, this, &MainWindow::resetProgress);
    settingsMenu->addAction(resetAction);

    auto* helpMenu = menuBar()->addMenu("&Help");
    auto* hintAction = new QAction("Show current hint (H)", this);
    connect(hintAction, &QAction::triggered, this, &MainWindow::showHelp);
    helpMenu->addAction(hintAction);
}

void MainWindow::applyStyle() {
    qApp->setStyleSheet(R"(
        QMainWindow, QWidget {
            background: #f7f2e8;
            color: #1f2e2a;
            font-family: "Avenir Next";
            font-size: 14px;
        }
        QLabel {
            background: transparent;
        }
        #SidebarScroll {
            background: transparent;
            border: none;
        }
        #Sidebar {
            background: qlineargradient(x1:0, y1:0, x2:0, y2:1,
                stop:0 #12392f, stop:0.58 #1e6254, stop:1 #244148);
            border: 1px solid #0f2a25;
            border-radius: 22px;
            color: #fff8e8;
        }
        #Sidebar QLabel {
            color: #fff8e8;
            background: transparent;
        }
        #AppTitle {
            font-size: 30px;
            font-weight: 800;
        }
        #HeroTitle {
            font-size: 34px;
            font-weight: 800;
            color: #163d35;
        }
        #SecondaryLabel {
            color: #45645c;
            font-weight: 600;
        }
        #Sidebar #SecondaryLabel {
            color: #dcefe5;
        }
        #CatSpeech {
            background: #315e55;
            border: 1px solid #6f9f91;
            border-radius: 12px;
            color: #fff8e8;
            padding: 9px;
        }
        QScrollBar:vertical {
            background: #173f37;
            width: 9px;
            border-radius: 4px;
        }
        QScrollBar::handle:vertical {
            background: #f0b85b;
            min-height: 28px;
            border-radius: 4px;
        }
        QScrollBar::add-line:vertical,
        QScrollBar::sub-line:vertical {
            height: 0;
        }
        #StatusLabel {
            background: #fffdf5;
            border: 1px solid #d6cdb8;
            border-radius: 12px;
            color: #283730;
            padding: 10px;
        }
        #StatusLabel[tone="good"] {
            background: #e5f7d7;
            border-color: #80b95d;
            color: #244f17;
        }
        #StatusLabel[tone="warn"] {
            background: #fff1d7;
            border-color: #e0a742;
            color: #6d4300;
        }
        #StatusLabel[tone="bad"] {
            background: #ffe2dd;
            border-color: #e17968;
            color: #7d1f15;
        }
        #TaskLabel {
            font-size: 24px;
            font-weight: 700;
            color: #163d35;
        }
        #Card {
            background: #fffdf7;
            border: 1px solid #ddd1bd;
            border-radius: 16px;
            padding: 14px;
        }
        #MiniCard {
            background: #ffffff;
            border: 1px solid #e2d7c6;
            border-radius: 12px;
            padding: 10px;
        }
        #SoundPad {
            background: #fffdf7;
            border: 1px solid #e2d7c6;
            border-radius: 16px;
        }
        #SoundPadButton {
            min-height: 52px;
            border-radius: 14px;
            padding: 10px 18px;
            font-size: 16px;
        }
        #SoundPadNote {
            color: #3e5f56;
            font-size: 13px;
            font-weight: 650;
            line-height: 1.25;
            padding: 2px 4px 0 4px;
        }
        #TinyBadge {
            background: #244148;
            border-radius: 10px;
            color: #fff8e8;
            font-size: 12px;
            font-weight: 700;
            padding: 5px 8px;
        }
        QPushButton {
            background: #236c5d;
            color: #fff8e8;
            border: none;
            border-radius: 12px;
            font-weight: 700;
            padding: 10px 14px;
            text-align: left;
        }
        QPushButton:hover {
            background: #2c806d;
        }
        QPushButton:pressed {
            background: #164c41;
        }
        QTextEdit {
            background: #fffdf7;
            border: 1px solid #d8c9b1;
            border-radius: 14px;
            padding: 12px;
            selection-background-color: #f4c96b;
        }
        QRadioButton {
            background: #fffdf7;
            border: 1px solid #ded2be;
            border-radius: 12px;
            padding: 12px;
            min-height: 34px;
        }
        QRadioButton:hover {
            border-color: #f0b85b;
            background: #fff6e3;
        }
        QProgressBar {
            border: 1px solid #d4c6ae;
            border-radius: 10px;
            background: #fff7e6;
            text-align: center;
            min-height: 22px;
            color: #1c332d;
            font-weight: 700;
        }
        QProgressBar::chunk {
            background: #f0b85b;
            border-radius: 10px;
        }
        QMenuBar {
            background: #efe4d2;
        }
        QMenuBar::item:selected {
            background: #f7d58a;
            border-radius: 6px;
        }
        QStatusBar {
            background: #efe4d2;
            color: #30443f;
        }
    )");
}

void MainWindow::refreshHeader() {
    scoreLabel_->setText(QString("Score: %1").arg(totalScore_));
    difficultyLabel_->setText("Difficulty: " + PracticeEngine::difficultyToString(engine_.difficulty()));
    startPage_->setDifficultyText(PracticeEngine::difficultyToString(engine_.difficulty()));
    streakLabel_->setText(QString("Combo: x%1").arg(std::max(1, streak_ / 2 + 1)));
    bestStreakLabel_->setText(QString("Best streak: %1").arg(bestStreak_));
    levelLabel_->setText(QString("Level: %1").arg(totalScore_ / 150 + 1));
    badgeLabel_->setText("Badges: " + badgeSummary());

    if (sessionAttempts_ == 0) {
        accuracyLabel_->setText("Accuracy: --");
    } else {
        const int accuracy = (100 * sessionCorrect_) / sessionAttempts_;
        accuracyLabel_->setText(QString("Accuracy: %1%").arg(accuracy));
    }

    if (sessionActive_) {
        const CatAiPrediction prediction = catAi_.predictDetailed(currentAiFeatures());
        currentMistakeRisk_ = prediction.ensembleRisk;
        aiRiskLabel_->setText(
            QString("CatAI risk: %1% (%2, kNN=%3, n=%4)")
                .arg(static_cast<int>(currentMistakeRisk_ * 100.0))
                .arg(CatAiModel::riskLabel(currentMistakeRisk_))
                .arg(prediction.memoryNeighbors)
                .arg(catAi_.examplesSeen()));
    } else {
        aiRiskLabel_->setText(
            QString("CatAI trained: %1 answers, memory %2")
                .arg(catAi_.examplesSeen())
                .arg(catAi_.memorySize()));
    }

    soundLabel_->setText(soundEnabled_ ? "Sound: on" : "Sound: off");
}

void MainWindow::loadProgress() {
    QSettings settings;
    totalScore_ = settings.value("progress/score", 0).toInt();
    bestStreak_ = settings.value("progress/bestStreak", 0).toInt();
    completedSessions_ = settings.value("progress/completedSessions", 0).toInt();
    perfectSessions_ = settings.value("progress/perfectSessions", 0).toInt();
    dailyStreak_ = settings.value("progress/dailyStreak", 0).toInt();
    lastDailyDate_ = settings.value("progress/lastDailyDate").toString();
    unlockedBadges_ = settings.value("progress/badges").toStringList();
    soundEnabled_ = settings.value("settings/soundEnabled", true).toBool();
    catAi_.setWeightsFromStrings(settings.value("ml/catAiWeights").toStringList());
    catAi_.setMemoryFromStrings(settings.value("ml/catAiMemory").toStringList());
    catAi_.setExamplesSeen(settings.value("ml/catAiExamples", 0).toInt());
}

void MainWindow::saveProgress() const {
    QSettings settings;
    settings.setValue("progress/score", totalScore_);
    settings.setValue("progress/bestStreak", bestStreak_);
    settings.setValue("progress/completedSessions", completedSessions_);
    settings.setValue("progress/perfectSessions", perfectSessions_);
    settings.setValue("progress/dailyStreak", dailyStreak_);
    settings.setValue("progress/lastDailyDate", lastDailyDate_);
    settings.setValue("progress/badges", unlockedBadges_);
    settings.setValue("settings/soundEnabled", soundEnabled_);
    settings.setValue("ml/catAiWeights", catAi_.weightsToStrings());
    settings.setValue("ml/catAiMemory", catAi_.memoryToStrings());
    settings.setValue("ml/catAiExamples", catAi_.examplesSeen());
}

void MainWindow::chooseDifficulty() {
    DifficultyDialog dialog(engine_.difficulty(), this);
    if (dialog.exec() == QDialog::Accepted) {
        engine_.setDifficulty(dialog.selectedDifficulty());
        refreshHeader();
        setStatus("Difficulty updated successfully.", "good");
        setCatMood("party", "Professor Cat adjusted the challenge level.");
        statusBar()->showMessage("Difficulty changed.", 2500);
    }
}

void MainWindow::resetProgress() {
    const auto answer = QMessageBox::question(
        this,
        "Reset progress",
        "Reset score, badges and saved statistics for this lab?");

    if (answer != QMessageBox::Yes) {
        return;
    }

    totalScore_ = 0;
    bestStreak_ = 0;
    sessionAttempts_ = 0;
    sessionCorrect_ = 0;
    completedSessions_ = 0;
    perfectSessions_ = 0;
    dailyStreak_ = 0;
    currentMistakeRisk_ = 0.0;
    lastDailyDate_.clear();
    unlockedBadges_.clear();
    mistakeReview_.clear();
    catAi_ = CatAiModel();
    saveProgress();
    refreshHeader();
    setStatus("Local progress was reset.", "warn");
    setCatMood("sleepy", "Clean slate. The cat has hidden the old scoreboard.");
}

void MainWindow::showAchievements() {
    QString text;
    text += QString("Score: %1\n").arg(totalScore_);
    text += QString("Level: %1\n").arg(totalScore_ / 150 + 1);
    text += QString("Best streak: %1\n").arg(bestStreak_);
    text += QString("Completed sessions: %1\n").arg(completedSessions_);
    text += QString("Perfect sessions: %1\n\n").arg(perfectSessions_);
    text += QString("Daily Cat streak: %1\n\n").arg(dailyStreak_);
    text += QString("CatAI training samples: %1\n").arg(catAi_.examplesSeen());
    text += QString("CatAI memory samples: %1\n").arg(catAi_.memorySize());
    text += QString("Current CatAI risk: %1%\n\n").arg(static_cast<int>(currentMistakeRisk_ * 100.0));
    text += "Badges: " + badgeSummary();

    QMessageBox::information(this, "Achievements", text);
}

void MainWindow::showLastMistakes() {
    if (mistakeReview_.isEmpty()) {
        QMessageBox::information(this, "Review mistakes", "No mistakes in the current session yet.");
        return;
    }

    QMessageBox::information(this, "Review mistakes", mistakeReview_.join("\n\n"));
}

void MainWindow::showCatTip() {
    const QString tip = coachSuggestion();
    setCatMood("ready", tip);
    QMessageBox::information(this, "Professor Cat", tip);
}

void MainWindow::showCatAiModelCard() {
    const QStringList weights = catAi_.weightsToStrings();
    const CatAiFeatures features = currentAiFeatures();
    const CatAiPrediction prediction = catAi_.predictDetailed(features);
    const QStringList names = {
        "bias",
        "difficulty",
        "grammar mode",
        "session progress",
        "time pressure",
        "mistake pressure",
        "streak strength",
        "hint pressure",
        "daily challenge",
    };

    QString text;
    text += "CatAI Ensemble is fully local and updates after every submitted answer.\n";
    text += "Model A: online logistic regression. Model B: kNN memory over similar past attempts.\n";
    text += "The final risk is a weighted ensemble of both models.\n\n";
    text += QString("Training samples: %1\n").arg(catAi_.examplesSeen());
    text += QString("Memory samples: %1\n").arg(catAi_.memorySize());
    text += "Learning phase: " + catAi_.learningPhase() + "\n";
    text += QString("Nearest memory neighbors: %1\n").arg(prediction.memoryNeighbors);
    text += QString("Logistic risk: %1%\n").arg(static_cast<int>(prediction.logisticRisk * 100.0));
    text += QString("kNN memory risk: %1%\n").arg(static_cast<int>(prediction.memoryRisk * 100.0));
    text += QString("Ensemble risk: %1% (%2)\n")
        .arg(static_cast<int>(prediction.ensembleRisk * 100.0))
        .arg(CatAiModel::riskLabel(prediction.ensembleRisk));
    text += "Recommendation: " + prediction.recommendation + "\n\n";
    text += "Explainability layer: top feature attributions\n";
    const QStringList explanations = catAi_.explainPrediction(features, 5);
    for (const QString& line : explanations) {
        text += "- " + line + "\n";
    }
    text += "\n";
    text += "Current feature vector:\n";
    text += QString("- difficulty: %1\n").arg(features.difficulty);
    text += QString("- grammar mode: %1\n").arg(features.grammarMode);
    text += QString("- session progress: %1\n").arg(features.progress);
    text += QString("- time pressure: %1\n").arg(features.timePressure);
    text += QString("- mistake pressure: %1\n").arg(features.mistakePressure);
    text += QString("- streak strength: %1\n").arg(features.streakStrength);
    text += QString("- hint pressure: %1\n").arg(features.hintPressure);
    text += QString("- daily challenge: %1\n\n").arg(features.dailyChallenge);
    text += "Features and weights:\n";
    for (int i = 0; i < names.size() && i < weights.size(); ++i) {
        text += QString("- %1: %2\n").arg(names[i], weights[i]);
    }

    QMessageBox::information(this, "CatAI Tutor Lab", text);
}

void MainWindow::showSemanticLab() {
    QDialog dialog(this);
    dialog.setWindowTitle("Semantic AI Lab");
    dialog.resize(760, 620);
    dialog.setMinimumSize(680, 560);

    auto* root = new QVBoxLayout(&dialog);
    root->setContentsMargins(18, 18, 18, 18);
    root->setSpacing(12);

    auto* title = new QLabel("Semantic AI Lab: meaning-based answer checker", &dialog);
    title->setObjectName("HeroTitle");
    title->setWordWrap(true);

    auto* intro = new QLabel(
        "This local model repairs mixed Latin/Cyrillic letters, understands common paraphrases "
        "and compares sentences through semantic concepts instead of exact words. Capabilities: " +
            SemanticSimilarityModel::capabilities().join("; ") + ".",
        &dialog);
    intro->setObjectName("SecondaryLabel");
    intro->setWordWrap(true);

    auto* expectedLabel = new QLabel("Expected answer", &dialog);
    auto* expectedEdit = new QTextEdit(&dialog);
    expectedEdit->setPlainText(QString::fromUtf8("Ей нравится зеленый чай"));
    expectedEdit->setMaximumHeight(88);

    auto* inputLabel = new QLabel("Student answer", &dialog);
    auto* inputEdit = new QTextEdit(&dialog);
    inputEdit->setPlainText(QString::fromUtf8("Oна любит зеленый чай"));
    inputEdit->setMaximumHeight(88);

    auto* compareButton = new QPushButton("Compare meaning", &dialog);
    compareButton->setMinimumHeight(44);

    auto* resultLabel = new QLabel(&dialog);
    resultLabel->setObjectName("StatusLabel");
    resultLabel->setProperty("tone", "info");
    resultLabel->setWordWrap(true);
    resultLabel->setMinimumHeight(240);

    auto updateResult = [expectedEdit, inputEdit, resultLabel]() {
        const QString input = inputEdit->toPlainText();
        const QString expected = expectedEdit->toPlainText();
        const MatchResult match = TextMatcher::compare(input, expected);
        const SemanticMatchResult semantic = SemanticSimilarityModel::compare(input, expected);

        QString text;
        text += "Model: " + SemanticSimilarityModel::modelVersion() + "\n";
        text += match.accepted ? "Result: ACCEPTED\n" : "Result: NOT ACCEPTED\n";
        text += QString("Classic/fuzzy similarity: %1%\n").arg(static_cast<int>(match.similarity * 100.0));
        text += QString("Semantic similarity: %1% (%2 confidence)\n")
            .arg(static_cast<int>(semantic.similarity * 100.0))
            .arg(semantic.confidence);
        text += QString("Concept coverage: %1%, precision: %2%\n")
            .arg(static_cast<int>(semantic.conceptCoverage * 100.0))
            .arg(static_cast<int>(semantic.precision * 100.0));
        text += "Matched concepts: " + semantic.matchedConcepts.join(", ") + "\n";
        text += QString("Missing concepts: ") +
            (semantic.missingConcepts.isEmpty() ? QString("none") : semantic.missingConcepts.join(", ")) + "\n";
        text += QString("Extra concepts: ") +
            (semantic.extraConcepts.isEmpty() ? QString("none") : semantic.extraConcepts.join(", ")) + "\n";
        if (semantic.blockedByGuard) {
            text += "Guard: blocked unsafe semantic match\n";
        }
        text += "Student concepts: " + semantic.inputConcepts.join(", ") + "\n";
        text += "Expected concepts: " + semantic.expectedConcepts.join(", ") + "\n";
        text += "Feedback: " + match.feedback;

        resultLabel->setText(text);
        resultLabel->setProperty("tone", match.accepted ? "good" : "bad");
        resultLabel->style()->unpolish(resultLabel);
        resultLabel->style()->polish(resultLabel);
        resultLabel->update();
    };

    connect(compareButton, &QPushButton::clicked, &dialog, updateResult);

    auto* buttons = new QDialogButtonBox(QDialogButtonBox::Close, &dialog);
    connect(buttons, &QDialogButtonBox::rejected, &dialog, &QDialog::reject);

    root->addWidget(title);
    root->addWidget(intro);
    root->addWidget(expectedLabel);
    root->addWidget(expectedEdit);
    root->addWidget(inputLabel);
    root->addWidget(inputEdit);
    root->addWidget(compareButton);
    root->addWidget(resultLabel);
    root->addWidget(buttons);

    updateResult();
    dialog.exec();
}

void MainWindow::toggleSound(bool enabled) {
    soundEnabled_ = enabled;
    if (soundAction_ && soundAction_->isChecked() != enabled) {
        const QSignalBlocker blocker(soundAction_);
        soundAction_->setChecked(enabled);
    }
    saveProgress();
    refreshHeader();
    setStatus(soundEnabled_ ? "Sound feedback enabled." : "Sound feedback disabled.", "info");
    setCatMood(soundEnabled_ ? "party" : "sleepy",
        soundEnabled_ ? "The cat bell is back online." : "Silent mode. The cat now communicates with intense eye contact.");
}

void MainWindow::testSound() {
    if (!soundEnabled_) {
        setStatus("Sound is disabled. Enable it in Settings first.", "warn");
        setCatMood("sleepy", "Silent mode is active. The cat refuses to ring the bell.");
        return;
    }

    playBeepPattern(3, 120);
    setStatus("Sound test played: three short beeps.", "good");
    setCatMood("party", "Sound check passed. Professor Cat heard the bell.");
}

void MainWindow::showSoundLab() {
    QDialog dialog(this);
    dialog.setWindowTitle("Sound Lab");
    dialog.resize(820, 680);
    dialog.setMinimumSize(720, 640);

    auto* root = new QVBoxLayout(&dialog);
    root->setContentsMargins(18, 18, 18, 18);
    root->setSpacing(12);

    auto* title = new QLabel("Sound Lab: cat-powered feedback console", &dialog);
    title->setObjectName("HeroTitle");
    title->setWordWrap(true);

    auto* intro = new QLabel(
        "All sounds are generated with built-in Qt/system beeps, so the lab stays self-contained. "
        "Use these buttons to demonstrate success, error, timer and playful cat sound patterns.",
        &dialog);
    intro->setObjectName("SecondaryLabel");
    intro->setWordWrap(true);

    auto* soundToggle = new QCheckBox("Sound enabled", &dialog);
    soundToggle->setChecked(soundEnabled_);
    connect(soundToggle, &QCheckBox::toggled, this, &MainWindow::toggleSound);

    auto ensureSound = [this]() {
        if (soundEnabled_) {
            return true;
        }
        setStatus("Sound is disabled. Turn it on in Sound Lab first.", "warn");
        setCatMood("sleepy", "The cat is in silent observer mode.");
        return false;
    };

    auto playTimeline = [this, ensureSound](std::initializer_list<int> offsetsMs) {
        if (!ensureSound()) {
            return false;
        }
        for (int offsetMs : offsetsMs) {
            QTimer::singleShot(offsetMs, this, [] {
                QApplication::beep();
            });
        }
        return true;
    };

    auto* gridHost = new QWidget(&dialog);
    auto* grid = new QGridLayout(gridHost);
    grid->setContentsMargins(0, 0, 0, 0);
    grid->setHorizontalSpacing(14);
    grid->setVerticalSpacing(14);

    auto addPad = [&](int row, int column, const QString& text, const QString& note, const std::function<void()>& handler) {
        auto* card = new QFrame(gridHost);
        card->setObjectName("SoundPad");
        card->setMinimumHeight(128);
        card->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
        auto* cardLayout = new QVBoxLayout(card);
        cardLayout->setContentsMargins(14, 14, 14, 14);
        cardLayout->setSpacing(12);

        auto* button = new QPushButton(text, card);
        button->setObjectName("SoundPadButton");
        button->setMinimumHeight(52);
        button->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
        auto* label = new QLabel(note, card);
        label->setObjectName("SoundPadNote");
        label->setWordWrap(true);
        label->setMinimumHeight(34);
        label->setAlignment(Qt::AlignLeft | Qt::AlignTop);

        connect(button, &QPushButton::clicked, this, [handler] {
            handler();
        });
        cardLayout->addWidget(button);
        cardLayout->addWidget(label);
        grid->addWidget(card, row, column);
    };

    addPad(0, 0, "Success chime", "Two quick beeps for correct answers.", [this, ensureSound] {
        if (!ensureSound()) {
            return;
        }
        playPositiveFeedback();
        setStatus("Sound Lab played the success chime.", "good");
        setCatMood("party", "Correct-answer chime armed. The cat approves the sparkle.");
    });
    addPad(0, 1, "Error thump", "One short beep for wrong answers.", [this, ensureSound] {
        if (!ensureSound()) {
            return;
        }
        playNegativeFeedback();
        setStatus("Sound Lab played the error thump.", "warn");
        setCatMood("sad", "Error sound tested. The cat calls it educational, not tragic.");
    });
    addPad(1, 0, "Timer panic", "Fast warning pattern for the last seconds.", [this, ensureSound] {
        if (!ensureSound()) {
            return;
        }
        playAlertFeedback();
        setStatus("Sound Lab played the timer warning.", "warn");
        setCatMood("panic", "Timer alarm tested. The cat has stopped blinking.");
    });
    addPad(1, 1, "Combo beat", "A playful rhythm for streak moments.", [playTimeline, this] {
        if (!playTimeline({0, 80, 160, 340, 420})) {
            return;
        }
        setStatus("Sound Lab played the combo beat.", "good");
        setCatMood("party", "Combo beat online. The cat is basically a tiny drummer.");
    });
    addPad(2, 0, "Meow Morse", "A silly long-short pattern for cat flavor.", [playTimeline, this] {
        if (!playTimeline({0, 110, 220, 520, 640, 760})) {
            return;
        }
        setStatus("Sound Lab played Meow Morse.", "good");
        setCatMood("party", "Meow Morse transmitted. Message: feed the streak.");
    });
    addPad(2, 1, "Focus metronome", "Four steady ticks for demoing audio timing.", [playTimeline, this] {
        if (!playTimeline({0, 250, 500, 750})) {
            return;
        }
        setStatus("Sound Lab played the focus metronome.", "info");
        setCatMood("ready", "Metronome tested. The cat is now counting grammar beats.");
    });

    auto* buttons = new QDialogButtonBox(QDialogButtonBox::Close, &dialog);
    connect(buttons, &QDialogButtonBox::rejected, &dialog, &QDialog::reject);

    root->addWidget(title);
    root->addWidget(intro);
    root->addWidget(soundToggle);
    root->addWidget(gridHost);
    root->addWidget(buttons);

    dialog.exec();
}

void MainWindow::startTranslation() {
    dailyChallengeActive_ = false;
    startSession(ExerciseMode::Translation);
}

void MainWindow::startGrammar() {
    dailyChallengeActive_ = false;
    startSession(ExerciseMode::Grammar);
}

void MainWindow::startDailyChallenge() {
    dailyChallengeActive_ = true;
    engine_.setDifficulty(Difficulty::Hard);
    refreshHeader();

    const ExerciseMode mode =
        (QDate::currentDate().dayOfYear() % 2 == 0) ? ExerciseMode::Translation : ExerciseMode::Grammar;
    setCatMood("party", "Daily Cat Quest loaded: hard mode, fresh tasks, extra bragging rights.");
    startSession(mode);
}

void MainWindow::startSession(ExerciseMode mode) {
    currentSession_ = engine_.createSession(mode);
    currentIndex_ = 0;
    remainingSeconds_ = currentSession_.config.timeLimitSeconds;
    mistakes_ = 0;
    streak_ = 0;
    sessionAttempts_ = 0;
    sessionCorrect_ = 0;
    advancedAccepts_ = 0;
    semanticAccepts_ = 0;
    usedHints_ = 0;
    mistakeReview_.clear();
    sessionActive_ = true;

    progressBar_->setRange(0, currentSession_.config.taskCount);
    progressBar_->setValue(0);
    progressBar_->setFormat(QString("Task 0/%1").arg(currentSession_.config.taskCount));
    timer_->start(1000);
    refreshHeader();

    modeLabel_->setText(mode == ExerciseMode::Translation ? "Mode: translation" : "Mode: grammar");
    timerLabel_->setText("Time: " + formatSeconds(remainingSeconds_));
    setStatus("Session started. New randomized task set is ready.", "info");
    setCatMood("ready", mode == ExerciseMode::Translation
        ? "Translation hunt started. The cat expects elegant wording."
        : "Grammar hunt started. The cat is inspecting every verb.");

    advanceTask();
}

void MainWindow::advanceTask() {
    const int total = currentSession_.config.taskCount;
    progressBar_->setValue(currentIndex_);
    progressBar_->setFormat(QString("Task %1/%2").arg(currentIndex_).arg(total));

    if (currentIndex_ >= total) {
        finishSession(true, "All tasks completed.");
        return;
    }

    if (currentSession_.config.mode == ExerciseMode::Translation) {
        translationPage_->setTask(currentSession_.translationTasks[currentIndex_]);
        stack_->setCurrentWidget(translationPage_);
        translationPage_->focusInput();
    } else {
        grammarPage_->setTask(currentSession_.grammarTasks[currentIndex_]);
        stack_->setCurrentWidget(grammarPage_);
    }

    refreshCatAiPrediction();
    if (currentMistakeRisk_ >= 0.72) {
        setCatMood("panic", "CatAI sees a risky task. A hint might be worth the tiny score penalty.");
    }
}

void MainWindow::submitTranslation() {
    if (!sessionActive_) {
        return;
    }

    const auto& task = currentSession_.translationTasks[currentIndex_];
    const QString answer = translationPage_->currentAnswer();
    if (answer.isEmpty()) {
        playNegativeFeedback();
        setStatus("Type a translation before submitting. Empty answers do not count as attempts.", "warn");
        setCatMood("sleepy", randomCatNudge());
        return;
    }

    ++sessionAttempts_;
    const MatchResult result = TextMatcher::compare(answer, task.expected);

    if (result.accepted) {
        trainCatAi(false);
        ++sessionCorrect_;
        if (!result.exactMatch) {
            ++advancedAccepts_;
        }
        if (result.acceptedBySemantic) {
            ++semanticAccepts_;
            unlockBadge(result.semanticConfidence == "high" ? "Semantic AI Pro" : "Semantic AI");
        }
        ++streak_;
        bestStreak_ = std::max(bestStreak_, streak_);
        playPositiveFeedback();
        setStatus(
            QString("%1 Similarity: %2%, word coverage: %3%, semantic match: %4% (%5 confidence).")
                .arg(result.feedback)
                .arg(static_cast<int>(result.similarity * 100.0))
                .arg(static_cast<int>(result.tokenCoverage * 100.0))
                .arg(static_cast<int>(result.semanticSimilarity * 100.0))
                .arg(result.semanticConfidence),
            result.acceptedByWordOrder ? "warn" : "good");
        setCatMood(result.acceptedByWordOrder ? "party" : "party", randomCatCheer());
        ++currentIndex_;
        advanceTask();
        refreshHeader();
        return;
    }

    streak_ = 0;
    trainCatAi(true);
    ++mistakes_;
    playNegativeFeedback();
    rememberMistake(task.prompt, task.expected);
    refreshHeader();

    setStatus(
        QString("Not accepted (%1/%2 mistakes). Expected close to: “%3”")
            .arg(mistakes_)
            .arg(currentSession_.config.maxMistakes)
            .arg(task.expected),
        "bad");
    setCatMood("sad", randomCatNudge());

    if (mistakes_ >= currentSession_.config.maxMistakes) {
        failByMistakeLimit();
    }
}

void MainWindow::submitGrammar() {
    if (!sessionActive_) {
        return;
    }

    const auto& task = currentSession_.grammarTasks[currentIndex_];
    if (grammarPage_->selectedIndex() < 0) {
        playNegativeFeedback();
        setStatus("Choose one answer before submitting. This does not count as a mistake.", "warn");
        setCatMood("sleepy", "The cat gently taps one radio button with a paw. Pick an answer first.");
        return;
    }

    ++sessionAttempts_;
    if (grammarPage_->selectedIndex() == task.correctIndex) {
        trainCatAi(false);
        ++sessionCorrect_;
        ++streak_;
        bestStreak_ = std::max(bestStreak_, streak_);
        playPositiveFeedback();
        setStatus("Correct choice. Moving on!", "good");
        setCatMood("party", randomCatCheer());
        ++currentIndex_;
        advanceTask();
        refreshHeader();
        return;
    }

    streak_ = 0;
    trainCatAi(true);
    ++mistakes_;
    playNegativeFeedback();
    rememberMistake(task.sentence, task.options[task.correctIndex]);
    refreshHeader();

    setStatus(
        QString("Wrong choice (%1/%2 mistakes). Correct answer: %3")
            .arg(mistakes_)
            .arg(currentSession_.config.maxMistakes)
            .arg(task.options[task.correctIndex]),
        "bad");
    setCatMood("sad", randomCatNudge());
    grammarPage_->clearSelection();

    if (mistakes_ >= currentSession_.config.maxMistakes) {
        failByMistakeLimit();
    }
}

void MainWindow::tick() {
    if (!sessionActive_) {
        return;
    }

    --remainingSeconds_;
    timerLabel_->setText("Time: " + formatSeconds(std::max(0, remainingSeconds_)));
    currentMistakeRisk_ = catAi_.predictMistakeRisk(currentAiFeatures());
    refreshHeader();

    if (remainingSeconds_ <= 0) {
        finishSession(false, "Time is over.", true, false);
    } else if (remainingSeconds_ <= 10) {
        playAlertFeedback();
        setStatus("Final seconds. Finish the current answer quickly.", "warn");
        setCatMood("panic", "Ten seconds. Even the cat stopped blinking.");
    }
}

void MainWindow::finishSession(bool success, const QString& reason, bool failedByTime, bool failedByMistakes) {
    timer_->stop();
    sessionActive_ = false;
    stack_->setCurrentWidget(startPage_);
    progressBar_->setValue(success ? currentSession_.config.taskCount : progressBar_->value());

    SessionStats stats;
    stats.completed = currentIndex_;
    stats.total = currentSession_.config.taskCount;
    stats.mistakes = mistakes_;
    stats.maxMistakes = currentSession_.config.maxMistakes;
    stats.streak = streak_;
    stats.finishedSuccessfully = success;
    stats.failedByTime = failedByTime;
    stats.failedByMistakes = failedByMistakes;
    stats.title = (currentSession_.config.mode == ExerciseMode::Translation) ? "Translation session" : "Grammar session";

    QString summary;
    summary += stats.title + "\n\n";
    summary += "Difficulty: " + PracticeEngine::difficultyToString(currentSession_.config.difficulty) + "\n";
    summary += QString("Completed tasks: %1 / %2\n").arg(stats.completed).arg(stats.total);
    summary += QString("Mistakes: %1 / %2\n").arg(stats.mistakes).arg(stats.maxMistakes);
    summary += QString("Attempts: %1\n").arg(sessionAttempts_);
    summary += QString("Accuracy: %1%\n").arg(sessionAttempts_ == 0 ? 0 : (100 * sessionCorrect_) / sessionAttempts_);
    summary += QString("Hints used: %1\n").arg(usedHints_);
    summary += QString("Advanced matcher accepts: %1\n").arg(advancedAccepts_);
    summary += QString("Semantic AI accepts: %1\n").arg(semanticAccepts_);
    summary += QString("CatAI risk before finish: %1%\n").arg(static_cast<int>(currentMistakeRisk_ * 100.0));
    summary += QString("CatAI training samples: %1\n").arg(catAi_.examplesSeen());

    if (success) {
        ++completedSessions_;
        unlockBadge("First Win");
        if (mistakes_ == 0) {
            ++perfectSessions_;
            unlockBadge("Perfect Run");
        }
        if (bestStreak_ >= 5) {
            unlockBadge("Hot Streak");
        }
        if (remainingSeconds_ >= currentSession_.config.timeLimitSeconds / 3) {
            unlockBadge("Speed Finish");
        }
        if (currentSession_.config.difficulty == Difficulty::Hard) {
            unlockBadge("Hard Mode");
        }
        if (advancedAccepts_ > 0) {
            unlockBadge("Fuzzy Master");
        }
        if (dailyChallengeActive_) {
            updateDailyProgress();
            unlockBadge("Daily Cat");
        }

        const int reward = computeReward();
        totalScore_ += reward;
        if (totalScore_ >= 300) {
            unlockBadge("Scholar");
        }
        stats.scoreAwarded = reward;
        summary += QString("Reward: +%1 points\n").arg(reward);
        summary += QString("New total score: %1\n\n").arg(totalScore_);
        summary += "Result: success\n";
        setStatus("Session completed successfully. Score and badges were updated.", "good");
        setCatMood("party", "Professor Cat signs this session with a golden paw.");
    } else {
        summary += "Reward: +0 points\n";
        summary += QString("Total score stays: %1\n\n").arg(totalScore_);
        summary += "Result: failed\n";
        setStatus(reason, "bad");
        setCatMood(failedByTime ? "panic" : "sad", "The cat suggests one more run after a tiny stretch.");
    }

    dailyChallengeActive_ = false;

    if (failedByTime) {
        summary += "Cause: time limit exceeded.\n";
    } else if (failedByMistakes) {
        summary += "Cause: too many mistakes.\n";
    } else {
        summary += "Cause: full completion.\n";
    }

    summary += "\nBonus features used:\n";
    summary += "- audio feedback and interactive Sound Lab\n";
    summary += "- fuzzy text comparison with word-order tolerance\n";
    summary += "- guarded semantic similarity model with confidence scoring\n";
    summary += "- randomized sessions\n";
    summary += "- combo multiplier, badges and persistent progress\n";
    summary += "- CatAI local ensemble coach: logistic regression plus kNN memory\n";

    if (!mistakeReview_.isEmpty()) {
        summary += "\nMistake review:\n";
        summary += mistakeReview_.join("\n");
        summary += "\n";
    }

    saveProgress();
    refreshHeader();

    QMessageBox::information(this, success ? "Session finished" : "Session ended", summary);
}

void MainWindow::failByMistakeLimit() {
    finishSession(false, "Too many mistakes. Session stopped.", false, true);
}

void MainWindow::initializeAudio() {
    // Built-in system beeps keep the lab self-contained and avoid extra multimedia setup.
}

void MainWindow::showHelp() {
    const QString text = currentHelpText();
    if (sessionActive_) {
        ++usedHints_;
        refreshHeader();
    }
    QMessageBox::information(this, "Hint", text.isEmpty() ? "No active exercise. Start a session first." : text);
    if (sessionActive_) {
        setCatMood("ready", randomCatHint());
    } else {
        setCatMood("sleepy", "Start a session and the cat will reveal useful secrets.");
    }
}

void MainWindow::playPositiveFeedback() {
    playBeepPattern(2, 90);
}

void MainWindow::playNegativeFeedback() {
    playBeepPattern(1, 0);
}

void MainWindow::playAlertFeedback() {
    playBeepPattern(2, 60);
}

void MainWindow::playBeepPattern(int count, int intervalMs) {
    if (!soundEnabled_ || count <= 0) {
        return;
    }

    for (int i = 0; i < count; ++i) {
        QTimer::singleShot(i * intervalMs, this, [] {
            QApplication::beep();
        });
    }
}

void MainWindow::setStatus(const QString& message, const QString& tone) {
    statusLabel_->setText(message);
    statusLabel_->setProperty("tone", tone);
    statusLabel_->style()->unpolish(statusLabel_);
    statusLabel_->style()->polish(statusLabel_);
    statusLabel_->update();
}

void MainWindow::setCatMood(const QString& mood, const QString& speech) {
    if (auto* mascot = dynamic_cast<CatMascotWidget*>(catMascot_)) {
        mascot->setMood(mood);
    }

    if (catSpeechLabel_) {
        catSpeechLabel_->setText(speech);
    }
}

void MainWindow::unlockBadge(const QString& badge) {
    if (!unlockedBadges_.contains(badge)) {
        unlockedBadges_.push_back(badge);
    }
}

QString MainWindow::badgeSummary() const {
    if (unlockedBadges_.isEmpty()) {
        return "none yet";
    }

    return unlockedBadges_.join(", ");
}

void MainWindow::rememberMistake(const QString& title, const QString& expected) {
    mistakeReview_.push_back(QString("%1\nExpected: %2").arg(title, expected));
}

QString MainWindow::randomCatCheer() const {
    const QStringList lines = {
        "Paw-sitive result. The cat approves.",
        "Clean answer. Professor Cat starts purring.",
        "That one landed softly on all four paws.",
        "Excellent. The grammar whiskers are aligned.",
        "Meowvelous. Keep the combo warm."
    };
    return lines[QRandomGenerator::global()->bounded(static_cast<int>(lines.size()))];
}

QString MainWindow::randomCatNudge() const {
    const QStringList lines = {
        "Tiny stumble. The cat says: slow down and try the core meaning.",
        "Almost. Check the verb form before the cat checks it for you.",
        "The cat squints at this sentence. Something is off.",
        "No panic. One careful reread usually saves the tail.",
        "Professor Cat recommends fewer heroic guesses."
    };
    return lines[QRandomGenerator::global()->bounded(static_cast<int>(lines.size()))];
}

QString MainWindow::randomCatHint() const {
    const QStringList lines = {
        "Cat tip: translate the meaning first, then polish the word order.",
        "Cat tip: in grammar tasks, find the time marker before choosing.",
        "Cat tip: short answers are dangerous when the sentence has conditionals.",
        "Cat tip: punctuation is forgiven, but missing key words are not.",
        "Cat tip: use H for a focused hint, not for surrender."
    };
    return lines[QRandomGenerator::global()->bounded(static_cast<int>(lines.size()))];
}

QString MainWindow::coachSuggestion() const {
    if (!sessionActive_) {
        if (dailyStreak_ >= 2) {
            return QString("Coach: daily streak %1. Keep it alive with Daily Cat Quest.").arg(dailyStreak_);
        }
        if (totalScore_ < 150) {
            return "Coach: start with Easy translation, then switch to Grammar once the score warms up.";
        }
        return "Coach: try Hard mode. The app already forgives typos, but not missing meaning.";
    }

    if (remainingSeconds_ <= 15) {
        return "Coach: time is low. Submit the best complete answer instead of perfecting punctuation.";
    }
    if (currentMistakeRisk_ >= 0.70) {
        return QString("Coach: CatAI estimates %1% mistake risk. Use H, then answer the core meaning first.")
            .arg(static_cast<int>(currentMistakeRisk_ * 100.0));
    }
    if (currentMistakeRisk_ <= 0.28 && streak_ >= 2) {
        return "Coach: CatAI sees low risk. Push the streak and save hints for later.";
    }
    if (mistakes_ + 1 >= currentSession_.config.maxMistakes) {
        return "Coach: one more mistake can end the session. Use H and read the sentence slowly.";
    }
    if (currentSession_.config.mode == ExerciseMode::Translation) {
        return "Coach: preserve the main nouns and verbs first. Word order can be flexible.";
    }
    return "Coach: grammar questions usually hide the answer in tense markers like now, yesterday, by next year.";
}

CatAiFeatures MainWindow::currentAiFeatures() const {
    CatAiFeatures features;

    switch (currentSession_.config.difficulty) {
        case Difficulty::Easy:
            features.difficulty = 0.0;
            break;
        case Difficulty::Medium:
            features.difficulty = 0.5;
            break;
        case Difficulty::Hard:
            features.difficulty = 1.0;
            break;
    }

    features.grammarMode = currentSession_.config.mode == ExerciseMode::Grammar ? 1.0 : 0.0;
    features.progress = currentSession_.config.taskCount <= 1
        ? 0.0
        : static_cast<double>(currentIndex_) / static_cast<double>(currentSession_.config.taskCount - 1);
    features.timePressure = currentSession_.config.timeLimitSeconds <= 0
        ? 0.0
        : 1.0 - static_cast<double>(std::max(0, remainingSeconds_)) /
            static_cast<double>(currentSession_.config.timeLimitSeconds);
    features.mistakePressure = currentSession_.config.maxMistakes <= 0
        ? 0.0
        : static_cast<double>(mistakes_) / static_cast<double>(currentSession_.config.maxMistakes);
    features.streakStrength = std::min(1.0, static_cast<double>(streak_) / 6.0);
    features.hintPressure = std::min(1.0, static_cast<double>(usedHints_) / 3.0);
    features.dailyChallenge = dailyChallengeActive_ ? 1.0 : 0.0;

    return features;
}

void MainWindow::refreshCatAiPrediction() {
    if (!sessionActive_) {
        currentMistakeRisk_ = 0.0;
        refreshHeader();
        return;
    }

    currentMistakeRisk_ = catAi_.predictMistakeRisk(currentAiFeatures());
    refreshHeader();
}

void MainWindow::trainCatAi(bool mistakeHappened) {
    catAi_.train(currentAiFeatures(), mistakeHappened);

    if (catAi_.examplesSeen() >= 8) {
        unlockBadge("CatAI Trainee");
    }
    if (catAi_.examplesSeen() >= 25) {
        unlockBadge("CatAI Whisperer");
    }
    if (catAi_.memorySize() >= 15) {
        unlockBadge("CatAI Ensemble");
    }
    if (catAi_.examplesSeen() >= 35) {
        unlockBadge("Explainable CatAI");
    }

    saveProgress();
}

void MainWindow::updateDailyProgress() {
    const QString today = QDate::currentDate().toString(Qt::ISODate);
    const QString yesterday = QDate::currentDate().addDays(-1).toString(Qt::ISODate);

    if (lastDailyDate_ == today) {
        return;
    }

    dailyStreak_ = (lastDailyDate_ == yesterday) ? dailyStreak_ + 1 : 1;
    lastDailyDate_ = today;

    if (dailyStreak_ >= 3) {
        unlockBadge("Three-Day Cat");
    }
}

int MainWindow::computeReward() const {
    int base = 40;
    switch (currentSession_.config.difficulty) {
        case Difficulty::Easy:
            base = 40;
            break;
        case Difficulty::Medium:
            base = 70;
            break;
        case Difficulty::Hard:
            base = 100;
            break;
    }

    const int comboMultiplier = std::max(1, streak_ / 2 + 1);
    const int accuracyBonus = (mistakes_ == 0) ? 25 : std::max(0, 12 - mistakes_ * 4);
    const int speedBonus = std::max(0, remainingSeconds_ / 6);
    const int hintPenalty = usedHints_ * 2;
    const int dailyBonus = dailyChallengeActive_ ? 35 : 0;
    return std::max(10, base + 5 * comboMultiplier + accuracyBonus + speedBonus + dailyBonus - hintPenalty);
}

QString MainWindow::currentHelpText() const {
    if (!sessionActive_) {
        return {};
    }

    if (currentSession_.config.mode == ExerciseMode::Translation &&
        currentIndex_ < static_cast<int>(currentSession_.translationTasks.size())) {
        return currentSession_.translationTasks[currentIndex_].help;
    }

    if (currentSession_.config.mode == ExerciseMode::Grammar &&
        currentIndex_ < static_cast<int>(currentSession_.grammarTasks.size())) {
        return currentSession_.grammarTasks[currentIndex_].help;
    }

    return {};
}

void MainWindow::keyPressEvent(QKeyEvent* event) {
    if (event->key() == Qt::Key_H) {
        showHelp();
        event->accept();
        return;
    }
    if ((event->key() == Qt::Key_Return || event->key() == Qt::Key_Enter) &&
        event->modifiers().testFlag(Qt::ControlModifier)) {
        if (sessionActive_ && currentSession_.config.mode == ExerciseMode::Translation) {
            submitTranslation();
            event->accept();
            return;
        }
        if (sessionActive_ && currentSession_.config.mode == ExerciseMode::Grammar) {
            submitGrammar();
            event->accept();
            return;
        }
    }
    QMainWindow::keyPressEvent(event);
}
