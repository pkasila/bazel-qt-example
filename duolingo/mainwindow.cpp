#include "mainwindow.h"

#include <QAction>
#include <QApplication>
#include <QCoreApplication>
#include <QDir>
#include <QFileInfo>
#include <QHBoxLayout>
#include <QMenuBar>
#include <QMessageBox>
#include <QRandomGenerator>
#include <QRegularExpression>
#include <QVBoxLayout>
#include <algorithm>
#include <random>

DifficultyDialog::DifficultyDialog(int currentDifficulty, QWidget *parent)
    : QDialog(parent), difficultyGroup(new QButtonGroup(this)) {
    setWindowTitle("Difficulty");
    setModal(true);
    setMinimumWidth(340);

    auto *layout = new QVBoxLayout(this);
    auto *title = new QLabel("Выберите уровень сложности", this);
    title->setObjectName("dialogTitle");

    auto *easy = new QRadioButton("Easy: 5 заданий, 3 ошибки, 90 секунд", this);
    auto *medium = new QRadioButton("Medium: 6 заданий, 2 ошибки, 70 секунд", this);
    auto *hard = new QRadioButton("Hard: 7 заданий, 1 ошибка, 50 секунд", this);
    auto *okButton = new QPushButton("Apply", this);

    difficultyGroup->addButton(easy, 0);
    difficultyGroup->addButton(medium, 1);
    difficultyGroup->addButton(hard, 2);

    if (auto *button = difficultyGroup->button(currentDifficulty)) {
        button->setChecked(true);
    } else {
        easy->setChecked(true);
    }

    layout->addWidget(title);
    layout->addWidget(easy);
    layout->addWidget(medium);
    layout->addWidget(hard);
    layout->addSpacing(8);
    layout->addWidget(okButton);

    connect(okButton, &QPushButton::clicked, this, &QDialog::accept);
}

int DifficultyDialog::selectedDifficulty() const {
    return difficultyGroup->checkedId();
}

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {
    buildMenuBar();
    buildInterface();
    buildSounds();
    applyTheme();

    timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &MainWindow::tickTimer);

    auto *shortcut = new QShortcut(QKeySequence(Qt::Key_H), this);
    connect(shortcut, &QShortcut::activated, this, [this]() {
        if (!translationInput->hasFocus()) {
            showHint();
        }
    });

    updateStatus();
    resize(980, 620);
    setMinimumSize(780, 480);
    setWindowTitle("Lingua Quest");
}

void MainWindow::buildMenuBar() {
    auto *settings = menuBar()->addMenu("Settings");
    auto *difficultyAction = settings->addAction("Change difficulty");
    connect(difficultyAction, &QAction::triggered, this, &MainWindow::changeDifficulty);
}

