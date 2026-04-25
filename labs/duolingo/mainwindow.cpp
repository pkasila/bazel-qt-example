#include "mainwindow.h"
#include <QApplication>
#include <QMessageBox>
#include <QFont>
#include <QtMath>
#include <QDialogButtonBox>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QMenuBar>
#include <QKeyEvent>
#include <QRandomGenerator>
#include <QIcon>
#include <QPalette>
#include <QStyleFactory>
#include <QSoundEffect>

static const char* const DARK_BG = "#1A1A1A";
static const char* const CARD_BG = "#2A2A2A";
static const char* const ORANGE_PRIMARY = "#FF6600";
static const char* const ORANGE_HOVER = "#FF8533";
static const char* const ORANGE_PRESSED = "#E05500";
static const char* const TEXT_PRIMARY = "#E0E0E0";
static const char* const TEXT_SECONDARY = "#A0A0A0";

DifficultyDialog::DifficultyDialog(QWidget *parent) : QDialog(parent) {
    setWindowTitle("Select Difficulty");
    setMinimumSize(320, 280);
    setStyleSheet(QString("QDialog { background-color: %1; }").arg(DARK_BG));
    
    auto *layout = new QVBoxLayout(this);
    layout->setSpacing(20);
    layout->setContentsMargins(25, 25, 25, 25);
    
    auto *title = new QLabel("CHOOSE DIFFICULTY");
    title->setAlignment(Qt::AlignCenter);
    title->setStyleSheet(QString(
        "font-size: 18px; font-weight: bold; color: %1; padding: 10px;"
    ).arg(ORANGE_PRIMARY));
    layout->addWidget(title);
    
    easyRadio_ = new QRadioButton("Easy (10 tasks)");
    mediumRadio_ = new QRadioButton("Medium (25 tasks)");
    hardRadio_ = new QRadioButton("Hard (50 tasks)");
    mediumRadio_->setChecked(true);
    
    QString radioStyle = QString(
        "QRadioButton { color: %1; font-size: 14px; padding: 8px; spacing: 10px; }"
        "QRadioButton::indicator { width: 20px; height: 20px; border-radius: 10px; }"
        "QRadioButton::indicator::unchecked { border: 2px solid %2; background-color: %3; }"
        "QRadioButton::indicator::checked { border: 2px solid %2; background-color: %2; }"
        "QRadioButton:hover { color: %4; }"
    ).arg(TEXT_PRIMARY).arg(ORANGE_PRIMARY).arg(CARD_BG).arg(ORANGE_HOVER);
    
    for (auto *r : {easyRadio_, mediumRadio_, hardRadio_}) {
        r->setStyleSheet(radioStyle);
        layout->addWidget(r);
    }
    
    auto *btns = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
    btns->setStyleSheet(QString(
        "QPushButton { background-color: %1; color: %2; border: none; padding: 10px 25px; "
        "font-size: 14px; font-weight: bold; border-radius: 6px; min-width: 80px; }"
        "QPushButton:hover { background-color: %3; }"
        "QPushButton:pressed { background-color: %4; }"
    ).arg(ORANGE_PRIMARY).arg(DARK_BG).arg(ORANGE_HOVER).arg(ORANGE_PRESSED));
    
    connect(btns, &QDialogButtonBox::accepted, this, [this]{
        if (easyRadio_->isChecked()) difficulty_ = 1;
        else if (hardRadio_->isChecked()) difficulty_ = 3;
        else difficulty_ = 2;
        accept();
    });
    connect(btns, &QDialogButtonBox::rejected, this, &QDialog::reject);
    layout->addWidget(btns);
}

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {
    qApp->setStyle(QStyleFactory::create("Fusion"));
    QPalette darkPalette;
    darkPalette.setColor(QPalette::Window, QColor(26, 26, 26));
    darkPalette.setColor(QPalette::WindowText, QColor(224, 224, 224));
    darkPalette.setColor(QPalette::Base, QColor(42, 42, 42));
    darkPalette.setColor(QPalette::AlternateBase, QColor(58, 58, 58));
    darkPalette.setColor(QPalette::ToolTipBase, Qt::white);
    darkPalette.setColor(QPalette::ToolTipText, Qt::white);
    darkPalette.setColor(QPalette::Text, QColor(224, 224, 224));
    darkPalette.setColor(QPalette::Button, QColor(42, 42, 42));
    darkPalette.setColor(QPalette::ButtonText, QColor(224, 224, 224));
    darkPalette.setColor(QPalette::BrightText, Qt::red);
    darkPalette.setColor(QPalette::Link, QColor(255, 102, 0));
    darkPalette.setColor(QPalette::Highlight, QColor(255, 102, 0));
    darkPalette.setColor(QPalette::HighlightedText, Qt::black);
    qApp->setPalette(darkPalette);
    
    audioOutput_ = new QAudioOutput(this);
    audioOutput_->setVolume(0.8);
    
    correctPlayer_ = new QMediaPlayer(this);
    correctPlayer_->setAudioOutput(audioOutput_);
    correctPlayer_->setSource(QUrl::fromLocalFile("labs/duolingo/resources/correct.mp3"));
    
    wrongPlayer_ = new QMediaPlayer(this);
    wrongPlayer_->setAudioOutput(audioOutput_);
    wrongPlayer_->setSource(QUrl::fromLocalFile("labs/duolingo/resources/wrong.mp3"));
    
    setupMenuBar();
    setupUI();
}

