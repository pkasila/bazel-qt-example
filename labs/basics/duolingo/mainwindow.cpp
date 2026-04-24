#include "mainwindow.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QMenuBar>
#include <QMessageBox>
#include <QApplication>
#include <algorithm>

// ==========================================
// ДИАЛОГ СЛОЖНОСТИ
// ==========================================
DifficultyDialog::DifficultyDialog(QWidget *parent) : QDialog(parent) {
    setWindowTitle("Выбор сложности");
    QVBoxLayout *layout = new QVBoxLayout(this);
    
    diffGroup = new QButtonGroup(this);
    QRadioButton *btnEasy = new QRadioButton("Легко (3 права на ошибку)", this);
    QRadioButton *btnMed = new QRadioButton("Средне (2 права на ошибку)", this);
    QRadioButton *btnHard = new QRadioButton("Сложно (1 право на ошибку)", this);
    
    btnEasy->setChecked(true);
    diffGroup->addButton(btnEasy, 0);
    diffGroup->addButton(btnMed, 1);
    diffGroup->addButton(btnHard, 2);
    
    layout->addWidget(btnEasy);
    layout->addWidget(btnMed);
    layout->addWidget(btnHard);
    
    QPushButton *okBtn = new QPushButton("ОК", this);
    connect(okBtn, &QPushButton::clicked, this, &QDialog::accept);
    layout->addWidget(okBtn);
}

int DifficultyDialog::getSelectedDifficulty() const {
    return diffGroup->checkedId();
}

// ==========================================
// ГЛАВНОЕ ОКНО
// ==========================================
MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {
    // --- 1. НАСТРОЙКА МЕНЮ (MenuBar) ---
    QMenu *settingsMenu = menuBar()->addMenu("Настройки");
    QAction *diffAction = settingsMenu->addAction("Изменить сложность");
    connect(diffAction, &QAction::triggered, this, &MainWindow::onChangeDifficulty);

    // --- 2. НАСТРОЙКА ОСНОВНОГО СТЕКА ЭКРАНОВ ---
    mainStack = new QStackedWidget(this);
    setCentralWidget(mainStack);

    // ==========================================
    // ЭКРАН 1: ГЛАВНОЕ МЕНЮ
    // ==========================================
    menuWidget = new QWidget();
    QVBoxLayout *menuLayout = new QVBoxLayout(menuWidget);
    
    scoreLabel = new QLabel("Твои баллы: 0", this);
    scoreLabel->setAlignment(Qt::AlignCenter);
    scoreLabel->setStyleSheet("font-size: 24px; font-weight: bold;");
    
    QPushButton *btnTrans = new QPushButton("Translation (Перевод)", this);
    QPushButton *btnGram = new QPushButton("Grammar (Грамматика)", this);
    
    menuLayout->addStretch();
    menuLayout->addWidget(scoreLabel);
    menuLayout->addWidget(btnTrans);
    menuLayout->addWidget(btnGram);
    menuLayout->addStretch();
    
    connect(btnTrans, &QPushButton::clicked, this, &MainWindow::onStartTranslation);
    connect(btnGram, &QPushButton::clicked, this, &MainWindow::onStartGrammar);

    // ==========================================
    // ЭКРАН 2: УПРАЖНЕНИЕ
    // ==========================================
    exerciseWidget = new QWidget();
    QVBoxLayout *exLayout = new QVBoxLayout(exerciseWidget);
    
    // Верхняя панель (Таймер, Жизни, Прогресс, Подсказка)
    QHBoxLayout *topLayout = new QHBoxLayout();
    timerLabel = new QLabel("Время: 30", this);
    livesLabel = new QLabel("❤️: 3", this);
    progressBar = new QProgressBar(this);
    
    // ДОБАВЛЯЕМ ВИДИМУЮ КНОПКУ HELP
    helpBtn = new QPushButton("Help (H)", this);
    connect(helpBtn, &QPushButton::clicked, this, &MainWindow::onShowHint);

    topLayout->addWidget(timerLabel);
    topLayout->addWidget(progressBar);
    topLayout->addWidget(livesLabel);
    topLayout->addWidget(helpBtn); // <--- Добавили кнопку на панель
    
    // Вопрос
    questionLabel = new QLabel("Вопрос", this);
    questionLabel->setAlignment(Qt::AlignCenter);
    questionLabel->setStyleSheet("font-size: 18px; margin: 20px;");
    
    // Динамический ввод (Стек внутри стека)
    inputStack = new QStackedWidget(this);
    
    // Ввод для перевода (текст)
    translationInput = new QLineEdit(this);
    translationInput->setPlaceholderText("Введите перевод...");
    
    // Ввод для грамматики (радио-кнопки)
    grammarWidget = new QWidget(this);
    grammarGroup = new QButtonGroup(this);
    // Радиокнопки добавим динамически позже
    
    inputStack->addWidget(translationInput);
    inputStack->addWidget(grammarWidget);
    
    // Кнопка подтверждения
    submitBtn = new QPushButton("Submit", this);
    connect(submitBtn, &QPushButton::clicked, this, &MainWindow::onSubmitAnswer);
    
    exLayout->addLayout(topLayout);
    exLayout->addWidget(questionLabel);
    exLayout->addWidget(inputStack);
    exLayout->addWidget(submitBtn);
    exLayout->addStretch();

    // Добавляем экраны в главный стек
    mainStack->addWidget(menuWidget);
    mainStack->addWidget(exerciseWidget);

    // --- 3. НАСТРОЙКИ ТАЙМЕРА И ПОДСКАЗОК ---
    timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &MainWindow::onTimeOut);

    QShortcut *helpShortcut = new QShortcut(QKeySequence("H"), this);
    connect(helpShortcut, &QShortcut::activated, this, [this]() {
        // Защита: чтобы окно подсказки не вылезало, когда мы просто печатаем слово с буквой 'h'
        if (!translationInput->hasFocus()) {
            onShowHint();
        }
    });

    // ИНИЦИАЛИЗАЦИЯ ЗВУКОВ
    correctSound = new QSoundEffect(this);
    // Указываем путь от корня проекта
    correctSound->setSource(QUrl::fromLocalFile("labs/basics/duolingo/correct.wav"));
    correctSound->setVolume(1.0f);

    wrongSound = new QSoundEffect(this);
    // Указываем путь от корня проекта
    wrongSound->setSource(QUrl::fromLocalFile("labs/basics/duolingo/wrong.wav"));
    wrongSound->setVolume(1.0f);
}

