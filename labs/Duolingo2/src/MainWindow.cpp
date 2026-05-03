#include "MainWindow.h"
#include "DifficultyDialog.h"
#include "Levenshtein.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QMenuBar>
#include <QMessageBox>
#include <QKeyEvent>
#include <QKeySequence>
#include <QUrl>
#include <QApplication>
#include <QDebug>
#include <QDir>
#include <QFileInfo>
#include <QAudioDevice>
#include <QMediaDevices>
#include <algorithm>
#include <random>

static QString resolveAssetPath(const QString &fileName) {
    QString localAsset = QDir::current().filePath("assets/" + fileName);
    if (QFileInfo::exists(localAsset)) {
        return localAsset;
    }

    QString appDir = QCoreApplication::applicationDirPath();
    QString runfilesAsset = QDir(appDir).filePath("LanguageApp.runfiles/_main/assets/" + fileName);
    if (QFileInfo::exists(runfilesAsset)) {
        return runfilesAsset;
    }

    QString altRunfilesAsset = QDir(appDir).filePath("../assets/" + fileName);
    if (QFileInfo::exists(altRunfilesAsset)) {
        return altRunfilesAsset;
    }

    return localAsset;
}

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), score(0), maxLives(3), maxTime(30), maxMistakes(3), currentTaskCount(4) {
    setupUI();
    applyTheme();

    timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &MainWindow::updateTimer);

    // Audio setup (Bonus)
    QAudioDevice defaultDevice = QMediaDevices::defaultAudioOutput();
    if (defaultDevice.isNull()) {
        auto outputs = QMediaDevices::audioOutputs();
        if (!outputs.isEmpty()) {
            defaultDevice = outputs.first();
        }
    }

    sndCorrect = new QSoundEffect(this);
    sndCorrect->setAudioDevice(defaultDevice);
    sndCorrect->setSource(QUrl::fromLocalFile(resolveAssetPath("correct.wav")));
    sndCorrect->setVolume(1.0f);
    connect(sndCorrect, &QSoundEffect::statusChanged, this, [this](){
        qDebug() << "sndCorrect statusChanged:" << sndCorrect->status()
                 << "loaded:" << sndCorrect->isLoaded()
                 << "playing:" << sndCorrect->isPlaying();
    });
    connect(sndCorrect, &QSoundEffect::playingChanged, this, [this](){
        qDebug() << "sndCorrect playingChanged:" << sndCorrect->isPlaying();
    });

    sndWrong = new QSoundEffect(this);
    sndWrong->setAudioDevice(defaultDevice);
    sndWrong->setSource(QUrl::fromLocalFile(resolveAssetPath("wrong.wav")));
    sndWrong->setVolume(1.0f);
    connect(sndWrong, &QSoundEffect::statusChanged, this, [this](){
        qDebug() << "sndWrong statusChanged:" << sndWrong->status()
                 << "loaded:" << sndWrong->isLoaded()
                 << "playing:" << sndWrong->isPlaying();
    });
    connect(sndWrong, &QSoundEffect::playingChanged, this, [this](){
        qDebug() << "sndWrong playingChanged:" << sndWrong->isPlaying();
    });
    
    // Debug output for audio files
    qDebug() << "Текущая рабочая папка:" << QDir::currentPath();
    qDebug() << "Resolved correct sound path:" << resolveAssetPath("correct.wav");
    qDebug() << "Resolved wrong sound path:" << resolveAssetPath("wrong.wav");
    qDebug() << "Status correct sound:" << sndCorrect->status();
    qDebug() << "Status wrong sound:" << sndWrong->status();
    qDebug() << "Correct audio device:" << sndCorrect->audioDevice().description()
             << "id:" << sndCorrect->audioDevice().id()
             << "default:" << sndCorrect->audioDevice().isDefault();
    qDebug() << "Wrong audio device:" << sndWrong->audioDevice().description()
             << "id:" << sndWrong->audioDevice().id()
             << "default:" << sndWrong->audioDevice().isDefault();
    
    // Create Menu
    QMenu *menu = menuBar()->addMenu("Настройки");
    QAction *actSettings = new QAction("Уровень сложности", this);
    menu->addAction(actSettings);
    connect(actSettings, &QAction::triggered, this, &MainWindow::openSettings);

    QAction *helpAction = new QAction("Памятка", this);
    helpAction->setShortcut(QKeySequence(Qt::Key_H));
    addAction(helpAction);
    connect(helpAction, &QAction::triggered, this, &MainWindow::showHelpMemo);
}