MainWindow::~MainWindow() {
    if (correctPlayer_) correctPlayer_->stop();
    if (wrongPlayer_) wrongPlayer_->stop();
}

void MainWindow::setupMenuBar() {
    auto *bar = new QMenuBar(this);
    bar->setStyleSheet(QString(
        "QMenuBar { background-color: %1; color: %2; border-bottom: 2px solid %3; }"
        "QMenuBar::item { padding: 8px 15px; }"
        "QMenuBar::item:selected { background-color: %3; color: %4; }"
        "QMenu { background-color: %1; color: %2; border: 1px solid %3; }"
        "QMenu::item:selected { background-color: %3; color: %4; }"
    ).arg(CARD_BG).arg(TEXT_PRIMARY).arg(ORANGE_PRIMARY).arg(DARK_BG));
    setMenuBar(bar);
    
    auto *game = bar->addMenu("Game");
    auto *diff = game->addAction("Difficulty...");
    connect(diff, &QAction::triggered, this, &MainWindow::selectDifficulty);
    
    auto *help = bar->addMenu("Help");
    auto *hint = help->addAction("How to Play");
    connect(hint, &QAction::triggered, this, &MainWindow::showHelp);
}

void MainWindow::setupUI() {
    stacked_ = new QStackedWidget(this);
    setCentralWidget(stacked_);
    
    setStyleSheet(QString(
        "QMainWindow { background-color: %1; }"
        "QLabel { color: %2; }"
        "QProgressBar { border: 2px solid %3; border-radius: 4px; text-align: center; "
        "color: white; background-color: %4; min-height: 20px; }"
        "QProgressBar::chunk { background-color: %5; border-radius: 2px; }"
        "QTextEdit { background-color: %4; border: 2px solid %5; border-radius: 8px; "
        "color: %2; font-size: 14px; padding: 10px; }"
        "QTextEdit:focus { border-color: %6; }"
        "QPushButton { background-color: %5; color: %7; border: none; border-radius: 8px; "
        "font-size: 14px; font-weight: bold; padding: 12px 20px; }"
        "QPushButton:hover { background-color: %6; }"
        "QPushButton:pressed { background-color: %8; }"
        "QRadioButton { color: %2; font-size: 14px; padding: 5px; }"
    ).arg(DARK_BG).arg(TEXT_PRIMARY).arg(ORANGE_PRIMARY).arg(CARD_BG)
      .arg(ORANGE_PRIMARY).arg(ORANGE_HOVER).arg(DARK_BG).arg(ORANGE_PRESSED));
    
    menuWidget_ = new QWidget();
    menuWidget_->setStyleSheet(QString("background-color: %1;").arg(DARK_BG));
    auto *menuLayout = new QVBoxLayout(menuWidget_);
    menuLayout->setSpacing(35);
    menuLayout->setContentsMargins(0, 40, 0, 40);
    
    auto *title = new QLabel("LANGUAGE MASTER");
    title->setAlignment(Qt::AlignCenter);
    title->setStyleSheet(QString(
        "font-size: 42px; font-weight: bold; color: %1; letter-spacing: 4px; padding: 25px;"
    ).arg(ORANGE_PRIMARY));
    menuLayout->addWidget(title);
    
    auto *subtitle = new QLabel("Learn with Modern Technology");
    subtitle->setAlignment(Qt::AlignCenter);
    subtitle->setStyleSheet(QString("font-size: 15px; color: %1; margin-bottom: 20px;").arg(TEXT_SECONDARY));
    menuLayout->addWidget(subtitle);
    
    transBtn_ = new QPushButton("▶ TRANSLATION EXERCISE");
    grammarBtn_ = new QPushButton("▶ GRAMMAR EXERCISE");
    
    QString btnStyle = QString(
        "QPushButton {"
        "background: qlineargradient(x1:0, y1:0, x2:1, y2:0, stop:0 %1, stop:1 %2);"
        "color: %3; border: none; border-radius: 12px;"
        "font-size: 16px; font-weight: bold; padding: 18px 35px;"
        "margin: 8px; min-width: 380px;}"
        "QPushButton:hover { background: qlineargradient(x1:0, y1:0, x2:1, y2:0, stop:0 %2, stop:1 %4); }"
        "QPushButton:pressed { background: qlineargradient(x1:0, y1:0, x2:1, y2:0, stop:0 %5, stop:1 %1); }"
    ).arg(ORANGE_PRIMARY).arg(ORANGE_HOVER).arg(DARK_BG).arg("#FF9933").arg(ORANGE_PRESSED);
    
    transBtn_->setStyleSheet(btnStyle);
    grammarBtn_->setStyleSheet(btnStyle);
    
    connect(transBtn_, &QPushButton::clicked, this, &MainWindow::startTranslation);
    connect(grammarBtn_, &QPushButton::clicked, this, &MainWindow::startGrammar);
    
    menuLayout->addStretch();
    menuLayout->addWidget(transBtn_, 0, Qt::AlignCenter);
    menuLayout->addWidget(grammarBtn_, 0, Qt::AlignCenter);
    menuLayout->addStretch();
    
    auto *footer = new QLabel("Press [H] for hints during exercises");
    footer->setAlignment(Qt::AlignCenter);
    footer->setStyleSheet(QString("font-size: 12px; color: %1; padding: 15px;").arg(TEXT_SECONDARY));
    menuLayout->addWidget(footer);
    
    exerciseWidget_ = new QWidget();
    exerciseWidget_->setStyleSheet(QString("background-color: %1;").arg(DARK_BG));
    auto *exLayout = new QVBoxLayout(exerciseWidget_);
    exLayout->setSpacing(22);
    exLayout->setContentsMargins(35, 35, 35, 35);
    
    auto *topFrame = new QFrame();
    topFrame->setStyleSheet(QString("QFrame { background-color: %1; border-radius: 12px; padding: 12px; }").arg(CARD_BG));
    auto *top = new QHBoxLayout(topFrame);
    top->setSpacing(15);
    
    auto *progressLabel = new QLabel("PROGRESS");
    progressLabel->setStyleSheet(QString("font-weight: bold; color: %1; font-size: 12px;").arg(ORANGE_PRIMARY));
    top->addWidget(progressLabel);
    
    progress_ = new QProgressBar();
    progress_->setFixedHeight(22);
    top->addWidget(progress_);
    
    auto *scoreCard = new QWidget();
    scoreCard->setStyleSheet(QString("background-color: %1; border-radius: 10px; padding: 8px; min-width: 90px;").arg(CARD_BG));
    auto *scoreLayout = new QVBoxLayout(scoreCard);
    scoreLayout->setSpacing(3);
    scoreLayout->setContentsMargins(10, 8, 10, 8);
    
    auto *scoreTitleLbl = new QLabel("SCORE");
    scoreTitleLbl->setStyleSheet("font-size: 11px; color: #888888;");
    scoreTitleLbl->setAlignment(Qt::AlignCenter);
    
    scoreValueLabel_ = new QLabel("0");
    scoreValueLabel_->setStyleSheet(QString("font-size: 20px; font-weight: bold; color: %1;").arg(ORANGE_PRIMARY));
    scoreValueLabel_->setAlignment(Qt::AlignCenter);
    
    scoreLayout->addWidget(scoreTitleLbl);
    scoreLayout->addWidget(scoreValueLabel_);
    
    auto *attemptsCard = new QWidget();
    attemptsCard->setStyleSheet(QString("background-color: %1; border-radius: 10px; padding: 8px; min-width: 90px;").arg(CARD_BG));
    auto *attemptsLayout = new QVBoxLayout(attemptsCard);
    attemptsLayout->setSpacing(3);
    attemptsLayout->setContentsMargins(10, 8, 10, 8);
    
    auto *attemptsTitleLbl = new QLabel("ATTEMPTS");
    attemptsTitleLbl->setStyleSheet("font-size: 11px; color: #888888;");
    attemptsTitleLbl->setAlignment(Qt::AlignCenter);
    
    attemptsValueLabel_ = new QLabel("0/3");
    attemptsValueLabel_->setStyleSheet(QString("font-size: 20px; font-weight: bold; color: %1;").arg(ORANGE_HOVER));
    attemptsValueLabel_->setAlignment(Qt::AlignCenter);
    
    attemptsLayout->addWidget(attemptsTitleLbl);
    attemptsLayout->addWidget(attemptsValueLabel_);
    
    auto *timerCard = new QWidget();
    timerCard->setStyleSheet(QString("background-color: %1; border-radius: 10px; padding: 8px; min-width: 90px;").arg(CARD_BG));
    auto *timerLayout = new QVBoxLayout(timerCard);
    timerLayout->setSpacing(3);
    timerLayout->setContentsMargins(10, 8, 10, 8);
    
    auto *timerTitleLbl = new QLabel("TIME");
    timerTitleLbl->setStyleSheet("font-size: 11px; color: #888888;");
    timerTitleLbl->setAlignment(Qt::AlignCenter);
    
    timerValueLabel_ = new QLabel("5:00");
    timerValueLabel_->setStyleSheet(QString("font-size: 20px; font-weight: bold; color: %1;").arg(ORANGE_PRIMARY));
    timerValueLabel_->setAlignment(Qt::AlignCenter);
    
    timerLayout->addWidget(timerTitleLbl);
    timerLayout->addWidget(timerValueLabel_);
    
    top->addWidget(scoreCard);
    top->addWidget(attemptsCard);
    top->addWidget(timerCard);
    top->addStretch();
    
    exLayout->addWidget(topFrame);
    
    auto *questionCard = new QFrame();
    questionCard->setStyleSheet(QString("QFrame { background-color: %1; border-radius: 16px; padding: 25px; }").arg(CARD_BG));
    auto *questionLayout = new QVBoxLayout(questionCard);
    
    promptLabel_ = new QLabel();
    promptLabel_->setWordWrap(true);
    promptLabel_->setStyleSheet(QString(
        "font-size: 19px; color: %1; padding: 25px; background-color: %2; border-radius: 12px;"
        "border-left: 4px solid %3;"
    ).arg(TEXT_PRIMARY).arg("#333333").arg(ORANGE_PRIMARY));
    promptLabel_->setAlignment(Qt::AlignCenter);
    promptLabel_->setMinimumHeight(90);
    questionLayout->addWidget(promptLabel_);
    
    exLayout->addWidget(questionCard);
    
    answerEdit_ = new QTextEdit();
    answerEdit_->setPlaceholderText("Type your answer here...");
    answerEdit_->setVisible(false);
    exLayout->addWidget(answerEdit_);
    
    auto *radioContainer = new QWidget();
    auto *radioLayout = new QGridLayout(radioContainer);
    radioLayout->setSpacing(12);
    radioLayout->setContentsMargins(5, 5, 5, 5);
    
    QString radioStyle = QString(
        "QRadioButton { color: %1; font-size: 15px; padding: 12px; background-color: %2; border-radius: 8px; }"
        "QRadioButton::indicator { width: 22px; height: 22px; border-radius: 11px; }"
        "QRadioButton::indicator::unchecked { border: 2px solid %3; background-color: %4; }"
        "QRadioButton::indicator::checked { border: 2px solid %3; background-color: %3; }"
        "QRadioButton:hover { background-color: %5; }"
    ).arg(TEXT_PRIMARY).arg("#333333").arg(ORANGE_PRIMARY).arg(CARD_BG).arg("#3A3A3A");
    
    for (int i = 0; i < 4; ++i) {
        auto *r = new QRadioButton();
        r->setStyleSheet(radioStyle);
        r->setVisible(false);
        radios_.append(r);
        radioLayout->addWidget(r, i/2, i%2);
    }
    radioGroup_ = new QButtonGroup(this);
    for (auto *r : radios_) radioGroup_->addButton(r);
    exLayout->addWidget(radioContainer);
    
    auto *buttonLayout = new QHBoxLayout();
    buttonLayout->setSpacing(18);
    
    submitBtn_ = new QPushButton("✓ SUBMIT ANSWER");
    hintBtn_ = new QPushButton("[H] SHOW HINT");
    
    QString actionBtnStyle = QString(
        "QPushButton { background-color: %1; color: %2; border: none; border-radius: 10px; "
        "font-size: 15px; font-weight: bold; padding: 14px 28px; min-width: 160px; }"
        "QPushButton:hover { background-color: %3; }"
        "QPushButton:pressed { background-color: %4; }"
    ).arg(ORANGE_PRIMARY).arg(DARK_BG).arg(ORANGE_HOVER).arg(ORANGE_PRESSED);
    
    submitBtn_->setStyleSheet(actionBtnStyle);
    hintBtn_->setStyleSheet(actionBtnStyle);
    
    buttonLayout->addStretch();
    buttonLayout->addWidget(hintBtn_);
    buttonLayout->addWidget(submitBtn_);
    buttonLayout->addStretch();
    
    exLayout->addLayout(buttonLayout);
    exLayout->addStretch();
    
    connect(submitBtn_, &QPushButton::clicked, this, &MainWindow::submitAnswer);
    connect(hintBtn_, &QPushButton::clicked, this, &MainWindow::showHint);
    
    timer_ = new QTimer(this);
    connect(timer_, &QTimer::timeout, this, &MainWindow::onTimerTimeout);
    
    stacked_->addWidget(menuWidget_);
    stacked_->addWidget(exerciseWidget_);
    stacked_->setCurrentWidget(menuWidget_);
}