// ==========================================
// ЛОГИКА
// ==========================================

void MainWindow::onChangeDifficulty() {
    DifficultyDialog dlg(this);
    if (dlg.exec() == QDialog::Accepted) {
        difficulty = dlg.getSelectedDifficulty();
        QMessageBox::information(this, "Успех", "Сложность изменена!");
    }
}

void MainWindow::onStartTranslation() {
    loadQuestions(ExerciseType::Translation);
}

void MainWindow::onStartGrammar() {
    loadQuestions(ExerciseType::Grammar);
}

void MainWindow::loadQuestions(ExerciseType type) {
    currentType = type;
    currentQuestions.clear();
    
    // Моковая база данных вопросов
    if (type == ExerciseType::Translation) {
        currentQuestions.append({"Яблоко красное", "The apple is red", {}, "Глагол to be обязателен."});
        currentQuestions.append({"Я люблю программировать", "I love programming", {}, "Используй герундий (ing)."});
        currentQuestions.append({"Кот спит", "The cat is sleeping", {}, "Present Continuous."});
    } else {
        currentQuestions.append({"He ___ to school every day.", "goes", {"go", "goes", "going"}, "Present Simple, 3 лицо ед.ч."});
        currentQuestions.append({"I ___ seen this movie already.", "have", {"has", "have", "had"}, "Present Perfect, 1 лицо."});
        currentQuestions.append({"She is ___ beautiful.", "very", {"many", "much", "very"}, "Наречие степени перед прилагательным."});
    }

    // Настройка жизней в зависимости от сложности
    lives = 3 - difficulty; 
    currentQuestionIndex = 0;
    
    progressBar->setMaximum(currentQuestions.size());
    progressBar->setValue(0);
    
    // Запуск таймера (30 секунд на всё упражнение)
    timeLeft = 30;
    timerLabel->setText(QString("Время: %1").arg(timeLeft));
    timer->start(1000);
    
    mainStack->setCurrentWidget(exerciseWidget);
    showNextQuestion();
}