void MainWindow::buildInterface() {
    rootWidget = new QWidget(this);
    setCentralWidget(rootWidget);

    auto *mainLayout = new QHBoxLayout(rootWidget);
    mainLayout->setContentsMargins(18, 18, 18, 18);
    mainLayout->setSpacing(18);

    auto *sidePanel = new QWidget(rootWidget);
    sidePanel->setObjectName("sidePanel");
    sidePanel->setMinimumWidth(260);
    sidePanel->setMaximumWidth(340);

    auto *sideLayout = new QVBoxLayout(sidePanel);
    sideLayout->setContentsMargins(22, 22, 22, 22);
    sideLayout->setSpacing(14);

    auto *appName = new QLabel("Lingua Quest", sidePanel);
    appName->setObjectName("appName");
    auto *appDescription = new QLabel("Мини-тренажёр английского языка", sidePanel);
    appDescription->setObjectName("appDescription");
    appDescription->setWordWrap(true);

    scoreLabel = new QLabel(sidePanel);
    scoreLabel->setObjectName("scorePill");
    scoreLabel->setAlignment(Qt::AlignCenter);
    difficultyLabel = new QLabel(sidePanel);
    difficultyLabel->setObjectName("difficultyPill");
    difficultyLabel->setAlignment(Qt::AlignCenter);

    translationButton = new QPushButton("Translation", sidePanel);
    grammarButton = new QPushButton("Grammar", sidePanel);
    translationButton->setObjectName("primaryButton");
    grammarButton->setObjectName("secondaryButton");

    auto *tip = new QLabel("Нажмите H во время упражнения, чтобы открыть подсказку.", sidePanel);
    tip->setObjectName("smallHint");
    tip->setWordWrap(true);

    sideLayout->addWidget(appName);
    sideLayout->addWidget(appDescription);
    sideLayout->addSpacing(12);
    sideLayout->addWidget(scoreLabel);
    sideLayout->addWidget(difficultyLabel);
    sideLayout->addSpacing(20);
    sideLayout->addWidget(translationButton);
    sideLayout->addWidget(grammarButton);
    sideLayout->addStretch();
    sideLayout->addWidget(tip);

    auto *exercisePanel = new QWidget(rootWidget);
    exercisePanel->setObjectName("exercisePanel");
    auto *exerciseLayout = new QVBoxLayout(exercisePanel);
    exerciseLayout->setContentsMargins(28, 28, 28, 28);
    exerciseLayout->setSpacing(16);

    auto *topBar = new QHBoxLayout();
    timerLabel = new QLabel(exercisePanel);
    timerLabel->setObjectName("metricPill");
    livesLabel = new QLabel(exercisePanel);
    livesLabel->setObjectName("metricPill");
    progressBar = new QProgressBar(exercisePanel);
    progressBar->setTextVisible(true);
    progressBar->setMinimum(0);
    helpButton = new QPushButton("Help (H)", exercisePanel);
    helpButton->setObjectName("ghostButton");

    topBar->addWidget(timerLabel);
    topBar->addWidget(progressBar, 1);
    topBar->addWidget(livesLabel);
    topBar->addWidget(helpButton);

    titleLabel = new QLabel("Выберите упражнение", exercisePanel);
    titleLabel->setObjectName("titleLabel");
    subtitleLabel = new QLabel("Меню всегда находится слева, а задания динамически меняются справа.", exercisePanel);
    subtitleLabel->setObjectName("subtitleLabel");
    subtitleLabel->setWordWrap(true);

    questionLabel = new QLabel("Начните Translation или Grammar.", exercisePanel);
    questionLabel->setObjectName("questionCard");
    questionLabel->setAlignment(Qt::AlignCenter);
    questionLabel->setWordWrap(true);
    questionLabel->setMinimumHeight(120);

    exerciseStack = new QStackedWidget(exercisePanel);
    translationInput = new QTextEdit(exerciseStack);
    translationInput->setPlaceholderText("Введите перевод здесь...");
    translationInput->setMinimumHeight(120);

    grammarWidget = new QWidget(exerciseStack);
    grammarLayout = new QVBoxLayout(grammarWidget);
    grammarLayout->setContentsMargins(0, 0, 0, 0);
    grammarLayout->setSpacing(10);
    grammarGroup = new QButtonGroup(this);

    exerciseStack->addWidget(translationInput);
    exerciseStack->addWidget(grammarWidget);

    statusLabel = new QLabel("За полностью выполненное упражнение начисляются баллы.", exercisePanel);
    statusLabel->setObjectName("statusLabel");
    statusLabel->setWordWrap(true);

    submitButton = new QPushButton("Submit", exercisePanel);
    submitButton->setObjectName("submitButton");
    submitButton->setEnabled(false);

    exerciseLayout->addLayout(topBar);
    exerciseLayout->addWidget(titleLabel);
    exerciseLayout->addWidget(subtitleLabel);
    exerciseLayout->addWidget(questionLabel, 1);
    exerciseLayout->addWidget(exerciseStack);
    exerciseLayout->addWidget(statusLabel);
    exerciseLayout->addWidget(submitButton);

    mainLayout->addWidget(sidePanel);
    mainLayout->addWidget(exercisePanel, 1);

    connect(translationButton, &QPushButton::clicked, this, &MainWindow::startTranslation);
    connect(grammarButton, &QPushButton::clicked, this, &MainWindow::startGrammar);
    connect(submitButton, &QPushButton::clicked, this, &MainWindow::submitAnswer);
    connect(helpButton, &QPushButton::clicked, this, &MainWindow::showHint);
}