void MainWindow::playCorrectSound() {
    if (correctPlayer_->source().isEmpty()) {
        QApplication::beep();
    } else {
        correctPlayer_->setPosition(0);
        correctPlayer_->play();
    }
}

void MainWindow::playWrongSound() {
    if (wrongPlayer_->source().isEmpty()) {
        for (int i = 0; i < 2; ++i) QApplication::beep();
    } else {
        wrongPlayer_->setPosition(0);
        wrongPlayer_->play();
    }
}

void MainWindow::keyPressEvent(QKeyEvent *event) {
    if (event->key() == Qt::Key_H && stacked_->currentWidget() == exerciseWidget_) {
        showHint();
        event->accept();
        return;
    }
    QMainWindow::keyPressEvent(event);
}

void MainWindow::selectDifficulty() {
    DifficultyDialog dlg(this);
    if (dlg.exec() == QDialog::Accepted) {
        difficulty_ = dlg.getDifficulty();
        QMessageBox::information(this, "Difficulty", 
            "Set to: " + QString::number(difficulty_ == 1 ? 10 : difficulty_ == 2 ? 25 : 50) + " tasks");
    }
}

void MainWindow::showHelp() {
    QMessageBox::information(this, "Help",
        "═══════════════════════════════════\n"
        "          HOW TO PLAY\n"
        "═══════════════════════════════════\n\n"
        "• Press [H] for hints during exercise\n"
        "• Translation: Type answer, click SUBMIT\n"
        "• Grammar: Choose option, click SUBMIT\n"
        "• Time limit: 5 minutes\n"
        "• Max wrong attempts: 3\n"
        "• Each correct answer: +10 points\n\n"
        "═══════════════════════════════════");
}