void MainWindow::showNextQuestion() {
    livesLabel->setText(QString("❤️: %1").arg(lives));
    progressBar->setValue(currentQuestionIndex);
    
    const Question &q = currentQuestions[currentQuestionIndex];
    questionLabel->setText(q.text);
    
    if (currentType == ExerciseType::Translation) {
        inputStack->setCurrentWidget(translationInput);
        translationInput->clear();
        translationInput->setFocus();
    } else {
        inputStack->setCurrentWidget(grammarWidget);
        
        // Удаляем старые радио-кнопки
        qDeleteAll(grammarWidget->children());
        QVBoxLayout *gramLayout = new QVBoxLayout(grammarWidget);
        
        // Создаем новые
        for (int i = 0; i < q.options.size(); ++i) {
            QRadioButton *rb = new QRadioButton(q.options[i], grammarWidget);
            grammarGroup->addButton(rb, i);
            gramLayout->addWidget(rb);
            if (i == 0) rb->setChecked(true); // по умолчанию выбран первый
        }
    }
}

void MainWindow::onSubmitAnswer() {
    const Question &q = currentQuestions[currentQuestionIndex];
    bool isCorrect = false;

    if (currentType == ExerciseType::Translation) {
        QString userInput = translationInput->text();
        // Используем продвинутое сравнение (Бонус)
        isCorrect = checkAnswerAdvanced(userInput, q.correctAnswer);
    } else {
        QRadioButton *checkedBtn = qobject_cast<QRadioButton*>(grammarGroup->checkedButton());
        if (checkedBtn && checkedBtn->text() == q.correctAnswer) {
            isCorrect = true;
        }
    }

    if (isCorrect) {
        correctSound->play(); // <--- ВОСПРОИЗВОДИМ ЗВУК УСПЕХА
        currentQuestionIndex++;
        if (currentQuestionIndex >= currentQuestions.size()) {
            finishExercise(true); 
        } else {
            showNextQuestion();
        }
    } else {
        wrongSound->play(); // <--- ВОСПРОИЗВОДИМ ЗВУК ОШИБКИ
        lives--;
        livesLabel->setText(QString("❤️: %1").arg(lives));
        if (lives <= 0) {
            finishExercise(false); 
        } else {
            QMessageBox::warning(this, "Ошибка", "Неверно! Попробуй еще раз.");
        }
    }
}

void MainWindow::onTimeOut() {
    timeLeft--;
    timerLabel->setText(QString("Время: %1").arg(timeLeft));
    if (timeLeft <= 0) {
        finishExercise(false);
    }
}

void MainWindow::finishExercise(bool success) {
    timer->stop();
    if (success) {
        score += 100;
        scoreLabel->setText(QString("Твои баллы: %1").arg(score));
        QMessageBox::information(this, "Победа!", "Отлично! Упражнение выполнено.");
    } else {
        QMessageBox::critical(this, "Поражение", "Попытки или время исчерпаны!");
    }
    mainStack->setCurrentWidget(menuWidget);
}

void MainWindow::onShowHint() {
    if (mainStack->currentWidget() == exerciseWidget) {
        QString hint = currentQuestions[currentQuestionIndex].hint;
        QMessageBox::information(this, "Подсказка", hint);
    }
}

// ==========================================
// ADVANCED: Алгоритм Левенштейна (Опечатки)
// ==========================================
bool MainWindow::checkAnswerAdvanced(const QString& user, const QString& target) {
    // 1. Приводим к нижнему регистру и удаляем лишние пробелы
    QString u = user.trimmed().toLower();
    QString t = target.trimmed().toLower();
    
    // 2. Идеальное совпадение
    if (u == t) return true;
    
    // 3. Допускаем 1-2 опечатки с помощью расстояния Левенштейна
    int dist = levenshteinDistance(u, t);
    int allowedMistakes = (t.length() > 5) ? 2 : 1; // Чем длиннее слово, тем больше опечаток прощаем
    
    return dist <= allowedMistakes;
}

int MainWindow::levenshteinDistance(const QString& s1, const QString& s2) {
    int len1 = s1.length();
    int len2 = s2.length();
    QVector<QVector<int>> d(len1 + 1, QVector<int>(len2 + 1));

    for (int i = 0; i <= len1; ++i) d[i][0] = i;
    for (int j = 0; j <= len2; ++j) d[0][j] = j;

    for (int i = 1; i <= len1; ++i) {
        for (int j = 1; j <= len2; ++j) {
            int cost = (s1[i - 1] == s2[j - 1]) ? 0 : 1;
            d[i][j] = std::min({ d[i - 1][j] + 1,       // Удаление
                                 d[i][j - 1] + 1,       // Вставка
                                 d[i - 1][j - 1] + cost // Замена
                               });
        }
    }
    return d[len1][len2];
}