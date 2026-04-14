#include "MainWindow.h"

#include "DifficultyDialog.h"
#include "Utils.h"

#include <QtCore/QCoreApplication>
#include <QtCore/QDir>
#include <QtGui/QKeyEvent>
#include <QtWidgets/QMenu>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QMessageBox>
#include <QtWidgets/QVBoxLayout>
#include <algorithm>
#include <random>

MainWindow::MainWindow(QWidget* parent) : QMainWindow(parent) {
    setupUi();
    applyStyle();

    QString appDir = QCoreApplication::applicationDirPath() + "/tasks.json";
    if (!manager.loadFromFile(appDir)) {
        if (!manager.loadFromFile("tasks.json")) {
            if (!manager.loadFromFile("duolingo/tasks.json")) {
                if (!manager.loadFromFile("labs/duolingo/tasks.json")) {
                    if (!manager.loadFromFile("../duolingo/tasks.json")) {
                        QMessageBox::critical(
                            this, "Error",
                            "Failed to load tasks.json. CWD: " + QDir::currentPath() +
                                "\nAppDir: " + appDir);
                    }
                }
            }
        }
    }

    tts = new QTextToSpeech(this);
    timer = new QTimer(this);
    notificationTimer = new QTimer(this);
    notificationTimer->setSingleShot(true);

    connect(transWidget, &TranslationWidget::submitted, this, &MainWindow::onSubmitTranslation);
    connect(transWidget, &TranslationWidget::playAudioRequested, this, [this]() {
        if (static_cast<size_t>(currentTaskIdx) < manager.translationTasks.size()) {
            tts->say(manager.translationTasks[currentTaskIdx].original);
        }
    });
    connect(grammarWidget, &GrammarWidget::submitted, this, &MainWindow::onSubmitGrammar);
    connect(timer, &QTimer::timeout, this, &MainWindow::updateTimer);
    connect(notificationTimer, &QTimer::timeout, this, &MainWindow::hideNotification);
}

void MainWindow::setupUi() {
    QWidget* central = new QWidget(this);
    QVBoxLayout* mainLayout = new QVBoxLayout(central);

    topBar = new QWidget(this);
    QHBoxLayout* topLayout = new QHBoxLayout(topBar);
    timerLabel = new QLabel("Время: --", this);
    timerLabel->setObjectName("timerLabel");
    livesLabel = new QLabel("", this);
    livesLabel->setObjectName("livesLabel");
    hintBtn = new QPushButton("Подсказка", this);
    hintBtn->setObjectName("hintBtn");
    giveUpBtn = new QPushButton("Сдаться", this);
    giveUpBtn->setObjectName("giveUpBtn");
    topLayout->addWidget(timerLabel);
    topLayout->addSpacing(20);
    topLayout->addWidget(livesLabel);
    topLayout->addStretch();
    topLayout->addWidget(hintBtn);
    topLayout->addWidget(giveUpBtn);
    mainLayout->addWidget(topBar);
    topBar->hide();

    connect(hintBtn, &QPushButton::clicked, this, &MainWindow::showHint);
    connect(giveUpBtn, &QPushButton::clicked, this, &MainWindow::giveUp);

    // Header info
    scoreLabel = new QLabel("Очки: 0", this);
    scoreLabel->setObjectName("scoreLabel");
    progress = new QProgressBar(this);
    progress->setValue(0);
    progress->setTextVisible(false);

    mainLayout->addWidget(scoreLabel);
    mainLayout->addWidget(progress);

    // Stacked widget для переключения экранов
    stack = new QStackedWidget(this);

    // Menu screen
    menuWidget = new QWidget();
    QVBoxLayout* menuLayout = new QVBoxLayout(menuWidget);
    menuLayout->setAlignment(Qt::AlignCenter);

    QLabel* titleLabel = new QLabel("Добро пожаловать в DuoClone!", this);
    titleLabel->setObjectName("titleLabel");
    titleLabel->setAlignment(Qt::AlignCenter);

    QPushButton* btnTrans = new QPushButton("Перевод", this);
    QPushButton* btnGrammar = new QPushButton("Грамматика", this);
    QPushButton* btnMath = new QPushButton("Математика", this);
    QPushButton* btnSettings = new QPushButton("Настройки сложности", this);
    btnSettings->setObjectName("settingsBtn");

    menuLayout->addWidget(titleLabel);
    menuLayout->addSpacing(20);
    menuLayout->addWidget(btnTrans);
    menuLayout->addWidget(btnGrammar);
    menuLayout->addWidget(btnMath);
    menuLayout->addSpacing(20);
    menuLayout->addWidget(btnSettings);

    connect(btnTrans, &QPushButton::clicked, this, &MainWindow::startTranslation);
    connect(btnGrammar, &QPushButton::clicked, this, &MainWindow::startGrammar);
    connect(btnMath, &QPushButton::clicked, this, &MainWindow::startMath);
    connect(btnSettings, &QPushButton::clicked, this, &MainWindow::changeDifficulty);

    transWidget = new TranslationWidget(this);
    grammarWidget = new GrammarWidget(this);
    mathWidget = new MathWidget(this);

    connect(mathWidget, &MathWidget::submitted, this, &MainWindow::onSubmitMath);

    notificationLabel = new QLabel(this);
    notificationLabel->setObjectName("notificationLabel");
    notificationLabel->setAlignment(Qt::AlignCenter);
    notificationLabel->hide();

    stack->addWidget(menuWidget);
    stack->addWidget(transWidget);
    stack->addWidget(grammarWidget);
    stack->addWidget(mathWidget);

    mainLayout->addWidget(stack);
    setCentralWidget(central);
}