MainWindow::~MainWindow() {}

void MainWindow::applyTheme() {
    // Duolingo-style QSS
    QString style = R"(
        QMainWindow { background-color: #FFFFFF; }
        QWidget { font-family: 'Segoe UI', Arial, sans-serif; }
        
        QPushButton {
            background-color: #58CC02;
            color: white;
            font-size: 16px;
            font-weight: bold;
            border-radius: 12px;
            padding: 12px 24px;
            border-bottom: 4px solid #46A302;
        }
        QPushButton:hover { background-color: #46A302; border-bottom: 4px solid #367A02; }
        QPushButton:pressed { background-color: #46A302; border-bottom: 0px; margin-top: 4px; }
        
        QPushButton#BtnSubmit { background-color: #1CB0F6; border-bottom: 4px solid #1899D6; }
        QPushButton#BtnSubmit:hover { background-color: #1899D6; border-bottom: 4px solid #147BB0; }
        
        QLineEdit, QTextEdit {
            border: 2px solid #E5E5E5;
            border-radius: 10px;
            padding: 12px;
            font-size: 18px;
            background: #F7F7F7;
        }
        QLineEdit:focus, QTextEdit:focus { border: 2px solid #1CB0F6; background: #FFFFFF; }
        
        QProgressBar {
            border: 2px solid #E5E5E5;
            border-radius: 10px;
            background: #E5E5E5;
            text-align: center;
            color: transparent;
            height: 20px;
        }
        QProgressBar::chunk { background-color: #58CC02; border-radius: 8px; }
        
        QLabel { color: #4B4B4B; font-size: 16px; }
        QLabel#Title { font-size: 24px; font-weight: bold; color: #3C3C3C; }
        QLabel#Stats { font-weight: bold; color: #FF4B4B; font-size: 18px;}
        
        QRadioButton { font-size: 18px; spacing: 10px; padding: 5px; }
        QRadioButton::indicator { width: 20px; height: 20px; border-radius: 10px; border: 2px solid #E5E5E5; }
        QRadioButton::indicator:checked { background-color: #1CB0F6; border: 2px solid #1899D6; }
    )";
    qApp->setStyleSheet(style);
}

void MainWindow::setupUI() {
    setWindowTitle("LinguistPro (Lab 3)");
    resize(700, 500);

    QWidget *central = new QWidget(this);
    setCentralWidget(central);
    QVBoxLayout *mainLayout = new QVBoxLayout(central);
    mainLayout->setContentsMargins(30, 20, 30, 30);
    mainLayout->setSpacing(20);

    // --- Top Bar (HUD) ---
    QHBoxLayout *hudLayout = new QHBoxLayout();
    scoreLabel = new QLabel("⭐ 0", this);
    scoreLabel->setObjectName("Stats");
    livesLabel = new QLabel("❤️ 3", this);
    livesLabel->setObjectName("Stats");
    mistakesLabel = new QLabel("❌ 0/3", this);
    mistakesLabel->setObjectName("Stats");
    timeLabel = new QLabel("⏱ --", this);
    timeLabel->setObjectName("Stats");
    
    progressBar = new QProgressBar(this);
    progressBar->setValue(0);
    progressBar->setTextVisible(false);

    hudLayout->addWidget(livesLabel);
    hudLayout->addWidget(mistakesLabel);
    hudLayout->addWidget(progressBar, 1); // stretch
    hudLayout->addWidget(timeLabel);
    hudLayout->addWidget(scoreLabel);
    mainLayout->addLayout(hudLayout);

    // --- Stacked Widget ---
    stackedWidget = new QStackedWidget(this);
    mainLayout->addWidget(stackedWidget);

    // Page 0: Main Menu
    QWidget *menuPage = new QWidget();
    QVBoxLayout *menuLayout = new QVBoxLayout(menuPage);
    menuLayout->setAlignment(Qt::AlignCenter);
    
    QLabel *logoLabel = new QLabel("Выбери путь ниндзя 🥷", this);
    logoLabel->setObjectName("Title");
    logoLabel->setAlignment(Qt::AlignCenter);
    
    QPushButton *btnTrans = new QPushButton("🌍 Тренировка перевода");
    QPushButton *btnGram = new QPushButton("📚 Практика грамматики");
    
    menuLayout->addWidget(logoLabel);
    menuLayout->addSpacing(30);
    menuLayout->addWidget(btnTrans);
    menuLayout->addWidget(btnGram);
    stackedWidget->addWidget(menuPage);

    connect(btnTrans, &QPushButton::clicked, this, &MainWindow::startTranslation);
    connect(btnGram, &QPushButton::clicked, this, &MainWindow::startGrammar);

    // Page 1: Translation Task
    QWidget *transPage = new QWidget();
    QVBoxLayout *transLayout = new QVBoxLayout(transPage);
    transLayout->setAlignment(Qt::AlignCenter);
    
    taskPromptLabel = new QLabel("...", this);
    taskPromptLabel->setObjectName("Title");
    taskPromptLabel->setWordWrap(true);
    taskPromptLabel->setAlignment(Qt::AlignCenter);
    
    translationInput = new QTextEdit(this);
    translationInput->setPlaceholderText("Введите перевод на русский...");
    translationInput->setFixedHeight(120);
    translationInput->setAcceptRichText(false);
    
    QPushButton *btnSubmitTrans = new QPushButton("Submit", this);
    btnSubmitTrans->setObjectName("BtnSubmit");
    
    transLayout->addWidget(taskPromptLabel);
    transLayout->addSpacing(20);
    transLayout->addWidget(translationInput);
    transLayout->addSpacing(20);
    transLayout->addWidget(btnSubmitTrans);
    stackedWidget->addWidget(transPage);

    connect(btnSubmitTrans, &QPushButton::clicked, this, &MainWindow::submitTranslation);

    // Page 2: Grammar Task
    QWidget *grammarPage = new QWidget();
    QVBoxLayout *grammarLayout = new QVBoxLayout(grammarPage);
    grammarLayout->setAlignment(Qt::AlignCenter);
    
    QHBoxLayout *textLayout = new QHBoxLayout();
    grammarText1 = new QLabel("...", this);
    grammarText1->setObjectName("Title");
    QLabel *gapLabel = new QLabel(" [___] ", this);
    gapLabel->setObjectName("Title");
    gapLabel->setStyleSheet("color: #1CB0F6;");
    grammarText2 = new QLabel("...", this);
    grammarText2->setObjectName("Title");
    
    textLayout->addStretch();
    textLayout->addWidget(grammarText1);
    textLayout->addWidget(gapLabel);
    textLayout->addWidget(grammarText2);
    textLayout->addStretch();

    radioGroup = new QButtonGroup(this);
    radioBtn1 = new QRadioButton("Опция 1", this);
    radioBtn2 = new QRadioButton("Опция 2", this);
    radioBtn3 = new QRadioButton("Опция 3", this);
    radioGroup->addButton(radioBtn1, 0);
    radioGroup->addButton(radioBtn2, 1);
    radioGroup->addButton(radioBtn3, 2);

    QVBoxLayout *radiosLayout = new QVBoxLayout();
    radiosLayout->addWidget(radioBtn1);
    radiosLayout->addWidget(radioBtn2);
    radiosLayout->addWidget(radioBtn3);
    radiosLayout->setAlignment(Qt::AlignCenter);

    QPushButton *btnSubmitGram = new QPushButton("Submit", this);
    btnSubmitGram->setObjectName("BtnSubmit");

    grammarLayout->addLayout(textLayout);
    grammarLayout->addSpacing(20);
    grammarLayout->addLayout(radiosLayout);
    grammarLayout->addSpacing(30);
    grammarLayout->addWidget(btnSubmitGram);
    stackedWidget->addWidget(grammarPage);

    connect(btnSubmitGram, &QPushButton::clicked, this, &MainWindow::submitGrammar);

    // Page 3: Result Page
    QWidget *resultPage = new QWidget();
    QVBoxLayout *resultLayout = new QVBoxLayout(resultPage);
    resultLayout->setAlignment(Qt::AlignCenter);
    
    QPushButton *btnMenu = new QPushButton("Вернуться в меню");
    btnMenu->setObjectName("BtnSubmit");
    resultLayout->addWidget(new QLabel("Упражнение завершено!", this));
    resultLayout->addWidget(btnMenu);
    stackedWidget->addWidget(resultPage);
    
    connect(btnMenu, &QPushButton::clicked, this, &MainWindow::returnToMenu);
}

void MainWindow::openSettings() {
    DifficultyDialog dlg(this);
    if (dlg.exec() == QDialog::Accepted) {
        maxLives = dlg.getSelectedLives();
        maxTime = dlg.getSelectedTime();
        QMessageBox::information(this, "Настройки", "Сложность успешно изменена!");
    }
}

void MainWindow::initGame(int type) {
    currentExerciseType = type;
    currentLives = maxLives;
    currentMistakes = 0;
    currentTime = maxTime;
    taskIndex = 0;

    if (type == 1) {
        auto pool = TaskDatabase::getTranslationTasks();
        std::shuffle(pool.begin(), pool.end(), std::mt19937(std::random_device{}()));
        int takeCount = std::min(currentTaskCount, (int)pool.size());
        tTasks.assign(pool.begin(), pool.begin() + takeCount);
        progressBar->setMaximum(tTasks.size());
    } else {
        auto pool = TaskDatabase::getGrammarTasks();
        std::shuffle(pool.begin(), pool.end(), std::mt19937(std::random_device{}()));
        int takeCount = std::min(currentTaskCount, (int)pool.size());
        gTasks.assign(pool.begin(), pool.begin() + takeCount);
        progressBar->setMaximum(gTasks.size());
    }

    progressBar->setValue(0);
    updateHUD();
    loadNextTask();

    stackedWidget->setCurrentIndex(type);
    timer->start(1000);
}

void MainWindow::startTranslation() { initGame(1); }
void MainWindow::startGrammar() { initGame(2); }

void MainWindow::updateHUD() {
    livesLabel->setText("❤️ " + QString::number(currentLives));
    mistakesLabel->setText("❌ " + QString::number(currentMistakes) + "/" + QString::number(maxMistakes));
    timeLabel->setText("⏱ " + QString::number(currentTime));
    scoreLabel->setText("⭐ " + QString::number(score));
}

void MainWindow::loadNextTask() {
    if (currentExerciseType == 1) {
        if (taskIndex >= static_cast<int>(tTasks.size())) { endGame(true, "Вы успешно выполнили все задания!"); return; }
        taskPromptLabel->setText("Переведите: \n\"" + tTasks[taskIndex].prompt + "\"");
        translationInput->clear();
        translationInput->setFocus();
        currentHint = tTasks[taskIndex].hint;
    } else {
        if (taskIndex >= static_cast<int>(gTasks.size())) { endGame(true, "Вы успешно выполнили все задания!"); return; }
        grammarText1->setText(gTasks[taskIndex].sentencePart1);
        grammarText2->setText(gTasks[taskIndex].sentencePart2);
        
        radioBtn1->setText(gTasks[taskIndex].options[0]);
        radioBtn2->setText(gTasks[taskIndex].options[1]);
        radioBtn3->setText(gTasks[taskIndex].options[2]);
        
        // Сброс выбора
        radioGroup->setExclusive(false);
        radioBtn1->setChecked(false);
        radioBtn2->setChecked(false);
        radioBtn3->setChecked(false);
        radioGroup->setExclusive(true);
        
        currentHint = gTasks[taskIndex].hint;
    }
}

void MainWindow::processAnswer(bool isCorrect) {
    if (isCorrect) {
        if (sndCorrect->status() == QSoundEffect::Ready) {
            qDebug() << "Playing correct sound";
            sndCorrect->play();
        }
        taskIndex++;
        progressBar->setValue(taskIndex);
        if (currentExerciseType == 1 ? taskIndex >= static_cast<int>(tTasks.size()) : taskIndex >= static_cast<int>(gTasks.size())) {
            int earned = currentTaskCount * 100;
            score += earned;
            updateHUD();
            endGame(true, QString("Вы успешно прошли упражнение и получили %1 очков!").arg(earned));
            return;
        }
        loadNextTask();
        updateHUD();
    } else {
        if (sndWrong->status() == QSoundEffect::Ready) {
            qDebug() << "Playing wrong sound";
            sndWrong->play();
        }
        currentMistakes++;
        currentLives--;
        updateHUD();
        QString warning = QString("Ответ неверный. Ошибок: %1/%2").arg(currentMistakes).arg(maxMistakes);
        QMessageBox::warning(this, "Ошибка!", warning);
        if (currentMistakes >= maxMistakes) {
            endGame(false, "Слишком много неверных ответов. Упражнение завершено.");
            return;
        }
        if (currentLives <= 0) {
            endGame(false, "Жизни закончились. Упражнение завершено.");
            return;
        }
    }
}

void MainWindow::submitTranslation() {
    QString answer = translationInput->toPlainText().trimmed();
    if (answer.isEmpty()) return;
    
    QString expected = tTasks[taskIndex].expectedAnswer;
    bool match = StringComparator::isFuzzyMatch(answer, expected);
    processAnswer(match);
}

void MainWindow::submitGrammar() {
    int checkedId = radioGroup->checkedId();
    if (checkedId == -1) {
        QMessageBox::information(this, "Внимание", "Пожалуйста, выберите вариант ответа.");
        return;
    }
    
    bool match = (checkedId == gTasks[taskIndex].correctIndex);
    processAnswer(match);
}

void MainWindow::updateTimer() {
    currentTime--;
    updateHUD();
    if (currentTime <= 0) {
        endGame(false, "Время истекло. Упражнение завершено.");
    }
}

void MainWindow::endGame(bool win, const QString &reason) {
    timer->stop();
    QString msg;
    if (win) {
        msg = reason.isEmpty() ? QString("Ура! Вы прошли все задания!\nТекущий счет: %1").arg(score)
                               : reason + QString("\nТекущий счет: %1").arg(score);
    } else {
        msg = reason.isEmpty() ? "Игра окончена. У вас закончились жизни или время." : reason;
    }
    QMessageBox::information(this, win ? "Победа" : "Поражение", msg);
    stackedWidget->setCurrentIndex(0); // Меню
}

void MainWindow::returnToMenu() {
    stackedWidget->setCurrentIndex(0);
}

void MainWindow::keyPressEvent(QKeyEvent *event) {
    // Система подсказок (Требование: Help по нажатию H)
    if (event->key() == Qt::Key_H) {
        showHelpMemo();
        return;
    }
    QMainWindow::keyPressEvent(event);
}

void MainWindow::showHelpMemo() {
    if (stackedWidget->currentIndex() == 1 || stackedWidget->currentIndex() == 2) {
        timer->stop();
        QString hintInfo;
        if (currentExerciseType == 1) {
            hintInfo = "Памятка по переводу:\n\n";
            hintInfo += "- Переводите фразу на русский язык.\n";
            hintInfo += "- Сравнение не чувствительно к регистру и пунктуации.\n";
            hintInfo += "- Допускаются небольшие опечатки и перестановки слов, если смысл понятен.\n\n";
            hintInfo += "Подсказка: " + currentHint;
        } else {
            hintInfo = "Памятка по грамматике:\n\n";
            hintInfo += "- Выберите единственно верный вариант.\n";
            hintInfo += "- Учитывайте время, лицо и форму глагола.\n";
            hintInfo += "- Если предложение содержит условное наклонение, выберите форму would + глагол.\n\n";
            hintInfo += "Подсказка: " + currentHint;
        }
        QMessageBox::information(this, "Памятка по упражнению", hintInfo);
        timer->start(1000);
    } else {
        QMessageBox::information(this, "Памятка", "Нажмите кнопку 'Тренировка перевода' или 'Практика грамматики' для начала упражнения.\nЗатем нажмите H, чтобы получить подсказку по текущему заданию.");
    }
}