void MainWindow::showHint() {
    if (currentIdx_ >= exercises_.size()) return;
    const auto &ex = exercises_[currentIdx_];
    QMessageBox::information(this, "💡 Hint", 
        "═══════ HINT ═══════\n\n" + ex.hint + "\n\n═══════════════════");
}

void MainWindow::startTranslation() {
    generateExercises(difficulty_, Exercise::Translation);
    currentIdx_ = score_ = wrongAttempts_ = 0;
    timeRemaining_ = 300;
    progress_->setMaximum(exercises_.size());
    progress_->setValue(0);
    scoreValueLabel_->setText("0");
    attemptsValueLabel_->setText("0/3");
    timerValueLabel_->setText("5:00");
    
    stacked_->setCurrentWidget(exerciseWidget_);
    showCurrentExercise();
    timer_->start(1000);
}

void MainWindow::startGrammar() {
    generateExercises(difficulty_, Exercise::Grammar);
    currentIdx_ = score_ = wrongAttempts_ = 0;
    timeRemaining_ = 300;
    progress_->setMaximum(exercises_.size());
    progress_->setValue(0);
    scoreValueLabel_->setText("0");
    attemptsValueLabel_->setText("0/3");
    timerValueLabel_->setText("5:00");
    
    stacked_->setCurrentWidget(exerciseWidget_);
    showCurrentExercise();
    timer_->start(1000);
}