void MainWindow::applyStyle() {
    // Duolingo-like styling
    this->setStyleSheet(R"(
        QMainWindow {
            background-color: #ffffff;
        }
        QLabel {
            font-size: 16px;
            color: #3c3c3c;
        }
        QLabel#titleLabel {
            font-size: 24px;
            font-weight: bold;
            color: #58cc02;
        }
        QLabel#scoreLabel {
            font-size: 18px;
            font-weight: bold;
            color: #ff9600;
        }
        QPushButton {
            background-color: #58cc02;
            color: white;
            font-weight: bold;
            font-size: 16px;
            border-radius: 12px;
            padding: 10px 20px;
            border-bottom: 4px solid #58a700;
        }
        QPushButton:hover {
            background-color: #61e002;
        }
        QPushButton:pressed {
            background-color: #58a700;
            border-bottom: 0px;
            margin-top: 4px;
        }
        QProgressBar {
            border: 2px solid #e5e5e5;
            border-radius: 10px;
            background-color: #e5e5e5;
            height: 20px;
        }
        QProgressBar::chunk {
            background-color: #58cc02;
            border-radius: 8px;
        }
        QTextEdit {
            border: 2px solid #e5e5e5;
            border-radius: 12px;
            padding: 10px;
            font-size: 16px;
            background-color: #f7f7f7;
        }
        QTextEdit:focus {
            border: 2px solid #1cb0f6;
            background-color: #ffffff;
        }
        QRadioButton {
            font-size: 16px;
            color: #3c3c3c;
            spacing: 10px;
        }
        QRadioButton::indicator {
            width: 20px;
            height: 20px;
        }
        QPushButton#giveUpBtn {
            background-color: #ff4b4b;
            border-bottom: 4px solid #cc0000;
        }
        QPushButton#giveUpBtn:hover {
            background-color: #ff6b6b;
        }
        QPushButton#giveUpBtn:pressed {
            background-color: #cc0000;
            border-bottom: 0px;
            margin-top: 4px;
        }
        QPushButton#settingsBtn {
            background-color: #1cb0f6;
            border-bottom: 4px solid #1899d6;
        }
        QPushButton#settingsBtn:hover {
            background-color: #2bd0ff;
        }
        QPushButton#settingsBtn:pressed {
            background-color: #1899d6;
            border-bottom: 0px;
            margin-top: 4px;
        }
        QPushButton#hintBtn {
            background-color: #ffc800;
            border-bottom: 4px solid #cc9900;
            color: #3c3c3c;
        }
        QPushButton#hintBtn:hover {
            background-color: #ffdb4d;
        }
        QPushButton#hintBtn:pressed {
            background-color: #cc9900;
            border-bottom: 0px;
            margin-top: 4px;
        }
        QLabel#timerLabel {
            font-size: 18px;
            font-weight: bold;
            color: #ff4b4b;
        }
        QLabel#livesLabel {
            font-size: 22px;
            color: #ff4b4b;
        }
    )");
}