void MainWindow::buildSounds() {
    correctSound = new QSoundEffect(this);
    correctSound->setSource(QUrl::fromLocalFile(resourcePath("correct.wav")));
    correctSound->setVolume(0.85f);

    wrongSound = new QSoundEffect(this);
    wrongSound->setSource(QUrl::fromLocalFile(resourcePath("wrong.wav")));
    wrongSound->setVolume(0.85f);
}

void MainWindow::applyTheme() {
    qApp->setStyleSheet(R"(
        QMainWindow, QWidget { background: #f4f7fb; color: #1f2937; font-family: Arial, Helvetica, sans-serif; font-size: 15px; }
        QMenuBar { background: #ffffff; border-bottom: 1px solid #e5e7eb; padding: 4px; }
        QMenuBar::item:selected, QMenu::item:selected { background: #dbeafe; border-radius: 6px; }
        #sidePanel, #exercisePanel { background: #ffffff; border: 1px solid #e5e7eb; border-radius: 22px; }
        #appName { font-size: 34px; font-weight: 900; color: #2563eb; }
        #appDescription, #subtitleLabel, #smallHint, #statusLabel { color: #64748b; }
        #scorePill, #difficultyPill, #metricPill { background: #eef2ff; color: #1e3a8a; border-radius: 14px; padding: 10px 14px; font-weight: 700; }
        #difficultyPill { background: #ecfeff; color: #155e75; }
        #metricPill { background: #f8fafc; color: #334155; border: 1px solid #e2e8f0; }
        #titleLabel { font-size: 28px; font-weight: 900; color: #111827; }
        #questionCard { background: qlineargradient(x1:0, y1:0, x2:1, y2:1, stop:0 #dbeafe, stop:1 #f0fdfa); border-radius: 22px; padding: 24px; font-size: 28px; font-weight: 800; color: #0f172a; }
        QPushButton { border: none; border-radius: 16px; padding: 13px 18px; font-weight: 800; }
        QPushButton:disabled { background: #cbd5e1; color: #f8fafc; }
        #primaryButton, #submitButton { background: #2563eb; color: white; }
        #primaryButton:hover, #submitButton:hover { background: #1d4ed8; }
        #secondaryButton { background: #14b8a6; color: white; }
        #secondaryButton:hover { background: #0f766e; }
        #ghostButton { background: #f1f5f9; color: #334155; border: 1px solid #e2e8f0; }
        QTextEdit { background: #f8fafc; border: 2px solid #dbeafe; border-radius: 16px; padding: 14px; selection-background-color: #bfdbfe; font-size: 18px; }
        QRadioButton { background: #f8fafc; border: 2px solid #e2e8f0; border-radius: 14px; padding: 12px; font-size: 18px; }
        QRadioButton:hover { border-color: #93c5fd; }
        QProgressBar { background: #e5e7eb; border: none; border-radius: 10px; height: 18px; text-align: center; font-weight: 800; color: #0f172a; }
        QProgressBar::chunk { background: #22c55e; border-radius: 10px; }
        #dialogTitle { font-size: 20px; font-weight: 800; margin-bottom: 8px; }
    )");
}

void MainWindow::changeDifficulty() {
    DifficultyDialog dialog(difficulty, this);
    if (dialog.exec() == QDialog::Accepted) {
        difficulty = dialog.selectedDifficulty();
        updateStatus();
        QMessageBox::information(this, "Difficulty", "Сложность изменена на " + difficultyName() + ".");
    }
}

void MainWindow::startTranslation() {
    loadQuestions(ExerciseType::Translation);
}

void MainWindow::startGrammar() {
    loadQuestions(ExerciseType::Grammar);
}

void MainWindow::loadQuestions(ExerciseType type) {
    currentType = type;
    currentQuestionIndex = 0;
    lives = initialLives();
    timeLeft = initialTime();
    currentQuestions = type == ExerciseType::Translation ? translationBank() : grammarBank();

    std::shuffle(currentQuestions.begin(), currentQuestions.end(), std::mt19937(QRandomGenerator::global()->generate()));
    while (currentQuestions.size() > exerciseSize()) {
        currentQuestions.removeLast();
    }

    progressBar->setMaximum(currentQuestions.size());
    progressBar->setValue(0);
    submitButton->setEnabled(true);
    titleLabel->setText(type == ExerciseType::Translation ? "Translation" : "Grammar");
    subtitleLabel->setText(type == ExerciseType::Translation
        ? "Переведите фразу на английский. Небольшие опечатки допускаются."
        : "Выберите единственный правильный вариант ответа.");

    timer->start(1000);
    updateStatus();
    showQuestion();
}

void MainWindow::showQuestion() {
    if (currentQuestionIndex >= currentQuestions.size()) {
        finishExercise(true, "Отлично! Все задания выполнены правильно.");
        return;
    }

    const Question &question = currentQuestions[currentQuestionIndex];
    questionLabel->setText(question.text);
    progressBar->setValue(currentQuestionIndex);
    progressBar->setFormat(QString("%1 / %2").arg(currentQuestionIndex).arg(currentQuestions.size()));

    if (currentType == ExerciseType::Translation) {
        exerciseStack->setCurrentWidget(translationInput);
        translationInput->clear();
        translationInput->setFocus();
    } else {
        exerciseStack->setCurrentWidget(grammarWidget);
        resetGrammarOptions(question);
    }

    updateStatus();
}

void MainWindow::resetGrammarOptions(const Question &question) {
    const QList<QAbstractButton *> buttons = grammarGroup->buttons();
    for (QAbstractButton *button : buttons) {
        grammarGroup->removeButton(button);
        delete button;
    }

    while (QLayoutItem *item = grammarLayout->takeAt(0)) {
        delete item;
    }

    for (int i = 0; i < question.options.size(); ++i) {
        auto *radio = new QRadioButton(question.options.at(i), grammarWidget);
        grammarGroup->addButton(radio, i);
        grammarLayout->addWidget(radio);
        if (i == 0) {
            radio->setChecked(true);
        }
    }

    grammarLayout->addStretch();
}

void MainWindow::submitAnswer() {
    if (currentQuestionIndex >= currentQuestions.size()) {
        return;
    }

    const Question &question = currentQuestions.at(currentQuestionIndex);
    bool correct = false;

    if (currentType == ExerciseType::Translation) {
        correct = isTranslationCorrect(translationInput->toPlainText(), question.correctAnswer);
    } else if (auto *checked = grammarGroup->checkedButton()) {
        correct = checked->text() == question.correctAnswer;
    }

    if (correct) {
        if (correctSound->isLoaded()) {
            correctSound->play();
        }
        ++currentQuestionIndex;
        if (currentQuestionIndex == currentQuestions.size()) {
            finishExercise(true, "Отлично! Упражнение выполнено полностью.");
        } else {
            showQuestion();
        }
        return;
    }

    if (wrongSound->isLoaded()) {
        wrongSound->play();
    }

    --lives;
    updateStatus();

    if (lives <= 0) {
        finishExercise(false, "Количество неверных попыток исчерпано.");
    } else {
        QMessageBox::warning(this, "Неверно", "Ответ не засчитан. Попробуйте ещё раз.");
    }
}

void MainWindow::tickTimer() {
    --timeLeft;
    updateStatus();

    if (timeLeft <= 0) {
        finishExercise(false, "Время, отведённое на упражнение, истекло.");
    }
}

void MainWindow::finishExercise(bool success, const QString &message) {
    timer->stop();
    submitButton->setEnabled(false);
    progressBar->setValue(success ? currentQuestions.size() : currentQuestionIndex);
    progressBar->setFormat(QString("%1 / %2").arg(progressBar->value()).arg(currentQuestions.size()));

    if (success) {
        score += reward();
        updateStatus();
        QMessageBox::information(this, "Готово", message + QString("\nНачислено баллов: %1.").arg(reward()));
    } else {
        QMessageBox::critical(this, "Упражнение завершено", message);
    }

    titleLabel->setText("Выберите упражнение");
    subtitleLabel->setText("Меню всегда находится слева, а задания динамически меняются справа.");
    questionLabel->setText("Начните Translation или Grammar.");
    statusLabel->setText("За полностью выполненное упражнение начисляются баллы.");
    translationInput->clear();
}

void MainWindow::showHint() {
    if (!submitButton->isEnabled() || currentQuestionIndex >= currentQuestions.size()) {
        QMessageBox::information(this, "Help", "Запустите упражнение, чтобы получить подсказку по текущему заданию.");
        return;
    }

    QMessageBox::information(this, "Help", currentQuestions.at(currentQuestionIndex).hint);
}

void MainWindow::updateStatus() {
    scoreLabel->setText(QString("Score: %1").arg(score));
    difficultyLabel->setText("Difficulty: " + difficultyName());
    timerLabel->setText(QString("⏱ %1 s").arg(std::max(0, timeLeft)));
    livesLabel->setText(QString("❤ %1").arg(std::max(0, lives)));

    if (submitButton && submitButton->isEnabled()) {
        statusLabel->setText(QString("Задание %1 из %2. Ошибок можно допустить: %3.")
            .arg(currentQuestionIndex + 1)
            .arg(currentQuestions.size())
            .arg(lives));
    }
}

QString MainWindow::resourcePath(const QString &fileName) const {
    const QStringList candidates = {
        QDir::current().absoluteFilePath(fileName),
        QCoreApplication::applicationDirPath() + QDir::separator() + fileName,
        QCoreApplication::applicationDirPath() + QDir::separator() + "duolingo" + QDir::separator() + fileName,
        QCoreApplication::applicationDirPath() + QDir::separator() + ".." + QDir::separator() + fileName,
        QCoreApplication::applicationDirPath() + QDir::separator() + ".." + QDir::separator() + "duolingo" + QDir::separator() + fileName
    };

    for (const QString &candidate : candidates) {
        const QFileInfo info(candidate);
        if (info.exists() && info.isFile()) {
            return info.absoluteFilePath();
        }
    }

    return QDir::current().absoluteFilePath(fileName);
}

bool MainWindow::isTranslationCorrect(const QString &userAnswer, const QString &correctAnswer) const {
    const QString user = normalizeAnswer(userAnswer);
    const QString target = normalizeAnswer(correctAnswer);

    if (user == target) {
        return true;
    }

    const int distance = levenshteinDistance(user, target);
    const int allowedDistance = target.length() >= 18 ? 3 : 2;

    if (distance <= allowedDistance) {
        return true;
    }

    QStringList userWords = user.split(' ', Qt::SkipEmptyParts);
    QStringList targetWords = target.split(' ', Qt::SkipEmptyParts);
    std::sort(userWords.begin(), userWords.end());
    std::sort(targetWords.begin(), targetWords.end());

    return userWords == targetWords && userWords.size() > 2;
}

QString MainWindow::normalizeAnswer(const QString &text) const {
    QString result = text.toLower().normalized(QString::NormalizationForm_D);
    result.remove(QRegularExpression("[\\x{0300}-\\x{036f}]"));
    result.replace(QRegularExpression("[^a-z0-9\\s']"), " ");
    result.replace(QRegularExpression("\\s+"), " ");
    return result.trimmed();
}

int MainWindow::levenshteinDistance(const QString &left, const QString &right) const {
    const int leftSize = left.size();
    const int rightSize = right.size();
    QVector<QVector<int>> distances(leftSize + 1, QVector<int>(rightSize + 1));

    for (int i = 0; i <= leftSize; ++i) {
        distances[i][0] = i;
    }

    for (int j = 0; j <= rightSize; ++j) {
        distances[0][j] = j;
    }

    for (int i = 1; i <= leftSize; ++i) {
        for (int j = 1; j <= rightSize; ++j) {
            const int cost = left.at(i - 1) == right.at(j - 1) ? 0 : 1;
            distances[i][j] = std::min({
                distances[i - 1][j] + 1,
                distances[i][j - 1] + 1,
                distances[i - 1][j - 1] + cost
            });
        }
    }

    return distances[leftSize][rightSize];
}

QList<Question> MainWindow::translationBank() const {
    return {
        {"Яблоко красное", "The apple is red", {}, "В английском предложении обычно нужен глагол-связка: is или are."},
        {"Я люблю программировать", "I love programming", {}, "После love можно использовать форму с -ing: programming."},
        {"Кот спит", "The cat is sleeping", {}, "Для действия прямо сейчас используйте Present Continuous: is + V-ing."},
        {"Мы читаем книгу", "We are reading a book", {}, "Для we нужен глагол are, затем глагол с окончанием -ing."},
        {"Она пьёт чай", "She is drinking tea", {}, "Для she нужен глагол is, затем drinking."},
        {"Они играют в футбол", "They are playing football", {}, "Для they нужен глагол are."},
        {"У меня есть собака", "I have a dog", {}, "Для владения используйте have."},
        {"Сегодня хорошая погода", "The weather is good today", {}, "Weather употребляется с артиклем the и глаголом is."}
    };
}

QList<Question> MainWindow::grammarBank() const {
    return {
        {"He ___ to school every day.", "goes", {"go", "goes", "going", "gone"}, "Present Simple: в 3 лице единственного числа добавляется -s или -es."},
        {"I ___ seen this movie already.", "have", {"has", "have", "had", "having"}, "Present Perfect строится как have или has + V3."},
        {"She is ___ beautiful.", "very", {"many", "much", "very", "few"}, "Перед прилагательным подходит наречие степени very."},
        {"They ___ playing football now.", "are", {"is", "am", "are", "be"}, "Для they используется are."},
        {"This book is ___ than that one.", "more interesting", {"interesting", "more interesting", "most interesting", "interestinger"}, "Для длинных прилагательных сравнительная степень образуется через more."},
        {"Yesterday we ___ to the museum.", "went", {"go", "goes", "went", "gone"}, "Yesterday указывает на Past Simple."},
        {"There ___ two cats in the room.", "are", {"is", "are", "am", "be"}, "С множественным числом используется there are."},
        {"She ___ coffee every morning.", "drinks", {"drink", "drinks", "drinking", "drunk"}, "Present Simple, 3 лицо единственного числа: drinks."}
    };
}

int MainWindow::exerciseSize() const {
    return difficulty == 0 ? 5 : difficulty == 1 ? 6 : 7;
}

int MainWindow::initialLives() const {
    return difficulty == 0 ? 3 : difficulty == 1 ? 2 : 1;
}

int MainWindow::initialTime() const {
    return difficulty == 0 ? 90 : difficulty == 1 ? 70 : 50;
}

int MainWindow::reward() const {
    return difficulty == 0 ? 100 : difficulty == 1 ? 150 : 220;
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