void MainWindow::generateExercises(int diff, Exercise::Type type) {
    exercises_.clear();
    int count = (diff == 1) ? 10 : (diff == 3) ? 50 : 25;
    
    QVector<QPair<QString,QString>> translations = {
        {"Hello", "Привет"}, {"Good morning", "Доброе утро"}, {"Good afternoon", "Добрый день"},
        {"Good evening", "Добрый вечер"}, {"Good night", "Спокойной ночи"}, {"How are you?", "Как дела?"},
        {"I'm fine, thank you", "У меня всё хорошо, спасибо"}, {"Nice to meet you", "Приятно познакомиться"},
        {"See you later", "Увидимся позже"}, {"Take care", "Береги себя"},
        {"Thank you", "Спасибо"}, {"Thanks a lot", "Большое спасибо"}, {"You're welcome", "Пожалуйста"},
        {"Sorry", "Извините"}, {"Excuse me", "Простите"}, {"No problem", "Без проблем"},
        {"What is your name?", "Как тебя зовут?"}, {"My name is...", "Меня зовут..."},
        {"Where are you from?", "Откуда ты?"}, {"I am from...", "Я из..."},
        {"How old are you?", "Сколько тебе лет?"}, {"What time is it?", "Который час?"},
        {"Where is the bathroom?", "Где туалет?"}, {"How much does it cost?", "Сколько это стоит?"},
        {"Can you help me?", "Можете мне помочь?"}, {"Do you speak English?", "Вы говорите по-английски?"},
        {"I don't understand", "Я не понимаю"}, {"Please speak slowly", "Пожалуйста, говорите медленнее"},
        {"Could you repeat that?", "Не могли бы вы повторить?"}, {"What does this mean?", "Что это значит?"},
        {"I love programming", "Я люблю программирование"}, {"Qt is awesome", "Qt потрясающий"},
        {"I work as a developer", "Я работаю разработчиком"}, {"Learning languages is fun", "Учить языки весело"},
        {"Let's go for a walk", "Пойдём гулять"}, {"I'm hungry", "Я голоден"}, {"I'm thirsty", "Я хочу пить"},
        {"I'm tired", "Я устал"}, {"I'm happy", "Я счастлив"}, {"I'm sad", "Мне грустно"},
        {"Where is the nearest station?", "Где ближайшая станция?"}, {"Turn left", "Поверните налево"},
        {"Turn right", "Поверните направо"}, {"Go straight", "Идите прямо"}, {"It's near the park", "Это рядом с парком"},
        {"The museum is closed", "Музей закрыт"}, {"What time does it open?", "Во сколько открывается?"},
        {"I'm looking for a hotel", "Я ищу гостиницу"}, {"Is it far from here?", "Это далеко отсюда?"},
        {"It's five minutes walk", "Это в пяти минутах ходьбы"}
    };
    
    QVector<std::tuple<QString,QString,QString,QString>> grammar = {
        {"I ___ to school every day.", "go,goes,going,went", "go", "Present Simple - I/you/we/they use base form"},
        {"She ___ coffee every morning.", "drink,drinks,drinking,drank", "drinks", "He/she/it adds -s in Present Simple"},
        {"They ___ football on weekends.", "play,plays,playing,played", "play", "For they/we/you, no -s needed"},
        {"He ___ to work by car.", "drive,drives,driving,drove", "drives", "3rd person singular requires -s/-es"},
        {"We ___ watching TV now.", "is,are,am,be", "are", "Present Continuous with 'we' uses 'are'"},
        {"The cat ___ sleeping at the moment.", "is,are,am,be", "is", "He/she/it + is + verb-ing"},
        {"I ___ reading a book right now.", "is,are,am,be", "am", "I + am + verb-ing"},
        {"The children ___ playing in the garden.", "is,are,am,be", "are", "Plural subjects use 'are'"},
        {"She ___ not working today.", "is,are,am,be", "is", "Negative: subject + is/am/are + not"},
        {"Yesterday I ___ to the cinema.", "go,went,gone,going", "went", "Past Simple irregular verb 'go' -> 'went'"},
        {"She ___ a new dress last week.", "buy,bought,buyed,buying", "bought", "Irregular verb: buy -> bought"},
        {"They ___ in London in 2010.", "live,lived, lives, living", "lived", "Regular verb: add -ed for past"},
        {"He ___ the answer yesterday.", "know,knew,known,knowing", "knew", "Irregular: know -> knew"},
        {"We ___ a great time at the party.", "have,had,having,has", "had", "Irregular: have -> had"},
        {"Tomorrow I ___ call you.", "will,would,shall,may", "will", "Future Simple: will + base verb"},
        {"She ___ going to travel next month.", "is,are,am,be", "is", "Going to future for plans"},
        {"They ___ arrive at 8 PM.", "will,would,shall,might", "will", "Use 'will' for future predictions"},
        {"What ___ you do tomorrow?", "will,do,are,is", "will", "Question form: will + subject + verb"},
        {"I have ___ visited Paris.", "never,ever,already,yet", "never", "Present Perfect for experience"},
        {"She has ___ finished her homework.", "already,yet,still,just", "already", "'Already' in positive sentences"},
        {"We haven't seen them ___.", "already,yet,just,ever", "yet", "'Yet' in negative sentences"},
        {"Have you ___ been to Italy?", "ever,never,already,yet", "ever", "Questions about experience use 'ever'"},
        {"You ___ smoke here. It's forbidden.", "mustn't,don't have to,shouldn't,might not", "mustn't", "Prohibition = mustn't"},
        {"I ___ speak three languages.", "can,could,may,might", "can", "Ability in present = can"},
        {"___ I open the window?", "Can,May,Must,Should", "Can", "Permission can be expressed with can/may"},
        {"You ___ see a doctor. You look sick.", "should,must,can,may", "should", "Advice = should"},
        {"If I ___ rich, I would travel the world.", "were,was,am,is", "were", "Second conditional: 'If I were'"},
        {"If it rains, we ___ stay home.", "will,would,shall,might", "will", "First conditional: if + present, will + verb"},
        {"If she had studied, she ___ passed.", "would have,will have,would has,will has", "would have", "Third conditional"},
        {"This is the ___ day of my life.", "best,good,better,well", "best", "Superlative: the + best"},
        {"She is ___ than her sister.", "taller,tall,the tallest,more tall", "taller", "Comparative: adjective-er + than"},
        {"This book is more ___ than that one.", "interesting,interest,interested,interests", "interesting", "Long adjectives: more + adjective"},
        {"I wake up ___ 7 AM every day.", "at,in,on,for", "at", "Time: at + specific time"},
        {"Let's meet ___ Monday.", "on,in,at,by", "on", "Day of week: on + day"},
        {"I was born ___ 1990.", "in,on,at,of", "in", "Years: in + year"},
        {"The cat is ___ the table.", "under,on,in,at", "on", "Position: on + surface"},
        {"I'm interested ___ learning Qt.", "in,of,at,for", "in", "Interested in + noun/gerund"},
        {"She doesn't ___ apples.", "like,likes,liking,liked", "like", "After 'doesn't', use base form"},
        {"He can ___ very fast.", "run,runs,running,ran", "run", "After modal verbs (can), use base form"},
        {"Let's ___ to the beach!", "go,went,going,goes", "go", "Let's + base form of verb"},
        {"I need ___ some groceries.", "to buy,buy,buying,bought", "to buy", "Need + infinitive with 'to'"},
        {"___ you like pizza?", "Do,Does,Is,Are", "Do", "Questions with 'you' use 'do'"},
        {"___ she speak French?", "Does,Do,Is,Are", "Does", "3rd person singular questions use 'does'"},
        {"Where ___ they live?", "do,does,is,are", "do", "Questions with they/we/you: do"},
        {"I wish I ___ more time.", "had,have,has,were", "had", "Wish + past simple for present wishes"},
        {"She suggested ___ to the museum.", "going,to go,go,went", "going", "Suggest + gerund (-ing)"},
        {"It's time we ___ home.", "went,go,going,goes", "went", "It's time + past simple for present"},
        {"By next year, I ___ here for 5 years.", "will have worked,will work,work,worked", "will have worked", "Future Perfect"}
    };
    
    auto *rng = QRandomGenerator::global();
    
    if (type == Exercise::Translation) {
        for (int i = 0; i < count; ++i) {
            int idx = rng->bounded(translations.size());
            Exercise ex;
            ex.type = Exercise::Translation;
            ex.prompt = "Translate to Russian: " + translations[idx].first;
            ex.correctAnswer = translations[idx].second;
            ex.hint = "Think about context. Type in Cyrillic (Russian letters)";
            exercises_.append(ex);
        }
    } else {
        for (int i = 0; i < count; ++i) {
            int idx = rng->bounded(grammar.size());
            Exercise ex;
            ex.type = Exercise::Grammar;
            ex.prompt = std::get<0>(grammar[idx]);
            ex.options = std::get<1>(grammar[idx]).split(',');
            ex.correctAnswer = std::get<2>(grammar[idx]);
            ex.hint = std::get<3>(grammar[idx]);
            exercises_.append(ex);
        }
    }
    
    for (int i = exercises_.size()-1; i > 0; --i) {
        int j = rng->bounded(i+1);
        std::swap(exercises_[i], exercises_[j]);
    }
}