void MainWindow::startTranslation() {
    if (manager.translationTasks.empty()) {
        showNotification("Ошибка: Нет заданий на перевод!", true);
        return;
    }
    currentType = Translation;
    mistakesCount = 0;
    tasksDoneInSession = 0;
    currentTaskIdx = 0;

    // Shuffle tasks
    std::mt19937 rng(std::random_device{}());
    std::shuffle(std::begin(manager.translationTasks), std::end(manager.translationTasks), rng);

    progress->setMaximum(tasksPerSession);
    progress->setValue(0);
    stack->setCurrentWidget(transWidget);
    topBar->show();
    updateLivesDisplay();
    nextTask();
}

void MainWindow::startGrammar() {
    if (manager.grammarTasks.empty()) {
        showNotification("Ошибка: Нет заданий на грамматику!", true);
        return;
    }
    currentType = Grammar;
    mistakesCount = 0;
    tasksDoneInSession = 0;
    currentTaskIdx = 0;

    // Shuffle tasks
    std::mt19937 rng(std::random_device{}());
    std::shuffle(std::begin(manager.grammarTasks), std::end(manager.grammarTasks), rng);

    progress->setMaximum(tasksPerSession);
    progress->setValue(0);
    stack->setCurrentWidget(grammarWidget);
    topBar->show();
    updateLivesDisplay();
    nextTask();
}

void MainWindow::startMath() {
    currentType = Math;
    mistakesCount = 0;
    tasksDoneInSession = 0;
    progress->setMaximum(tasksPerSession);
    progress->setValue(0);
    stack->setCurrentWidget(mathWidget);
    topBar->show();
    updateLivesDisplay();
    nextTask();
}

void MainWindow::changeDifficulty() {
    DifficultyDialog dlg(tasksPerSession, maxMistakes, timeLimit, currentMathDifficulty, this);
    if (dlg.exec() == QDialog::Accepted) {
        tasksPerSession = dlg.getQuestions();
        maxMistakes = dlg.getLives();
        timeLimit = dlg.getTime();
        currentMathDifficulty = static_cast<DifficultyLevel>(dlg.getMathDifficulty());

        showNotification("Настройки успешно сохранены!", false);
    }
}

void MainWindow::generateMathTask() {
    int a, b, c;
    QString expr;
    int ans;

    std::random_device rd;
    std::mt19937 gen(rd());

    if (currentMathDifficulty == UltraHard) {
        if (!manager.mathHardTasks.empty()) {
            std::uniform_int_distribution<> disUltra(
                0, static_cast<int>(manager.mathHardTasks.size() - 1));
            int idx = disUltra(gen);
            expr = manager.mathHardTasks[idx].expression;
            ans = manager.mathHardTasks[idx].answer;
        } else {
            expr = "2 + 2 = ?";
            ans = 4;
        }
    } else if (currentMathDifficulty == Easy) {
        std::uniform_int_distribution<> dis(1, 20);
        a = dis(gen);
        b = dis(gen);
        if (dis(gen) % 2 == 0) {
            expr = QString("%1 + %2 = ?").arg(a).arg(b);
            ans = a + b;
        } else {
            if (a < b) {
                std::swap(a, b);
            }
            expr = QString("%1 - %2 = ?").arg(a).arg(b);
            ans = a - b;
        }
    } else if (currentMathDifficulty == Medium) {
        std::uniform_int_distribution<> disType(0, 2);
        int type = disType(gen);
        if (type == 0) {
            std::uniform_int_distribution<> disSmall(2, 9);
            a = disSmall(gen);
            b = disSmall(gen);
            expr = QString("%1 * %2 = ?").arg(a).arg(b);
            ans = a * b;
        } else {
            std::uniform_int_distribution<> dis(1, 50);
            a = dis(gen);
            b = dis(gen);
            c = dis(gen);
            if (type == 1) {
                expr = QString("%1 + %2 - %3 = ?").arg(a).arg(b).arg(c);
                ans = a + b - c;
            } else {
                expr = QString("%1 - %2 + %3 = ?").arg(a).arg(b).arg(c);
                ans = a - b + c;
            }
        }
    } else {
        std::uniform_int_distribution<> disType(0, 1);
        if (disType(gen) == 0) {
            std::uniform_int_distribution<> disDiv(2, 15);
            b = disDiv(gen);
            int tempAns = disDiv(gen);
            a = b * tempAns;
            std::uniform_int_distribution<> disC(1, 100);
            c = disC(gen);
            if (disC(gen) % 2 == 0) {
                expr = QString("%1 / %2 + %3 = ?").arg(a).arg(b).arg(c);
                ans = tempAns + c;
            } else {
                expr = QString("%1 / %2 - %3 = ?").arg(a).arg(b).arg(c);
                ans = tempAns - c;
            }
        } else {
            std::uniform_int_distribution<> disMult(2, 15);
            a = disMult(gen);
            b = disMult(gen);
            std::uniform_int_distribution<> disC(1, 100);
            c = disC(gen);
            if (disC(gen) % 2 == 0) {
                expr = QString("%1 * %2 + %3 = ?").arg(a).arg(b).arg(c);
                ans = a * b + c;
            } else {
                expr = QString("%1 * %2 - %3 = ?").arg(a).arg(b).arg(c);
                ans = a * b - c;
            }
        }
    }
    currentMathAnswer = ans;
    mathWidget->setTask(expr);
}