void MainWindow::showCurrentExercise() {
    if (currentIdx_ >= exercises_.size()) { finishExercise(true); return; }
    
    const auto &ex = exercises_[currentIdx_];
    promptLabel_->setText(QString("[%1/%2] %3").arg(currentIdx_+1).arg(exercises_.size()).arg(ex.prompt));
    progress_->setValue(currentIdx_);
    
    answerEdit_->setVisible(false);
    answerEdit_->clear();
    for (auto *r : radios_) { r->setVisible(false); r->setChecked(false); }
    
    if (ex.type == Exercise::Translation) {
        answerEdit_->setVisible(true);
        answerEdit_->setPlaceholderText("Enter translation in Russian (Cyrillic)...");
        answerEdit_->setFocus();
    } else {
        QStringList opts = ex.options;
        for (int i = opts.size()-1; i > 0; --i) {
            int j = QRandomGenerator::global()->bounded(i+1);
            std::swap(opts[i], opts[j]);
        }
        for (int i = 0; i < 4 && i < opts.size(); ++i) {
            radios_[i]->setText(opts[i]);
            radios_[i]->setVisible(true);
        }
    }
}

bool MainWindow::checkTranslation(const QString &user, const QString &correct) {
    QString u = user.trimmed().toLower(), c = correct.trimmed().toLower();
    u.remove(QRegularExpression("[.,!?;:()\"']"));
    c.remove(QRegularExpression("[.,!?;:()\"']"));
    u = u.simplified(); c = c.simplified();
    if (u == c) return true;
    
    int maxDiff = qMax(1, c.length() / 5), diff = 0;
    for (int i = 0, n = qMax(u.length(), c.length()); i < n; ++i) {
        QChar a = i < u.length() ? u[i] : QChar();
        QChar b = i < c.length() ? c[i] : QChar();
        if (a != b && ++diff > maxDiff) return false;
    }
    return true;
}