void MainWindow::nextTask() {
    if (tasksDoneInSession >= tasksPerSession) {
        endExercise(true);
        return;
    }

    if (currentType == Translation &&
        static_cast<size_t>(currentTaskIdx) < manager.translationTasks.size()) {
        auto task = manager.translationTasks[currentTaskIdx];
        transWidget->setTask(task.original);
        currentHint = task.hint;
    } else if (
        currentType == Grammar &&
        static_cast<size_t>(currentTaskIdx) < manager.grammarTasks.size()) {
        auto task = manager.grammarTasks[currentTaskIdx];
        grammarWidget->setTask(task.question, task.options);
        currentHint = task.hint;
    } else if (currentType == Math) {
        generateMathTask();
        currentHint = "Решите математическое выражение.";
    } else {
        endExercise(true);
        return;
    }

    timeLeft = timeLimit;
    timerLabel->setText(QString("Время: %1 сек").arg(timeLeft));
    timer->start(1000);
}

void MainWindow::onSubmitTranslation() {
    timer->stop();
    QString userAns = transWidget->getAnswer();
    QString correctAns = manager.translationTasks[currentTaskIdx].translation;

    int dist = calculateLevenshtein(userAns, correctAns);
    int maxDist = std::max(1, (int)correctAns.length() / 5);  // Allow 1 typo per 5 characters

    if (dist <= maxDist) {
        showNotification("Правильно!", false);
        tasksDoneInSession++;
        currentTaskIdx++;
        progress->setValue(tasksDoneInSession);
        nextTask();
    } else {
        mistakesCount++;
        updateLivesDisplay();
        showNotification("Неправильно!", true);
        if (mistakesCount >= maxMistakes) {
            failExercise("Слишком много ошибок!");
        } else {
            timer->start(1000);  // Продолжаем попытки
        }
    }
}

void MainWindow::onSubmitGrammar() {
    timer->stop();
    QString userAns = grammarWidget->getSelected();
    QString correctAns = manager.grammarTasks[currentTaskIdx].answer;

    if (userAns == correctAns) {
        showNotification("Правильно!", false);
        tasksDoneInSession++;
        currentTaskIdx++;
        progress->setValue(tasksDoneInSession);
        nextTask();
    } else {
        mistakesCount++;
        updateLivesDisplay();
        showNotification("Неправильно!", true);
        if (mistakesCount >= maxMistakes) {
            failExercise("Слишком много ошибок!");
        } else {
            timer->start(1000);
        }
    }
}

void MainWindow::onSubmitMath() {
    timer->stop();
    QString userAnsStr = mathWidget->getAnswer();
    bool ok;
    int userAns = userAnsStr.toInt(&ok);

    if (ok && userAns == currentMathAnswer) {
        showNotification("Правильно!", false);
        tasksDoneInSession++;
        progress->setValue(tasksDoneInSession);
        nextTask();
    } else {
        mistakesCount++;
        updateLivesDisplay();
        showNotification("Неправильно!", true);
        if (mistakesCount >= maxMistakes) {
            failExercise("Слишком много ошибок!");
        } else {
            timer->start(1000);
        }
    }
}

void MainWindow::endExercise(bool success) {
    timer->stop();
    topBar->hide();
    if (success) {
        score += 100;
        scoreLabel->setText("Очки: " + QString::number(score));
        showNotification("Упражнение завершено! Очки начислены.", false);
    }
    currentType = None;
    stack->setCurrentWidget(menuWidget);
}

void MainWindow::failExercise(const QString& reason) {
    timer->stop();
    topBar->hide();
    showNotification(reason, true);
    currentType = None;
    stack->setCurrentWidget(menuWidget);
}

void MainWindow::updateTimer() {
    timeLeft--;
    timerLabel->setText(QString("Время: %1 сек").arg(timeLeft));
    if (timeLeft <= 0) {
        timer->stop();
        mistakesCount++;
        updateLivesDisplay();
        showNotification("Время вышло!", true);
        if (mistakesCount >= maxMistakes) {
            failExercise("Слишком много ошибок!");
        } else {
            nextTask();  // Переходим к следующей задаче, чтобы не застрять
        }
    }
}

void MainWindow::showHint() {
    if (!currentHint.isEmpty()) {
        showNotification("Подсказка: " + currentHint, false);
    } else {
        showNotification("Для этого задания нет подсказки.", false);
    }
}

void MainWindow::giveUp() {
    failExercise("Вы сдались!");
}

void MainWindow::submitCurrentTask() {
    if (currentType == Translation) {
        onSubmitTranslation();
    } else if (currentType == Grammar) {
        onSubmitGrammar();
    } else if (currentType == Math) {
        onSubmitMath();
    }
}

void MainWindow::keyPressEvent(QKeyEvent* event) {
    if (stack->currentWidget() == menuWidget) {
        QMainWindow::keyPressEvent(event);
        return;
    }

    if (currentType == Grammar &&
        (event->key() == Qt::Key_Return || event->key() == Qt::Key_Enter)) {
        submitCurrentTask();
    } else if (currentType == Grammar && event->key() >= Qt::Key_1 && event->key() <= Qt::Key_4) {
        grammarWidget->selectOption(event->key() - Qt::Key_1);
    } else {
        QMainWindow::keyPressEvent(event);
    }
}

void MainWindow::showNotification(const QString& text, bool isError) {
    notificationLabel->setText(text);
    if (isError) {
        notificationLabel->setStyleSheet(
            "QLabel#notificationLabel { background-color: #ff4b4b; color: white; font-weight: "
            "bold; font-size: 16px; border-radius: 10px; padding: 10px; }");
    } else {
        notificationLabel->setStyleSheet(
            "QLabel#notificationLabel { background-color: #58cc02; color: white; font-weight: "
            "bold; font-size: 16px; border-radius: 10px; padding: 10px; }");
    }
    notificationLabel->adjustSize();
    notificationLabel->move(
        this->width() - notificationLabel->width() - 20,
        this->height() - notificationLabel->height() - 20);
    notificationLabel->raise();
    notificationLabel->show();
    notificationTimer->start(2000);
}

void MainWindow::hideNotification() {
    notificationLabel->hide();
}

void MainWindow::updateLivesDisplay() {
    QString hearts;
    int remaining = maxMistakes - mistakesCount;
    for (int i = 0; i < remaining; ++i) {
        hearts += "❤️";
    }
    for (int i = 0; i < mistakesCount; ++i) {
        hearts += "♡";
    }
    livesLabel->setText(hearts);
}

void MainWindow::resizeEvent(QResizeEvent* event) {
    QMainWindow::resizeEvent(event);
    if (notificationLabel && notificationLabel->isVisible()) {
        notificationLabel->move(
            this->width() - notificationLabel->width() - 20,
            this->height() - notificationLabel->height() - 20);
    }
}