int MainWindow::levenshteinDistance(const QString &s1, const QString &s2) {
    int m = s1.length(), n = s2.length();
    QVector<QVector<int>> dp(m + 1, QVector<int>(n + 1));
    
    for (int i = 0; i <= m; ++i) dp[i][0] = i;
    for (int j = 0; j <= n; ++j) dp[0][j] = j;
    
    for (int i = 1; i <= m; ++i) {
        for (int j = 1; j <= n; ++j) {
            int cost = (s1[i-1] == s2[j-1]) ? 0 : 1;
            dp[i][j] = qMin(qMin(dp[i-1][j] + 1, dp[i][j-1] + 1), dp[i-1][j-1] + cost);
        }
    }
    return dp[m][n];
}

void MainWindow::submitAnswer() {
    if (currentIdx_ >= exercises_.size()) return;
    const auto &ex = exercises_[currentIdx_];
    bool correct = false;
    
    if (ex.type == Exercise::Translation) {
        QString answer = answerEdit_->toPlainText();
        if (answer.trimmed().isEmpty()) {
            QMessageBox::warning(this, "!", "Please enter a translation");
            return;
        }
        QString u = answer.trimmed().toLower(), c = ex.correctAnswer.trimmed().toLower();
        u.remove(QRegularExpression("[.,!?;:()\"']"));
        c.remove(QRegularExpression("[.,!?;:()\"']"));
        int dist = levenshteinDistance(u, c);
        int maxLen = qMax(u.length(), c.length());
        correct = (maxLen == 0) || (static_cast<double>(dist) / maxLen <= 0.2);
    } else {
        auto *sel = radioGroup_->checkedButton();
        if (!sel) { QMessageBox::warning(this, "!", "Please select an answer"); return; }
        correct = (sel->text() == ex.correctAnswer);
    }
    
    if (correct) {
        score_ += 10;
        scoreValueLabel_->setText(QString::number(score_));
        playCorrectSound();
        QMessageBox::information(this, "✓ Correct!", "Great job! +10 points");
        ++currentIdx_;
        showCurrentExercise();
    } else {
        if (++wrongAttempts_ >= 3) {
            attemptsValueLabel_->setText(QString::number(wrongAttempts_) + "/3");
            playWrongSound();
            QMessageBox::warning(this, "✗ Game Over", 
                "Too many mistakes!\nCorrect: " + ex.correctAnswer);
            finishExercise(false);
            return;
        }
        attemptsValueLabel_->setText(QString::number(wrongAttempts_) + "/3");
        playWrongSound();
        QMessageBox::warning(this, "✗ Wrong", 
            "Correct: " + ex.correctAnswer + "\nAttempts left: " + QString::number(3 - wrongAttempts_));
    }
}

void MainWindow::onTimerTimeout() {
    if (--timeRemaining_ <= 0) {
        timer_->stop();
        playWrongSound();
        QMessageBox::information(this, "⏰ Time's Up!", "Exercise finished.");
        finishExercise(false);
        timeRemaining_ = 300;
        return;
    }
    timerValueLabel_->setText(QString("%1:%2")
        .arg(timeRemaining_/60).arg(timeRemaining_%60, 2, 10, QChar('0')));
}

void MainWindow::finishExercise(bool success) {
    timer_->stop();
    bool bonus = success && currentIdx_ >= exercises_.size() && wrongAttempts_ < 3;
    QString msg = bonus 
        ? QString("🎉 Excellent!\nAll %1 tasks completed!\nFinal Score: %2")
            .arg(exercises_.size()).arg(score_)
        : QString("Finished.\nCompleted: %1/%2\nScore: %3")
            .arg(currentIdx_).arg(exercises_.size()).arg(score_);
    QMessageBox::information(this, "Results", msg);
    stacked_->setCurrentWidget(menuWidget_);
}